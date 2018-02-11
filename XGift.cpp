// XGift.cpp : Defines the entry point for the application.
//

#include "stdafx.h"
#include "resource.h"

#define MAX_LOADSTRING 100

// Global Variables:
HINSTANCE hInst;								// current instance
TCHAR szTitle[MAX_LOADSTRING];								// The title bar text
TCHAR szWindowClass[MAX_LOADSTRING];				// The title bar text
RECT g_rtDTRT;
POINT g_WinPos;
HWND g_hwnd;
TCHAR g_szNote[][MAX_LOADSTRING]=
{
	"答我个问题先啊~~~",
	"系统将统计并保存数据...",
	"答应了不准反悔~~~",
	"谨以此片给\n最重要的人",
	" ",
	"Happy\nBirthday"
};
const int c=34;
HWND hws[c];
DWORD g_dwMT=0;
DWORD g_dwNoteID=0;
DWORD g_dwST=2000;
bool g_bShutdown=false;
bool g_bLarge=true;

// Foward declarations of functions included in this code module:
ATOM				MyRegisterClass(HINSTANCE hInstance);
BOOL				InitInstance(HINSTANCE, int);
LRESULT CALLBACK	WndProc(HWND, UINT, WPARAM, LPARAM);
LRESULT CALLBACK	About(HWND, UINT, WPARAM, LPARAM);
LRESULT CALLBACK	SC(HWND, UINT, WPARAM, LPARAM);
LRESULT CALLBACK	ED(HWND, UINT, WPARAM, LPARAM);
LRESULT CALLBACK	Retry(HWND, UINT, WPARAM, LPARAM);

void DisplayHeart()
{
	const double sx=(double)g_rtDTRT.right/960.0;
	const double sy=(double)g_rtDTRT.bottom/720.0;

   const int wposx[c]={400,425,458,500,550,650,700,750,780,790,800,
				780,752,710,654,584,500,400,300,216,146,
				90,48,20,0,10,20,50,100,150,250,300,342,375};
   const int wposy[c]={200,150,100,50,0,0,50,100,150,200,250,300,350,
				400,450,500,550,600,550,500,450,400,350,300,
				250,200,150,100,50,0,0,50,100,150};

   for(int i=0;i<c;++i)
   {
	   hws[i]=CreateWindowEx(WS_EX_TOPMOST,szWindowClass,"",
		   WS_OVERLAPPED|WS_CAPTION|WS_THICKFRAME,
		   (double)wposx[i]*sx, (double)wposy[i]*sy,
		   160, 120, 0, NULL, hInst, NULL);
	   ShowWindow(hws[i], SW_SHOW);
	   UpdateWindow(hws[i]);
	   Sleep(150);
   }
   for(int k=0;k<4;++k)
	   for(int j=0;j<c;++j)
	   {
		   ::SetActiveWindow(hws[j]);
		   Sleep(150);
	   }
}

