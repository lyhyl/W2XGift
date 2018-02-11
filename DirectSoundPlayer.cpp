#pragma once

#include "DirectSoundPlayer.h"

#pragma comment(lib,"winmm")
#pragma comment(lib,"dsound")
#pragma comment(lib,"dxguid")
#pragma comment(lib,"dxerr")


//-----------------------------------------------------------------------------
// Typing macros 
//-----------------------------------------------------------------------------
#define WAVEFILE_READ   1
#define WAVEFILE_WRITE  2

//-----------------------------------------------------------------------------
// Miscellaneous helper functions
//-----------------------------------------------------------------------------
#define SAFE_DELETE(p)       { if(p) { delete (p);     (p)=NULL; } }
#define SAFE_DELETE_ARRAY(p) { if(p) { delete[] (p);   (p)=NULL; } }
#define SAFE_RELEASE(p)      { if(p) { (p)->Release(); (p)=NULL; } }

//-----------------------------------------------------------------------------
// Construction/Destruction
//-----------------------------------------------------------------------------
DirectSoundPlayer::DirectSoundPlayer()
{
	memset(m_strWaveFileName, 0 ,sizeof(m_strWaveFileName));
	m_dwNotifyThreadID=0;
	m_nBufferPlayLength = 3;
	m_nNotificationsNum = 16;	
	m_pDirectSound = NULL;
	m_pDSBuffer = NULL;
	m_dwDSBufferSize = 0;
	m_dwNotifySize = 0;
	m_bFillNextNotificationWithSilence = FALSE;
	m_dwPlayProgress = 0;
	m_dwLastPlayPos = 0;
	m_dwNextWriteOffset = 0;
	m_pWaveFile = NULL;
	m_lVolume = 0;
	m_bPause = FALSE;
	m_bMute = FALSE;
}

DirectSoundPlayer::~DirectSoundPlayer()
{
	if(m_dwNotifyThreadID)
	{
        // �ر��߳�
		PostThreadMessage( m_dwNotifyThreadID, WM_QUIT, 0, 0 );
	    WaitForSingleObject( m_hNotifyThread, INFINITE );
	    CloseHandle( m_hNotifyThread );
		CloseHandle( m_hNotificationEvent );
	}

	//�ͷŻ�����
	SAFE_RELEASE(m_pDSBuffer);
	//�ͷ�directsound
	SAFE_RELEASE(m_pDirectSound);

	//�ر��ļ�
	if(m_pWaveFile)
	{
		m_pWaveFile->Close();
		SAFE_DELETE(m_pWaveFile);
	}

}

//-----------------------------------------------------------------------------
// ���úͻ�ȡwave�ļ���
//-----------------------------------------------------------------------------

void DirectSoundPlayer::SetWaveFileName(const char * strFileName)
{
	strcpy(m_strWaveFileName, strFileName);
}

void DirectSoundPlayer::GetWaveFileName(char *strFileName)
{
	strcpy(strFileName, m_strWaveFileName);
}

//-----------------------------------------------------------------------------
// �ж��ǲ���wave�ļ�
//-----------------------------------------------------------------------------

BOOL DirectSoundPlayer::CheckWaveFile()
{
	HRESULT hr;


	//�ж��ǲ���WAVE���͵��ļ�
	WaveFile fWaveFile;
	if(FAILED(hr=fWaveFile.Open(m_strWaveFileName, NULL, WAVEFILE_READ )))
	{
		return FALSE;
	}
	fWaveFile.Close();
	return TRUE;
}

//-----------------------------------------------------------------------------
// ��ʼ������Ҫ�����ɲ��Ż�����������֪ͨ�¼�
// ע�⣺�ڳ�ʼ��֮ǰ����Ҫ����SetWaveFileName()����
//       �ڲ���SetWaveFileName(),SetBufferLength()��SetNotificationsNum()
//       ��Ҫ����Initialize()����
//-----------------------------------------------------------------------------

