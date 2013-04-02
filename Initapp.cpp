//
// Initapp.cpp
//
// Declares main app globals and the following functions:
//
//            GetPathFromFullPathName()
//            InitApplication()
//            InitInstance()
//            InitializePane1()
//            InitializePane2()
//
//
#ifndef STRICT
#define STRICT 1
#endif

#include <windows.h>
#include <commctrl.h>
#include <mmsystem.h>
#include <stdio.h>
#include <commdlg.h>
#include <string.h>
#include <time.h>

#include "headers\resource.h"
#include "headers\PDW.h"
#include "headers\slicer.h"
#include "headers\toolbar.h"
#include "headers\gfx.h"
#include "headers\initapp.h"

#include "headers\helper_funcs.h"

PaneStruct Pane1;
PaneStruct Pane2;

HINSTANCE ghInstance;       // current instance
HWND    ghWnd;              // main window handle
HWND    hToolbar=NULL;      // Toolbar handle

HACCEL  ghAccel;
HMENU   ghMenu=NULL;        // Main application menu.
TCHAR   szAppName[128];     // name of the application (PH: was 64)
TCHAR   szShortAppName[32]; // short name of the application
TCHAR   szApiFailedMsg[64]; // "A Windows API Failed" message

FILE *pLogFile       = NULL; // Logged messages
FILE *pFilterFile    = NULL; // Filtered messages
FILE *pSepFilterFile = NULL; // Separate filtered messages
FILE *pStatFile      = NULL; // Statistics
FILE *pFiltersFile   = NULL; // PH: Filters.ini

unsigned int cxChar, cyChar, cxCaps;  // all windows use the same font size
int iMaxWidth;
int sizeSet = 0;
int pane1Height, pane2Height, pane1Pos, pane2Pos, pane1Top;

char  gszPDWClass[15] = "WinPDWWndClass";
char  gszPane1Class[17] = "WinPDWPane1Class";
char  gszPane2Class[17] = "WinPDWPane2Class";
char  gszPane2LabelClass[22] = "WinPDWPane2LabelClass";
char  gszColorClass[17] = "WinPDWColorClass";
char  gszACARSColorClass[19] = "WinACARSColorClass";
char  gszMOBITEXColorClass[21] = "WinMOBITEXColorClass";
char  gszERMESColorClass[19] = "WinERMESColorClass";

LPCTSTR lpszSourceFileName = TEXT(__FILE__);

TCHAR szDialogErrorMsg[80];
TCHAR szCenterOpenDlgMsg[80];

TCHAR szPath[MAX_PATH];          // path where the running application resides
TCHAR szLogPathName[MAX_PATH];   // Logfile-path
TCHAR szWavePathName[MAX_PATH];  // Wavefile-path
TCHAR szExePathName[MAX_PATH];   // full pathname of the application
TCHAR szHelpPathName[MAX_PATH];  // full pathname of the application's help file
TCHAR szIniPathName[MAX_PATH];   // full pathname of the application's ini file
TCHAR szFilterPathName[MAX_PATH];// full pathname of the application's filtersfile
TCHAR szFilterBackup[MAX_PATH];  // full pathname of the application's filters backupfile


UINT GetPathFromFullPathName(LPCTSTR lpFullPathName, LPTSTR lpPathBuffer,
                              UINT nPathBufferLength)
{
   UINT nLength;
   int i;

   if ((nLength = (UINT) lstrlen(lpFullPathName)) > nPathBufferLength) return(nLength);

   lstrcpy(lpPathBuffer, lpFullPathName);

   for(i = lstrlen(lpPathBuffer);(lpPathBuffer[i] != '\\') && (lpPathBuffer[i] != ':'); i--);
   if (':' == lpPathBuffer[i]) lpPathBuffer[i+1] = '\0';
   else								 lpPathBuffer[i] = '\0';

   return((UINT) i);
}