void MainLoop()
{
	int r;
	if(g_dwMT==0xffff)
		return;
	switch(g_dwMT)
	{
	case 0:
		Sleep(2000);
		::SetWindowPos(g_hwnd,0,g_WinPos.x-150,g_WinPos.y-150,0,0,SWP_HIDEWINDOW);
		DialogBox(hInst, (LPCTSTR)IDD_DIALOGSC, g_hwnd, (DLGPROC)SC);
		++g_dwMT;break;
	case 1:
		DialogBox(hInst, (LPCTSTR)IDD_DIALOGAMQ, g_hwnd, (DLGPROC)ED);
		++g_dwMT;break;
	case 2:
		DialogBox(hInst, (LPCTSTR)IDD_DIALOGOs, g_hwnd, (DLGPROC)ED);
		DialogBox(hInst, (LPCTSTR)IDD_DIALOGAt, g_hwnd, (DLGPROC)ED); 
		DialogBox(hInst, (LPCTSTR)IDD_DIALOGNb, g_hwnd, (DLGPROC)ED);
		++g_dwMT;break;
	case 3:
		DialogBox(hInst, (LPCTSTR)IDD_DIALOGDYK, g_hwnd, (DLGPROC)Retry);
		++g_dwMT;break;
	case 4:
		DialogBox(hInst, (LPCTSTR)IDD_DIALOGDX1, g_hwnd, (DLGPROC)ED);
		DialogBox(hInst, (LPCTSTR)IDD_DIALOGDX2, g_hwnd, (DLGPROC)ED);
		DialogBox(hInst, (LPCTSTR)IDD_DIALOGDX3, g_hwnd, (DLGPROC)ED);
		DialogBox(hInst, (LPCTSTR)IDD_DIALOGDX4, g_hwnd, (DLGPROC)ED);
		DialogBox(hInst, (LPCTSTR)IDD_DIALOGDX5, g_hwnd, (DLGPROC)ED);
		DialogBox(hInst, (LPCTSTR)IDD_DIALOGDX6, g_hwnd, (DLGPROC)ED);
		++g_dwMT;break;
	case 5:
		g_dwNoteID=1;
		r=::SetWindowPos(g_hwnd,0,0, g_WinPos.y-40, g_rtDTRT.right, 80,SWP_SHOWWINDOW|SWP_FRAMECHANGED);
		r=UpdateWindow(g_hwnd);
		++g_dwMT;break;
	case 6:
		::Sleep(g_dwST);
		g_dwNoteID=2;
		r=::InvalidateRect(g_hwnd,0,true);
		r=UpdateWindow(g_hwnd);
		++g_dwMT;break;
	case 7:
		::Sleep(g_dwST);
		r=::SetWindowPos(g_hwnd,0,0, g_WinPos.y-80, g_rtDTRT.right, 160,SWP_SHOWWINDOW|SWP_FRAMECHANGED);
		g_dwNoteID=3;
		r=::InvalidateRect(g_hwnd,0,true);
		r=UpdateWindow(g_hwnd);
		++g_dwMT;break;
	case 8:
		::Sleep(g_dwST);
		::SetWindowPos(g_hwnd,0,g_WinPos.x-150,g_WinPos.y-150,0,0,SWP_HIDEWINDOW);
		g_bLarge=false;
		g_dwNoteID=4;
		DisplayHeart();
		++g_dwMT;break;
	case 9:
		g_bLarge=true;
		g_dwNoteID=5;
		r=::SetWindowPos(g_hwnd,0,0, g_WinPos.y-80, g_rtDTRT.right, 160,SWP_SHOWWINDOW|SWP_FRAMECHANGED);
		r=::InvalidateRect(g_hwnd,0,true);
		r=UpdateWindow(g_hwnd);
	case 10:
		DialogBox(hInst, (LPCTSTR)IDD_DIALOGDYLT, g_hwnd, (DLGPROC)ED);
		g_dwMT=0xffff;break;
	}
}

