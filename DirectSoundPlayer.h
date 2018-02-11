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
	
	//设置播放缓冲区的长度，以秒为单位,如果不设置，默认的是3秒
	void SetBufferLength(int nSecond){m_nBufferPlayLength = nSecond;}
	DWORD GetBufferLength(){return m_nBufferPlayLength;}
	//设置该长度的播放缓冲区的通知事件数量,如果不设置，默认的是16个
	void SetNotificationsNum(int nCount){m_nNotificationsNum = nCount;}
	DWORD GetNotificationsNum(){return m_nNotificationsNum;}

	BOOL Initialize(HWND hWnd, BOOL bGlobalFocus);//bGlobalFocus如果为TRUE，在失去焦点时仍然播放

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
	//定义成友元线程函数，以便访问该类私有数据
	friend DWORD WINAPI HandleNotificationThread(void* pParam);
private:
	char m_strWaveFileName[256];//文件名
	WaveFile *m_pWaveFile;

	int m_nBufferPlayLength;//缓冲区的长度(单位为秒)
	int m_nNotificationsNum;//该缓冲区的通知个数
	
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

	//处理HandleNotificationThread相关的参数
	BOOL m_bFillNextNotificationWithSilence;
	DWORD m_dwPlayProgress;
	DWORD m_dwLastPlayPos;
	DWORD m_dwNextWriteOffset;
};