BOOL DirectSoundPlayer::Initialize(HWND hWnd, BOOL bGlobalFocus)
{
	LRESULT hr;
	DSBPOSITIONNOTIFY*  pPosNotify = NULL; 
	LPDIRECTSOUNDNOTIFY pDSNotify  = NULL;

	//ɾ����ǰ������
	m_bFillNextNotificationWithSilence = FALSE;
	m_dwPlayProgress = 0;
	m_dwLastPlayPos = 0;
	m_dwNextWriteOffset = 0;

	if(m_dwNotifyThreadID  != 0)
	{
		// �ر��߳�
		PostThreadMessage( m_dwNotifyThreadID, WM_QUIT, 0, 0 );
		WaitForSingleObject( m_hNotifyThread, INFINITE );
		CloseHandle( m_hNotifyThread );	
	}

	if(m_pDSBuffer)
	{
		m_pDSBuffer->Stop();
		SAFE_RELEASE(m_pDSBuffer);
	}

	if(m_pDirectSound)
	{
		SAFE_RELEASE(m_pDirectSound);
	}

	if(m_pWaveFile)
	{
		m_pWaveFile->Close();
		SAFE_DELETE(m_pWaveFile);
	}

	else
	{
		//��WAVE�ļ�
		m_pWaveFile = new WaveFile();
		if(FAILED(hr=m_pWaveFile->Open(m_strWaveFileName, NULL, WAVEFILE_READ )))
		{
			SAFE_DELETE(m_pWaveFile);
			return FALSE;
		}
	}

	//����directsound
	if(FAILED(hr = DirectSoundCreate(NULL, &m_pDirectSound, NULL)))
	{
		return FALSE;
	}
	hr=m_pDirectSound->SetCooperativeLevel(hWnd, DSSCL_PRIORITY );
	
	//������������
	LPDIRECTSOUNDBUFFER pDSBPrimary = NULL;
	WAVEFORMATEX wfx;

	// Get the primary buffer 
	DSBUFFERDESC dsbd;
	ZeroMemory( &dsbd, sizeof(DSBUFFERDESC) );
	dsbd.dwSize        = sizeof(DSBUFFERDESC);
	dsbd.dwFlags       = DSBCAPS_PRIMARYBUFFER;
	dsbd.dwBufferBytes = 0;
	dsbd.lpwfxFormat   = NULL;
	
	if( FAILED( hr = m_pDirectSound->CreateSoundBuffer( &dsbd, &pDSBPrimary, NULL ) ))
	{
		return 0;
	}

	ZeroMemory( &wfx, sizeof(WAVEFORMATEX) ); 
	wfx.wFormatTag      = (WORD) WAVE_FORMAT_PCM; 
	wfx.nChannels       = 2; 
	wfx.nSamplesPerSec  = 44100; 
	wfx.wBitsPerSample  = 16; 
	wfx.nBlockAlign     = (WORD) (wfx.wBitsPerSample / 8 * wfx.nChannels);
	wfx.nAvgBytesPerSec = (DWORD) (wfx.nSamplesPerSec * wfx.nBlockAlign);
	
	if( FAILED( hr = pDSBPrimary->SetFormat(&wfx) ) )
	{
		return 0;
	}
	
	pDSBPrimary->Release();	

	//����һ��m_nBufferPlayLength��Ļ�����,��������������ֳ�m_nNotificationsNum��֪ͨ��;
	m_dwNotifySize = m_pWaveFile->m_pwfx->nSamplesPerSec*m_nBufferPlayLength*m_pWaveFile->m_pwfx->nBlockAlign/m_nNotificationsNum;
	//ȷ��m_dwNotifySize��nBlockAlign�ı���
	m_dwNotifySize = m_dwNotifySize - m_dwNotifySize%m_pWaveFile->m_pwfx->nBlockAlign;		
	m_dwDSBufferSize = m_dwNotifySize*m_nNotificationsNum;

	//�����ڶ�������
	DSBUFFERDESC BufferDesc;//����DSUBUFFERDESC�ṹ����;
	memset (&BufferDesc, 0, sizeof (DSBUFFERDESC)); 	
	BufferDesc.lpwfxFormat = m_pWaveFile->m_pwfx;
	BufferDesc.dwSize = sizeof(DSBUFFERDESC);
	BufferDesc.dwBufferBytes = m_dwDSBufferSize;
	BufferDesc.guid3DAlgorithm = GUID_NULL;
	if(bGlobalFocus) BufferDesc.dwFlags = DSBCAPS_GLOBALFOCUS;
	else BufferDesc.dwFlags = 0;
	BufferDesc.dwFlags |= DSBCAPS_CTRLVOLUME;
	BufferDesc.dwFlags |= DSBCAPS_CTRLPOSITIONNOTIFY|DSBCAPS_GETCURRENTPOSITION2;
	
	if(FAILED(hr = m_pDirectSound->CreateSoundBuffer(&BufferDesc, &m_pDSBuffer, 0)))
	{
		return FALSE;
	}
	
	if( FAILED( hr = m_pDSBuffer->QueryInterface( IID_IDirectSoundNotify, 
		(VOID**)&pDSNotify ) ) )
	{
		return FALSE;
	}	

	//������СΪm_nNotificationsNum��DSBPOSITIONNOTIFY����
	m_hNotificationEvent = CreateEvent( NULL, FALSE, FALSE, NULL );
	pPosNotify = new DSBPOSITIONNOTIFY[m_nNotificationsNum];
	for(int i=0; i < m_nNotificationsNum; i++)
	{
		pPosNotify[i].dwOffset = m_dwDSBufferSize/m_nNotificationsNum*(i+1) - 1;
		pPosNotify[i].hEventNotify = m_hNotificationEvent;
	}

	// Tell DirectSound when to notify us. The notification will come in the from 
	// of signaled events that are handled in WinMain()
	if( FAILED( hr = pDSNotify->SetNotificationPositions(m_nNotificationsNum, pPosNotify ) ) )
	{
		SAFE_RELEASE(pDSNotify);
		SAFE_DELETE_ARRAY(pPosNotify);
		return FALSE;
	}
	
	SAFE_RELEASE(pDSNotify);
	SAFE_DELETE_ARRAY(pPosNotify);

	//����һ���߳̽���notify��Ϣ
	m_hNotifyThread = CreateThread( NULL, 0, HandleNotificationThread, this, 0, &m_dwNotifyThreadID); 
	return TRUE;
}