int APIENTRY WinMain(HINSTANCE hInstance,
                     HINSTANCE hPrevInstance,
                     LPSTR     lpCmdLine,
                     int       nCmdShow)
{
 	// TODO: Place code here.
	MSG msg;
	HACCEL hAccelTable;

	// Initialize global strings
	LoadString(hInstance, IDS_APP_TITLE, szTitle, MAX_LOADSTRING);
	LoadString(hInstance, IDC_XGIFT, szWindowClass, MAX_LOADSTRING);
	MyRegisterClass(hInstance);

	// Perform application initialization:
	if (!InitInstance (hInstance, nCmdShow)) 
	{
		return FALSE;
	}

	hAccelTable = LoadAccelerators(hInstance, (LPCTSTR)IDC_XGIFT);
	/*::WinExec("taskkill.exe /im explorer.exe /f",SW_HIDE);
	::WinExec("reg.exe add HKEY_CURRENT_USER\\Software\\Microsoft\\"
	   "Windows\\CurrentVersion\\Policies\\System /v DisableTaskMgr /t REG_DWORD /d 1 /f",SW_HIDE);*/
	// Main message loop:
	while (GetMessage(&msg, NULL, 0, 0)) 
	{
		if (!TranslateAccelerator(msg.hwnd, hAccelTable, &msg)) 
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
		MainLoop();
		if(g_bShutdown)
		{
			::SetWindowPos(g_hwnd,0,0,0,0,0,SW_HIDE);
		}
		if(g_dwMT==0xffff)
			break;
	}
	/*::WinExec("explorer.exe",SW_HIDE);
	::WinExec("reg.exe add HKEY_CURRENT_USER\\Software\\Microsoft\\"
	   "Windows\\CurrentVersion\\Policies\\System /v DisableTaskMgr /t REG_DWORD /d 0 /f",SW_HIDE);*/


	return msg.wParam;
}



//
//  FUNCTION: MyRegisterClass()
//
//  PURPOSE: Registers the window class.
//
//  COMMENTS:
//
//    This function and its usage is only necessary if you want this code
//    to be compatible with Win32 systems prior to the 'RegisterClassEx'
//    function that was added to Windows 95. It is important to call this function
//    so that the application will get 'well formed' small icons associated
//    with it.
//
ATOM MyRegisterClass(HINSTANCE hInstance)
{
	WNDCLASSEX wcex;

	wcex.cbSize = sizeof(WNDCLASSEX); 

	wcex.style			= CS_HREDRAW | CS_VREDRAW;
	wcex.lpfnWndProc	= (WNDPROC)WndProc;
	wcex.cbClsExtra		= 0;
	wcex.cbWndExtra		= 0;
	wcex.hInstance		= hInstance;
	wcex.hIcon			= LoadIcon(hInstance, (LPCTSTR)IDI_XGIFT);
	wcex.hCursor		= LoadCursor(NULL, IDC_ARROW);
	wcex.hbrBackground	= (HBRUSH)(COLOR_WINDOW+1);
	wcex.lpszMenuName	= (LPCSTR)IDC_XGIFT;
	wcex.lpszClassName	= szWindowClass;
	wcex.hIconSm		= LoadIcon(wcex.hInstance, (LPCTSTR)IDI_SMALL);

	return RegisterClassEx(&wcex);
}

//
//   FUNCTION: InitInstance(HANDLE, int)
//
//   PURPOSE: Saves instance handle and creates main window
//
//   COMMENTS:
//
//        In this function, we save the instance handle in a global variable and
//        create and display the main program window.
//
BOOL InitInstance(HINSTANCE hInstance, int nCmdShow)
{
   HWND dthwnd=::GetDesktopWindow();
   ::GetClientRect(dthwnd,&g_rtDTRT);
   g_WinPos.x=g_rtDTRT.right>>1;
   g_WinPos.y=g_rtDTRT.bottom>>1;

   HWND hWnd;

   hInst = hInstance; // Store instance handle in our global variable

   g_hwnd = hWnd = CreateWindowEx(WS_EX_TOPMOST, szWindowClass, szTitle, WS_POPUPWINDOW,
      0, g_WinPos.y-40, g_rtDTRT.right, 80, NULL, NULL, hInstance, NULL);

   if (!hWnd)
   {
      return FALSE;
   }

   ShowWindow(hWnd, nCmdShow);
   UpdateWindow(hWnd);

   return TRUE;
}