BOOL NEAR InitApplication(HINSTANCE hInstance)
{
	WNDCLASS  wndclass;

	LPCTSTR lpszIniFileExt  = TEXT("INI");
	LPCTSTR lpszHelpFileExt = TEXT("HLP");
	TCHAR szApiFailedMsg[]	= TEXT("A Windows API Failed");

	//-- Load the "A Windows API Failed" resource string

	if (!LoadString(ghInstance, IDS_API_FAILED_MSG, szApiFailedMsg, sizeof(szApiFailedMsg)))
		 ErrorMessageBox(TEXT("1 LoadString()"), szApiFailedMsg, lpszSourceFileName, __LINE__);

	//-- Load resource strings
	if (!LoadString(ghInstance, IDS_APPNAME, szAppName, sizeof(szAppName)))
		 ErrorMessageBox(TEXT("2 LoadString()"), szApiFailedMsg, lpszSourceFileName, __LINE__);
	if (!LoadString(ghInstance, IDS_SHORT_APPNAME, szShortAppName, sizeof(szShortAppName)))
		 ErrorMessageBox(TEXT("3 LoadString()"), szApiFailedMsg, lpszSourceFileName, __LINE__);

	//-- get application pathname and store the ini and help file pathname
	//   (which is located in the same directory as the application)
	GetModuleFileName((HINSTANCE) NULL, szExePathName, sizeof(szExePathName)/sizeof(TCHAR));
	GetPathFromFullPathName(szExePathName, szPath, sizeof(szPath)/sizeof(TCHAR));

	wsprintf(szLogPathName,   TEXT("%s\\%s"),		szPath, "Logfiles");
	wsprintf(szWavePathName,  TEXT("%s\\%s"),		szPath, "Wavfiles");
	wsprintf(szIniPathName,   TEXT("%s\\%s.%s"),	szPath, szShortAppName, lpszIniFileExt);
	wsprintf(szHelpPathName,  TEXT("%s\\%s.%s"),	szPath, szShortAppName, lpszHelpFileExt);
	wsprintf(szFilterPathName,TEXT("%s\\%s"),		szPath, "filters.ini");
	wsprintf(szFilterBackup,  TEXT("%s\\%s"),		szPath, "filters.bak");

	if (!FileExists(szWavePathName)) CreateDirectory(szWavePathName, NULL);

	GetPrivateProfileSettings(szShortAppName, szIniPathName, &Profile);

	// After getting user profile settings get all drawing objects(gfx.cpp)
	if (!(Get_Drawing_Objects())) return(FALSE);

	// register window class

	wndclass.style			= 0;
	wndclass.lpfnWndProc	= PDWWndProc;
	wndclass.cbClsExtra		= 0;
	wndclass.cbWndExtra		= sizeof(LONG);
	wndclass.hInstance		= hInstance;
	wndclass.hIcon			= LoadIcon(hInstance, MAKEINTRESOURCE(PDWICON));
	wndclass.hCursor		= LoadCursor(NULL, IDC_ARROW);
	wndclass.hbrBackground	= hbr;  // see gfx.cpp
	wndclass.lpszMenuName	= MAKEINTRESOURCE(PDWMENU);
	wndclass.lpszClassName	= gszPDWClass;

	if (!RegisterClass(&wndclass)) return (FALSE);

	wndclass.style			= 0;
	wndclass.lpfnWndProc	= Pane1WndProc;
	wndclass.cbClsExtra		= 0;
	wndclass.cbWndExtra		= sizeof(LONG);
	wndclass.hInstance		= hInstance;
	wndclass.hIcon			= NULL;
	wndclass.hCursor		= LoadCursor(NULL, IDC_ARROW);
	wndclass.hbrBackground	= hbr;
	wndclass.lpszMenuName	= NULL;
	wndclass.lpszClassName	= gszPane1Class;

	if (!RegisterClass(&wndclass)) return (FALSE);

	wndclass.style			= 0;
	wndclass.lpfnWndProc	= Pane2WndProc;
	wndclass.cbClsExtra		= 0;
	wndclass.cbWndExtra		= sizeof(LONG);
	wndclass.hInstance		= hInstance;
	wndclass.hIcon			= NULL;
	wndclass.hCursor		= LoadCursor(NULL, IDC_ARROW);
	wndclass.hbrBackground	= hbr;
	wndclass.lpszMenuName	= NULL;
	wndclass.lpszClassName	= gszPane2Class;

	if (!RegisterClass(&wndclass)) return (FALSE);

	wndclass.style			= 0;
	wndclass.lpfnWndProc	= ColorWndProc;
	wndclass.cbClsExtra		= 0;
	wndclass.cbWndExtra		= sizeof(LONG);
	wndclass.hInstance		= hInstance;
	wndclass.hIcon			= NULL;
	wndclass.hCursor		= LoadCursor(NULL, IDC_ARROW);
	wndclass.hbrBackground	= hboxbr;
	wndclass.lpszMenuName	= NULL;
	wndclass.lpszClassName	= gszColorClass;

	if (!RegisterClass(&wndclass)) return (FALSE);

	wndclass.style			= 0;
	wndclass.lpfnWndProc	= ACARSColorWndProc;
	wndclass.cbClsExtra		= 0;
	wndclass.cbWndExtra		= sizeof(LONG);
	wndclass.hInstance		= hInstance;
	wndclass.hIcon			= NULL;
	wndclass.hCursor		= LoadCursor(NULL, IDC_ARROW);
	wndclass.hbrBackground	= hboxbr;
	wndclass.lpszMenuName	= NULL;
	wndclass.lpszClassName	= gszACARSColorClass;

	if (!RegisterClass(&wndclass)) return (FALSE);

	wndclass.style			= 0;
	wndclass.lpfnWndProc	= MOBITEXColorWndProc;
	wndclass.cbClsExtra		= 0;
	wndclass.cbWndExtra		= sizeof(LONG);
	wndclass.hInstance		= hInstance;
	wndclass.hIcon			= NULL;
	wndclass.hCursor		= LoadCursor(NULL, IDC_ARROW);
	wndclass.hbrBackground	= hboxbr;
	wndclass.lpszMenuName	= NULL;
	wndclass.lpszClassName	= gszMOBITEXColorClass;

	if (!RegisterClass(&wndclass)) return (FALSE);

	wndclass.style			= 0;
	wndclass.lpfnWndProc	= ERMESColorWndProc;
	wndclass.cbClsExtra		= 0;
	wndclass.cbWndExtra		= sizeof(LONG);
	wndclass.hInstance		= hInstance;
	wndclass.hIcon			= NULL;
	wndclass.hCursor		= LoadCursor(NULL, IDC_ARROW);
	wndclass.hbrBackground	= hboxbr;
	wndclass.lpszMenuName	= NULL;
	wndclass.lpszClassName	= gszERMESColorClass;

	if (!RegisterClass(&wndclass)) return (FALSE);

	return (TRUE);
} // end of InitApplication()


