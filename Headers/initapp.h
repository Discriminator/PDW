#ifndef INITAPP_H
#define INITAPP_H

extern PaneStruct Pane1;
extern PaneStruct Pane2;

extern HINSTANCE ghInstance;       // current instance
extern HWND    ghWnd;              // main window handle
extern HWND    hToolbar;           // Toolbar handle
extern HACCEL  ghAccel;
extern HMENU   ghMenu; 
extern TCHAR   szAppName[128];     // name of the application (PH: was 64)
extern TCHAR   szShortAppName[32]; // short name of the application
extern TCHAR   szApiFailedMsg[64]; // "A Windows API Failed" message

extern FILE *pLogFile;			// Logged messages
extern FILE *pFilterFile;		// Filtered messages
extern FILE *pSepFilterFile;	// Separate filtered messages
extern FILE *pStatFile;			// Statistics
extern FILE *pClipboardFile;
extern FILE *pFiltersFile;		// PH: Filter.ini

extern unsigned int cxChar, cyChar, cxCaps;
extern int iMaxWidth;
extern int sizeSet;
extern int pane1Height, pane2Height, pane1Pos, pane2Pos, pane1Top;

extern char gszPDWClass[15];
extern char gszPane1Class[17];
extern char gszPane2Class[17];
//extern char gszPane2LabelClass[22];
extern char gszColorClass[17];
extern char gszACARSColorClass[19];
extern char gszMOBITEXColorClass[21];
extern char gszERMESColorClass[19];

extern LPCTSTR lpszSourceFileName;

extern TCHAR szDialogErrorMsg[80];
extern TCHAR szCenterOpenDlgMsg[80];

extern TCHAR szPath[MAX_PATH];          // path where the running application resides
extern TCHAR szLogPathName[MAX_PATH];   // Logfile-path
extern TCHAR szWavePathName[MAX_PATH];  // Wavefile-path
extern TCHAR szExePathName[MAX_PATH];   // full pathname of the application
extern TCHAR szHelpPathName[MAX_PATH];  // full pathname of the application's help file
extern TCHAR szIniPathName[MAX_PATH];   // full pathname of the application's ini file
extern TCHAR szFilterPathName[MAX_PATH];// full pathname of the application's filter file
extern TCHAR szFilterBackup[MAX_PATH];  // full pathname of the application's filter backupfile
extern TCHAR szVolPathName[MAX_PATH];   // full pathname to the PDW volume control.

// functions
UINT GetPathFromFullPathName(LPCTSTR lpFullPathName, LPTSTR lpPathBuffer, UINT nPathBufferLength);
BOOL NEAR InitApplication(HINSTANCE hInstance);
HWND NEAR InitInstance(HINSTANCE hInstance, int nCmdShow);
void InitializePane(PaneStruct *pane);

#endif