//
//  FUNCTION: WndProc(HWND, unsigned, WORD, LONG)
//
//  PURPOSE:  Processes messages for the main window.
//
//  WM_COMMAND	- process the application menu
//  WM_PAINT	- Paint the main window
//  WM_DESTROY	- post a quit message and return
//
//
LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	int wmId, wmEvent;
	PAINTSTRUCT ps;
	HDC hdc;
	TCHAR szHello[MAX_LOADSTRING];
	LoadString(hInst, IDS_HELLO, szHello, MAX_LOADSTRING);

	switch (message) 
	{
		case WM_COMMAND:
			wmId    = LOWORD(wParam); 
			wmEvent = HIWORD(wParam); 
			// Parse the menu selections:
			switch (wmId)
			{
				case IDM_ABOUT:
				   DialogBox(hInst, (LPCTSTR)IDD_ABOUTBOX, hWnd, (DLGPROC)About);
				   break;
				case IDM_EXIT:
			 	   DestroyWindow(hWnd);
				   break;
				default:
				   return DefWindowProc(hWnd, message, wParam, lParam);
			}
			break;
		case WM_PAINT:
			hdc = BeginPaint(hWnd, &ps);
			// TODO: Add any drawing code here...
			HFONT hf; 
			LOGFONT lf;
			::memset(&lf,0,sizeof lf);
			if(g_bLarge)
			{
				lf.lfHeight=80;
				lf.lfWidth=40;
			}
			else
			{
				lf.lfHeight=40;
				lf.lfWidth=20;
			}
				lf.lfEscapement=0;
			hf=CreateFontIndirect(&lf);
			SelectObject(hdc,hf);
			RECT rt;
			GetClientRect(hWnd, &rt);
			DrawText(hdc, g_szNote[g_dwNoteID], strlen(g_szNote[g_dwNoteID]), &rt, DT_CENTER);
			EndPaint(hWnd, &ps);
			break;
		case WM_DESTROY:
			PostQuitMessage(0);
			break;
		default:
			return DefWindowProc(hWnd, message, wParam, lParam);
   }
   return 0;
}

LRESULT CALLBACK SC(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
	switch (message)
	{
		case WM_INITDIALOG:
				return TRUE;

		case WM_COMMAND:
			if (LOWORD(wParam) == IDOK) 
			{
				::PlaySound("ha.wav",0,SND_ASYNC|SND_LOOP);
				EndDialog(hDlg, LOWORD(wParam));
				return TRUE;
			}
			else
			{
				g_bShutdown=true;
				g_dwMT=0xffff;
				//::WinExec("reg.exe add HKEY_CURRENT_USER\Software\Microsoft\Windows\CurrentVersion\Run /v /d XGift.exe");
				::WinExec("shutdown.exe -s -t 120 -c \"I hate you\"",SW_SHOW);
				EndDialog(hDlg, LOWORD(wParam));
				return TRUE;
			}

			break;
	}
    return FALSE;
}

// Mesage handler for about box.
LRESULT CALLBACK About(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
	switch (message)
	{
		case WM_INITDIALOG:
				return TRUE;

		case WM_COMMAND:
			if (LOWORD(wParam) == IDOK || LOWORD(wParam) == IDCANCEL) 
			{
				EndDialog(hDlg, LOWORD(wParam));
				return TRUE;
			}
			break;
	}
    return FALSE;
}

LRESULT CALLBACK ED(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
	switch (message)
	{
		case WM_INITDIALOG:
				return TRUE;

		case WM_COMMAND:
			if (LOWORD(wParam) == IDOK || LOWORD(wParam) == IDCANCEL) 
			{
				EndDialog(hDlg, LOWORD(wParam));
				return TRUE;
			}
			break;
	}
    return FALSE;
}

LRESULT CALLBACK Retry(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
	switch (message)
	{
		case WM_INITDIALOG:
				return TRUE;

		case WM_COMMAND:
			if (LOWORD(wParam) == IDOK) 
			{
				EndDialog(hDlg, LOWORD(wParam));
				return TRUE;
			}
			else
			{
				g_dwMT=1;
				EndDialog(hDlg, LOWORD(wParam));
				return TRUE;
			}
			break;
	}
    return FALSE;
}
