#pragma once

#include "DX\dsound.h"
#include "WaveFile.h"

class DirectSoundPlayer  
{
public:
	DirectSoundPlayer();
	virtual ~DirectSoundPlayer();

	void SetWaveFileName(const char* strFileName);
	void GetWaveFileName(char* strFileName);
	BOOL CheckWaveFile();
	
	//���ò��Ż������ĳ��ȣ�����Ϊ��λ,��������ã�Ĭ�ϵ���3��
	void SetBufferLength(int nSecond){m_nBufferPlayLength = nSecond;}
	DWORD GetBufferLength(){return m_nBufferPlayLength;}
	//���øó��ȵĲ��Ż�������֪ͨ�¼�����,��������ã�Ĭ�ϵ���16��
	void SetNotificationsNum(int nCount){m_nNotificationsNum = nCount;}
	DWORD GetNotificationsNum(){return m_nNotificationsNum;}

	BOOL Initialize(HWND hWnd, BOOL bGlobalFocus);//bGlobalFocus���ΪTRUE����ʧȥ����ʱ��Ȼ����

	BOOL LoadCompleted(){return !this->m_pWaveFile->m_bIsReadingFromMemory;}

	BOOL Play();
	void Pause();
	BOOL Stop();
	void SetMute(BOOL bMute);
	void SetVolume(long lVolume);

	BOOL IsSoundPlaying();
	int  GetVolume(){return m_lVolume;}
	BOOL GetMute(){return m_bMute;}
	BOOL GetPause(){return m_bPause;}
private:
	BOOL PlayBuffer();
	BOOL RestoreBuffer();
	//�������Ԫ�̺߳������Ա���ʸ���˽������
	friend DWORD WINAPI HandleNotificationThread(void* pParam);
private:
	char m_strWaveFileName[256];//�ļ���
	WaveFile *m_pWaveFile;

	int m_nBufferPlayLength;//�������ĳ���(��λΪ��)
	int m_nNotificationsNum;//�û�������֪ͨ����
	
	LPDIRECTSOUND m_pDirectSound;
	LPDIRECTSOUNDBUFFER m_pDSBuffer;
	DWORD m_dwDSBufferSize;
	DWORD m_dwNotifySize;

	long m_lVolume;
	BOOL m_bPause;
	BOOL m_bMute;	

	DWORD m_dwNotifyThreadID;
	HANDLE m_hNotifyThread;
	HANDLE m_hNotificationEvent;

	//����HandleNotificationThread��صĲ���
	BOOL m_bFillNextNotificationWithSilence;
	DWORD m_dwPlayProgress;
	DWORD m_dwLastPlayPos;
	DWORD m_dwNextWriteOffset;
};