//-----------------------------------------------------------------------------
// �ָ�������������
//-----------------------------------------------------------------------------

BOOL DirectSoundPlayer::RestoreBuffer()
{
	HRESULT hr; 
	VOID*   pDSLockedBuffer      = NULL; // Pointer to locked buffer memory
	DWORD   dwDSLockedBufferSize = 0;    // Size of the locked DirectSound buffer
	DWORD   dwWavDataRead        = 0;    // Amount of data read from the wav file 
	//�����������ʧ��Ҫ�ָ�
	DWORD dwStatus;
	if( FAILED( hr = m_pDSBuffer->GetStatus( &dwStatus ) ) )		
	{
		return FALSE;
	}
	if( dwStatus & DSBSTATUS_BUFFERLOST )
	{
		// Since the app could have just been activated, then
		// DirectSound may not be giving us control yet, so 
		// the restoring the buffer may fail.  
		// If it does, sleep until DirectSound gives us control.
		do 
		{
			hr = m_pDSBuffer->Restore();
			if( hr == DSERR_BUFFERLOST )
				Sleep( 10 );
		}
		while( ( hr = m_pDSBuffer->Restore() ) == DSERR_BUFFERLOST );
		

		//�򻺳���д����
		if(FAILED(hr = m_pDSBuffer->Lock(0, m_dwDSBufferSize, &pDSLockedBuffer,
			&dwDSLockedBufferSize,NULL,NULL,0L)))
		{
			return FALSE;
		}
		
		//���ȶ������������ȵ�����
		m_pWaveFile->Read((BYTE *)pDSLockedBuffer,dwDSLockedBufferSize,&dwWavDataRead);
		if( dwWavDataRead == 0 )
		{
			// Wav is blank, so just fill with silence
			FillMemory( (BYTE*) pDSLockedBuffer, 
				dwDSLockedBufferSize, 
				(BYTE)(m_pWaveFile->m_pwfx->wBitsPerSample == 8 ? 128 : 0 ) );
		}		
		else if(dwWavDataRead < dwDSLockedBufferSize)//����ļ�С�ڻ�����,�þ��������ʣ��Ŀռ�
		{
			FillMemory( (BYTE*) pDSLockedBuffer + dwWavDataRead, 
				dwDSLockedBufferSize - dwWavDataRead, 
				(BYTE)(m_pWaveFile->m_pwfx->wBitsPerSample == 8 ? 128 : 0 ) );			
		}
		
		if(FAILED(hr = m_pDSBuffer->Unlock(pDSLockedBuffer,
			dwDSLockedBufferSize,NULL,NULL)))
		{
			return FALSE;
		}
	}
	return TRUE;
	
}