// Instance stuff
HWND NEAR InitInstance(HINSTANCE hInstance, int nCmdShow)
{
	WINDOWPLACEMENT wpl;
	POINT pt1,pt2;
	RECT rc;

	// load accelerators
	ghAccel = LoadAccelerators(hInstance, MAKEINTRESOURCE( PDWACCEL ));

	ghWnd = CreateWindow(gszPDWClass, szAppName,
						 WS_OVERLAPPEDWINDOW,
						 Profile.xPos, Profile.yPos,
						 Profile.xSize, Profile.ySize,
						 NULL, NULL, hInstance, NULL);

	if (NULL == ghWnd) return (NULL);

	ShowWindow(ghWnd, nCmdShow);

	// Maximize window?
	if (Profile.maximize_tmp) ShowWindow(ghWnd, SW_SHOWMAXIMIZED);

	UpdateWindow(ghWnd);

	// Incase first Maximize attempt failed, set again,
	// also set minimize info!
	if (Profile.maximize_tmp)
	{
		pt1.x=0; pt1.y=0;		// Set upper left corner for when minimized.
		pt2.x=0; pt2.y=0;		// Set upper left corner for when maximized.

		// Set restore info (use profile defaults)
		rc.left		= 0;		// Profile.xPos;
		rc.right	= 593;		// Profile.xSize;
		rc.top		= 0;		// Profile.yPos;
		rc.bottom	= 442;		// Profile.ySize;

		wpl.length = sizeof(WINDOWPLACEMENT);
		wpl.flags = WPF_RESTORETOMAXIMIZED;
		wpl.showCmd = SW_SHOWMAXIMIZED;
		wpl.ptMinPosition = pt1;
		wpl.ptMaxPosition = pt2;
		wpl.rcNormalPosition = rc;
		SetWindowPlacement(ghWnd,&wpl);
		Profile.maximize_flg=1;
	}
	return (ghWnd);
} // end of InitInstance()


void InitializePane(PaneStruct *pane)
{
	char *pchar;
	BYTE *pcolor;

	pane->Bottom = 0;

	pchar  = pane->buff_char;
	pcolor = pane->buff_color;

	for (int x=0; x<pane->buff_lines; x++)
	{
		pchar [x*(LINE_SIZE+1) + 0] = 0;
		pcolor[x*(LINE_SIZE+1) + 0] = COLOR_UNUSED;
	}

	pane->currentPos   = 0;
	pane->currentColor = COLOR_UNUSED;

	pane->iVscrollPos = 0;
	pane->iVscrollMax = 0;
	pane->iHscrollPos = 0;
	pane->iHscrollMax = 0;

	SetScrollRange(pane->hWnd, SB_VERT, 0, 0, TRUE);
	SetScrollRange(pane->hWnd, SB_HORZ, 0, 0, TRUE);

	return;
} // end InitializePane
