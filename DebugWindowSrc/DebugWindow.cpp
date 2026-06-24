//
// Debug window support
//
#define	WIN32_LEAN_AND_MEAN
#include <windows.h>

#include "resource.h"

HINSTANCE	hAppInstance;
HFONT		hFont = NULL;

LOGFONT		logFont={ 12, 0, 0, 0, 0, FALSE, FALSE, FALSE, SHIFTJIS_CHARSET, OUT_DEFAULT_PRECIS,
			CLIP_DEFAULT_PRECIS, DEFAULT_QUALITY, DEFAULT_PITCH, "ＭＳ ゴシック" };

//LOGFONT		logFont={ 12, 0, 0, 0, 0, FALSE, FALSE, FALSE, SHIFTJIS_CHARSET, OUT_DEFAULT_PRECIS,
//			CLIP_DEFAULT_PRECIS, DEFAULT_QUALITY, DEFAULT_PITCH, "MS Gothic" };

//LOGFONT		logFont={ 14, 0, 0, 0, 0, FALSE, FALSE, FALSE, SHIFTJIS_CHARSET, OUT_DEFAULT_PRECIS,
//			CLIP_DEFAULT_PRECIS, DEFAULT_QUALITY, DEFAULT_PITCH, "Terminal" };

const CHAR	*pszTaskBar = "Shell_TrayWnd";

// Window procedure
LRESULT CALLBACK WndProc( HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam )
{ 
	switch ( msg ) {
		case	WM_CREATE:
			{
			if( !(hFont = CreateFontIndirect( &logFont )) ) {
				OutputDebugString( "CreateFontIndirect faild.\n" );
				return	-1;
			}

			// エディットコントロールの作成
			RECT	rc;
			GetClientRect( hWnd, &rc );
			HWND hWndEdit = CreateWindow(
					"EDIT",
					"",
					WS_CHILD|WS_VSCROLL|WS_HSCROLL|WS_VISIBLE|ES_MULTILINE|ES_AUTOVSCROLL|ES_AUTOHSCROLL|ES_READONLY,
					rc.left,
					rc.top,
					rc.right-rc.left,
					rc.bottom-rc.top,
					hWnd,
					(HMENU)IDC_EDITBOX,
					hAppInstance,
					NULL );

			if( !hWndEdit ) {
				OutputDebugString( "CreateWindow faild.\n" );
				return	-1;
			}
			SendMessage( hWndEdit, EM_LIMITTEXT, (WPARAM)0x40000, 0 );	// 256KB
			SendMessage( hWndEdit, WM_SETFONT, (WPARAM)hFont, MAKELPARAM(TRUE,0) );

			// 右下に張り付く
			HWND	hWndTaskBar = FindWindow( pszTaskBar, NULL );
			RECT	rcW, rcTaskBar;
			::GetWindowRect( hWnd, &rcW );
			INT	LimitX, LimitY;
			if( hWndTaskBar ) {
				INT	DesktopWidth  = GetSystemMetrics( SM_CXFULLSCREEN );
				INT	DesktopHeight = GetSystemMetrics( SM_CYFULLSCREEN );
				LimitX = GetSystemMetrics( SM_CXSCREEN );
				LimitY = GetSystemMetrics( SM_CYSCREEN );
				::GetWindowRect( hWndTaskBar, &rcTaskBar );
				if( (rcTaskBar.left < DesktopWidth/2) && (rcTaskBar.right < DesktopWidth/2) ) {
				// タスクバーが左側
				} else
				if( (rcTaskBar.left >= DesktopWidth/2) && (rcTaskBar.right >= DesktopWidth/2) ) {
				// タスクバーが右側
					LimitX = rcTaskBar.left-1;
				} else
				if( (rcTaskBar.top < DesktopHeight/2) && (rcTaskBar.bottom < DesktopHeight/2) ) {
				// タスクバーが上側

				} else
				if( (rcTaskBar.top >= DesktopHeight/2) && (rcTaskBar.bottom >= DesktopHeight/2) ) {
				// タスクバーが下側
					LimitY = rcTaskBar.top-1;
				}
			} else {
				LimitX = GetSystemMetrics( SM_CXSCREEN );
				LimitY = GetSystemMetrics( SM_CYSCREEN );
			}
			SetWindowPos( hWnd, NULL, LimitX-(rcW.right-rcW.left), LimitY-(rcW.bottom-rcW.top), 0, 0, SWP_NOSIZE|SWP_NOZORDER );
			}
			break;
		case	WM_DESTROY:
			if( hFont ) {
				DeleteObject( hFont );
			}
			break;

		case	WM_CLOSE:
			PostQuitMessage( 0 );
			return	0;
		case	WM_SIZE:
			MoveWindow( GetDlgItem( hWnd, IDC_EDITBOX ), 0, 0, LOWORD(lParam), HIWORD(lParam), TRUE );
			break;

		case	WM_COMMAND:
			switch( LOWORD(wParam) ) {
				case	ID_CLEAR:
					SendDlgItemMessage( hWnd, IDC_EDITBOX, EM_SETSEL, 0, -1 );
					SendDlgItemMessage( hWnd, IDC_EDITBOX, EM_REPLACESEL, FALSE, (WPARAM)"" );
					return	0L;
				default:
					break;
			}
			break;

		// 送られてきた文字データをエディットコントロールに送る
		case	WM_COPYDATA:
			{
			COPYDATASTRUCT* pcds = (COPYDATASTRUCT*)lParam;
			CHAR*	lpStr = (CHAR*)pcds->lpData;

			INT	src, dst;
			CHAR	szTextBuffer[1024];
			src = dst = 0;
			while( lpStr[src] != '\0' ) {
				if( lpStr[src] == '\n' ) {
					szTextBuffer[dst++] = '\r';
					szTextBuffer[dst++] = '\n';
				} else {
					szTextBuffer[dst++] = lpStr[src];
				}
				src++;
			}
			szTextBuffer[dst] = '\0';

			INT n = GetWindowTextLength( GetDlgItem( hWnd, IDC_EDITBOX ) );
			SendDlgItemMessage( hWnd, IDC_EDITBOX, EM_SETSEL, (WPARAM)n, (LPARAM)n );
			SendDlgItemMessage( hWnd, IDC_EDITBOX, EM_REPLACESEL, (WPARAM)TRUE, (LPARAM)szTextBuffer );
			}
			return	0L;

		// Read onlyなEDITコントロールはWM_CTLCOLORSTATICが呼ばれる(試したらこうだった:p)
		case	WM_CTLCOLORSTATIC:
			SetBkColor( (HDC)wParam, (COLORREF)0x00FFFFFF );
			return	(LRESULT)GetStockObject( WHITE_BRUSH );

		case	WM_APP+1:
			SendDlgItemMessage( hWnd, IDC_EDITBOX, EM_SETSEL, 0, -1 );
			SendDlgItemMessage( hWnd, IDC_EDITBOX, EM_REPLACESEL, FALSE, (WPARAM)"" );
			return	0L;

		default:
			break;
	}
	return	DefWindowProc( hWnd, msg, wParam, lParam );
}