//-----------------------------------------------------------------------------
// ���Ż�����
//-----------------------------------------------------------------------------

BOOL DirectSoundPlayer::PlayBuffer()
{
	HRESULT hr;
	VOID*   pDSLockedBuffer      = NULL; // Pointer to locked buffer memory
	DWORD   dwDSLockedBufferSize = 0;    // Size of the locked DirectSound buffer
	DWORD   dwWavDataRead        = 0;    // Amount of data read from the wav file 

	if(m_pDSBuffer == NULL)
	{
		return FALSE;
	}
	if(m_bPause == TRUE) 
	{
		if(RestoreBuffer() == FALSE)
		{
			return FALSE;
		}
		m_bPause = FALSE;
	}
	else
	{
		m_bFillNextNotificationWithSilence = FALSE;
		m_dwPlayProgress = 0;
		m_dwLastPlayPos = 0;
		m_dwNextWriteOffset = 0;
		
		//�����������ʧ��Ҫ�ָ�
		DWORD dwStatus;
		if( FAILED( hr = m_pDSBuffer->GetStatus( &dwStatus ) ) )		
		{
			return FALSE;
		}
		if( dwStatus & DSBSTATUS_BUFFERLOST )
		{
			// Since the app could have just been activated, then
			// DirectSound may not be giving us control yet, so 
			// the restoring the buffer may fail.  
			// If it does, sleep until DirectSound gives us control.
			do 
			{
				hr = m_pDSBuffer->Restore();
				if( hr == DSERR_BUFFERLOST )
					Sleep( 10 );
			}
			while( ( hr = m_pDSBuffer->Restore() ) == DSERR_BUFFERLOST );
			
		}

		//�򻺳���д����
		m_pDSBuffer->Stop();
		m_pDSBuffer->SetCurrentPosition(0L);	
		if(FAILED(hr = m_pDSBuffer->Lock(0, m_dwDSBufferSize, &pDSLockedBuffer,
			&dwDSLockedBufferSize,NULL,NULL,0L)))
		{
			return FALSE;
		}
		
		// Reset the wave file to the beginning 
		m_pWaveFile->ResetFile();
		//���ȶ������������ȵ�����
		m_pWaveFile->Read((BYTE *)pDSLockedBuffer,dwDSLockedBufferSize,&dwWavDataRead);

		//����ļ�С�ڻ�����,�þ��������ʣ��Ŀռ�
		if(dwWavDataRead < dwDSLockedBufferSize)
		{
			FillMemory( (BYTE*) pDSLockedBuffer + dwWavDataRead, 
				dwDSLockedBufferSize - dwWavDataRead, 
				(BYTE)(m_pWaveFile->m_pwfx->wBitsPerSample == 8 ? 128 : 0 ) );			
		}

		if(FAILED(hr = m_pDSBuffer->Unlock(pDSLockedBuffer,
			dwDSLockedBufferSize,NULL,NULL)))
		{
			return FALSE;
		}
		
	}

	//�����������ٿ�������ʱ����������������֪��ԭ��
	m_pDSBuffer->SetVolume(-10000);
	m_pDSBuffer->SetVolume(m_lVolume);

	SetMute(m_bMute);
	SetVolume(m_lVolume);
	m_pDSBuffer->Play(0, 0, DSBPLAY_LOOPING );
	return TRUE;
}

//-----------------------------------------------------------------------------
// ���ţ�ֹͣ����ͣ��������������
//-----------------------------------------------------------------------------
BOOL DirectSoundPlayer::Play()
{
	return PlayBuffer();
}

BOOL DirectSoundPlayer::Stop()
{
	if(m_pDSBuffer == NULL)
	{
		return FALSE;
	}

	m_bPause = FALSE;
	m_bFillNextNotificationWithSilence =FALSE;
	m_dwPlayProgress = 0;
	m_dwLastPlayPos = 0;
	m_dwNextWriteOffset = 0;
	if(FAILED(m_pDSBuffer->Stop()))
	{
		return FALSE;
	}
	
	if(FAILED(m_pDSBuffer->SetCurrentPosition(0L)))
	{
		return FALSE;
	}
	return TRUE;
}

void DirectSoundPlayer::Pause()
{

	if(m_pDSBuffer == NULL)
	{
		return;
	}

	m_pDSBuffer->Stop();
	m_bPause = TRUE;
}

void DirectSoundPlayer::SetMute(BOOL bMute)
{
	if(m_pDSBuffer == NULL)
	{
		return;
	}

	m_bMute = bMute;
	if(bMute)
	{
		m_pDSBuffer->SetVolume(-10000);
	}
	else
	{
		m_pDSBuffer->SetVolume(m_lVolume);
	}

}

void DirectSoundPlayer::SetVolume(long lVolume)
{
	if(m_pDSBuffer == NULL)
	{
		return;
	}
	
	m_lVolume = lVolume;
	if(m_bMute == FALSE)
	{
		m_pDSBuffer->SetVolume(m_lVolume);
	}
}

BOOL DirectSoundPlayer::IsSoundPlaying()
{
	BOOL bIsPlaying = FALSE;	
	if( m_pDSBuffer == NULL )
		return FALSE; 
	DWORD dwStatus = 0;
	m_pDSBuffer->GetStatus( &dwStatus );
	bIsPlaying |= ( ( dwStatus & DSBSTATUS_PLAYING ) != 0 );
	return bIsPlaying;
}

//-----------------------------------------------------------------------------
// ����֪ͨ�¼��̺߳���
//-----------------------------------------------------------------------------