static const CHAR szClassName[] = "DebugWindow_wndclass";
static const CHAR szWindowName[] = "DebugWindow";
static const CHAR szMutexName[] = "DebugWindow_mutex";

INT WINAPI WinMain( HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, INT nShowCmd )
{
	// 二重起動の防止
	HANDLE hMutex = CreateMutex( NULL, FALSE, szMutexName );
	if( GetLastError() == ERROR_ALREADY_EXISTS ) {
		CloseHandle( hMutex );
		return	-1;
	}

	hAppInstance = hInstance;

	WNDCLASSEX	wcl;
	ZeroMemory( &wcl, sizeof(wcl) );
	wcl.cbSize		= sizeof(wcl);
	wcl.lpszClassName	= szClassName;
	wcl.lpfnWndProc		= WndProc;
	wcl.style		= CS_HREDRAW | CS_VREDRAW | CS_DBLCLKS;
	wcl.cbClsExtra		= wcl.cbWndExtra = 0;
	wcl.hInstance		= hInstance;
	wcl.lpszMenuName	= MAKEINTRESOURCE(IDR_MENU);
	wcl.hIcon		= 
	wcl.hIconSm		= ::LoadIcon( hInstance, MAKEINTRESOURCE(IDI_ICON) );
//	wcl.hIconSm		= NULL;
	wcl.hCursor		= ::LoadCursor( NULL, IDC_ARROW );
	wcl.hbrBackground	= 0;

	if( !RegisterClassEx( &wcl ) ) {
		OutputDebugString( "RegisterClassEx faild.\n" );
		ReleaseMutex( hMutex );
		CloseHandle( hMutex );
		return	-1;
	}

	HWND hWnd = CreateWindowEx(
			WS_EX_CLIENTEDGE,
			szClassName,
			szWindowName,
			WS_OVERLAPPEDWINDOW,
			CW_USEDEFAULT,
			CW_USEDEFAULT,
			420,
			320,
			NULL,
			NULL,
			hInstance,
			NULL );

	if( !hWnd ) {
		OutputDebugString( "CreateWindow faild.\n" );
		ReleaseMutex( hMutex );
		CloseHandle( hMutex );
		return	-1;
	}

	ShowWindow( hWnd, nShowCmd );
	UpdateWindow( hWnd );

	MSG	msg;
	while( GetMessage( &msg, NULL, 0, 0 ) ) {
		TranslateMessage( &msg );
		DispatchMessage( &msg );
	}

	ReleaseMutex( hMutex );
	CloseHandle( hMutex );

	return	msg.wParam;
}