DWORD WINAPI HandleNotificationThread(void* pParam)
{
	DirectSoundPlayer *pThis =(DirectSoundPlayer *)pParam;
	HRESULT hr;
	MSG     msg;
	DWORD   dwResult;
	DWORD   dwCurrentPlayPos = 0;
	DWORD   dwPlayDelta = 0;
	DWORD   dwBytesWrittenToBuffer = 0;
	VOID*   pDSLockedBuffer = NULL;
	VOID*   pDSLockedBuffer2 = NULL;
	DWORD   dwDSLockedBufferSize = 0;
	DWORD   dwDSLockedBufferSize2 = 0;

	while(TRUE)
	{

		dwResult = MsgWaitForMultipleObjects(1,&(pThis->m_hNotificationEvent),
						     FALSE,INFINITE, QS_ALLEVENTS );
		switch(dwResult)
		{
		case WAIT_OBJECT_0 + 0:

			//�����������ʧ��Ҫ�ָ�
			if(pThis->RestoreBuffer() == FALSE) return 0;

			// Lock the DirectSound buffer
			if( FAILED( hr = pThis->m_pDSBuffer->Lock(
				pThis->m_dwNextWriteOffset, pThis->m_dwNotifySize, 
				&pDSLockedBuffer, &dwDSLockedBufferSize, 
				&pDSLockedBuffer2, &dwDSLockedBufferSize2, 0L ) ) )
			{
				return 0;
			}
			if( pDSLockedBuffer2 != NULL )
			{
				return 0;	
			}
		
			//ÿһС�鲥����󣬾������
			if(pThis->m_bFillNextNotificationWithSilence != TRUE)
			{
				pThis->m_pWaveFile->Read( (BYTE*) pDSLockedBuffer,
						dwDSLockedBufferSize,
						&dwBytesWrittenToBuffer);
			}
			else
			{
				// Fill the DirectSound buffer with silence
				FillMemory( pDSLockedBuffer, dwDSLockedBufferSize, 
					(BYTE)( pThis->m_pWaveFile->m_pwfx->wBitsPerSample == 8 ? 128 : 0 ) );
				dwBytesWrittenToBuffer = dwDSLockedBufferSize;			
			}

			// If the number of bytes written is less than the 
			// amount we requested, we have a short file.
			if( dwBytesWrittenToBuffer < dwDSLockedBufferSize )
			{
				// Fill in silence for the rest of the buffer.
				FillMemory( (BYTE*) pDSLockedBuffer + dwBytesWrittenToBuffer, 
					dwDSLockedBufferSize - dwBytesWrittenToBuffer, 
					(BYTE)(pThis->m_pWaveFile->m_pwfx->wBitsPerSample == 8 ? 128 : 0 ) );
					
				// Any future notifications should just fill the buffer with silence
				pThis->m_bFillNextNotificationWithSilence = TRUE;
			}

			// unlock the DirectSound buffer
			if( FAILED( hr = pThis->m_pDSBuffer->Unlock( 
				pDSLockedBuffer, dwDSLockedBufferSize, 
				NULL, 0L) ) )
			{
				return 0;
			}
			
			if( FAILED( hr = pThis->m_pDSBuffer->GetCurrentPosition( &dwCurrentPlayPos, NULL ) ))
			{
				return 0;
			}
			
			// Check to see if the position counter looped
			if( dwCurrentPlayPos < pThis->m_dwLastPlayPos )
				dwPlayDelta = ( pThis->m_dwDSBufferSize - pThis->m_dwLastPlayPos ) + dwCurrentPlayPos;
			else
				dwPlayDelta = dwCurrentPlayPos - pThis->m_dwLastPlayPos;
			
			pThis->m_dwPlayProgress += dwPlayDelta;
			pThis->m_dwLastPlayPos = dwCurrentPlayPos;
						
			if(pThis->m_dwPlayProgress > pThis->m_pWaveFile->GetSize())
			{
				hr=pThis->m_pDSBuffer->Stop();
				pThis->m_dwPlayProgress = 0;
				pThis->m_dwLastPlayPos = 0;
			}
			
			// Update where the buffer will lock (for next time)
			pThis->m_dwNextWriteOffset += dwDSLockedBufferSize; 
			pThis->m_dwNextWriteOffset %= pThis->m_dwDSBufferSize; // Circular buffer

		case WAIT_OBJECT_0 + 1:
			// Messages are available
			while( PeekMessage( &msg, NULL, 0, 0, PM_REMOVE ) ) 
			{ 
				if( msg.message == WM_QUIT ) return 0;
			}
			break;
		}
	}
	
	return 0;
}