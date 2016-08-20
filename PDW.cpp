//  PDW.cpp
//
//  Main program loop.
//
//  This file uses the following functions:
//
//  ClearPanes()
//  WinMain(), PDWWndProc(), Pane1WndProc(), Pane2WndProc()
//  InvertSelection(), CopyToClipboard(), PanePaint(), PaneHScroll()
//  PaneVScroll(), GoModalDialogBoxParam(), ErrDlgProc(), ExitDlgProc()
//  OkCancelDlgProc(), LogFileDlgProc(), SetupDlgProc(), SelectFont()
//  ColorsDlgProc(), ColorWndProc(), OptionsDlgProc(), ScrollDlgProc()
//  Copy_Filter_Fields(), BuildFilterString(), FilterDlgProc()
//  UpdateSigInd(), MonStatDlgProc(), MonKeepDlgProc()
//  CenterOpenDlgBox(), CenterWindow(), ErrorMessageBox()
//  SetTitle(), GetPrivateProfileSettings(), GetPrivateProfileDWord()
//  WritePrivateProfileSettings(), WritePrivateProfileInt()
//  WritePrivateProfileDWord(), GetEditSaveName()
//
//
// PH : Peter Hunt
// RAH: Rutger A. Heunks
// AVE: Andreas Verhoeven
// DD : Danny D.
// AG : Andrey Grodzovsky
// DNL: discriminator.nl
// 
//
// 06/04/2003 PH:  Added Short Instructions
// 07/04/2003 PH:  Short Instructions / Frames optional via menu
// 08/04/2003 PH:  Added option to choose separate color for Short Instructions
// 09/04/2003 PH:  Displaying of Tone-Only / Numeric now separately selectable
// 10/04/2003 PH:  FLEX vector-type is now also displayed
// 11/04/2003 PH:  Enlarged filterwindow (CTRL-F)
// 15/04/2003 PH:  Switch '/labellog' writes the filterlabels also in monitorlogfile
// 19/05/2003 RAH: Included playback 
// 23/05/2003 AVE: Added "PDW Filters(#)" in filter window titlebar.
// 09/06/2003 PH:  Added 2-level 3200sps sync-up
// 11/06/2003 PH:  Added switch '/callmax' for optimized Callmax-pages
// 26/06/2003 AVE: Added html.cpp & html.h for HTML-logfiles
// 28/06/2003 PH:  Added  IRC-logfiles via switch '/irc'
// 01/09/2003 PH:  Linefeed character will be replaced by '»'
// 02/09/2003 PH:  Added extra column "type" for FLEX/POCSAG
// 03/09/2003 PH:  Changed GetTimeFormat/Date, now always leading zeros are displayed
// 07/09/2033 PH:  Removed Flush-Time. All logfiles will be flushed immediately
// 01/10/2003 PH:  Labellog is now optional via Filter-dialog
// 03/10/2003 PH:  Bug fixed in FLEX.CPP regarding displayed phases
// 05/10/2003 PH:  Added code.wav (for FLEX-short only)
// 06/10/2003 PH:  Separate directories for Wavfiles and Logfiles
// 07/10/2003 PH:  Filters will now be written to "filters.ini" via WriteFilters()
// 08/10/2003 PH:  Code.wav also usable for pocsag
// 11/10/2003 AVE: Added FTP-support (FTP.DLL)
// 12/10/2003 AVE: Filters will now be read from "filters.ini" via ReadFilters() (up to 32.000!)
// 30/10/2003 PH:  When exiting PDW, a backup of FILTERS.INI will be made, named FILTERS.BAK
// 03/11/2003 PH:  FTP-password is now encrypted (in PDW.INI)
// 11/11/2003 PH:  changed WM_KEYDOWN and added VK_SHIFT for scrolling in Pane2
// 12/11/2003 PH:  Added "Reload Filters", for use with PDW-Filter
// 13/11/2003 PH:  Added ClearScreenDlgProc
//
//	  160/196	
//
// 14/11/2003 PH:	 Version 1.2 released !!!
//
// 17/11/2003 PH:  Added: hitcounter, date&time of last hit
// 18/11/2003 PH:  HTML: Added HTMLTitle & Refresh
// 26/11/2003 PH:  HTML: Added URL-filter
// 28/11/2003 PH:  FTP : Added FTP-File
//						 Added Haaglanden.html
//   72/250
//
// 01/12/2003 PH:	 Version 1.21 released !!!
//
// 01/12/2003 PH:  Separate filter messages are now logged to normal filterfile
//						 AND separate filter file
// 02/12/2003 PH:	 Added Separate filter HTML-files
// 03/12/2003 PH:  Added Statistics.html
//
// 12/04/2004 PH/AVE: New FTP, FTP.DLL not needed anymore
// 21/04/2004 PH:  Monitor-only labels made public
// 28/04/2004 PH:  Added "Invisible for search engines" (in HTML)
// 28/04/2004 PH:  Added FTP-passive mode
// 08/05/2004 PH:  Added Label colors
// 15/05/2004 PH:  Short Instructions gan now be converted to textmessage
// 18/05/2004 PH:  Added cycle/frame information in statusbar
// 06/06/2004 PH:  Changed update of HTML-files
// 11/06/2004 PH:  Little bugix on SI-conversion (rejecting 20295xx)
// 17/06/2004 PH:  Test: cycle/frame instead of MODE
// 23/06/2004 PH:  Changed linefeeds for P2000-Amsterdam
// 24/06/2004 PH:  Added separate color for biterrors in HTML (CLASS="B")
// 24/06/2004 PH:  Changed colors for Convert-SI
//
//  497/3659
//
// 02/07/2004 PH:	 Version 1.22 released !!!
//
// 04/07/2004 PH:  Wordwrap / Linefeed are now selectable options
// 05/07/2004 PH:  Bugfix: separate files can now contain pathname
// 06/07/2004 PH:  HTML-Statistics is now made an option
// 19/07/2004 PH:  Changed command file arguments (%1 %2 %3 %4 %5 %6 %7 %8)
// 29/07/2004 PH:  Changed ERMES-format, added 'type'-column
// 30/07/2004 PH:  All modes can now log messages as columns
// 01/08/2004 PH:  Some little changes in ACARS
// 12/08/2004 PH:  Fixed bugs: rejecting FLEX-groupcodes, deleting FLEX tempfiles
//						 Added "converting to textmessage" for all FLEX-vectors
//  395/1736
//
// 15/08/2004 PH:  Version 1.23 released !!!
//
// 16/08/2004 PH:  Fixed ERMES bug. Fixed parity-check bug in ACARS
//						 Modified the columns
//
//   52/176
//
// 18/08/2004 PH:  Version 1.24 released !!!
//
// 19/08/2004 PH:  Fixed some ACARS bugs
// 22/08/2004 PH:  Several ACARS changes + added routes.df + ACARS-HTML
// 25/08/2004 PH:  Added: ACARS-filter (Aircraft reg)
// 28/08/2004 PH:  Fixed little bug when using HTML & soundcard
// 30/08/2004 PH:  Added: Profile.MonthNumber for logfilenames
// 02/09/2004 PH:  Added: Class="NOCSS" in case PDW.CSS is missing
// 08/09/2004 PH:  Fixed little bug : filtering capcode+text
// 20/09/2004 PH:  Added some extra label colors
// 06/10/2004 PH:  Added Profile.LogfilePath
// 16/10/2004 PH:  Added: user can enter capcode variable in description (%1234567)
// 17/10/2004 PH:  Changed ScrollDlgProc (user can enter percentage)
// 01/11/2004 PH:  Added: Monitor-Only.wav
// 05/11/2004 PH:  Replaced collogfile editboxes by checkboxes
// 08/11/2004 PH:  Statistics.HTML can also be uploaded
// 08/11/2004 PH:  Added: Warning when resolution is < 800x600
// 09/11/2004 PH:  Splitted FilterDlgProc into FilterDlgProc & FilterOptionsDlgProc
//
// 10/11/2004 : Version 2.0 released !!!
//   555/3221
//
// 14/11/2004 PH:  Added: Profile.filter_default_type
// 16/11/2004 PH:  Added: Profile.Linefeed in Filterwindow
// 17/11/2004 PH:  Added: Profile.LabelNewline
// 18/11/2004 PH:  Added: Profile.BlockDuplicate
// 19/11/2004 PH:  Fixed FLEX SH/TONE bug
// 26/11/2004 PH:  Added capcode.wav for wildcards (?->x)
// 29/11/2004 PH:  Combined Add and Edit Filter Dialogs
// 30/11/2004 PH:  New : UpdateFilterControls()
// 30/11/2004 PH:  New : UpdateHTMLControls()
// 30/11/2004 PH:  Rejecting texfilters is now possible
// 06/12/2004 HWi: Added FlexTIME
// 18/12/2004 PH:  Monitor_Only.wav can be turned on/off per filter
// 19/12/2004 PH:  Bugfix displaying phases
// 21/12/2004 HWi: Bugfix slicer.sys
// 22/12/2004 PH:  Linefeeds also possible in logfiles
// 23/12/2004 PH:  Short Instructions are now displayed in reversed order
// 28/12/2004 HWi: Improved scrolling when dragging in FilterDlg
// 28/12/2004 HWi: When the PC falls in standby mode, PDW doesn't stop anymore
//
// 30/12/2004 : Version 2.1 released!!
//	 344/1700
//
//    01/2005 PH:  Fixed several bugs regarding pocsag and mobitex
//
// 20/01/2005 : Version 2.11 released!!
//   584/2913
//
// 21/01/2005 PH:  Added 'filter.match_exact_text'
//
// 07/03/2005 : Version 2.12 released!!
//  1348/8217
//
// 10/07/2005 PH:  Added 'Profile.Hide_Column'
// 21/07/2005 HWi: Fixed disappearing emailaddresses
//				   Check to see if PDW is already running (in the current directory)
// 01/08/2005 PH:  Fixed StNum bug in FLEX
// 05/08/2005 Hwi: The Filterwindow will now automatically resize depending on resolution
// 10/08/2005 PH:  The button sizes in filterwindow are now also resolution dependant
// 15/08/2005 PH:  Added: Find filter
// 25/08/2005 PH:  Added: Search while typing
//
// 27/08/2005 : Version 2.13 released!!
//  1453/9482
//
// 22/09/2005 PH:  Added:  Additional information in titlebar (FTP/reject/blocked)
// 22/09/2005 PH:  Bugfix: When started up, sometimes the first message didn't appear
//                         correctly (if Hide Column)
// 04/10/2005 PH:  Change: CODE.WAV will allways be played, also if no filtermatch
// 03/11/2005 PH:  Added:  Empty lines / HTML separator after ALPHA/GROUP messages
// 27/11/2005 PH:  Added:  User can select soundcard
// 02/11/2005 PH:  Bugfix: Early wrapping when screen_x=1280 && Hide Column
// 05/12/2005 PH:  Added:  Reset ALL hitcounters / backup filters.xxx
// 07/12/2005 PH:  Added:  zDlgProc()
// 09/12/2005 PH:  Change: In SelectFont() charactersize limited to 8-12 and a MessageBox
//                         will appear if the selected fontsize is too big to fit in the columns
// 11/12/2005 PH:  Change: Font in IDC_FILTERS is now Lucida Console (fixed font)
//						   Also changed BuildFilterString, now everyting lines up
// 12/12/2005 PH:  Change: InitListControl() now makes a better measurement of the width
//						   of the ListView to use the window more efficently
// 15/12/2005 PH:  Added:  FilterEdit => Multiple Edit
// 17/12/2005 PH:  Added:  Minimize to SystemTray => MoveToSystemTray()
// 13/01/2006 PH:  FLEX Groupcalls are now converted via array (no more tempfiles)
// 29/01/2006 PH:  Bugfix: ERMES bug (no more junk / ghost messages are being displayed)
//
// 22/02/2006 : Version 2.14 released!!
//  2251/14503
//
// 24/02/2006 PH:  Bugfix: The filter window titlebar sometimes displayed wrong filter
// 25/02/2006 PH:  Bugfix: Minor fixes on main titlebar
// 26/02/2006 PH:  Bugfix: Break on EOT character
// 03/03/2006 PH:  Change: HTML-labels same colors as in PDW itself
// 05/03/2006 PH:  Optimized Empty lines / HTML separator
// 09/04/2005 PH:  Added : Empty lines / HTML separator -> now also for filter window
// 13/04/2006 HWi: Fixed : SMTP TCP/IP bug
// 17/04/2005 PH:  Added : Empty lines / HTML separator -> option: also check time
// 19/04/2006 PH:  Added : Log only FTP-errors to FTP.LOG
// 26/04/2006 PH:  Optimized Empty lines (clear screen / new logfile)
// 27/04/2006 PH:  Change: Block Duplicate Messages also blocks duplicate groupcalls
// 29/04/2006 PH:  Added : Filtered messages can only be displayed in filterwindow
// 30/04/2006 PH:  Change: display_show_crlf() now opens files via OpenGetFilename()
// 17/05/2006 PH:  Change: CODE.WAV -> Prio wavfiles
// 18/05/2006 PH:  FLEX Groupcalls are now converted via int GroupCapcodes[16][MAXIMUM_GROUPSIZE]
// 01/06/2006 PH:  Started using more arrays and definitions (MISC.CPP : CURRENT/PREVIOUS)
// 02/06/2006 PH:  Added : Empty lines -> now also for separate filterfiles
// 26/06/2006 PH:  Added : iDateUSA
// 29/06/2006 PH:  Added : Filtered messages can only be displayed in filterwindow
// 26/07/2006 PH:  Fixed : StNum bug in FLEX
// 04/08/2006 PH:  Added : Multiple separate filter files (only HTML)
// 05/08/2006 PH:  Change: UpdateFilterControls() integrated in FilterEditDlgProc
//                         using WM_WININICHANGE
// 07/08/2006 Hwi: Change: SMTP.CPP now uses 8-bit charset
// 07/08/2006 PH:  Change: Replaced character remapping, so messages appear also remapped
//                         in logfile and SMTP.
// 13/08/2006 Hwi: Added : Charset selection in SMTP Dialog
// 16/10/2006 PH:  Added : Multiple separate filter files (also logfiles)
// 22/10/2006 PH:  Added : Custom HTML folder
//
//
// 11/01/2007 : Version 2.15 released!!
//  559/1238
//
// 23/08/2007 : Version 2.2 released!!
//  1716/8104
//
// 19/12/2007 : Version 2.3 released!!
//  638/3497
//
// 16/03/2008 : Version 2.4 released!!
//  774/6400
//
// 19/07/2008 : Version 2.5 released!!
//  188/1529
//
// 16/08/2008 : Version 2.51 released!!
//  431/3252
//
// 12/10/2008 : Version 2.52 released!!
//  556/4571
//
// 11/01/2009 : Version 2.53 released!!
//  601/5057
//
// 24/03/2010 : Version 2.60 released!!
//
// 10/08/2010 : Version 3.0 released!!

// 13/7/2013 DD: Updated About. web url. 
// 13/7/2013 DD: Version 3.12 released!
//
// FILTERS.INI lines :
//
// Filter1=0,"1234567","Description / label","text",1,2,3,4,5,"sepfile",118,20-04-04,16:22:22
//
// 0: 1=FLEX , 2=POCSAG , 3=TEXT
// 1: 1=Reject / 2=Monitor-only
// 2: Wavenumber
// 3: 1=CMD-enabled
// 4: 1=Label-enabled += label-color
// 5: 1=SEP-enabled , 2=SEP-HTML , 4=SEP-FTP , 8=SMTP , 16=Match exact text
//
// 12-JUN-2016 AG : SSL support for SMTP mail client
// 20-AUG-2016 DNL: Beta release 3.2b01
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
#include <shlobj.h>

#include "headers\resource.h"
#include "headers\PDW.h"
#include "headers\slicer.h"
#include "headers\toolbar.h"
#include "headers\gfx.h"
#include "headers\initapp.h"
#include "headers\sigind.h"
#include "headers\decode.h"
#include "headers\sound_in.h"
#include "headers\printer.h"
#include "headers\misc.h"
#include "headers\menu.h"
#include "headers\acars.h"
#include "headers\language.h"
#include "headers\mobitex.h"
#include "headers\ermes.h"
#include "utils\rs232.h"
#include "utils\debug.h"
#include "utils\ostype.h"
#include "utils\smtp.h"

#include "headers\helper_funcs.h"	// Extra functies van Andreas

#define MIN_X_WIN_SIZE		444		// Smallest main win X size can be (was 444)
#define MIN_Y_WIN_SIZE		261		// Smallest main win Y size can be (was 261)

#define TOOLBAR_SIZE		49		// Space for Toolbar/Top of Pane1 (was 52)
#define WIN_DIVIDER_SIZE	19		// Pane1/Pane2 seperator size (was 56)
#define PANE1_SIZE			1000	// Pane1 default scrollback size
#define PANE2_SIZE			200		// Pane2 default scrollback size
#define PDW_TIMER			101		// Timer ID
#define SCROLL_TIMER		102
#define SECOND_TIMER		103		// Timer ID
#define MINUTE_TIMER		104		// Timer ID
#define CLICK_TIMER			105		// Timer ID

#define WM_MOUSEWHEEL		0x020A

#define HMENU_MAIN			1
#define HMENU_FILTERS		2
#define HMENU_SYSTRAY		3

#define DRIVER_NOT_LOADED	0
#define DRIVER_VXD_LOADED	1
#define DRIVER_COM_LOADED	2

#define min(a,b) (((a) < (b)) ? (a) : (b))
#define max(a,b) (((a) > (b)) ? (a) : (b))

PROFILE Profile;					// User profile information
HANDLE  hDriver = 0;				// Handle of Virtual Device
int		*pComPorts;					// Array with available comports

bool bWin9x;

int  nDriverLoaded	= DRIVER_NOT_LOADED;// VxD/comport loaded?
bool bEditFilter	= false;		// Set before/after calling edit dialog
bool bPauseFlag		= false;		// Decides if message output is paused.
bool bUpdateFilters	= false;		// PH: Needs FILTERS.INI to be updated?

double dRX_Quality;

int nCount_Messages=0;				// PH: Counts # Messages
int nCount_Groupcalls=0;			// PH: Counts # Groupcalls
int nCount_Biterrors=0;				// PH: Counts # Message-biterrors
int nCount_Rejected=0;				// PH: Counts # Rejected Messages
int nCount_Blocked=0;				// PH: Counts # Blocked Messages
int nCount_Missed[2] = { 0,0 };		// PH: Counts # Missed Groupcalls
int nCount_BlockBuffer[2] = { 0,0 };// PH: Counts # Block Buffer

//int iDebug_Test=0;

int nSMTPsessions=0;
int nSMTPemails=0;
int nSMTPerrors=0;
int iSMTPlastError=0;

int iMouseClick=0;					// PH: Used for detecting double click

//int Notification=0;				// PH: Used for notification windows

bool bTrayed=false;					// PH: Is true if trayed
bool bLBUTTONDBLCLK=false;			// PH: Is true after 2 WM_LBUTTONDOWN messages
bool bFilterFindCASE=false;			// PH: Filter Find Case Sensitive?

int   iTEMP = 0;					// Global temporary int
char szTEMP[MAX_STR_LEN];			// Global temporary buffer

int iDuplicateFilter=0;

char szFilenameDate[16];			// PH: Global buffer for date as filename
char szDebugStarted[32];			// PH: Time and date when PDW has been started

char szWindowText[6][1000];			// [0] = PDW version
									// [1] = SPARE
									// [2] = Current mode
									// [3] = FLEX/ERMES cycle/frame(/batch)
									// [4] = FLEX - Groupcall
									// [5] = Rejected/Blocked messages

// Text editing globals
unsigned int iSelectionStartCol, iSelectionStartRow, iSelectionEndCol, iSelectionEndRow;
int select_on = 0, selected = 0, selecting = 0;
PaneStruct *select_pane;			// selects pain to copy text from

// The following COLORREFS are used while changing POCSAG/FLEX colors
COLORREF custom_colors[16];
COLORREF tmp_background;
COLORREF tmp_address;
COLORREF tmp_timestamp;
COLORREF tmp_modetypebit;
COLORREF tmp_numeric;
COLORREF tmp_message;
COLORREF tmp_misc;
COLORREF tmp_filterlabel;
COLORREF tmp_filtermatch;
COLORREF tmp_biterrors;

// Tempory fonts-see gfx.c for other fonts
HFONT   tmp_hfont=NULL;
LOGFONT tmp_logfont;

time_t tStarted;	// Contains the time when PDW was started

// If copy upper/lower pane or just copy is successful then this flag is set to TRUE.
bool bOK_to_save=false;

char *pdw_version = "PDW v3.2b01";		// Current version info

// RAH: record and playback stuff
OPENFILENAME openplayback;
OPENFILENAME openrecording;
char szPlayback[MAX_PATH];
char szRecording[MAX_PATH];

//HWi
typedef enum sroll
{
	scrollNull,
	scrollUp,
	scrollDown
} EScrollDirection;


HIMAGELIST		hDragImageList;
LPNMHDR			pnmhdr;
bool			bDragging, bDragLine;
bool			bSortingFilters, bDeletingFilters;
LVHITTESTINFO	lvhti;
HWND			hListView, hDebugDlg, hSystemTrayDlg, hFilterDlg, hCurrentHWND;
int				m_uScrollTimer, m_ScrollDirection, m_nScrollOffset, m_nDropIndex=-1;
int				nCopyStart=-1, nCopyEnd=-1;

void OnBeginDrag(NMHDR* pnmhdr);
void InitListControl(HWND hDlg);
bool WINAPI InsertListViewItem(char *szLine, int nItem);
void OnMouseMove(UINT nFlags, int x, int y);
void OnMouseWheel(WPARAM wParam, int x, int y, RECT g_rect);
void OnLButtonUp(UINT nFlags, int x, int y);
void DropItemOnList(int x, int y);
void OnTimer(UINT nIDEvent);
void KillScrollTimer(void);
void SetScrollTimer(EScrollDirection ScrollDirection);
LONG OnCustomDraw(LPNMLVCUSTOMDRAW lpDraw);
void SortFilter(HWND hDlg, bool bAddress);
bool CheckSelection(bool bMore);
int  CompareCapcodes(char *szCapcode1, char *szCapcode2);
void CopyFilter(void);
void PasteFilter(void);
void ResetHitcounters(bool bAll);
void ShowContextMenu(int menu, HWND hWindow);
void GoogleMaps(int iPosition);
void SelectByDoubleClick(HWND hWnd, PaneStruct *pane, int iPosition, int StartRow);
int  FindFilter(int index, char *szSearchString, bool bShowHits, bool bBackwards);
void pumpMessages(void);

DWORD GetColorRGB(BYTE color);

void AutoRecording();	// PH: temp/test


int PASCAL WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpszCmdLine, int nCmdShow)
{
	// RAH: setup common file dialog data
	ZeroMemory(&openplayback, sizeof(openplayback));
	ZeroMemory(&openrecording,sizeof(openrecording));

	openplayback.lStructSize  = sizeof(openplayback);
	openrecording.lStructSize = sizeof(openrecording);

	szPlayback[0] = '\0';

	MSG msg;

	ghWnd = NULL;
	ghInstance = hInstance;

	Profile.LabelLog			= 1;	// Flag for labels in monitored-logfile
	Profile.LabelNewline		= 1;	// Flag for labels on new line
	Profile.ColLogfile[0]		= '\0';	// Flag for columns in logfile
	Profile.ColFilterfile[0]	= '\0';	// Flag for columns in filterfiles
	Profile.Linefeed			= 1;	// Flag for converting » to linefeed
	Profile.Separator			= 1;	// Flag for separating messages (empty line)
	Profile.MonthNumber			= 1;	// Flag for using monthnumber in logfilenames
	Profile.DateFormat			= 0;	// Flag for date format
	Profile.BlockDuplicate		= 0;	// Flag for blocking duplicate messages
	Profile.FilterWindowColors	= 1;	// Flag for showing label colors in filterwindow
	Profile.FilterWindowExtra	= 1;	// Flag for showing CMD/DESC/SEP/etc in filterwindow

	Profile.SystemTray	        = 0;	// Flag for enabeling the system tray
	Profile.SystemTrayRestore	= 0;	// Flag for enabeling auto restore from tray
	Profile.SMTP = 0;					// Flag for SMTP-email

	Profile.FlexTIME			= 0;	// Flag for FlexTIME as systemtime
	Profile.FlexGroupMode		= 0;

	Profile.xPos  = 0;
	Profile.yPos  = 0;
	Profile.xSize = 593;
	Profile.ySize = 442;

	Profile.showtone		= 1;
	Profile.shownumeric		= 1;
	Profile.showmisc		= 1;
	Profile.filterbeep		= 0;
	Profile.filterwindowonly = 0;	// Show filtered messages only in filterwindow

	Profile.confirmExit     = 1;
	Profile.decodepocsag    = 1;
	Profile.decodeflex      = 1;
	Profile.showinstr       = 1;	// PH: Show Short Instructions
	Profile.convert_si      = 1;	// PH: Convert Short Instructions to textmessage
	Profile.pocsag_512      = 1;
	Profile.pocsag_1200     = 1;
	Profile.pocsag_2400     = 1;
	Profile.pocsag_fnu      = 0;
	Profile.pocsag_showboth = 0;
	Profile.flex_1600       = 1;
	Profile.flex_3200       = 1;
	Profile.flex_6400       = 1;
	Profile.acars_parity_check = 0;

	Profile.comPortEnabled	= 0;
	Profile.comPort			= 2;
	Profile.comPortRS232	= 0;
	Profile.comPortAddr		= 0x2F8;
	Profile.comPortIRQ		= 3;
	Profile.invert			= 0;	// Current invert status. 0=No,1=Yes
	Profile.fourlevel		= 0;
	Profile.percent			= 65;	// Set pane1 to 65% of client area

	// setup default font information
	Profile.fontInfo.lfHeight			= -11;
	Profile.fontInfo.lfWidth			= 0;
	Profile.fontInfo.lfEscapement		= 0;
	Profile.fontInfo.lfOrientation		= 0;
	Profile.fontInfo.lfWeight			= FW_NORMAL;
	Profile.fontInfo.lfItalic			= 0;
	Profile.fontInfo.lfUnderline		= 0;
	Profile.fontInfo.lfStrikeOut		= 0;
	Profile.fontInfo.lfCharSet			= OEM_CHARSET;
	Profile.fontInfo.lfOutPrecision		= OUT_STROKE_PRECIS;
	Profile.fontInfo.lfClipPrecision	= CLIP_DEFAULT_PRECIS;
	Profile.fontInfo.lfQuality			= DEFAULT_QUALITY;
	Profile.fontInfo.lfPitchAndFamily	= FIXED_PITCH | FF_MODERN;
	lstrcpy(Profile.fontInfo.lfFaceName, "Courier New");

	Profile.reverse_msg = FALSE;	// Flag to reverse message output.
	Profile.lang_mi_index = 0;		// Set default language menu item.
	Profile.lang_tbl_index= 0;		// Decides language character map.

	Profile.logfile_enabled	 = 1;
	Profile.logfile[0]		 = '\0';
	Profile.logfile_use_date = 1;

	Profile.edit_save_file[0] = '\0';
	Profile.maximize_flg	  = 1;

	Profile.filterfile_enabled		= 1;
	Profile.filterfile[0]			= '\0';
	Profile.filterfile_use_date		= 1;
	Profile.filter_cmd_file_enabled	= 0;
	Profile.filter_cmd[0]			= '\0';
	Profile.filter_cmd_args[0]		= '\0';
	Profile.filter_default_type		= 0;

	Profile.filters.clear();

	// COLORS.
	Profile.color_background	= RGB(rgbColor[BLACK][0],  rgbColor[BLACK][1],  rgbColor[BLACK][2]);
	Profile.color_address		= RGB(rgbColor[WHITE][0],  rgbColor[WHITE][1],  rgbColor[WHITE][2]);
	Profile.color_modetypebit	= RGB(rgbColor[RED][0],    rgbColor[RED][1],    rgbColor[RED][2]);
	Profile.color_timestamp		= RGB(rgbColor[LTBLUE][0], rgbColor[LTBLUE][1], rgbColor[LTBLUE][2]);
	Profile.color_numeric		= RGB(rgbColor[RED][0],    rgbColor[RED][1],    rgbColor[RED][2]);
	Profile.color_message		= RGB(rgbColor[LTCYAN][0], rgbColor[LTCYAN][1], rgbColor[LTCYAN][2]);
	Profile.color_misc			= RGB(rgbColor[BROWN][0],  rgbColor[BROWN][1],  rgbColor[BROWN][2]);
	Profile.color_biterrors		= RGB(rgbColor[LTGRAY][0], rgbColor[LTGRAY][1], rgbColor[LTGRAY][2]);
	Profile.color_filtermatch	= RGB(rgbColor[LTGREEN][0],rgbColor[LTGREEN][1],rgbColor[LTGREEN][2]);
	Profile.color_instructions	= RGB(rgbColor[LTCYAN][0], rgbColor[LTCYAN][1], rgbColor[LTCYAN][2]);
	Profile.color_ac_message_nr	= RGB(rgbColor[WHITE][0],  rgbColor[WHITE][1],  rgbColor[WHITE][2]);
	Profile.color_ac_dbi		= RGB(rgbColor[RED][0],    rgbColor[RED][1],    rgbColor[RED][2]);
	Profile.color_mb_sender		= RGB(rgbColor[RED][0],    rgbColor[RED][1],    rgbColor[RED][2]);

	// Filter label colors
	Profile.color_filterlabel[0]	= RGB(rgbColor[LTBLUE][0], rgbColor[LTBLUE][1], rgbColor[LTBLUE][2]);
	Profile.color_filterlabel[1]	= RGB(rgbColor[YELLOW][0], rgbColor[YELLOW][1], rgbColor[YELLOW][2]);
	Profile.color_filterlabel[2]	= RGB(rgbColor[RED][0],    rgbColor[RED][1],    rgbColor[RED][2]);
	Profile.color_filterlabel[3]	= RGB(0xFF, 0xAA, 0x00);      /* orange */
	Profile.color_filterlabel[4]	= RGB(rgbColor[LTBLUE][0], rgbColor[LTBLUE][1], rgbColor[LTBLUE][2]);
	Profile.color_filterlabel[5]	= RGB(rgbColor[CYAN][0],   rgbColor[CYAN][1],   rgbColor[CYAN][2]);
	Profile.color_filterlabel[6]	= RGB(rgbColor[WHITE][0],  rgbColor[WHITE][1],  rgbColor[WHITE][2]);
	Profile.color_filterlabel[7]	= RGB(rgbColor[LTGREEN][0],rgbColor[LTGREEN][1],rgbColor[LTGREEN][2]);
	Profile.color_filterlabel[8]	= RGB(rgbColor[LTGRAY][0], rgbColor[LTGRAY][1], rgbColor[LTGRAY][2]);

	Profile.color_filterlabel[9]	= RGB(rgbColor[BROWN][0],  rgbColor[BROWN][1],  rgbColor[BROWN][2]);
	Profile.color_filterlabel[10]	= RGB(rgbColor[LTCYAN][0], rgbColor[LTCYAN][1], rgbColor[LTCYAN][2]);
	Profile.color_filterlabel[11]	= RGB(0x00, 0x33, 0x99);      /* navy blue */
	Profile.color_filterlabel[12]	= RGB(0xFF, 0x00, 0xFF);      /* magenta */
	Profile.color_filterlabel[13]	= RGB(0x33, 0xCC, 0x99);      /* sea green */
	Profile.color_filterlabel[14]	= RGB(0xFF, 0x99, 0xCC);	  /* pink */
	Profile.color_filterlabel[15]	= RGB(0x99, 0xFF, 0xFF);      /* ice blue */
	Profile.color_filterlabel[16]	= RGB(0x99, 0xFF, 0xCC);      /* turquoise */

	Profile.pane1_size			= PANE1_SIZE;
	Profile.pane2_size			= PANE2_SIZE;
	Profile.ScrollSpeed			= 1;

	Profile.stat_file_enabled	= 0;
	Profile.stat_file[0]		= '\0';
	Profile.stat_file_use_date	= 0;

	// These control Audio input.
	Profile.audioEnabled		= 1;
	Profile.audioDevice			= 0;
	Profile.audioSampleRate		= 44100;
	Profile.audioConfig			= 5;		// Audio input configuration.
	Profile.audioThreshold[4]	= 0;
	Profile.audioResync[4]		= 0;
	Profile.audioCentering[4]	= 0;
	SetAudioConfig(Profile.audioConfig); // Set default audio config

	Profile.minimize_flg	= 0;	// Keep track of minimized state (user could exit while minimized)

	Profile.monitor_paging	= true; // Default is to decode POCSAG/FLEX signals.
	Profile.monitor_acars	= false;
	Profile.monitor_mobitex	= false;
	Profile.monitor_ermes	= false;

	nOSType = GetOSType(NULL);	// HWi
	
	bWin9x = (nOSType < OS_WIN2000) ? true : false;

	if (!hPrevInstance && !InitApplication(hInstance))
	{
		ErrorMessageBox(TEXT("InitApplication() Failed"),
				szApiFailedMsg, lpszSourceFileName, __LINE__);
		return (FALSE);
	}

	CreateMutex(NULL, FALSE, szPath);	// Check if PDW is already running

	if (GetLastError() == ERROR_ALREADY_EXISTS)	// Shut down, as an instance of PDW is
	{											// already running in this folder
		MessageBox(ghWnd, "PDW is already running", "Warning", MB_ICONINFORMATION);
		return (FALSE);
	}

	if ((ghWnd = InitInstance(hInstance, nCmdShow)) == NULL)
	{
		ErrorMessageBox(TEXT("InitInstance() Failed"),
					szApiFailedMsg, lpszSourceFileName, __LINE__);
		return (FALSE);
	}

	if (hToolbar) TB_AutoSize(hToolbar);	// keep toolbar correct size!

	acars.read_data();		// Load Acars data base files.
	read_language_tables();	// Load language tables.

	set_lang_menu();		// Add languge menu items.
	set_menu_items();		// Check/Uncheck required menu items.

	SetWindowPaneSize(WINDOW_SIZE);	// PH: Set Max_X_Client / iMaxWidth / NewLinePoint

	sprintf(szWindowText[0], " %s", pdw_version);	// PH: Set version info in szWindowText buffer

	Get_Date_Time();
	sprintf(szDebugStarted, "%s %s", szCurrentDate, szCurrentTime);

	pComPorts = FindComPorts();	// Put all available comports in array pComports

	tStarted = time(NULL);		// Time when PDW was started

	// Load VxD if using serial port else see if using sound card
	if (Profile.comPortEnabled && !nDriverLoaded)
	{
		if (LoadDriver())
		{
			SetTimer(ghWnd, PDW_TIMER, 100, (TIMERPROC) NULL); // start timer.
		}
	}
	else if (Profile.audioEnabled && !bCapturing)
	{
		if (Start_Capturing())
		{
			SetTimer(ghWnd, PDW_TIMER, 100, (TIMERPROC) NULL); // start timer.
		}
	}
	SetTimer(ghWnd, MINUTE_TIMER, 1000*60, (TIMERPROC) NULL); // start minute timer
	SetTimer(ghWnd, SECOND_TIMER, 1000,    (TIMERPROC) NULL); // start second timer

	if (Max_X_Client < 800)	// PH: Check if the screen width is less than 800
	{
		MessageBox(ghWnd, "WARNING! PDW is optimized for 800x600 resolution (and higher)\nYour computer seems to be using a lower resolution...", "PDW Resolution", MB_ICONWARNING);
	}

	while (GetMessage(&msg, NULL, 0, 0))
	{
		if (!TranslateAccelerator(ghWnd, ghAccel, &msg))
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
	}
	return ((int) msg.wParam);
} // end of WinMain()


// Used if WM_CREATE fails or when program exits.
void Free_Common_Objects(void)
{
	if (Pane1.buff_char  != NULL) free(Pane1.buff_char);
	if (Pane1.buff_color != NULL) free(Pane1.buff_color);
	if (Pane2.buff_char  != NULL) free(Pane2.buff_char);
	if (Pane2.buff_color != NULL) free(Pane2.buff_color);

	Free_Drawing_Objects();			// see gfx.cpp
	FreeSysObjects();				// see gfx.cpp
	FreeLogFONTS();					// see gfx.cpp
	FreeSigInd();					// see sigind.cpp
	FreeToolBarImages(ghInstance);	// Free toolbar button bitmaps
}

LRESULT FAR PASCAL PDWWndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	HDC ghDC;
	TEXTMETRIC g_tm;
	static RECT g_rect={0};
	RECT g_sys_rect;
	size_t g_mem_size;
	LPMINMAXINFO lpMMI;
	time_t lTime;
	struct tm *recTm; 

	int g_xNew, g_yNew, g_scrollSize;
	unsigned int g_min_col, g_max_col, g_min_row, g_max_row, g_index;
	
	extern bool bMode_IDLE;			// Set if no signal
	extern bool bEmpty_Frame;		// Set if FLEX-Frame=EMTPY / ERMES-Batch=0

	char szFile[MAX_PATH];
	char filters_reload[64];		// PH: Buffer for reloading filters
	char filters_temp[64];			// PH: Buffer for reloading filters

	bool pane1=false;

	extern unsigned long int aMessages[1000][3];		// PH: Array used for blocking messages
	int BlockTimer = (Profile.BlockDuplicate >> 4) * 60;

	switch (uMsg)
	{
		case WM_TIMER: // Decode POCSAG/FLEX with comport or sound card.

		if (!bPauseFlag)
		{
			if		(bPlayback) pdw_playback();				// RAH: Playing recording?
			else if (nDriverLoaded) pdw_decode();			// Log messages/statistics & Update signal indicator.
			else if (bCapturing) Process_ReadyBuffers(hWnd);// Using sound card?

			switch (wParam) 
			{ 
				case SECOND_TIMER:
				
				DrawPaneLabels(ghWnd, PANERXQUAL);

				lTime = time(NULL);		// Get current systemtime
				iSecondsElapsed = difftime(lTime, tStarted);

				if (BlockTimer)
				{
					while (aMessages[0][0] && (iSecondsElapsed > (aMessages[0][1] + BlockTimer)))	// First entry Overdue?
					{
						memmove(aMessages[0], aMessages[1], sizeof(aMessages));	// Move entries
						nCount_BlockBuffer[0]--;
					}
				}
				if (hDebugDlg) SendMessage(hDebugDlg, WM_WININICHANGE, 0, 0L);

				break;

				case MINUTE_TIMER:	// Handle minute timer

				if (bMode_IDLE || bEmpty_Frame || Profile.monitor_acars || Profile.monitor_mobitex)
				{
					if (bUpdateFilters)
					{
						WriteFilters(&Profile, 0);	// PH: Save FILTERS.INI
						bUpdateFilters = false;		// PH: Reset for new messages
					}
				}
				lTime = time(NULL);
				recTm = localtime(&lTime);

				if ((recTm->tm_mon == 4) && (recTm->tm_mday == 4)) strcpy(szWindowText[1], "Today is Peter Hunt's birthday :-)");
				else if ((recTm->tm_mon == 11) && ((recTm->tm_mday == 25) || (recTm->tm_mday == 26))) strcpy(szWindowText[1], "Merry Christmas!");
				else if ((recTm->tm_mon == 0) && (recTm->tm_mday == 1)) strcpy(szWindowText[1], "Happy New Year!");
				else strcpy(szWindowText[1], "");

				if (FindWindow(NULL, "PDW Resolution"))	// Check if there is a "PDW Resolution" dialog
				{
					PostMessage(FindWindow(NULL, "PDW Resolution"), WM_CLOSE, 0, 0L);	// Close it
				}

				if (bAutoRecording)
				{
					AutoRecording(); // PH: temp/test
				}
				break;

				case CLICK_TIMER:	// Handle click timer

				KillTimer(ghWnd, CLICK_TIMER); // Stop Click Timer
				iMouseClick=0;

				break;
			}
		}
		break;

		case WM_POWERBROADCAST :

		OUTPUTDEBUGMSG((("WM_POWERBROADCAST PowerMode = 0x%08lX Data 0x%08lX\n"), (DWORD) wParam, (DWORD) lParam));

		if (bWin9x)
		{
			switch(wParam)
			{
				case PBT_APMRESUMECRITICAL :
				case PBT_APMRESUMESUSPEND :
				case PBT_APMRESUMESTANDBY :
				case PBT_APMRESUMEAUTOMATIC :

				OUTPUTDEBUGMSG((("Out of Suspend: Reload VxD!\n")));
				UnloadDriver();
				pd_reset_all();

				if (LoadDriver())
				{
					SetTimer(ghWnd, PDW_TIMER, 100, (TIMERPROC) NULL);
				}
				break ;

				default:
				break ;
			}
		}
		break ;

		case WM_PAINT: // Keep main gfx data valid

		PAINTSTRUCT pdw_ps;
		BeginPaint(hWnd,&pdw_ps);
		DrawSigInd(hWnd);			// Draw the signal indicator
		DrawTitleBarGfx(hWnd);		// Draw pane1/pane2 title bars
		EndPaint(hWnd,&pdw_ps);

		break;

		case WM_NOTIFY:	// If we receive this message, need to get toolbar tooltip text

		switch (((LPNMHDR)lParam)->code)
		{
			case TTN_NEEDTEXT:
			SetToolTXT(ghInstance, lParam);
			break;

			default:
			DrawSigInd(hWnd); // Draw the signal indicator
			break;
		}
		DrawTitleBarGfx(hWnd);		// Draw pane1/pane2 title bars
		break;
		
		case WM_CREATE:

		Pane1.buff_char  = NULL;
		Pane1.buff_color = NULL;
		Pane2.buff_char  = NULL;
		Pane2.buff_color = NULL;

		if (!(LoadSigInd(ghInstance))) return(-1);

		if (!(GetSysObjects(hWnd)))// Get general purpose drawing objects.
		{
			Free_Common_Objects();	// Free any objects we got!
			return(-1);					// These use standard system colors.
		}

		if (!(GetLogFONTS()))		// Get general purpose font objects.
		{
			Free_Common_Objects();	// Free any objects we got!
			return(-1);
		}

		if (!(hToolbar = (HWND)ShowMakeToolBar(hWnd,ghInstance)))	// Display tool bar
		{
			Free_Common_Objects();	// Free any objects we got!
			return(-1);
		}

		hfont = CreateFontIndirect(&Profile.fontInfo);

		ghDC = GetDC(hWnd);
		SelectObject(ghDC, hfont);
		GetTextMetrics(ghDC, &g_tm);

		cxChar = g_tm.tmAveCharWidth;
		cxCaps = (g_tm.tmPitchAndFamily & 1 ? 3 : 2) * cxChar / 2;
		cyChar = g_tm.tmHeight + g_tm.tmExternalLeading;

		ReleaseDC(hWnd, ghDC);

		g_mem_size = (Profile.pane1_size+1) * (LINE_SIZE+1);
		Pane1.buff_char  = (char *)malloc(g_mem_size);
		Pane1.buff_color = (BYTE *)malloc(g_mem_size);

		if ((Pane1.buff_char == NULL) || (Pane1.buff_color == NULL))
		{
			if (Pane1.buff_char  != NULL) free (Pane1.buff_char);
			if (Pane1.buff_color != NULL) free (Pane1.buff_color);

			Pane1.buff_char  = NULL;
			Pane1.buff_color = NULL;

			Profile.pane1_size = PANE1_SIZE;
			g_mem_size = (Profile.pane1_size+1) * (LINE_SIZE+1);
			Pane1.buff_char = (char *)malloc(g_mem_size);

			if (Pane1.buff_char == NULL)
			{
				Free_Common_Objects();	// Free any objects we got!
				return(-1);
			}
			Pane1.buff_color = (BYTE *)malloc(g_mem_size);

			if (Pane1.buff_color == NULL)
			{
				Free_Common_Objects();	// Free any objects we got!
				return(-1);
			}
		}

		Pane1.buff_lines = Profile.pane1_size;

		g_mem_size = (Profile.pane2_size+1) * (LINE_SIZE+1);
		Pane2.buff_char  = (char *)malloc(g_mem_size);
		Pane2.buff_color = (BYTE *)malloc(g_mem_size);

		if ((Pane2.buff_char == NULL) || (Pane2.buff_color == NULL))
		{
			if (Pane2.buff_char  != NULL) free (Pane2.buff_char);
			if (Pane2.buff_color != NULL) free (Pane2.buff_color);

			Pane2.buff_char  = NULL;
			Pane2.buff_color = NULL;

			Profile.pane2_size = PANE2_SIZE;
			g_mem_size = (Profile.pane2_size+1) * (LINE_SIZE+1);
			Pane2.buff_char = (char *)malloc(g_mem_size);

			if (Pane2.buff_char == NULL)
			{
				Free_Common_Objects();	// Free any objects we got!
				return(-1);
			}
			Pane2.buff_color = (BYTE *)malloc(g_mem_size);

			if (Pane2.buff_color == NULL)
			{
				Free_Common_Objects();	// Free any objects we got!
				return(-1);
			}
		}

		Pane2.buff_lines = Profile.pane2_size;

		InitializePane(&Pane1);
		InitializePane(&Pane2);

		Pane1.hWnd = CreateWindow(gszPane1Class, NULL, WS_CHILD | WS_VISIBLE, 0,0,0,0, hWnd,
								  NULL, (HINSTANCE) GetWindowLong(hWnd, GWL_HINSTANCE), 0);
		if (Pane1.hWnd == NULL)
		{
			Free_Common_Objects();  // Free any objects we got!
			return(-1);
		}

		Pane2.hWnd = CreateWindow(gszPane2Class, NULL, WS_CHILD | WS_VISIBLE, 0,0,0,0, hWnd,
								  NULL, (HINSTANCE) GetWindowLong(hWnd, GWL_HINSTANCE), 0);

		if (Pane2.hWnd == NULL)
		{
			Free_Common_Objects();  // Free any objects we got!
			return(-1);
		}
		ClearPanes(true, true);

		setupecc();

		phase_A.ppp='A';		// tag each phase
		phase_B.ppp='B';
		phase_C.ppp='C';
		phase_D.ppp='D';

		for (g_index=0; g_index<64; g_index++) rcver[g_index] = 0.0;

		rcv_clkt = rcv_clkt_hi;

		// ct is the number of expected clock ticks per bit
		ct1600 = 1.0/(1600.0*838.8e-9);	// 745.1
		ct3200 = 1.0/(3200.0*838.8e-9);	// 372.5
		ct_bit = ct1600;

		for (g_index=0; g_index<16; g_index++)
		{
			custom_colors[g_index] = RGB(255,255,255);  // default to WHITE
		}

		for (g_index=0; g_index < NUM_STAT; g_index++)
		{
			hourly_stat[g_index][STAT_NUMERIC] = 0L;
			hourly_char[g_index][STAT_NUMERIC] = 0L;
			daily_stat [g_index][STAT_NUMERIC] = 0L;
			daily_char [g_index][STAT_NUMERIC] = 0L;

			hourly_stat[g_index][STAT_ALPHA] = 0L;
			hourly_char[g_index][STAT_ALPHA] = 0L;
			daily_stat [g_index][STAT_ALPHA] = 0L;
			daily_char [g_index][STAT_ALPHA] = 0L;
		}

		GetLocalTime(&statTime);
		prev_statTime = statTime;
		stat_timer = 100 - ((statTime.wSecond % 10) * 10);

		// invert audio input-bits?.
		low_audio  = Profile.invert ? DEFAULT_HI_AUDIO : DEFAULT_LO_AUDIO;
		high_audio = Profile.invert ? DEFAULT_LO_AUDIO : DEFAULT_HI_AUDIO;

		pd_reset_all();		// see decode.cpp.

		break;

		case WM_COMMAND:
		{
			switch (LOWORD(wParam))
			{
				case IDM_ENGLISH:     // select English character mapping
				case IDM_ENGLISH+1:   // select Hebrew character mapping
				case IDM_ENGLISH+2:   // select ? character mapping
				case IDM_ENGLISH+3:   // select ? character mapping
				case IDM_ENGLISH+4:   // select ? character mapping
				case IDM_ENGLISH+5:   // select ? character mapping
				case IDM_ENGLISH+6:   // select ? character mapping
				case IDM_ENGLISH+7:   // select ? character mapping
				case IDM_ENGLISH+8:   // select ? character mapping
				case IDM_ENGLISH+9:   // select ? character mapping

					Profile.lang_mi_index = wParam - IDM_ENGLISH;

				break;

				case IDT_TOOLBAR_BTN12: // Cycle data modes

				if (Profile.monitor_paging)			// Paging -> ACARS
				{
					ChangeDataMode(hWnd, MODE_ACARS);
				}
				else if (Profile.monitor_acars)		// ACARS -> Mobitex
				{
					ChangeDataMode(hWnd, MODE_MOBITEX);
				}
				else if (Profile.monitor_mobitex)	// Mobitex -> ERMES
				{
					ChangeDataMode(hWnd, MODE_ERMES);
				}
				else if (Profile.monitor_ermes)		// ERMES -> Paging
				{
					ChangeDataMode(hWnd, MODE_PAGING);
				}
				Profile.FlexGroupMode = 0;

				break;

				case IDM_POCSAGFLEX: // Monitor POCSAG/FLEX signals?

				if (!Profile.monitor_paging)
				{
					ChangeDataMode(hWnd, MODE_PAGING);
				}
				break;

				case IDM_ACARS:      // Monitor ACARS signals?

				if (!Profile.monitor_acars)
				{
					ChangeDataMode(hWnd, MODE_ACARS);
					Profile.FlexGroupMode = 0;
				}
				break;

				case IDM_MOBITEX:      // Monitor Mobitex signals?

				if (!Profile.monitor_mobitex)
				{
					ChangeDataMode(hWnd, MODE_MOBITEX);
					Profile.FlexGroupMode = 0;
				}
				break;
               
				case IDM_ERMES:      // Monitor Ermes signals?

				if (!Profile.monitor_ermes)
				{
					ChangeDataMode(hWnd, MODE_ERMES);
					Profile.FlexGroupMode = 0;
				}
				break;
               
				case IDM_VOLUME: // Run the PDW volume control (Note: runs windows mixer)

					ShellExecute(ghWnd, NULL, "Sndvol32.exe","", "",SW_SHOWNORMAL);

				break;

				case IDT_TOOLBAR_BTN0:
				case IDM_LOGFILE:
						GoModalDialogBoxParam(ghInstance, MAKEINTRESOURCE(LOGFILEDLGBOX),
													 hWnd, (DLGPROC) LogFileDlgProc, 0L);
				break;

				case IDT_TOOLBAR_BTN11:
				case IDM_CLEARDISPLAY:

				GoModalDialogBoxParam(ghInstance, MAKEINTRESOURCE(CLEARSCREENDLGBOX),
										hWnd, (DLGPROC) ClearScreenDlgProc, 0L);
				break;

				case IDM_PLAYBACK:	// RAH

				if (bCapturing)
				{
					MessageBox(ghWnd, "This option can't be used when using the soundcard!", "PDW Playback", MB_ICONWARNING);
					break;
				}
				else if (bPlayback)
				{
					Stop_Playback();
				}
				else if (bRecording)	// HWi
				{
					Stop_Recording();
				}
				if (Open_Recording(&openplayback,szPlayback,true))
				{
					if (Start_Playback(openplayback.lpstrFile))
					{
						SetTimer(ghWnd, PDW_TIMER, 100, (TIMERPROC) NULL);
					}
				}
				break;

                // HWi 
				case IDM_RECORD:	// RAH

				if (bCapturing)
				{
					MessageBox(ghWnd, "This option can't be used when using the soundcard!", "PDW Recording", MB_ICONWARNING);
					break;
				}

				if (bPlayback)	Stop_Playback();
				if (bRecording)	Stop_Recording();

				else if (Open_Recording(&openrecording,szRecording,false))
				{
					Start_Recording(openrecording.lpstrFile);
				}
				break;

				case IDM_AUTORECORD:

				if (bCapturing)
				{
					MessageBox(ghWnd, "This option can't be used when using the soundcard!", "PDW Recording", MB_ICONWARNING);
					break;
				}

				if (!bRecording && !bPlayback) AutoRecording();
				else if (bAutoRecording)
				{
					Stop_Recording();
					bAutoRecording=false;
					MessageBox(ghWnd, "Recording Stopped!", "PDW Recording", MB_ICONINFORMATION);
				}
				break;

				case IDM_EXIT:
					PostMessage(hWnd, WM_CLOSE, 0, 0L);
				break;

				case IDT_TOOLBAR_BTN4:
				case IDM_COPY_SAVE:

				if (bOK_to_save)
				{
					if (GetEditSaveName(hWnd))
					{
						if (Need_Ext(Profile.edit_save_file))
						strcat(Profile.edit_save_file,".txt");
						SaveClipToDisk(Profile.edit_save_file);
					}
				}
				else MessageBox(ghWnd, "Nothing to save!", "Save Clipboard data", MB_ICONWARNING);

				break;

				case IDT_TOOLBAR_BTN5: // Print clip.
				case IDM_COPY_PRINT:

				if (bOK_to_save) PrintClip();
				else MessageBox(ghWnd, "Nothing to print!", "Print Clipboard data", MB_ICONWARNING);

				break;

				case IDT_TOOLBAR_BTN2:
				case IDM_COPY_UPPER:
				g_max_row = min(Pane1.Bottom, Pane1.cyLines);

				if (g_max_row > 0)
				{
					CopyToClipboard(&Pane1, 0,NewLinePoint, 0, g_max_row - 1);
					bOK_to_save=true;
				}
				else MessageBox(ghWnd,"Nothing to copy!", "Copy data",MB_ICONWARNING);

				break;

				case IDT_TOOLBAR_BTN3:
				case IDM_COPY_LOWER:
				g_max_row = min(Pane2.Bottom, Pane2.cyLines);

				if (g_max_row > 0)
				{
					CopyToClipboard(&Pane2, 0,NewLinePoint, 0, g_max_row - 1);
					bOK_to_save=true;
				}
				else MessageBox(ghWnd, "Nothing to copy!", "Copy data",MB_ICONWARNING);

				break;

				case IDT_TOOLBAR_BTN1:
				case IDM_COPY_SELECTION:
				if (select_on)
				{
					select_on = 0;
					selected  = 0;
					selecting = 0;

					InvertSelection();

					g_min_col = min(iSelectionStartCol, iSelectionEndCol);
					g_max_col = max(iSelectionStartCol, iSelectionEndCol);
					g_min_row = min(iSelectionStartRow, iSelectionEndRow);
					g_max_row = max(iSelectionStartRow, iSelectionEndRow);

					CopyToClipboard(select_pane, g_min_col, g_max_col, g_min_row, g_max_row);
					bOK_to_save=true;
				}
				else MessageBox(ghWnd, "Nothing selected to copy!", "Copy data", MB_ICONWARNING);

				break;

				case IDM_INTERFACE:

				int tmp_comPort, tmp_comPortAddr, tmp_comPortIRQ, tmp_comPortRS232;
				int tmp_audioChannels, tmp_audioSampleRate;

				tmp_comPort			= Profile.comPort;
				tmp_comPortAddr		= Profile.comPortAddr;
				tmp_comPortIRQ		= Profile.comPortIRQ;
				tmp_comPortRS232	= Profile.comPortRS232;
				tmp_audioChannels	= 0;
				tmp_audioSampleRate	= Profile.audioSampleRate;

				GoModalDialogBoxParam(ghInstance, MAKEINTRESOURCE(SETUPDLGBOX),
										hWnd, (DLGPROC) SetupDlgProc, 0L);

				if (Profile.comPortEnabled) // User selected serial port?.
				{
					if (bCapturing)		// If currently using sound card then
					{					// close and reset audio device.
						KillTimer(ghWnd, PDW_TIMER);
						Stop_Capturing();
					}

					if (bPlayback)	// RAH: If currently playback active, stop it
					{
						Stop_Playback();
					}

					// check if the COM Port parameters changed
					if (!nDriverLoaded || (tmp_comPort		!= Profile.comPort) ||
										  (tmp_comPortAddr	!= Profile.comPortAddr) ||
										  (tmp_comPortRS232	!= Profile.comPortRS232) ||
										  (tmp_comPortIRQ	!= Profile.comPortIRQ))
					{
						// Dynamically UNLOAD the comport driver
						if (nDriverLoaded)	// HWi
						{
							UnloadDriver();
						}
						pd_reset_all();	// Reset globals for serial port decoding. see "decode.cpp"

						if (LoadDriver())	// HWi
						{
							if (Profile.comPortRS232 && (GetRs232DriverType() == DRIVER_TYPE_SLICER))
							{
								MessageBox(ghWnd, "You have selected an RS232 interface.\nFor best results, de-install Slicer.sys", "PDW Slicer.sys", MB_ICONINFORMATION);
							}
							SetTimer(ghWnd, PDW_TIMER, 100, (TIMERPROC) NULL);
						}
					}
				}
				else if (Profile.audioEnabled) // User selected sound card.
				{
					if (nDriverLoaded)	// HWi
					{
						UnloadDriver();
					}

					// check if audio info has changed
					if (!bCapturing ||  (tmp_audioChannels   != Profile.audioChannels) ||
										(tmp_audioSampleRate != Profile.audioSampleRate))
					{
						if (bCapturing) // close and reset audio device.
						{
							KillTimer(ghWnd, PDW_TIMER);
							Stop_Capturing();
						}

						if (Start_Capturing())
						{
							SetTimer(ghWnd, PDW_TIMER, 100, (TIMERPROC) NULL);
						}
					}
				}
				// If user has not selected the serial port or sound card for input
				// then ensure neither the audio device or serial port is in use.
				else
				{
					if (nDriverLoaded)	// HWi
					{
						UnloadDriver();
					}
					if (bCapturing)		// If currently using sound card then
					{					// close and reset audio device.
						KillTimer(ghWnd, PDW_TIMER);
						Stop_Capturing();
					}
				}
				break;

				case IDT_TOOLBAR_BTN6:
				case IDM_OPTIONS:
					GoModalDialogBoxParam(ghInstance, MAKEINTRESOURCE(OPTIONSDLGBOX),
												 hWnd, (DLGPROC) OptionsDlgProc, 0L);
				break;

				case IDM_FONT:
					while (SelectFont(hWnd) != TRUE) continue;
				break;

				case IDM_COLOR: // Change POCSAG/FLEX/ACARS/MOBITEX/ERMES colors.

				if (Profile.monitor_acars)
				{
					GoModalDialogBoxParam(ghInstance, MAKEINTRESOURCE(ACARS_COLORSDLGBOX),
												 hWnd, (DLGPROC) ACARSColorsDlgProc, 0L);
				}
				else if (Profile.monitor_mobitex)
				{
					GoModalDialogBoxParam(ghInstance, MAKEINTRESOURCE(MOBITEX_COLORSDLGBOX),
												 hWnd, (DLGPROC) MOBITEXColorsDlgProc, 0L);
				}
				else if (Profile.monitor_paging)
				{
					GoModalDialogBoxParam(ghInstance, MAKEINTRESOURCE(COLORSDLGBOX),
												 hWnd, (DLGPROC) ColorsDlgProc, 0L);
				}
				else if (Profile.monitor_ermes)
				{
					GoModalDialogBoxParam(ghInstance, MAKEINTRESOURCE(ERMES_COLORSDLGBOX),
												 hWnd, (DLGPROC) ERMESColorsDlgProc, 0L);                                    
				}
				break;

				case IDM_SCREENOPTIONS:
					GoModalDialogBoxParam(ghInstance, MAKEINTRESOURCE(SCREENOPTIONSDLGBOX),
												 hWnd, (DLGPROC) ScreenOptionsDlgProc, 0L);
				break;

				case IDM_SCROLLBACK:
					GoModalDialogBoxParam(ghInstance, MAKEINTRESOURCE(SCROLLDLGBOX),
												 hWnd, (DLGPROC) ScrollDlgProc, 0L);
				break;

				case IDM_SYSTEMTRAY:

				if (!hSystemTrayDlg)
				{
					GoModalDialogBoxParam(ghInstance, MAKEINTRESOURCE(SYSTEMTRAYDLGBOX),
										  hWnd, (DLGPROC) SystemTrayDlgProc, 0L);
				}
				break;

				case IDT_TOOLBAR_BTN7:
				case IDM_FILTERS:

				if (!hFilterDlg)
				{
					GoModalDialogBoxParam(ghInstance, MAKEINTRESOURCE(FILTERDLGBOX),
										  hWnd, (DLGPROC) FilterDlgProc, 0L);
				}
				break;

				case IDM_FILTEROPTIONS:

				GoModalDialogBoxParam(ghInstance, MAKEINTRESOURCE(FILTEROPTIONSDLGBOX),
									  hWnd, (DLGPROC) FilterOptionsDlgProc, 0L);
				break;

				case IDT_TOOLBAR_BTN9:  // Pause program.

				if (bPauseFlag)
				{
					SetNewWindowText("");
					bPauseFlag=false;
				}
				else
				{
					SetNewWindowText(" - PAUSED...");
					bPauseFlag=true;
				}
				break;

				case IDT_TOOLBAR_BTN8:
				case IDM_MONSTAT:
					GoModalDialogBoxParam(ghInstance, MAKEINTRESOURCE(MONSTATDLGBOX),
												 hWnd, (DLGPROC) MonStatDlgProc, 0L);
				break;

				case IDM_MAIL:
					GoModalDialogBoxParam(ghInstance, MAKEINTRESOURCE(MAIL_DLGBOX),
												 hWnd, (DLGPROC) MailDlgProc, 0L);
				break;

				case IDM_RELOAD:

				if (FileExists(szFilterPathName))
				{
					sprintf(filters_temp, "Old  number  of filters :  %u\n",Profile.filters.size());
					strcpy (filters_reload, filters_temp);
						
					Profile.filters.clear();
							
					if (ReadFilters(szFilterPathName, &Profile, false))
					{
						sprintf(filters_temp, "New number of filters :  %u",Profile.filters.size());
						strcat (filters_reload, filters_temp);

						MessageBox(ghWnd, filters_reload, "PDW Reloaded filters", MB_ICONINFORMATION);
					}
					else
					{
						if (MessageBox(ghWnd, "An error occured while loading filters.ini.\nTry to load a backup instead?", "PDW Filters", MB_ICONQUESTION | MB_OKCANCEL) == IDOK)
						{
							if (ReadFilters(szFilterBackup, &Profile, false) == false)
							{
								MessageBox(ghWnd, "Also failed...", "PDW Filters", MB_ICONINFORMATION);
							}
						}
					}

				}
				break;

				case IDM_RESET_HITCOUNTERS :
					ResetHitcounters(true) ;	// PH: Reset ALL hitcounters
				break;

				case IDT_TOOLBAR_BTN10:
				case IDM_HELP:

					sprintf(szFile, "%s\\PDW.pdf", szPath);

					if (FileExists(szFile)) ShellExecute(NULL, "open", szFile, NULL, NULL, SW_SHOWNORMAL);
					else MessageBox(ghWnd, "Unable to find PDW.pdf ", "PDW Manual", MB_OK|MB_ICONHAND);

					break;

				case IDM_ABOUT:
					GoModalDialogBoxParam(ghInstance, MAKEINTRESOURCE(ABOUTDLGBOX),
												 hWnd, (DLGPROC) AboutDlgProc, 0L);
				break;

				case IDM_DEBUG:

					GoModalDialogBoxParam(ghInstance, MAKEINTRESOURCE(DEBUGDLGBOX),
												 hWnd, (DLGPROC) DebugDlgProc, 0L);
				break;

				case IDM_FILTERFILE_EN:

					Profile.filterfile_enabled ^= 0x01;

				break;

				case IDM_FILTERCOMMANDFILE:

					Profile.filter_cmd_file_enabled ^= 0x01;

				break;

				case IDM_PANE_SWITCH:
					SetWindowPaneSize(PANE_SWITCH);
				break;

				case IDM_GENERAL:

					GoModalDialogBoxParam(ghInstance, MAKEINTRESOURCE(GENERALOPTIONSDLGBOX),
										 hWnd, (DLGPROC) GeneralOptionsDlgProc, 0L);
				break;

				case IDT_MENU_RESTORE :

				if (bTrayed) SystemTrayWindow(false); // PH: Restore PDW from systemtray

				break;
			}
			set_menu_items();
		}
		break;

		case WM_CONTEXTMENU:

		ShowContextMenu(HMENU_MAIN, ghWnd);

		break ;

		case WM_NCMOUSEMOVE:

		if ((wParam == HTMENU) || (wParam == HTCAPTION))
		{
			hCurrentHWND=ghWnd;
		}
		break;

		case WM_MOUSEMOVE:

		hCurrentHWND = hWnd;
		
		break;

		case WM_MOUSEWHEEL:

		if (hCurrentHWND != ghWnd) // Don't scroll if not inside Pane1/Pane2
		{
			OnMouseWheel(wParam, LOWORD(lParam), HIWORD(lParam), g_rect);
		}
		break;

		case WM_VSCROLL:

		if (Profile.ScrollSpeed)
		{
			SendMessage(hCurrentHWND, WM_VSCROLL, wParam, Profile.ScrollSpeed);
		}
		else SendMessage(hCurrentHWND, WM_VSCROLL, wParam, 0L);

		break;

		case WM_KEYDOWN: // PH: Changed and added VK_SHIFT for Pane2 (12-11-2003)

		if (GetKeyState(VK_ESCAPE) & 0x80)
		{
			sndPlaySound(NULL, 0);	// Stop WAV file, if playing
		}

		if (GetKeyState(VK_F7) & 0x80)
		{
			sprintf(szFile, "%s\\LogBrowser.exe", Profile.LogfilePath);
			if (FileExists(szFile)) ShellExecute(NULL, "open", szFile, NULL, NULL, SW_SHOWNORMAL);
		}

		if (GetKeyState(VK_F4) & 0x80)
		{
			if (Profile.SystemTray) SystemTrayWindow(false);
			break;
		}

		pane1 = (GetKeyState(VK_SHIFT) & 0x80) ? false : true;
		pane1 = (Profile.percent < 50) ? !pane1 : pane1;

		if (GetKeyState(VK_HOME)  & 0x80) SendMessage(pane1 ? Pane1.hWnd : Pane2.hWnd, WM_VSCROLL, SB_TOP, 0L);
		if (GetKeyState(VK_END)   & 0x80) SendMessage(pane1 ? Pane1.hWnd : Pane2.hWnd, WM_VSCROLL, SB_BOTTOM, 0L);
		if (GetKeyState(VK_PRIOR) & 0x80) SendMessage(pane1 ? Pane1.hWnd : Pane2.hWnd, WM_VSCROLL, SB_PAGEUP, 0L);
		if (GetKeyState(VK_NEXT)  & 0x80) SendMessage(pane1 ? Pane1.hWnd : Pane2.hWnd, WM_VSCROLL, SB_PAGEDOWN, 0L);
		if (GetKeyState(VK_SPACE) & 0x80) SendMessage(pane1 ? Pane1.hWnd : Pane2.hWnd, WM_VSCROLL, SB_PAGEDOWN, 0L);
		if (GetKeyState(VK_UP)    & 0x80) SendMessage(pane1 ? Pane1.hWnd : Pane2.hWnd, WM_VSCROLL, SB_LINEUP, 0L);
		if (GetKeyState(VK_DOWN)  & 0x80) SendMessage(pane1 ? Pane1.hWnd : Pane2.hWnd, WM_VSCROLL, SB_LINEDOWN, 0L);
		if (GetKeyState(VK_LEFT)  & 0x80) SendMessage(pane1 ? Pane1.hWnd : Pane2.hWnd, WM_HSCROLL, SB_PAGEUP, 0L);
		if (GetKeyState(VK_RIGHT) & 0x80) SendMessage(pane1 ? Pane1.hWnd : Pane2.hWnd, WM_HSCROLL, SB_PAGEDOWN, 0L);

		break;

		case WM_MOVE:	
		{
			GetWindowRect(hWnd, &g_rect);
			Profile.xPos = (INT) g_rect.left;
			Profile.yPos = (INT) g_rect.top;
		}
		break;

	    case SYSTEMTRAY_ICON_MESSAGE:

		switch(lParam)
        {
			case WM_LBUTTONDBLCLK:

			if (bTrayed) SystemTrayWindow(false); // PH: Restore PDW from systemtray

			break;

			case WM_CONTEXTMENU:
			case WM_RBUTTONUP:
			{
				ShowContextMenu(HMENU_SYSTRAY, ghWnd);
				SetForegroundWindow(ghWnd);
				PostMessage(ghWnd, WM_NULL, 0, 0);
			}
			break;
		}

		case WM_GETMINMAXINFO:

		if (bTrayed || lParam < 1000) break; // FIX: Don't handle WM_GETMINMAXINFO messages if trayed

		lpMMI=(LPMINMAXINFO)lParam;
		lpMMI->ptMinTrackSize.x = GetSystemMetrics(SM_CXFULLSCREEN) / 2; // Window's Minimum X-size
		lpMMI->ptMinTrackSize.y = GetSystemMetrics(SM_CYFULLSCREEN) / 3; // Window's Minimum Y-size

		break;

		case WM_SYSCOMMAND:

		if (wParam == SC_MINIMIZE)
		{
			if (Profile.SystemTray && !bTrayed)
			{
				SystemTrayWindow(false); // PH: Move PDW to systemtray
				break;
			}
		}
		return (DefWindowProc(hWnd, uMsg, wParam, lParam));

		case WM_SIZE:

		if (bTrayed) break;		// FIX: Don't handle WM_SIZE messages if trayed

		if (wParam != SIZE_MINIMIZED)
		{
			GetWindowRect(hWnd, &g_rect);

			if (wParam == SIZE_RESTORED)
			{
				if (Profile.minimize_flg)			// no longer maximized/minimized?
				{
					Profile.minimize_flg=0;
				}
				else if (Profile.maximize_flg)		// no longer maximized/minimized?
				{
					Profile.maximize_flg=0;			// HWi Fix 
				}
			}

			// see if setting beyond minimal size
			if (Pane1.iHscrollMax != 0) g_scrollSize = GetSystemMetrics(SM_CYHSCROLL);
			else						g_scrollSize = 0;

			// Is main win Y size to small?
			if ((g_rect.bottom - g_rect.top) < MIN_Y_WIN_SIZE)
			{
				MoveWindow(hWnd, Profile.xPos, Profile.yPos,
                          (g_rect.right - g_rect.left), MIN_Y_WIN_SIZE, TRUE);
			}

			// Is main win X size to small?
			if ((g_rect.right - g_rect.left) < MIN_X_WIN_SIZE)
			{
				MoveWindow(hWnd, Profile.xPos, Profile.yPos,
                           MIN_X_WIN_SIZE, (g_rect.bottom - g_rect.top), TRUE);
			}

			if (wParam != SIZE_MAXIMIZED)
			{
				// Get the limits of the 'workarea' (exclude tray area for WIN95)
				if (!SystemParametersInfo(SPI_GETWORKAREA, sizeof(RECT), &g_sys_rect, 0 ))
				{
					g_sys_rect.left   = g_sys_rect.top = 0;
					g_sys_rect.right  = GetSystemMetrics(SM_CXSCREEN);
					g_sys_rect.bottom = GetSystemMetrics(SM_CYSCREEN);
				}
				if (((g_rect.right  - g_rect.left) > g_sys_rect.right) ||
					((g_rect.bottom - g_rect.top)  > g_sys_rect.bottom))
				{
					g_xNew = min(g_rect.right  - g_rect.left, g_sys_rect.right);
					g_yNew = min(g_rect.bottom - g_rect.top,  g_sys_rect.bottom);
					MoveWindow(hWnd, Profile.xPos, Profile.yPos, g_xNew, g_yNew, TRUE);
				}
			}
			else
			{
				PL1_SCount=0; // Reset pane1 label scroll position
				PL2_SCount=0; // Reset pane2 label scroll position
				Profile.maximize_flg=1; // Keep track of maximize.
			}
			sizeSet = 1;

			GetClientRect(hWnd, &g_rect);
			g_xNew = g_rect.right - g_rect.left;   // Width of client area
			g_yNew = g_rect.bottom - g_rect.top;   // Height of client area
			g_yNew -= TOOLBAR_SIZE+WIN_DIVIDER_SIZE; // Allow space for tool bar etc

			// The following code sets pane1 to n% percent of main win client area.
			pane1Pos    = TOOLBAR_SIZE+1;
			pane1Height = (g_yNew * Profile.percent) / 100;
			pane1Height /= cyChar;	// Divide by character height to get number of lines (without 'rest')
			pane1Height *= cyChar;	// Multiply by character height again
			MoveWindow(Pane1.hWnd, 0, pane1Pos, g_xNew, pane1Height, TRUE);

			// The following code sets pane2 to n% percent of main win client area,
			// it also allows for toolbar space and dividing space between both panes.
			pane2Pos = TOOLBAR_SIZE+pane1Height+WIN_DIVIDER_SIZE+1;
			pane2Height = (g_yNew - pane1Height)-2;
			MoveWindow(Pane2.hWnd, 0, pane2Pos, g_xNew, pane2Height, TRUE);

			GetWindowRect(Pane1.hWnd, &g_rect);
			pane1Top = g_rect.top;

			GetWindowRect(hWnd, &g_rect);
			Profile.xSize = g_rect.right  - g_rect.left;
			Profile.ySize = g_rect.bottom - g_rect.top;

			SetWindowPaneSize(WINDOW_SIZE);	// PH: Set Max_X_Client / iMaxWidth / NewLinePoint

			SendMessage(Pane1.hWnd, WM_VSCROLL, SB_BOTTOM, 0L);
			SendMessage(Pane2.hWnd, WM_VSCROLL, SB_BOTTOM, 0L);

			TB_AutoSize(hToolbar);	// keep toolbar correct size!
		}
		break;

		case WM_DESTROY:

		KillTimer(ghWnd, PDW_TIMER);
		KillTimer(ghWnd, MINUTE_TIMER);
		KillTimer(ghWnd, SECOND_TIMER);

		if (pLogFile)    fclose(pLogFile);
		if (pFilterFile) fclose(pFilterFile);
		if (pStatFile)   fclose(pStatFile);

		if (Profile.minimize_flg)	// Make sure PDW doesn't open minimized next time.
		{
			Profile.xPos = 0;
			Profile.yPos = 0;

			if (Profile.maximize_flg)
			{
				Profile.xSize = 593;
				Profile.ySize = 442;
			}
		}
		WriteSettings();

		if (Profile.SystemTray) SystemTrayIcon(true);	// Remove PDW-icon from systemtray
		if (bUpdateFilters) WriteFilters(&Profile, 0);	// Save FILTERS.INI

		// Free all drawing objects(gfx.cpp)
		// Free all tool bar images.
		// Free all message buffers.
		Free_Common_Objects();
		free_lang_tables();	// Free language tables.
		acars.free_data();	// Free acars database info.

		if (nDriverLoaded)	// HWi
		{
			UnloadDriver();
		}

		// HWi, stop Mail thread.....
		MailInit(NULL, NULL, NULL, NULL, NULL, NULL, 0, 0);

		if (bCapturing)	Stop_Capturing();		// Reset and close audio device.
		if (bPlayback)	Stop_Playback();		// RAH: stop playback
		if (bRecording) Stop_Recording();		// HWi Added, seems to be missing

//		if (em.symbol_fp) fclose(em.symbol_fp);	// ermes debug data file.
		if (pd_raw_fp)    fclose(pd_raw_fp);	// pocsag/flex debug data file.

		PostQuitMessage(0);

		break;

		case WM_CLOSE:

		if (Profile.confirmExit)
		{
			if (MessageBox(ghWnd, "Exit PDW - Sure?", "PDW Exit",
				           MB_ICONQUESTION | MB_OKCANCEL) == IDCANCEL) break;
		}
		// fall through

		default:
		return (DefWindowProc(hWnd, uMsg, wParam, lParam));
   }
   return 0L;

} // end of PDWWndProc()


void ChangeDataMode(HWND hWnd, int mode)
{
	Profile.monitor_paging  = false;
	Profile.monitor_acars   = false;
	Profile.monitor_mobitex = false;
	Profile.monitor_ermes   = false;

	switch (mode)
	{
		default:
		case MODE_PAGING:
			Profile.monitor_paging = true;
			strcpy(szWindowText[2], "POCSAG/FLEX...");
		break;
		
		case MODE_ACARS:
			Profile.monitor_acars = true;
			strcpy(szWindowText[2], "ACARS...");
		break;
		
		case MODE_MOBITEX:
			Profile.monitor_mobitex = true;
			strcpy(szWindowText[2], "MOBITEX...");
		break;
		
		case MODE_ERMES:
			Profile.monitor_ermes = true;
			strcpy(szWindowText[2], "ERMES...");
		break;
	}
	set_menu_items();
	Reset_ATB();            // Reset audio input variables.
	pd_reset_all();         // Reset serial port variables.
	SetNewWindowText("");
}


// Clear both pane 1 and pane2.
// Used with 'Clear Display' menu item + 'Font' menu item.
void ClearPanes(bool bPane1, bool bPane2)
{
	if (!bPane1 && !bPane2) return;

	if (bPane1)
	{
		InitializePane(&Pane1);
		InvalidateRect(Pane1.hWnd, NULL, TRUE);
	}
	if (bPane2)
	{
		InitializePane(&Pane2);
		InvalidateRect(Pane2.hWnd, NULL, TRUE);
	}
	SetNewWindowText("");
}


BOOL FAR PASCAL ClearScreenDlgProc(HWND hDlg, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	int panes=0;

	static bool pane1=true,pane2=true;
	
	switch (uMsg)
	{
		case WM_INITDIALOG:

		if (!CenterWindow(hDlg)) return (FALSE);

		CheckDlgButton(hDlg, IDC_CLEAR_PANE1, pane1);
		CheckDlgButton(hDlg, IDC_CLEAR_PANE2, pane2);

		return (TRUE);

		case WM_COMMAND:
		switch (LOWORD(wParam))
		{
			case IDC_CLEAR_PANE1:

			pane1 = !pane1;

			break;

			case IDC_CLEAR_PANE2:

			pane2 = !pane2;

			break;

			case IDOK:

			ClearPanes(pane1, pane2);

			EndDialog(hDlg, TRUE);
	
			return (TRUE);

			case IDCANCEL:
			EndDialog(hDlg, TRUE);
			return (TRUE);
		}
		break;
	}
	return (FALSE);
} // end of ClearScreenDlgProc


LRESULT FAR PASCAL Pane1WndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	RECT  rect;
	RECT  parent_rect;
	RECT  client_rect;
	int   scrollSize, scroll_amt, scroll_chars;
	int   iVscrollInc, iPosition;
	static int iLastPosition=0;

	switch (uMsg)
	{
		case WM_CREATE:
		break;

		case WM_SIZE:

		if (wParam != SIZE_MINIMIZED)
		{
			Pane1.cxClient = LOWORD(lParam);
			Pane1.cyClient = HIWORD(lParam);

			GetClientRect(GetParent(hWnd), &parent_rect);
			GetWindowRect(hWnd, &rect);

			// Keep client area at a minimum of one line!.
			if (Pane1.iHscrollMax) scrollSize = GetSystemMetrics(SM_CYHSCROLL);
			else                   scrollSize = 0;

			if (Pane1.cyClient < (cyChar + scrollSize))
			{
				pane1Height = 2 * GetSystemMetrics(SM_CYFRAME) + cyChar + scrollSize + 1;
				pane1Pos = TOOLBAR_SIZE;

				MoveWindow(hWnd, 0, pane1Pos, parent_rect.right, pane1Height, TRUE);

				GetWindowRect(hWnd, &rect);
				pane1Top = rect.top;

				GetClientRect(hWnd, &client_rect);
				Pane1.cxClient = client_rect.right;
				Pane1.cyClient = client_rect.bottom;
			}

			iVscrollInc = Pane2.iVscrollMax - Pane2.iVscrollPos;
			iVscrollInc = max (-(Pane2.iVscrollPos),
			min(iVscrollInc, Pane2.iVscrollMax - Pane2.iVscrollPos));

			if (iVscrollInc)
			{
				Pane2.iVscrollPos += iVscrollInc;
				scroll_amt = cyChar * iVscrollInc;
			
				// Scroll pane 2.
				ScrollWindow  (Pane2.hWnd, 0, -scroll_amt, NULL, NULL);
				SetScrollRange(Pane2.hWnd, SB_VERT, 0, Pane2.iVscrollMax, FALSE);
				SetScrollPos  (Pane2.hWnd, SB_VERT, Pane2.iVscrollPos, TRUE);
				UpdateWindow  (Pane2.hWnd);
			}

			Pane1.cyLines = Pane1.cyClient / cyChar;
			Pane1.iVscrollMax = max(0, Pane1.Bottom - Pane1.cyLines);

			if (select_on && selected && (&Pane1 == select_pane))
			{
				iSelectionStartRow -= (Pane1.iVscrollMax-Pane1.iVscrollPos);	// also scroll selection
				iSelectionEndRow   -= (Pane1.iVscrollMax-Pane1.iVscrollPos);

				InvertSelection();
			}
			Pane1.iVscrollPos = min(Pane1.iVscrollPos, Pane1.iVscrollMax);

			iVscrollInc = Pane1.iVscrollMax - Pane1.iVscrollPos;
			iVscrollInc = max (-(Pane1.iVscrollPos),
			min(iVscrollInc, Pane1.iVscrollMax - Pane1.iVscrollPos));

			if (iVscrollInc)
			{
				Pane1.iVscrollPos += iVscrollInc;
				scroll_amt = cyChar * iVscrollInc;

				// Scroll pane1
				ScrollWindow  (Pane1.hWnd, 0, -scroll_amt, NULL, NULL);
				SetScrollRange(Pane1.hWnd, SB_VERT, 0, Pane1.iVscrollMax, FALSE);
				SetScrollPos  (Pane1.hWnd, SB_VERT, Pane1.iVscrollPos, TRUE);
				UpdateWindow  (Pane1.hWnd);
			}

			scroll_chars = (iMaxWidth - Pane1.cxClient);
			scroll_chars = scroll_chars / (signed int)(cxChar);
			Pane1.iHscrollMax = max(0, scroll_chars + 2);
			Pane1.iHscrollPos = min(Pane1.iHscrollPos, Pane1.iHscrollMax);

			// is User adjusting win x size beyond endof max text line and scroll
			// position not 0, then adjust pane1 label fields to new text position.
			if (PL1_SCount)
			{
				PL1_SCount=0;
				PL1_SCount =- (Pane1.iHscrollPos * cxChar);
				DrawPaneLabels(ghWnd, PANE1);
			}

			if (Pane1.Bottom)
			{
				SetScrollRange(hWnd, SB_HORZ, 0, Pane1.iHscrollMax, FALSE);
				SetScrollPos  (hWnd, SB_HORZ, Pane1.iHscrollPos, TRUE);
			}

			rect.top    = 0;
			rect.bottom = Pane1.cyClient;
			rect.left   = 0;
			rect.right  = Pane1.cxClient;

			InvalidateRect(hWnd, &rect, TRUE);

			UpdateWindow(hWnd);
		}
		break;

		case WM_LBUTTONDOWN:

		if (select_on && selected) InvertSelection();

		// get the new start position
		iSelectionStartCol = LOWORD(lParam) / cxChar;
		iSelectionEndCol   = iSelectionStartCol;
		iSelectionStartRow = HIWORD(lParam) / cyChar;
		iSelectionEndRow   = iSelectionStartRow;

		SetCapture(hWnd);
		select_on   = 1;
		selecting   = 1;
		selected    = 0;
		select_pane = &Pane1;

		break;

		case WM_MOUSEMOVE:

		if (select_on && selecting)
		{
			if ((iSelectionEndCol != (LOWORD(lParam) / cxChar)) ||
				(iSelectionEndRow != (HIWORD(lParam) / cyChar)))
			{
				if (selected) InvertSelection();

				iSelectionEndCol = LOWORD(lParam) / cxChar;
				iSelectionEndRow = HIWORD(lParam) / cyChar;

				InvertSelection();
				selected = 1;
			}
		}

		// fall through:

		case WM_NCMOUSEMOVE:

		hCurrentHWND = hWnd;

		break;

		case WM_LBUTTONUP:

		if (iMouseClick && (iLastPosition == (LOWORD(lParam)*HIWORD(lParam))))
		{
			bLBUTTONDBLCLK=true;
		}
		else
		{
			iMouseClick=SetTimer(ghWnd, CLICK_TIMER, GetDoubleClickTime(), (TIMERPROC) NULL); // start click timer
			iLastPosition=LOWORD(lParam)*HIWORD(lParam);
		}

		if (bLBUTTONDBLCLK)
		{
			iPosition = LOWORD(lParam) / cxChar;
			iLastPosition=0;

			SelectByDoubleClick(hWnd, &Pane1, iPosition, HIWORD(lParam) / cyChar);
			GoogleMaps(iPosition);

			bLBUTTONDBLCLK=false;
		}
		ReleaseCapture();
		selecting = 0;

		break;

		case WM_PAINT:
		PanePaint(&Pane1);
//		DrawPaneLabels(ghWnd, PANE1);
		break;

		case WM_HSCROLL:
		PaneHScroll(&Pane1, wParam);
		break;

		case WM_VSCROLL:
		PaneVScroll(&Pane1, wParam, lParam);
		break;

		case WM_CLOSE: // fall through

		default:
		return (DefWindowProc(hWnd, uMsg, wParam, lParam));
	}
	return 0L;
} // end of Pane1WndProc


LRESULT FAR PASCAL Pane2WndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	RECT rect;
	int  scroll_chars, iPosition;
	static int iLastPosition=0;

	switch (uMsg)
	{
		case WM_CREATE:
		break;

		case WM_SIZE:

		if (wParam != SIZE_MINIMIZED)
		{
			Pane2.cxClient = LOWORD(lParam);
			Pane2.cyClient = HIWORD(lParam);

			Pane2.cyLines = Pane2.cyClient / cyChar;

				Pane2.iVscrollMax = max(0, Pane2.Bottom  - Pane2.cyLines);
				if (select_on && selected && (&Pane2 == select_pane))
				{
					iSelectionStartRow -= (Pane2.iVscrollMax-Pane2.iVscrollPos);	// also scroll selection
					iSelectionEndRow   -= (Pane2.iVscrollMax-Pane2.iVscrollPos);

					InvertSelection();
				}
				Pane2.iVscrollPos = min(Pane2.iVscrollPos, Pane2.iVscrollMax);

			SetScrollRange(hWnd, SB_VERT, 0, Pane2.iVscrollMax, FALSE);
			SetScrollPos  (hWnd, SB_VERT, Pane2.iVscrollPos, TRUE);

			scroll_chars = (iMaxWidth - Pane2.cxClient);
			scroll_chars = scroll_chars / (signed int)(cxChar);
			Pane2.iHscrollMax = max (0, scroll_chars + 2);
			Pane2.iHscrollPos = min (Pane2.iHscrollPos, Pane2.iHscrollMax);

			// is User adjusting win x size beyond endof max text line and scroll position not 0,
			// then adjust pane2 label fields to new text position.

			if (PL2_SCount)
			{
				PL2_SCount=0;
				PL2_SCount = -(Pane2.iHscrollPos * cxChar);
				DrawPaneLabels(ghWnd, PANE2);
			}

			if (Pane2.Bottom)
			{
				SetScrollRange(hWnd, SB_HORZ, 0, Pane2.iHscrollMax, FALSE);
				SetScrollPos  (hWnd, SB_HORZ, Pane2.iHscrollPos, TRUE);
			}

			rect.top    = 0;
			rect.bottom = Pane2.cyClient;
			rect.left   = 0;
			rect.right  = Pane2.cxClient;

			InvalidateRect(hWnd, &rect, TRUE);

			UpdateWindow(hWnd);
		}
		break;

		case WM_LBUTTONDOWN:

		if (select_on && selected) InvertSelection();

		// get the new start position
		iSelectionStartCol = LOWORD(lParam) / cxChar;
		iSelectionEndCol   = iSelectionStartCol;
		iSelectionStartRow = HIWORD(lParam) / cyChar;
		iSelectionEndRow   = iSelectionStartRow;

		SetCapture(hWnd);
		select_on = 1;
		selecting = 1;
		selected  = 0;
		select_pane = &Pane2;

		break;

		case WM_MOUSEMOVE:

		if (select_on && selecting)
		{
			if ((iSelectionEndCol != (LOWORD(lParam) / cxChar)) ||
				(iSelectionEndRow != (HIWORD(lParam) / cyChar)))
			{
				if (selected) InvertSelection();

				iSelectionEndCol = LOWORD(lParam) / cxChar;
				iSelectionEndRow = HIWORD(lParam) / cyChar;

				InvertSelection();
				selected = 1;
			}
		}

		// fall through:

		case WM_NCMOUSEMOVE:

		hCurrentHWND = hWnd;

		break;

		case WM_LBUTTONUP:

		if (iMouseClick && (iLastPosition == (LOWORD(lParam)*HIWORD(lParam))))
		{
			bLBUTTONDBLCLK=true;
		}
		else
		{
			iMouseClick=SetTimer(ghWnd, CLICK_TIMER, GetDoubleClickTime(), (TIMERPROC) NULL); // start click timer
			iLastPosition=LOWORD(lParam)*HIWORD(lParam);
		}

		if (bLBUTTONDBLCLK)
		{
			iPosition = LOWORD(lParam) / cxChar;
			iLastPosition=0;

			// Select text below double mouse click
			SelectByDoubleClick(hWnd, &Pane2, iPosition, HIWORD(lParam) / cyChar);

			bLBUTTONDBLCLK=false;
		}
		ReleaseCapture();
		selecting = 0;

		break;

		case WM_PAINT:
		PanePaint(&Pane2);
		break;

		case WM_HSCROLL:
		PaneHScroll(&Pane2, wParam);
		break;

		case WM_VSCROLL:
		PaneVScroll(&Pane2, wParam, lParam);
		break;

		case WM_CLOSE: // fall through

		default:
		return (DefWindowProc(hWnd, uMsg, wParam, lParam));
	}
	return 0L;
} // end of Pane2WndProc


void InvertSelection(void)
{
	HDC	hDC;
	RECT	rect;
	unsigned int min_col, max_col, min_row, max_row;

	hDC = GetDC(select_pane->hWnd);

	min_col = min(iSelectionStartCol, iSelectionEndCol);
	max_col = max(iSelectionStartCol, iSelectionEndCol);
	min_row = min(iSelectionStartRow, iSelectionEndRow);
	max_row = max(iSelectionStartRow, iSelectionEndRow);

	rect.left   = min_col * cxChar;
	rect.top    = min_row * cyChar;
	rect.right  = (max_col + 1) * cxChar - 1;
	rect.bottom = (max_row + 1) * cyChar - 1;

	InvertRect(hDC, &rect);
	ReleaseDC(select_pane->hWnd, hDC);

	return;
}


void SaveClipToDisk(char *fname)
{
	HGLOBAL hClipBuffer;
	LPSTR lpClipBuffer;
	FILE *pClip=NULL;
	int i=0;

	if (!OpenClipboard(ghWnd))
	{
		MessageBox(ghWnd,"Could not open Clipboard!", "PDW Clipboard",MB_ICONWARNING);
		return;
	}

	if (!(hClipBuffer=(HGLOBAL)GetClipboardData(CF_TEXT)))
	{
		MessageBox(ghWnd,"Could not get Clipboard Data!", "PDW Clipboard",MB_ICONWARNING);
		CloseClipboard();
		return;
	}

	if (!(lpClipBuffer = (LPTSTR) GlobalLock(hClipBuffer)))
	{
		GlobalFree(hClipBuffer);

		MessageBox(ghWnd,"Could not lock Clipboard Memory!", "PDW Clipboard",MB_ICONWARNING);
		CloseClipboard();
		return;
	}

	if (!(pClip = fopen(fname, "a")))
	{
		MessageBox(ghWnd,"Could not open output file!", "PDW Clipboard",MB_ICONWARNING);
		GlobalUnlock(hClipBuffer);
		CloseClipboard();
		return;
	}

	while ((*lpClipBuffer) && (i < 15000))
	{
		fwrite((LPSTR)lpClipBuffer++, 1, 1, pClip);
		i++;
	}

	GlobalUnlock(hClipBuffer);
	fclose(pClip);
	CloseClipboard();
}


void PrintClip(void)
{
	HGLOBAL hClipBuffer;
	LPSTR lpClipBuffer;

	if (!OpenClipboard(ghWnd))
	{
		MessageBox(ghWnd,"Could not open Clipboard!", "PDW Clipboard",MB_ICONWARNING);
		return;
	}

	if (!(hClipBuffer=(HGLOBAL)GetClipboardData(CF_TEXT)))
	{
		MessageBox(ghWnd,"Could not get Clipboard Data!", "PDW Clipboard",MB_ICONWARNING);
		CloseClipboard();
		return;
	}

	if (!(lpClipBuffer = (LPTSTR) GlobalLock(hClipBuffer)))
	{
		GlobalFree(hClipBuffer);

		MessageBox(ghWnd,"Could not lock Clipboard Memory!", "PDW Clipboard",MB_ICONWARNING);
		CloseClipboard();
		return;
	}

	PrintBuffer(lpClipBuffer); // Send clip to printer.
	GlobalUnlock(hClipBuffer);
	CloseClipboard();
}


void CopyToClipboard(PaneStruct *pane, UINT min_col, UINT max_col, UINT min_row, UINT max_row)
{
	unsigned int num_lines, index, line_no;
	unsigned int offset, xx;
	HGLOBAL	hClipBuffer;
	LPTSTR	lpClipBuffer;
	char	*pchar;

	if (!OpenClipboard(pane->hWnd))
	{
		MessageBox(ghWnd, "Could not open Clipboard!", "PDW Clipboard",MB_ICONWARNING);
		return;
	}

	if (!EmptyClipboard())
	{
		MessageBox(ghWnd,"Could not empty Clipboard!", "PDW Clipboard",MB_ICONWARNING);
		CloseClipboard();
		return;
	}

	num_lines = (max_col - min_col + 1);

	if (!(hClipBuffer = GlobalAlloc(GMEM_DDESHARE, num_lines * (LINE_SIZE+3))))
	{
		MessageBox(ghWnd,"Could not get Clipboard Memory!", "PDW Clipboard",MB_ICONWARNING);
		CloseClipboard();
		return;
	}

	if (!(lpClipBuffer = (LPTSTR) GlobalLock(hClipBuffer)))
	{
		GlobalFree(hClipBuffer);

		MessageBox(ghWnd,"Could not lock Clipboard Memory!", "PDW Clipboard",MB_ICONWARNING);
		CloseClipboard();
		return;
	}

	*lpClipBuffer = '\0';
	pchar = pane->buff_char;

	for (index = min_row; index <= max_row; index++)
	{
		line_no = pane->iVscrollPos + index;
		offset  = line_no * (LINE_SIZE+1);

		for (xx = min_col; xx <= max_col; xx++)
		{
			if (pchar[offset + xx]) *lpClipBuffer++ = pchar[offset + xx];
			else break;
		}

		*lpClipBuffer++ = '\r';
		*lpClipBuffer++ = '\n';
	}

	*lpClipBuffer = '\0';

	GlobalUnlock(hClipBuffer);

	if (!SetClipboardData(CF_TEXT, hClipBuffer))
	{
		GlobalFree(hClipBuffer);

		MessageBox(ghWnd,"Could not set Clipboard Text!", "PDW Clipboard",MB_ICONWARNING);
   }

	CloseClipboard();

	return;
} // CopyToClipboard()


void PanePaint(PaneStruct *pane)
{
	HDC			hDC;
	HFONT		hOldFont;
	PAINTSTRUCT	ps;
	POINT		lpPoint;
	int			x, y, iPaintBeg, iPaintEnd;
	int			len, pos, size, num_lines;
	char		*pchar;
	BYTE		*pcolor;
	BYTE		color = COLOR_UNUSED;

	if (select_on && selected && (pane == select_pane)) InvertSelection();

	hDC = BeginPaint(pane->hWnd, &ps);
	hOldFont = (HFONT)SelectObject(hDC, hfont);

	num_lines = ps.rcPaint.top / cyChar;

	iPaintBeg = max(0, pane->iVscrollPos + num_lines);
	iPaintEnd = min(pane->Bottom, pane->iVscrollPos + ps.rcPaint.bottom / cyChar);

	SetBkMode(hDC, OPAQUE);
	SetBkColor(hDC, Profile.color_background);

	// let Windows handle aligning the characters as TextOut goes along
	SetTextAlign(hDC, TA_UPDATECP);

	pchar  = pane->buff_char;
	pcolor = pane->buff_color;

	for (int i=iPaintBeg; i<=iPaintEnd; i++)
	{
		x = cxChar * (1 - pane->iHscrollPos) - cxChar;
		y = cyChar * (1 - pane->iVscrollPos + i) - cyChar;

		// set the current text position
		MoveToEx(hDC, x, y, &lpPoint);

		len = strlen(&pchar[i*(LINE_SIZE+1)]);
		pos = 0;

		// output a line of text
		while (pos < len)
		{
			// check if color has changed
			if (pcolor[i*(LINE_SIZE+1) + pos] != color)
			{
				color = pcolor[i*(LINE_SIZE+1) + pos];
				SetTextColor(hDC, GetColorRGB(color));	// now set the text color
			}
			// calculate how many characters are this color
			size = 0;
			while ((pcolor[i*(LINE_SIZE+1) + pos + size] == color) && (pos+size < len)) size++;

			// print out the characters in the current color
			TextOut(hDC, 0, 0, &pchar[i*(LINE_SIZE+1) + pos], size);

			// update the position in the current line
			pos += size;
		}
	}

	SelectObject(hDC, hOldFont);
	EndPaint(pane->hWnd, &ps);

	if (select_on && selected && (pane == select_pane)) InvertSelection();

	return;
}


DWORD GetColorRGB(BYTE color)
{
	DWORD rgb;

	switch (color)
	{
		case COLOR_BACKGROUND:		rgb = Profile.color_background;
			break;
		case COLOR_ADDRESS:			rgb = Profile.color_address;
			break;
		case COLOR_MODETYPEBIT:     rgb = Profile.color_modetypebit;
			break;
		case COLOR_TIMESTAMP:		rgb = Profile.color_timestamp;
			break;
		case COLOR_NUMERIC:			rgb = Profile.color_numeric;
			break;
		case COLOR_MESSAGE:			rgb = Profile.color_message;
			break;
		case COLOR_MISC:			rgb = Profile.color_misc;
			break;
		case COLOR_BITERRORS:		rgb = Profile.color_biterrors;
			break;
		case COLOR_INSTRUCTIONS:	rgb = Profile.color_instructions;
			break;
		case COLOR_AC_MESSAGE_NR:	rgb = Profile.color_ac_message_nr;
			break;
		case COLOR_AC_DBI:			rgb = Profile.color_ac_dbi;
			break;
		case COLOR_MB_SENDER:		rgb = Profile.color_mb_sender;
			break;
		case COLOR_FILTERMATCH:		rgb = Profile.color_filtermatch;
			break;
		case COLOR_FILTERLABEL:		rgb = Profile.color_filterlabel[0];
			break;
		case COLOR_FILTERLABEL+1:	rgb = Profile.color_filterlabel[1];
			break;
		case COLOR_FILTERLABEL+2:	rgb = Profile.color_filterlabel[2];
			break;
		case COLOR_FILTERLABEL+3:	rgb = Profile.color_filterlabel[3];
			break;
		case COLOR_FILTERLABEL+4:	rgb = Profile.color_filterlabel[4];
			break;
		case COLOR_FILTERLABEL+5:	rgb = Profile.color_filterlabel[5];
			break;
		case COLOR_FILTERLABEL+6:	rgb = Profile.color_filterlabel[6];
			break;
		case COLOR_FILTERLABEL+7:	rgb = Profile.color_filterlabel[7];
			break;
		case COLOR_FILTERLABEL+8:	rgb = Profile.color_filterlabel[8];
			break;
		case COLOR_FILTERLABEL+9:	rgb = Profile.color_filterlabel[9];
			break;
		case COLOR_FILTERLABEL+10:	rgb = Profile.color_filterlabel[10];
			break;
		case COLOR_FILTERLABEL+11:	rgb = Profile.color_filterlabel[11];
			break;
		case COLOR_FILTERLABEL+12:	rgb = Profile.color_filterlabel[12];
			break;
		case COLOR_FILTERLABEL+13:	rgb = Profile.color_filterlabel[13];
			break;
		case COLOR_FILTERLABEL+14:	rgb = Profile.color_filterlabel[14];
			break;
		case COLOR_FILTERLABEL+15:	rgb = Profile.color_filterlabel[15];
			break;
		case COLOR_FILTERLABEL+16:	rgb = Profile.color_filterlabel[16];
			break;
	}
	return(rgb);
}


void PaneHScroll(PaneStruct *pane, WPARAM wParam)
{
	int iHscrollInc;
	int scroll_amt;

	switch (LOWORD(wParam))
	{
		case SB_LINEUP :
			iHscrollInc = -1;
		break;

		case SB_LINEDOWN :
			iHscrollInc = 1;
		break;

		case SB_PAGEUP :
			iHscrollInc = -8;
		break;

		case SB_PAGEDOWN :
			iHscrollInc = 8;
		break;

		case SB_THUMBTRACK :
			iHscrollInc = HIWORD(wParam) - pane->iHscrollPos;
		break;

		default :
			iHscrollInc = 0;
		break;
	}

	iHscrollInc = max(-(pane->iHscrollPos), min(iHscrollInc, pane->iHscrollMax - pane->iHscrollPos));

	if (iHscrollInc != 0)
	{
		if (select_on && selected && (pane == select_pane))
		{
			InvertSelection();
		}

		scroll_amt = cxChar * iHscrollInc;
		pane->iHscrollPos += iHscrollInc;

		if (pane == &Pane1)	// Scroll labels above pane1
		{
			PL1_SCount += (-scroll_amt);
			DrawPaneLabels(ghWnd, PANE1);
		}
		else	// Scroll labels above pane2
		{
			PL2_SCount += (-scroll_amt);
			DrawPaneLabels(ghWnd, PANE2);
		}

		ScrollWindow (pane->hWnd, -scroll_amt, 0, NULL, NULL);
		SetScrollPos (pane->hWnd, SB_HORZ, pane->iHscrollPos, TRUE);

		if (select_on && selected && (pane == select_pane))
		{
			iSelectionStartCol -= iHscrollInc;	// also scroll selection
			iSelectionEndCol   -= iHscrollInc;

			InvertSelection();
		}
	}
	return;
} // end of PaneHScroll


void PaneVScroll(PaneStruct *pane, WPARAM wParam, LPARAM lParam)
{
	int iVscrollInc;
	int scroll_amt;

	int nLines = LOWORD(lParam);

	if (!nLines) nLines=1;

	switch (LOWORD(wParam))
	{
		case SB_TOP :
			iVscrollInc = -(pane->iVscrollPos);
		break;

		case SB_BOTTOM :
			iVscrollInc = pane->iVscrollMax - pane->iVscrollPos;
		break;

		case SB_LINEUP :
			iVscrollInc = -nLines;
		break;

		case SB_LINEDOWN :
			iVscrollInc = nLines;
		break;

		case SB_PAGEUP :
			iVscrollInc = min(-1, -(signed int)(pane->cyLines));
		break;

		case SB_PAGEDOWN :
			iVscrollInc = max(1, pane->cyLines);
		break;

		case SB_THUMBTRACK :
			iVscrollInc = HIWORD(wParam) - pane->iVscrollPos;
		break;

		default :
			iVscrollInc = 0;
		break;
	}

	iVscrollInc = max(-(pane->iVscrollPos), min(iVscrollInc, pane->iVscrollMax - pane->iVscrollPos));

	if (iVscrollInc != 0)
	{
		if (select_on && selected && (pane == select_pane)) InvertSelection();

		pane->iVscrollPos += iVscrollInc;
		scroll_amt = cyChar * iVscrollInc;
		ScrollWindow (pane->hWnd, 0, -scroll_amt, NULL, NULL);
		SetScrollRange (pane->hWnd, SB_VERT, 0, pane->iVscrollMax, FALSE);
		SetScrollPos (pane->hWnd, SB_VERT, pane->iVscrollPos, TRUE);

		if (select_on && selected && (pane == select_pane))
		{
			iSelectionStartRow -= iVscrollInc;	// also scroll selection
			iSelectionEndRow   -= iVscrollInc;

			InvertSelection();
		}
	}
	return;
} // end of PaneVScroll


VOID NEAR GoModalDialogBoxParam(HINSTANCE hInstance, LPCSTR lpszTemplate, HWND hWnd, DLGPROC lpDlgProc, LPARAM lParam)
{
	DLGPROC  lpProcInstance;

	lpProcInstance = (DLGPROC) MakeProcInstance((FARPROC) lpDlgProc,hInstance);
	DialogBoxParam(hInstance, lpszTemplate, hWnd, lpProcInstance, lParam);
	FreeProcInstance((FARPROC) lpProcInstance);

	return;

} // end of GoModalDialogBoxParam()


// PDW Version/Copyright info
BOOL FAR PASCAL AboutDlgProc(HWND hDlg, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	switch (uMsg)
	{
		case WM_INITDIALOG:

		if (!CenterWindow(hDlg)) return (FALSE);

		// Display Pager logo in top left corner
		TmpDlgChildWin(hDlg, ghInstance, IDS_ABOUTLOGO, 12, 16);

		SetDlgItemText(hDlg, IDC_VERSION, pdw_version);

		return (TRUE);

		case WM_COMMAND:
		switch (LOWORD(wParam))
		{
			case IDC_WEBSITE:
				ShellExecute(NULL, "open", "http://www.discriminator.nl/pdw/index-en.html", NULL, NULL, SW_SHOWNORMAL);

			// fall through

			case IDCANCEL:
				EndDialog(hDlg, TRUE);
			break;
		}
		break;
	}
	return (FALSE);
} // end of AboutDlgProc


// PH: PDW Debug information
BOOL FAR PASCAL DebugDlgProc(HWND hDlg, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	extern bool bFlexTIME_detected;

	char rxqual[10]="- %";
	char temp[32];

	int days, hours, minutes, seconds;

	switch (uMsg)
	{
		case WM_INITDIALOG:

		if (!CenterWindow(hDlg)) return (FALSE);

		hDebugDlg = hDlg;

		SetDlgItemText(hDlg, IDC_DEBUG_OS,      szOSType);
		SetDlgItemText(hDlg, IDC_DEBUG_STARTED, szDebugStarted);

		if (Profile.audioEnabled)
		{
			strcpy(szTEMP, "Soundcard");
		}
		else if (Profile.comPortEnabled)
		{
			sprintf(szTEMP, "%s/COM%i", Profile.comPortRS232 ? "RS232" : "Slicer", Profile.comPort);
		}
		SetDlgItemText(hDlg, IDC_DEBUG_INPUT, szTEMP);
		SetDlgItemText(hDlg, IDC_DEBUG_FLEXTIME, bFlexTIME_detected ? "Detected" : "Not detected");

		SendMessage(hDlg, WM_WININICHANGE, 0, 0L);

		return (TRUE);

		case WM_WININICHANGE:

		days	= iSecondsElapsed/86400;
//		hours	= (iSecondsElapsed-(days*86400))/3600;
//		minutes	= (iSecondsElapsed-(days*86400)-(hours*3600))/60;
//		seconds = (iSecondsElapsed-(days*86400)-(hours*3600)-(minutes*60));
		hours	= (iSecondsElapsed/3600) % 24;
		minutes	= (iSecondsElapsed/60) % 60;
		seconds = iSecondsElapsed % 60;

		sprintf(temp, "%id%ih%im%is", days, hours, minutes, seconds);

		SetDlgItemText(hDlg, IDC_DEBUG_RUNNING, temp);
		SetDlgItemInt(hDlg, IDC_DEBUG_MSG, nCount_Messages, false);

		if (Profile.monitor_paging)
		{
			SetDlgItemInt (hDlg, IDC_DEBUG_GROUPMSG,nCount_Groupcalls, false);
			sprintf(szTEMP, "%i / %i", nCount_Missed[0], nCount_Missed[1]);
			SetDlgItemText(hDlg, IDC_DEBUG_MISSED, szTEMP);
		}
		else
		{
			SetDlgItemText(hDlg, IDC_DEBUG_GROUPMSG, "-");
			SetDlgItemText(hDlg, IDC_DEBUG_MISSED,   "-");
		}
		SetDlgItemInt(hDlg, IDC_DEBUG_REJECTED,nCount_Rejected, false);
		SetDlgItemInt(hDlg, IDC_DEBUG_BLOCKED, nCount_Blocked,  false);

//		SetDlgItemInt(hDlg, IDC_DEBUG_TEST, iDebug_Test, false);
		sprintf(szTEMP, "%i / %i", nCount_BlockBuffer[0], nCount_BlockBuffer[1]);
		SetDlgItemText(hDlg, IDC_DEBUG_BLOCKBUFFER, szTEMP);

		return (TRUE);

		case WM_COMMAND:

		switch (LOWORD(wParam))
		{
			case IDCANCEL:

			EndDialog(hDlg, TRUE);
			hDebugDlg = NULL;

			break;
		}
		break;
	}
	return (FALSE);
} // end of DebugDlgProc


BOOL FAR PASCAL SystemTrayDlgProc(HWND hDlg, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	static int iRestore = Profile.SystemTrayRestore;

	switch (uMsg)
	{
		case WM_INITDIALOG:

		if (!CenterWindow(hDlg)) return (FALSE);

		hSystemTrayDlg = hDlg;

		CheckDlgButton(hDlg, IDC_SYSTEMTRAY,         Profile.SystemTray);
		CheckDlgButton(hDlg, IDC_SYSTEMTRAY_RESTORE, Profile.SystemTrayRestore);

		EnableWindow(GetDlgItem(hDlg, IDC_SYSTEMTRAY_RESTORE), Profile.SystemTray ? true : false);

		switch (Profile.SystemTrayRestore)
		{
			case 0:
			EnableWindow(GetDlgItem(hDlg, IDC_SYSTEMTRAY_NEW),    false);
			EnableWindow(GetDlgItem(hDlg, IDC_SYSTEMTRAY_MONLY),  false);
			EnableWindow(GetDlgItem(hDlg, IDC_SYSTEMTRAY_FILTER), false);
			break;

			case 1:
			CheckDlgButton(hDlg, IDC_SYSTEMTRAY_NEW, BST_CHECKED);
			break;

			case 2:
			CheckDlgButton(hDlg, IDC_SYSTEMTRAY_MONLY, BST_CHECKED);
			break;

			case 3:
			CheckDlgButton(hDlg, IDC_SYSTEMTRAY_FILTER, BST_CHECKED);
			break;
		}
		return (TRUE);

		case WM_COMMAND:

		switch (LOWORD(wParam))
		{
			case IDC_SYSTEMTRAY:

			if (!IsDlgButtonChecked(hDlg, IDC_SYSTEMTRAY))
			{
				EnableWindow(GetDlgItem(hDlg, IDC_SYSTEMTRAY_RESTORE), false);
				EnableWindow(GetDlgItem(hDlg, IDC_SYSTEMTRAY_NEW),     false);
				EnableWindow(GetDlgItem(hDlg, IDC_SYSTEMTRAY_MONLY),   false);
				EnableWindow(GetDlgItem(hDlg, IDC_SYSTEMTRAY_FILTER),  false);

				iRestore=0;
			}
			else EnableWindow(GetDlgItem(hDlg, IDC_SYSTEMTRAY_RESTORE), true);

			break;

			case IDC_SYSTEMTRAY_RESTORE:

			iRestore=IsDlgButtonChecked(hDlg, IDC_SYSTEMTRAY_RESTORE);

			EnableWindow(GetDlgItem(hDlg, IDC_SYSTEMTRAY_NEW),    iRestore ? true : false);
			EnableWindow(GetDlgItem(hDlg, IDC_SYSTEMTRAY_MONLY),  iRestore ? true : false);
			EnableWindow(GetDlgItem(hDlg, IDC_SYSTEMTRAY_FILTER), iRestore ? true : false);

			if ((!IsDlgButtonChecked(hDlg, IDC_SYSTEMTRAY_NEW)) &&
			    (!IsDlgButtonChecked(hDlg, IDC_SYSTEMTRAY_MONLY)) &&
			    (!IsDlgButtonChecked(hDlg, IDC_SYSTEMTRAY_FILTER)))
			{
				if (IsDlgButtonChecked(hDlg, IDC_SYSTEMTRAY_RESTORE))
				{
					CheckDlgButton(hDlg, IDC_SYSTEMTRAY_NEW, BST_CHECKED);
					iRestore=1;
				}
			}
			break;

			case IDC_SYSTEMTRAY_NEW:
			iRestore=1;
			break;

			case IDC_SYSTEMTRAY_MONLY:
			iRestore=2;
			break;

			case IDC_SYSTEMTRAY_FILTER:
			iRestore=3;
			break;

			case IDOK:

			Profile.SystemTray        = IsDlgButtonChecked(hDlg, IDC_SYSTEMTRAY);
			Profile.SystemTrayRestore = iRestore;

			if (!Profile.SystemTray)
			{
				if (bTrayed) SystemTrayWindow(false);
				SystemTrayIcon(true); // PH: Remove PDW-icon from systemtray
			}
			WriteSettings();

			hSystemTrayDlg = NULL;
			EndDialog(hDlg, TRUE);

			return (TRUE);

			case IDCANCEL:

			hSystemTrayDlg = NULL;
			EndDialog(hDlg, TRUE);

			break;
		}
		break;
	}
	return (FALSE);
}


// This deals with logfile dialog box options.
BOOL FAR PASCAL LogFileDlgProc(HWND hDlg, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	static char szLog[] = {"Log Files (*.log)\0*.log\0All Files (*.*)\0*.*\0\0"};

	OPENFILENAME ofn = {0};
	char szFileLog[MAX_FILE_LEN+1], szFileTitle[256], szFileLogExt[5]="log";
	int i, enabled, date;
	FILE *pFileLog = NULL;
	char temp_logfile[MAX_PATH]="";
	char tmp_message[128];				// PH : Buffer for errormessages

	CreateDateFilename(".log", NULL);	// PH: Create date for filename

	switch (uMsg)
	{
		case WM_WINDOWPOSCHANGED: // Give focus to the logfile edit control
			SetFocus(GetDlgItem(hDlg, IDC_LOGFILE));
		break;

		case WM_INITDIALOG:

		if (!CenterWindow(hDlg)) return (FALSE);

		// Check/uncheck the 'use date as filename checkbox'.
		CheckDlgButton(hDlg, IDC_LOGFILEEN,   Profile.logfile_enabled);
		CheckDlgButton(hDlg, IDC_LOGFILEDATE, Profile.logfile_use_date);

		if (Profile.monitor_mobitex)
		{
			SendDlgItemMessage(hDlg, IDC_LOGCOLUMN+1, WM_SETTEXT, 0, (LPARAM) "MAN");
			SendDlgItemMessage(hDlg, IDC_LOGCOLUMN+4, WM_SETTEXT, 0, (LPARAM) "Sender");
		}
		else if (Profile.monitor_acars)
		{
			SendDlgItemMessage(hDlg, IDC_LOGCOLUMN+1, WM_SETTEXT, 0, (LPARAM) "Reg.No.");
			SendDlgItemMessage(hDlg, IDC_LOGCOLUMN+4, WM_SETTEXT, 0, (LPARAM) "Msg.No.");
			SendDlgItemMessage(hDlg, IDC_LOGCOLUMN+5, WM_SETTEXT, 0, (LPARAM) "DBI");
			SendDlgItemMessage(hDlg, IDC_LOGCOLUMN+6, WM_SETTEXT, 0, (LPARAM) "Mode");
		}
		SendMessage(hDlg, WM_WININICHANGE, NULL, 0L);

		return (TRUE);

		case WM_WININICHANGE:

		enabled = IsDlgButtonChecked(hDlg, IDC_LOGFILEEN);
		date    = IsDlgButtonChecked(hDlg, IDC_LOGFILEDATE);

		EnableWindow(GetDlgItem(hDlg, IDC_LOGFILEDATE), enabled);
		EnableWindow(GetDlgItem(hDlg, IDC_LOGFILE),     enabled && !date);
		EnableWindow(GetDlgItem(hDlg, IDC_LOGBROWSE),   enabled && !date);

		if (date)
		{
			SetDlgItemText(hDlg, IDC_LOGFILE, szFilenameDate);
		}
		else if (Profile.logfile[0])
		{
			SetDlgItemText(hDlg, IDC_LOGFILE, Profile.logfile);
		}

		for (i=1; i<=7; i++)
		{
			EnableWindow(GetDlgItem(hDlg, IDC_LOGCOLUMN+i), enabled);
			CheckDlgButton(hDlg, IDC_LOGCOLUMN+i, strchr(Profile.ColLogfile, '0'+i) != 0);
		}

		SetDlgItemText(hDlg, IDC_LOGFILE, date ? szFilenameDate : "");

		break;

		case WM_COMMAND:

		switch (LOWORD(wParam))
		{
			case IDC_LOGBROWSE:  // Browse for file name.

			strcpy(szFileLog, "*.log");

			ofn.lStructSize = sizeof(OPENFILENAME);
			ofn.hwndOwner = hDlg;
			ofn.lpstrFilter = szLog;
			ofn.lpstrCustomFilter = NULL;
			ofn.nMaxCustFilter = 0L;
			ofn.nFilterIndex = 1;
			ofn.lpstrFile = szFileLog;
			ofn.nMaxFile = sizeof(szFileLog)-5;
			ofn.lpstrFileTitle = szFileTitle;
			ofn.nMaxFileTitle = sizeof(szFileTitle);
			ofn.lpstrInitialDir = Profile.LogfilePath;
			ofn.lpstrTitle = "Open Logfile...";
			ofn.Flags = OFN_HIDEREADONLY;
			ofn.nFileOffset = 0;
			ofn.nFileExtension = 0;
			ofn.lpstrDefExt = szFileLogExt;

			if (GetOpenFileName(&ofn))
			{
				SetDlgItemText(hDlg, IDC_LOGFILE, szFileLog);
			}
			SetFocus(GetDlgItem(hDlg, IDOK));

			break;

			case IDC_LOGFILEEN:
			case IDC_LOGFILEDATE:

			SendMessage(hDlg, WM_WININICHANGE, NULL, 0L);

			break;

			case IDOK:

			Profile.logfile_enabled  = IsDlgButtonChecked(hDlg, IDC_LOGFILEEN);
			Profile.logfile_use_date = IsDlgButtonChecked(hDlg, IDC_LOGFILEDATE);

			GetDlgItemText(hDlg, IDC_LOGFILE, szFileLog, MAX_FILE_LEN+1);

			if (!FileExists(szLogPathName)) CreateDirectory(szLogPathName, NULL);

			// Check if filename is acceptable
			if (!Profile.logfile_use_date)
			{
				if (!szFileLog[0])
				{
					MessageBox(hDlg,"You haven't entered a file name!","PDW Logfile",MB_ICONERROR);
					SetFocus(GetDlgItem(hDlg, IDC_LOGFILE));
					return (FALSE);
				}
				else if (Need_Ext(szFileLog)) strcat(szFileLog,".log");

				if (stricmp(szFileLog, Profile.filterfile) == 0)
				{
					SetDlgItemText(hDlg, IDC_LOGFILE, "");
					sprintf(tmp_message, "Logfile can't be same as Filterfile!\n( %s )", szFileLog);
					MessageBox(hDlg, tmp_message, "PDW Logfile",MB_ICONERROR);
					SendDlgItemMessage(hDlg,  IDC_LOGFILE, EM_SETSEL, 0, -1);
					SetFocus(GetDlgItem(hDlg, IDC_LOGFILE));
					return (FALSE);
				}

				if (strstr(szFileLog, "\\") != 0)
				{
					if (szFileLog[1] == ':')
					{
						strcpy(temp_logfile, szFileLog); // Full path
					}
					else sprintf(temp_logfile, "%s\\%s", szPath, szFileLog);
				}
				else sprintf(temp_logfile, "%s\\%s", szLogPathName, szFileLog);

				if (!(pFileLog = fopen(temp_logfile, "a")))
				{
					sprintf(tmp_message, "Error opening logfile!\n( %s )", szFileLog);
					MessageBox(hDlg, tmp_message, "PDW Logfile",MB_ICONERROR);
					SetFocus(GetDlgItem(hDlg, IDC_LOGFILE));
					return (FALSE);
				}
				else
				{
					strcpy(Profile.logfile, szFileLog);
					fclose(pFileLog);
				}
			}
			else strcpy(Profile.logfile, "");

			strcpy(Profile.ColLogfile, "");

			if (IsDlgButtonChecked(hDlg, IDC_LOGCOLUMN+1)) strcat(Profile.ColLogfile, "1");
			if (IsDlgButtonChecked(hDlg, IDC_LOGCOLUMN+2)) strcat(Profile.ColLogfile, "2");
			if (IsDlgButtonChecked(hDlg, IDC_LOGCOLUMN+3)) strcat(Profile.ColLogfile, "3");
			if (IsDlgButtonChecked(hDlg, IDC_LOGCOLUMN+4)) strcat(Profile.ColLogfile, "4");
			if (IsDlgButtonChecked(hDlg, IDC_LOGCOLUMN+5)) strcat(Profile.ColLogfile, "5");
			if (IsDlgButtonChecked(hDlg, IDC_LOGCOLUMN+6)) strcat(Profile.ColLogfile, "6");
			if (IsDlgButtonChecked(hDlg, IDC_LOGCOLUMN+7)) strcat(Profile.ColLogfile, "7");

			// Update profile
			WriteSettings();

			EndDialog(hDlg, TRUE);
			return (TRUE);

			case IDCANCEL:

			EndDialog(hDlg, TRUE);
			return (TRUE);
		}
		break;
	}
	return (FALSE);

}  // end of LogFileDlgProc()

// Get custom audio information.
BOOL FAR PASCAL CustomAudioDlgProc(HWND hDlg, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	char buf[20];
	int i;

	switch (uMsg)
	{
		case WM_INITDIALOG:

		if (!CenterWindow(hDlg)) return (FALSE);

		// Set threshold values / Set Resync value / Set Centering values.

		for (i=0; i<10; i++)
		{
			sprintf(buf,"%d",i);

			SendDlgItemMessage(hDlg, IDC_THRESHOLD512,  CB_ADDSTRING, 0, (LPARAM) (LPSTR) buf);
			SendDlgItemMessage(hDlg, IDC_THRESHOLD1200, CB_ADDSTRING, 0, (LPARAM) (LPSTR) buf);
			SendDlgItemMessage(hDlg, IDC_THRESHOLD1600, CB_ADDSTRING, 0, (LPARAM) (LPSTR) buf);
			SendDlgItemMessage(hDlg, IDC_THRESHOLD2400, CB_ADDSTRING, 0, (LPARAM) (LPSTR) buf);

			SendDlgItemMessage(hDlg, IDC_RESYNC512,     CB_ADDSTRING, 0, (LPARAM) (LPSTR) buf);
			SendDlgItemMessage(hDlg, IDC_RESYNC1200,    CB_ADDSTRING, 0, (LPARAM) (LPSTR) buf);
			SendDlgItemMessage(hDlg, IDC_RESYNC1600,    CB_ADDSTRING, 0, (LPARAM) (LPSTR) buf);
			SendDlgItemMessage(hDlg, IDC_RESYNC2400,    CB_ADDSTRING, 0, (LPARAM) (LPSTR) buf);

			SendDlgItemMessage(hDlg, IDC_CENTERING512,  CB_ADDSTRING, 0, (LPARAM) (LPSTR) buf);
			SendDlgItemMessage(hDlg, IDC_CENTERING1200, CB_ADDSTRING, 0, (LPARAM) (LPSTR) buf);
			SendDlgItemMessage(hDlg, IDC_CENTERING1600, CB_ADDSTRING, 0, (LPARAM) (LPSTR) buf);
			SendDlgItemMessage(hDlg, IDC_CENTERING2400, CB_ADDSTRING, 0, (LPARAM) (LPSTR) buf);
		}

		SendDlgItemMessage(hDlg, IDC_THRESHOLD512,  CB_SETCURSEL, (WPARAM) Profile.audioThreshold[INDEX512],  0L);
		SendDlgItemMessage(hDlg, IDC_THRESHOLD1200, CB_SETCURSEL, (WPARAM) Profile.audioThreshold[INDEX1200], 0L);
		SendDlgItemMessage(hDlg, IDC_THRESHOLD1600, CB_SETCURSEL, (WPARAM) Profile.audioThreshold[INDEX1600], 0L);
		SendDlgItemMessage(hDlg, IDC_THRESHOLD2400, CB_SETCURSEL, (WPARAM) Profile.audioThreshold[INDEX2400], 0L);

		SendDlgItemMessage(hDlg, IDC_RESYNC512,     CB_SETCURSEL, (WPARAM) Profile.audioResync[INDEX512],  0L);
		SendDlgItemMessage(hDlg, IDC_RESYNC1200,    CB_SETCURSEL, (WPARAM) Profile.audioResync[INDEX1200], 0L);
		SendDlgItemMessage(hDlg, IDC_RESYNC1600,    CB_SETCURSEL, (WPARAM) Profile.audioResync[INDEX1600], 0L);
		SendDlgItemMessage(hDlg, IDC_RESYNC2400,    CB_SETCURSEL, (WPARAM) Profile.audioResync[INDEX2400], 0L);

		SendDlgItemMessage(hDlg, IDC_CENTERING512,  CB_SETCURSEL, (WPARAM) Profile.audioResync[INDEX512],  0L);
		SendDlgItemMessage(hDlg, IDC_CENTERING1200, CB_SETCURSEL, (WPARAM) Profile.audioResync[INDEX1200], 0L);
		SendDlgItemMessage(hDlg, IDC_CENTERING1600, CB_SETCURSEL, (WPARAM) Profile.audioResync[INDEX1600], 0L);
		SendDlgItemMessage(hDlg, IDC_CENTERING2400, CB_SETCURSEL, (WPARAM) Profile.audioResync[INDEX2400], 0L);

		return (TRUE);

		case WM_COMMAND:

		switch (LOWORD(wParam))
		{
			case IDOK:

			Profile.audioThreshold[INDEX512]  = SendDlgItemMessage(hDlg, IDC_THRESHOLD512,  CB_GETCURSEL, 0, 0L);
			Profile.audioThreshold[INDEX1200] = SendDlgItemMessage(hDlg, IDC_THRESHOLD1200, CB_GETCURSEL, 0, 0L);
			Profile.audioThreshold[INDEX1600] = SendDlgItemMessage(hDlg, IDC_THRESHOLD1600, CB_GETCURSEL, 0, 0L);
			Profile.audioThreshold[INDEX2400] = SendDlgItemMessage(hDlg, IDC_THRESHOLD2400, CB_GETCURSEL, 0, 0L);

			Profile.audioResync[INDEX512]  = SendDlgItemMessage(hDlg, IDC_RESYNC512,  CB_GETCURSEL, 0, 0L);
			Profile.audioResync[INDEX1200] = SendDlgItemMessage(hDlg, IDC_RESYNC1200, CB_GETCURSEL, 0, 0L);
			Profile.audioResync[INDEX1600] = SendDlgItemMessage(hDlg, IDC_RESYNC1600, CB_GETCURSEL, 0, 0L);
			Profile.audioResync[INDEX2400] = SendDlgItemMessage(hDlg, IDC_RESYNC2400, CB_GETCURSEL, 0, 0L);

			Profile.audioCentering[INDEX512]  = SendDlgItemMessage(hDlg, IDC_CENTERING512,  CB_GETCURSEL, 0, 0L);
			Profile.audioCentering[INDEX1200] = SendDlgItemMessage(hDlg, IDC_CENTERING1200, CB_GETCURSEL, 0, 0L);
			Profile.audioCentering[INDEX1600] = SendDlgItemMessage(hDlg, IDC_CENTERING1600, CB_GETCURSEL, 0, 0L);
			Profile.audioCentering[INDEX2400] = SendDlgItemMessage(hDlg, IDC_CENTERING2400, CB_GETCURSEL, 0, 0L);

			// Update audio configuration.
			SetAudioConfig(Profile.audioConfig);

			WriteSettings();

			EndDialog(hDlg, TRUE);
			return (TRUE);

			case IDCANCEL:
				EndDialog(hDlg, TRUE);
			return (TRUE);
		}
		break;
	}
	return (FALSE);
} // end of CustomAudioDlgProc


BOOL FAR PASCAL SetupDlgProc(HWND hDlg, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	static char *comports[5] = { "COM1", "COM2", "COM3", "COM4", "Custom" };
	static char *cirqs[9]	 = {"2/9", "3", "4", "5", "6", "7", "10", "11", "12" };
	static int    irqs[9]	 = { 2, 3, 4, 5, 6, 7, 10, 11, 12 };

	char szDeviceName[128];
	char temp[10];

	static int old_device=0, more_audio=0, audio_devices=0;
	int i, state=0, config, comport, slicer, rs232, serial, soundcard;
	int	tmp_audioConfig = Profile.audioConfig; // May need to restore configuration later.

	DWORD value;

	WAVEINCAPS WaveInCaps = {0};

	switch (uMsg)
	{   
		case WM_INITDIALOG:

		if (!CenterWindow(hDlg)) return (FALSE);

		if (bWin9x && !Profile.comPortRS232)
		{
			for (i=0; i<5; i++)
			{
				SendDlgItemMessage(hDlg, IDC_COMPORT, CB_ADDSTRING, 0, (LPARAM) (LPSTR) comports[i]);
			}
			if (Profile.comPort)
			{
				SendDlgItemMessage(hDlg, IDC_COMPORT, CB_SETCURSEL, (WPARAM) Profile.comPort - 1, 0L);

				if (Profile.comPort == 5)
				{
					EnableWindow(GetDlgItem(hDlg, IDC_COMADDR), true);
					EnableWindow(GetDlgItem(hDlg, IDC_COMIRQ),  true);
				}
			}
			for (i=0; i<9; i++)
			{
				SendDlgItemMessage(hDlg, IDC_COMIRQ, CB_ADDSTRING, 0, (LPARAM) (LPSTR) cirqs[i]);

				if (Profile.comPortIRQ == irqs[i])
				SendDlgItemMessage(hDlg, IDC_COMIRQ, CB_SETCURSEL, i, 0L);
			}

			sprintf(temp, "0x%03X", Profile.comPortAddr);
			SetDlgItemText(hDlg, IDC_COMADDR, (LPCSTR) temp);
		}
		else
		{
			for (i=0; pComPorts[i] != 0; i++)
			{
				sprintf(temp, "COM%i", pComPorts[i]);
				SendDlgItemMessage(hDlg, IDC_COMPORT, CB_ADDSTRING, 0, (LPARAM) (LPSTR) temp);
				if (Profile.comPort == pComPorts[i]) state=i;
			}
			SendDlgItemMessage(hDlg, IDC_COMPORT, CB_SETCURSEL,    (WPARAM) state, 0L);
			SendDlgItemMessage(hDlg, IDC_COMIRQ,  CB_ADDSTRING, 0, (LPARAM) (LPSTR) "N/A");
			SendDlgItemMessage(hDlg, IDC_COMIRQ,  CB_SETCURSEL, 0, 0L);
			SetDlgItemText(hDlg, IDC_COMADDR, (LPCSTR) "N/A");
		}
		SendDlgItemMessage(hDlg, IDC_COMADDR, EM_LIMITTEXT, 5, 0L);

		// Set audio configurations.
		SendDlgItemMessage(hDlg, IDC_AUDIOCONFIG, CB_ADDSTRING, 0, (LPARAM) (LPSTR) "Custom");
		SendDlgItemMessage(hDlg, IDC_AUDIOCONFIG, CB_ADDSTRING, 0, (LPARAM) (LPSTR) "Discriminator 1");
		SendDlgItemMessage(hDlg, IDC_AUDIOCONFIG, CB_ADDSTRING, 0, (LPARAM) (LPSTR) "Discriminator 2");
		SendDlgItemMessage(hDlg, IDC_AUDIOCONFIG, CB_ADDSTRING, 0, (LPARAM) (LPSTR) "Discriminator 3");
		SendDlgItemMessage(hDlg, IDC_AUDIOCONFIG, CB_ADDSTRING, 0, (LPARAM) (LPSTR) "Discriminator 4");
		SendDlgItemMessage(hDlg, IDC_AUDIOCONFIG, CB_ADDSTRING, 0, (LPARAM) (LPSTR) "Earphone 1");
		SendDlgItemMessage(hDlg, IDC_AUDIOCONFIG, CB_ADDSTRING, 0, (LPARAM) (LPSTR) "Earphone 2");
		SendDlgItemMessage(hDlg, IDC_AUDIOCONFIG, CB_ADDSTRING, 0, (LPARAM) (LPSTR) "Earphone 3");
		SendDlgItemMessage(hDlg, IDC_AUDIOCONFIG, CB_ADDSTRING, 0, (LPARAM) (LPSTR) "Speaker Out 1");
		SendDlgItemMessage(hDlg, IDC_AUDIOCONFIG, CB_ADDSTRING, 0, (LPARAM) (LPSTR) "Speaker Out 2");
		SendDlgItemMessage(hDlg, IDC_AUDIOCONFIG, CB_ADDSTRING, 0, (LPARAM) (LPSTR) "Speaker Out 3");
		SendDlgItemMessage(hDlg, IDC_AUDIOCONFIG, CB_ADDSTRING, 0, (LPARAM) (LPSTR) "Tape/Rec Out 1");
		SendDlgItemMessage(hDlg, IDC_AUDIOCONFIG, CB_ADDSTRING, 0, (LPARAM) (LPSTR) "Tape/Rec Out 2");
		SendDlgItemMessage(hDlg, IDC_AUDIOCONFIG, CB_ADDSTRING, 0, (LPARAM) (LPSTR) "Tape/Rec Out 3");
		SendDlgItemMessage(hDlg, IDC_AUDIOCONFIG, CB_SETCURSEL,    (WPARAM) Profile.audioConfig, 0L);

		// Set sample rates.
		SendDlgItemMessage(hDlg, IDC_AUDIOSAMPLERATE, CB_ADDSTRING, 0, (LPARAM) (LPSTR) "8000");
		SendDlgItemMessage(hDlg, IDC_AUDIOSAMPLERATE, CB_ADDSTRING, 0, (LPARAM) (LPSTR) "11025");
		SendDlgItemMessage(hDlg, IDC_AUDIOSAMPLERATE, CB_ADDSTRING, 0, (LPARAM) (LPSTR) "22050");
		SendDlgItemMessage(hDlg, IDC_AUDIOSAMPLERATE, CB_ADDSTRING, 0, (LPARAM) (LPSTR) "44100");

		switch (Profile.audioSampleRate)
		{
			case 8000:  state = 0;
			break;

			case 11025: state = 1;
			break;

			case 22050: state = 2;
			break;

			default:
			case 44100: state = 3;
			break;
		}

		SendDlgItemMessage(hDlg, IDC_AUDIOSAMPLERATE, CB_SETCURSEL, (WPARAM) state, 0L);

		// Fill Level combo box
		SendDlgItemMessage(hDlg, IDC_LEVEL, CB_ADDSTRING, 0, (LPARAM) (LPSTR) "2 Level");
		SendDlgItemMessage(hDlg, IDC_LEVEL, CB_ADDSTRING, 0, (LPARAM) (LPSTR) "4 Level");
		SendDlgItemMessage(hDlg, IDC_LEVEL, CB_SETCURSEL, (WPARAM) Profile.fourlevel, 0L);
		EnableWindow(GetDlgItem(hDlg, IDC_LEVEL), state);

		CheckDlgButton(hDlg, IDC_COMENABLE,   Profile.comPortEnabled);
		CheckDlgButton(hDlg, IDC_COMSLICER,  !Profile.comPortRS232);
		CheckDlgButton(hDlg, IDC_COMRS232,    Profile.comPortRS232);
		CheckDlgButton(hDlg, IDC_AUDIOENABLE, Profile.audioEnabled);

		SendDlgItemMessage(hDlg, IDC_RS232MODE, CB_ADDSTRING, 0, (LPARAM) (LPSTR) "Pocsag");
		SendDlgItemMessage(hDlg, IDC_RS232MODE, CB_ADDSTRING, 0, (LPARAM) (LPSTR) "Flex-1600");
		SendDlgItemMessage(hDlg, IDC_RS232MODE, CB_ADDSTRING, 0, (LPARAM) (LPSTR) "Mobitex");
		SendDlgItemMessage(hDlg, IDC_RS232MODE, CB_SETCURSEL, (WPARAM) Profile.comPortRS232-1, 0L);

		if (Profile.comPortEnabled)
		{
			EnableWindow(GetDlgItem(hDlg, IDC_RS232MODE), Profile.comPortRS232);
			EnableWindow(GetDlgItem(hDlg, IDC_LEVEL), !Profile.comPortRS232);
			EnableWindow(GetDlgItem(hDlg, IDC_AUDIODEVICES),	false);
			EnableWindow(GetDlgItem(hDlg, IDC_AUDIOCONFIG),		false);
			EnableWindow(GetDlgItem(hDlg, IDC_AUDIOSAMPLERATE), false);
			EnableWindow(GetDlgItem(hDlg, IDC_AUDIOCUSTOM),		false);
		}
		else if (Profile.audioEnabled)
		{
			EnableWindow(GetDlgItem(hDlg, IDC_COMPORT),   false);
			EnableWindow(GetDlgItem(hDlg, IDC_COMSLICER), false);
			EnableWindow(GetDlgItem(hDlg, IDC_COMRS232),  false);
			EnableWindow(GetDlgItem(hDlg, IDC_RS232MODE), false);
			EnableWindow(GetDlgItem(hDlg, IDC_LEVEL),     false);

			if (Profile.audioConfig > 0) // config != custom?
				EnableWindow(GetDlgItem(hDlg, IDC_AUDIOCUSTOM),false);
		}

		old_device = Profile.audioDevice;	// PH: Get the old audiodevice-#

		audio_devices = waveInGetNumDevs();	// PH: Get the number of audiodevices

		more_audio = (audio_devices < 2) ? 0 : 1;

		if (audio_devices == 0)
		{
			SendDlgItemMessage(hDlg, IDC_AUDIODEVICES, CB_ADDSTRING, 0, (LPARAM) (LPSTR) "No soundcards present/available");
			EnableWindow(GetDlgItem(hDlg, IDC_AUDIOENABLE), false);
		}
		else
		{
			for (i=0; i<audio_devices; i++)		// PH: Display available audiodevices
			{
				waveInGetDevCaps(i, &WaveInCaps, sizeof(WAVEINCAPS));	// PH: Get device name
				sprintf(szDeviceName, "%i. %s", i+1, WaveInCaps.szPname);		
				SendDlgItemMessage(hDlg, IDC_AUDIODEVICES, CB_ADDSTRING, 0, (LPARAM) (LPSTR) szDeviceName);
			}
		}
		SendDlgItemMessage(hDlg, IDC_AUDIODEVICES, CB_SETCURSEL, more_audio ? Profile.audioDevice : 0, 0L);
		if (!more_audio) EnableWindow(GetDlgItem(hDlg, IDC_AUDIODEVICES), false);

		value = GetWindowLong(GetDlgItem(hDlg, IDC_AUDIODEVICES), GWL_STYLE) - CBS_DROPDOWNLIST;
		SetWindowLong(GetDlgItem(hDlg, IDC_AUDIODEVICES), GWL_STYLE, value | ES_READONLY);

		CheckDlgButton(hDlg, IDC_COMENABLE,   Profile.comPortEnabled);
		CheckDlgButton(hDlg, IDC_AUDIOENABLE, Profile.audioEnabled);

		return (TRUE);

		case WM_COMMAND:

		switch (LOWORD(wParam))
		{
			case IDC_AUDIOCUSTOM:

			Profile.audioConfig = 0; // must be custom here!
			GoModalDialogBoxParam(ghInstance,
			MAKEINTRESOURCE(CUSTOM_AUDIO_DLG),
			hDlg, (DLGPROC) CustomAudioDlgProc, 0L);
			
			break;

			case IDC_AUDIOCONFIG:

			config = SendDlgItemMessage(hDlg, IDC_AUDIOCONFIG, CB_GETCURSEL, 0, 0L);
			EnableWindow(GetDlgItem(hDlg, IDC_AUDIOCUSTOM), config ? false : true);

			break;

			case IDC_COMENABLE:
			case IDC_AUDIOENABLE:

			if (LOWORD(wParam) == IDC_AUDIOENABLE)
			{
				CheckDlgButton(hDlg, IDC_COMENABLE, !IsDlgButtonChecked(hDlg, IDC_AUDIOENABLE));
			}
			else if (LOWORD(wParam) == IDC_COMENABLE)
			{
				CheckDlgButton(hDlg, IDC_AUDIOENABLE, !IsDlgButtonChecked(hDlg, IDC_COMENABLE));
			}
			// fall through...

			case IDC_COMRS232:
			case IDC_COMSLICER:
			
			serial    = IsDlgButtonChecked(hDlg, IDC_COMENABLE);
			slicer    = IsDlgButtonChecked(hDlg, IDC_COMSLICER);
			rs232     = IsDlgButtonChecked(hDlg, IDC_COMRS232);
			comport   = SendDlgItemMessage(hDlg, IDC_COMPORT, CB_GETCURSEL, 0, 0L);
			soundcard = IsDlgButtonChecked(hDlg, IDC_AUDIOENABLE);
			config    = SendDlgItemMessage(hDlg, IDC_AUDIOCONFIG, CB_GETCURSEL, 0, 0L);

			if (slicer) SendDlgItemMessage(hDlg, IDC_LEVEL, CB_SETCURSEL, 0, 0L);

			if ((nOSType == OS_WINNT) && slicer) // HWi
			{
				MessageBox(hDlg,"In Windows NT, only the soundcard and RS232 are supported!", "PDW Setup", MB_ICONERROR);
				CheckDlgButton(hDlg, IDC_COMRS232,  true);
				CheckDlgButton(hDlg, IDC_COMSLICER, false);
			}
			else
			{
				SendDlgItemMessage(hDlg, IDC_COMPORT, CB_RESETCONTENT, 0, 0);

				if (bWin9x && !rs232)	// no RS232
				{
					for (i=0; i<5; i++)
					{
						SendDlgItemMessage(hDlg, IDC_COMPORT, CB_ADDSTRING, 0, (LPARAM) (LPSTR) comports[i]);
					}
					SendDlgItemMessage(hDlg, IDC_COMPORT, CB_SETCURSEL, (WPARAM) Profile.comPort - 1, 0L);

					for (i=0; i<9; i++)
					{
						SendDlgItemMessage(hDlg, IDC_COMIRQ, CB_ADDSTRING, 0, (LPARAM) (LPSTR) cirqs[i]);

						if (Profile.comPortIRQ == irqs[i])
						SendDlgItemMessage(hDlg, IDC_COMIRQ, CB_SETCURSEL, i, 0L);
					}
					sprintf(temp, "0x%03X", Profile.comPortAddr);
					SetDlgItemText(hDlg, IDC_COMADDR, (LPCSTR) temp);
				}
				else	// RS232 or win2k
				{
					for (i=0; pComPorts[i] != 0; i++)
					{
						sprintf(temp, "COM%i", pComPorts[i]);
						SendDlgItemMessage(hDlg, IDC_COMPORT, CB_ADDSTRING, 0, (LPARAM) (LPSTR) temp);
						if (Profile.comPort == pComPorts[i])
						{
							SendDlgItemMessage(hDlg, IDC_COMPORT, CB_SETCURSEL, (WPARAM) i, 0L);
						}
					}
					SendDlgItemMessage(hDlg, IDC_RS232MODE, CB_SETCURSEL, (WPARAM) Profile.comPortRS232, 0L);
					SendDlgItemMessage(hDlg, IDC_COMIRQ, CB_RESETCONTENT, 0, 0);
					SendDlgItemMessage(hDlg, IDC_COMIRQ, CB_ADDSTRING, 0, (LPARAM) (LPSTR) "N/A");
					SendDlgItemMessage(hDlg, IDC_COMIRQ, CB_SETCURSEL, 0, 0L);
					SetDlgItemText(hDlg, IDC_COMADDR, (LPCSTR) "N/A");
				}
			}
			EnableWindow(GetDlgItem(hDlg, IDC_COMADDR), (comport == 4) && slicer && bWin9x ? true : false);
			EnableWindow(GetDlgItem(hDlg, IDC_COMIRQ),  (comport == 4) && slicer && bWin9x ? true : false);
			EnableWindow(GetDlgItem(hDlg, IDC_COMPORT), serial);
			EnableWindow(GetDlgItem(hDlg, IDC_COMSLICER), serial);
			EnableWindow(GetDlgItem(hDlg, IDC_COMRS232), serial);
			EnableWindow(GetDlgItem(hDlg, IDC_RS232MODE), rs232 && !soundcard);
			EnableWindow(GetDlgItem(hDlg, IDC_LEVEL), serial && slicer);
			EnableWindow(GetDlgItem(hDlg, IDC_AUDIOCONFIG),  soundcard && audio_devices);
			EnableWindow(GetDlgItem(hDlg, IDC_AUDIODEVICES), soundcard && more_audio);
			EnableWindow(GetDlgItem(hDlg, IDC_AUDIOSAMPLERATE), soundcard && audio_devices);
			EnableWindow(GetDlgItem(hDlg, IDC_AUDIOCUSTOM), soundcard && !config);

			break;

			case IDC_COMPORT:

			slicer = IsDlgButtonChecked(hDlg, IDC_COMSLICER);

			if (bWin9x && slicer)
			{
				if (HIWORD(wParam) == CBN_SELENDOK)
				{
					comport = SendDlgItemMessage(hDlg, IDC_COMPORT, CB_GETCURSEL, 0, 0L);
					EnableWindow(GetDlgItem(hDlg, IDC_COMADDR),(comport == 4) ? true : false);
					EnableWindow(GetDlgItem(hDlg, IDC_COMIRQ), (comport == 4) ? true : false);

					switch (comport)
					{
						case 0:
							SetDlgItemText(hDlg, IDC_COMADDR, (LPCSTR) "0x3F8");
							SendDlgItemMessage(hDlg, IDC_COMIRQ, CB_SETCURSEL, 2, 0L);
						break;

						case 1:
							SetDlgItemText(hDlg, IDC_COMADDR, (LPCSTR) "0x2F8");
							SendDlgItemMessage(hDlg, IDC_COMIRQ, CB_SETCURSEL, 1, 0L);
						break;

						case 2:
							SetDlgItemText(hDlg, IDC_COMADDR, (LPCSTR) "0x3E8");
							SendDlgItemMessage(hDlg, IDC_COMIRQ, CB_SETCURSEL, 2, 0L);
						break;

						case 3:
 							SetDlgItemText(hDlg, IDC_COMADDR, (LPCSTR) "0x2E8");
							SendDlgItemMessage(hDlg, IDC_COMIRQ, CB_SETCURSEL, 1, 0L);
						break;
					}
				}
			}
			break;

			case IDOK:

			// Set audio configuration.
			Profile.audioConfig = SendDlgItemMessage(hDlg, IDC_AUDIOCONFIG, CB_GETCURSEL, 0, 0L);
			SetAudioConfig(Profile.audioConfig);

			// Set COMPORT Address/IRQ
			value = SendDlgItemMessage(hDlg, IDC_COMPORT, CB_GETCURSEL, 0, 0L);
			if (value == CB_ERR) Profile.comPort = 0;
			else if (bWin9x)
			{
				Profile.comPort = value + 1;

				switch (Profile.comPort)
				{
					case 1:
					default:
						Profile.comPortAddr = 0x3F8;
						Profile.comPortIRQ  = 4;
					break;

					case 2:
						Profile.comPortAddr = 0x2F8;
						Profile.comPortIRQ  = 3;
					break;

					case 3:
						Profile.comPortAddr = 0x3E8;
						Profile.comPortIRQ  = 4;
					break;

					case 4:
						Profile.comPortAddr = 0x2E8;
						Profile.comPortIRQ  = 3;
					break;

					case 5:

					if (GetDlgItemText(hDlg, IDC_COMADDR,(LPSTR) temp, 6) == 0)
					{
						MessageBox(hDlg,"You must provide an I/O Address!","PDW Setup",MB_ICONERROR);
						SetFocus(GetDlgItem(hDlg, IDC_COMADDR));
						return (FALSE);
					}

					if (SendDlgItemMessage(hDlg, IDC_COMIRQ, CB_GETCURSEL, 0, 0L) == CB_ERR)
					{
						MessageBox(hDlg,"You must select an IRQ vector!","PDW Setup",MB_ICONERROR);
						SetFocus(GetDlgItem(hDlg, IDC_COMIRQ));
						return (FALSE);
					}
					sscanf(temp, "%x", &Profile.comPortAddr);
					Profile.comPortIRQ = irqs[value];
				}
			}
			else	// OS => 2000
			{
				value = SendDlgItemMessage(hDlg, IDC_COMPORT, CB_GETCURSEL, 0, 0L);
		        SendDlgItemMessage(hDlg, IDC_COMPORT, CB_GETLBTEXT, value, (LPARAM) temp);
				Profile.comPort = atoi(&temp[3]);
			}

			// Get audio samplerate.
			value = SendDlgItemMessage(hDlg, IDC_AUDIOSAMPLERATE, CB_GETCURSEL, 0, 0L);

			if (value == CB_ERR) Profile.audioSampleRate = 44100;
			else
			{
				switch (value)
				{
					case 0: Profile.audioSampleRate = 8000;
					break;

					case 1: Profile.audioSampleRate = 11025;
					break;

					case 2: Profile.audioSampleRate = 22050;
					break;

					default:
					case 4: Profile.audioSampleRate = 44100;
					break;
				}
			}

			Profile.comPortEnabled = IsDlgButtonChecked(hDlg, IDC_COMENABLE);
			Profile.comPortRS232   = IsDlgButtonChecked(hDlg, IDC_COMRS232);
			if (Profile.comPortRS232) Profile.comPortRS232 += SendDlgItemMessage(hDlg, IDC_RS232MODE, CB_GETCURSEL, 0, 0L);
			Profile.audioEnabled   = IsDlgButtonChecked(hDlg, IDC_AUDIOENABLE);
			Profile.audioDevice    = SendDlgItemMessage(hDlg, IDC_AUDIODEVICES, CB_GETCURSEL, 0, 0L);

			if (Profile.audioDevice != old_device)
			{
				if (bCapturing)
				{
					KillTimer(ghWnd, PDW_TIMER);
					Stop_Capturing();
				}
				if (Start_Capturing())
				{
					SetTimer(ghWnd, PDW_TIMER, 100, (TIMERPROC) NULL);
				}
				else return (FALSE);
			}

			if (Profile.audioEnabled) Profile.fourlevel = 0;
			else Profile.fourlevel = SendDlgItemMessage(hDlg, IDC_LEVEL, CB_GETCURSEL, 0, 0L);

			WriteSettings();

			EndDialog(hDlg, TRUE);
			return (TRUE);

			case IDCANCEL:
			Profile.audioConfig = tmp_audioConfig;
			SetAudioConfig(Profile.audioConfig);
			EndDialog(hDlg, TRUE);
			return (TRUE);
		}
		break;
	}
	return (FALSE);
} // end of SetupDlgProc


BOOL NEAR SelectFont(HWND hDlg)
{
	CHOOSEFONT	cfFont;
	HDC			hDC;
	TEXTMETRIC	tm;
	RECT		rect;

	tmp_logfont.lfHeight		= Profile.fontInfo.lfHeight;
	tmp_logfont.lfWidth			= Profile.fontInfo.lfWidth;
	tmp_logfont.lfEscapement	= Profile.fontInfo.lfEscapement;
	tmp_logfont.lfOrientation	= Profile.fontInfo.lfOrientation;
	tmp_logfont.lfWeight		= Profile.fontInfo.lfWeight;
	tmp_logfont.lfItalic		= Profile.fontInfo.lfItalic;
	tmp_logfont.lfUnderline		= Profile.fontInfo.lfUnderline;
	tmp_logfont.lfStrikeOut		= Profile.fontInfo.lfStrikeOut;
	tmp_logfont.lfCharSet		= Profile.fontInfo.lfCharSet;
	tmp_logfont.lfOutPrecision	= Profile.fontInfo.lfOutPrecision;
	tmp_logfont.lfClipPrecision	= Profile.fontInfo.lfClipPrecision;
	tmp_logfont.lfQuality		= Profile.fontInfo.lfQuality;
	tmp_logfont.lfPitchAndFamily= Profile.fontInfo.lfPitchAndFamily;

	strcpy(tmp_logfont.lfFaceName,Profile.fontInfo.lfFaceName);

	cfFont.lStructSize		= sizeof(CHOOSEFONT);
	cfFont.hwndOwner		= hDlg;
	cfFont.hDC				= NULL;
	cfFont.rgbColors		= 0;
	cfFont.lpLogFont		= &tmp_logfont;
	cfFont.Flags			= CF_SCREENFONTS | CF_ENABLEHOOK |  CF_NOOEMFONTS | CF_LIMITSIZE |
							  CF_INITTOLOGFONTSTRUCT | CF_FIXEDPITCHONLY | CF_FORCEFONTEXIST;
	cfFont.lCustData		= 0;
	cfFont.lpfnHook			= CenterOpenDlgBox;
	cfFont.lpTemplateName	= NULL;
	cfFont.hInstance		= ghInstance;
	cfFont.nSizeMin			= 8;	// PH: Added 9-12-2005
	cfFont.nSizeMax			= 12;	// PH: Added 9-12-2005

	lstrcpy(szCenterOpenDlgMsg, TEXT("PDW Font"));

	if (ChooseFont(&cfFont))
	{
		// Font size must be limited to keep text within correct label fields.
		hDC = GetDC(hDlg);
		tmp_hfont = CreateFontIndirect(&tmp_logfont);
		SelectObject(hDC, tmp_hfont);
		GetTextMetrics(hDC, &tm);
		DeleteObject(tmp_hfont);

		Profile.fontInfo.lfHeight			= tmp_logfont.lfHeight;
		Profile.fontInfo.lfWidth			= tmp_logfont.lfWidth;
		Profile.fontInfo.lfEscapement		= tmp_logfont.lfEscapement;
		Profile.fontInfo.lfOrientation		= tmp_logfont.lfOrientation;
		Profile.fontInfo.lfWeight			= tmp_logfont.lfWeight;
		Profile.fontInfo.lfItalic			= tmp_logfont.lfItalic;
		Profile.fontInfo.lfUnderline		= tmp_logfont.lfUnderline;
		Profile.fontInfo.lfStrikeOut		= tmp_logfont.lfStrikeOut;
		Profile.fontInfo.lfCharSet			= tmp_logfont.lfCharSet;
		Profile.fontInfo.lfOutPrecision		= tmp_logfont.lfOutPrecision;
		Profile.fontInfo.lfClipPrecision	= tmp_logfont.lfClipPrecision;
		Profile.fontInfo.lfQuality			= tmp_logfont.lfQuality;
		Profile.fontInfo.lfPitchAndFamily	= tmp_logfont.lfPitchAndFamily;
		strcpy(Profile.fontInfo.lfFaceName,tmp_logfont.lfFaceName);

		DeleteObject(hfont);
		hfont = CreateFontIndirect(&Profile.fontInfo);
		SelectObject(hDC, hfont);
		GetTextMetrics(hDC, &tm);

		cxChar = tm.tmAveCharWidth;
		cxCaps = (tm.tmPitchAndFamily & 1 ? 3 : 2) * cxChar / 2;
		cyChar = tm.tmHeight + tm.tmExternalLeading;

		SetWindowPaneSize(WINDOW_SIZE);	// PH: Set Max_X_Client / iMaxWidth / NewLinePoint
		
		ReleaseDC(hDlg, hDC);

		GetWindowRect(Pane1.hWnd, &rect);
		SendMessage(Pane1.hWnd, WM_SIZE, SIZENORMAL, (LPARAM) MAKELONG(rect.right - rect.left, rect.bottom - rect.top));

		GetWindowRect(Pane2.hWnd, &rect);
		SendMessage(Pane2.hWnd, WM_SIZE, SIZENORMAL, (LPARAM) MAKELONG(rect.right - rect.left, rect.bottom - rect.top));

		InitializePane(&Pane1);
		InitializePane(&Pane2);

		DrawTitleBarGfx(ghWnd);		// Redraw pane1/pane2 title bars

		ClearPanes(true, true);		// Must start over so everything lines up

		WriteSettings();
	}
	return (TRUE);
} // end of SelectFont


BOOL FAR PASCAL ColorsDlgProc(HWND hDlg, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	static HWND hWndBox;
	static HWND hColorWnd;
	static RECT rect;
	static RECT client_rect;
	int			xPos, yPos;
	int			xSize, ySize;
	HDC			hDC;
	static CHOOSECOLOR cc;

	switch (uMsg)
	{
		case WM_INITDIALOG:
		if (!CenterWindow(hDlg)) return (FALSE);

		tmp_background	= Profile.color_background;
		tmp_address		= Profile.color_address;
		tmp_modetypebit	= Profile.color_modetypebit;
		tmp_timestamp	= Profile.color_timestamp;
		tmp_biterrors	= Profile.color_biterrors;
		tmp_numeric		= Profile.color_numeric;
		tmp_message		= Profile.color_message;
		tmp_misc		= Profile.color_misc;
		tmp_filtermatch	= Profile.color_filtermatch;
		tmp_filterlabel	= Profile.color_filterlabel[0];

		hWndBox = GetDlgItem(hDlg, IDC_COLORWIN);
		GetClientRect (hWndBox, &client_rect);
		xSize = client_rect.right;
		ySize = client_rect.bottom;

		GetWindowRect (hDlg, &rect);
		GetWindowRect (hWndBox, &client_rect);
		xPos = client_rect.left - rect.left -
		GetSystemMetrics(SM_CXDLGFRAME);

		yPos = client_rect.top - rect.top -
		GetSystemMetrics(SM_CYMENU) -
		GetSystemMetrics(SM_CYDLGFRAME);

		hColorWnd = CreateWindow(gszColorClass, NULL, WS_CHILD | WS_VISIBLE, xPos, yPos,
			xSize, ySize,	hDlg, NULL,	(HINSTANCE) GetWindowLong(hDlg, GWL_HINSTANCE),	0);

		DeleteObject(hboxbr);

		hboxbr = CreateSolidBrush(tmp_background);

		SetClassLong(hColorWnd, GCL_HBRBACKGROUND, (LONG) hboxbr);

		InvalidateRect(hColorWnd, NULL, TRUE);

		memset(&cc, 0, sizeof(CHOOSECOLOR));

		cc.lStructSize		= sizeof(CHOOSECOLOR);
		cc.hwndOwner		= hDlg;
		cc.rgbResult		= 0;
		cc.lpCustColors		= custom_colors;
		cc.Flags			= CC_RGBINIT | CC_ENABLEHOOK;
		cc.lCustData		= 0;
		cc.lpfnHook			= CenterOpenDlgBox;
		cc.lpTemplateName	= NULL;

		return (TRUE);

		case WM_COMMAND:
		switch (LOWORD(wParam))
		{
			case IDC_COLORBACKGND:
			lstrcpy(szCenterOpenDlgMsg, TEXT("Pocsag/Flex Background"));
			cc.rgbResult = tmp_background;

			if (ChooseColor(&cc))
			{
				hDC = GetDC(hDlg);
				tmp_background = GetNearestColor(hDC, cc.rgbResult);
				ReleaseDC(hDlg, hDC);

				DeleteObject(hboxbr);
				hboxbr = CreateSolidBrush(tmp_background);

				SetClassLong(hColorWnd, GCL_HBRBACKGROUND, (LONG) hboxbr);
				InvalidateRect(hColorWnd, NULL, TRUE);
			}
			break;

			case IDC_COLORCAPCODE:
			lstrcpy(szCenterOpenDlgMsg, TEXT("Pocsag/Flex Address"));
			cc.rgbResult = tmp_address;

			if (ChooseColor(&cc))
			{
				hDC = GetDC(hDlg);
				tmp_address = GetNearestColor(hDC, cc.rgbResult);
				ReleaseDC(hDlg, hDC);
				InvalidateRect(hColorWnd, NULL, TRUE);
			}
			break;

			case IDC_COLORFLEXPHASE:
			lstrcpy(szCenterOpenDlgMsg, TEXT("Pocsag Function / Flex Phase"));
			cc.rgbResult = tmp_modetypebit;

			if (ChooseColor(&cc))
			{
				hDC = GetDC(hDlg);
				tmp_modetypebit = GetNearestColor(hDC, cc.rgbResult);
				ReleaseDC(hDlg, hDC);
				InvalidateRect(hColorWnd, NULL, TRUE);
			}
			break;

			case IDC_COLORTIMESTAMP:
			lstrcpy(szCenterOpenDlgMsg, TEXT("Pocsag/Flex Time/Date"));
			cc.rgbResult = tmp_timestamp;

			if (ChooseColor(&cc))
			{
				hDC = GetDC(hDlg);
				tmp_timestamp = GetNearestColor(hDC, cc.rgbResult);
				ReleaseDC(hDlg, hDC);
				InvalidateRect(hColorWnd, NULL, TRUE);
			}
			break;

			case IDC_COLORBITERRORS:
			lstrcpy(szCenterOpenDlgMsg, TEXT("Pocsag/Flex Bit Errors"));
			cc.rgbResult = tmp_biterrors;

			if (ChooseColor(&cc))
			{
				hDC = GetDC(hDlg);
				tmp_biterrors = GetNearestColor(hDC, cc.rgbResult);
				ReleaseDC(hDlg, hDC);
				InvalidateRect(hColorWnd, NULL, TRUE);
			}
			break;

			case IDC_COLORNUMERIC:
			lstrcpy(szCenterOpenDlgMsg, TEXT("Pocsag/Flex Numeric/Tone"));
			cc.rgbResult = tmp_numeric;

			if (ChooseColor(&cc))
			{
				hDC = GetDC(hDlg);
				tmp_numeric = GetNearestColor(hDC, cc.rgbResult);
				ReleaseDC(hDlg, hDC);
				InvalidateRect(hColorWnd, NULL, TRUE);
			}
			break;

			case IDC_COLORALPHANUM:
			lstrcpy(szCenterOpenDlgMsg, TEXT("Pocsag/Flex Message"));
			cc.rgbResult = tmp_message;

			if (ChooseColor(&cc))
			{
				hDC = GetDC(hDlg);
				tmp_message = GetNearestColor(hDC, cc.rgbResult);
				ReleaseDC(hDlg, hDC);
				InvalidateRect(hColorWnd, NULL, TRUE);
			}
			break;

			case IDC_COLORFLEXBIN:
			lstrcpy(szCenterOpenDlgMsg, TEXT("Flex Binary"));
			cc.rgbResult = tmp_misc;

			if (ChooseColor(&cc))
			{
				hDC = GetDC(hDlg);
				tmp_misc = GetNearestColor(hDC, cc.rgbResult);
				ReleaseDC(hDlg, hDC);
				InvalidateRect(hColorWnd, NULL, TRUE);
			}
			break;

			case IDC_COLORFILTMATCH:
			lstrcpy(szCenterOpenDlgMsg, TEXT("Pocsag/Flex Filter Match"));
			cc.rgbResult = tmp_filtermatch;

			if (ChooseColor(&cc))
			{
				hDC = GetDC(hDlg);
				tmp_filtermatch = GetNearestColor(hDC, cc.rgbResult);
				ReleaseDC(hDlg, hDC);
				InvalidateRect(hColorWnd, NULL, TRUE);
			}
			break;

			case IDC_COLORFILTERLABEL:
			lstrcpy(szCenterOpenDlgMsg, TEXT("Pocsag/Flex Filter Label"));
			cc.rgbResult = tmp_filterlabel;

			if (ChooseColor(&cc))
			{
				hDC = GetDC(hDlg);
				tmp_filterlabel = GetNearestColor(hDC, cc.rgbResult);
				ReleaseDC(hDlg, hDC);
				InvalidateRect(hColorWnd, NULL, TRUE);
			}
			break;

			case IDC_COLORDEFAULT:

			if (MessageBox(hDlg, "Reset all colors to Default?", "PDW Reset colors", MB_ICONQUESTION | MB_OKCANCEL) == IDOK)
			{
				tmp_background	= RGB(rgbColor[BLACK][0],  rgbColor[BLACK][1],  rgbColor[BLACK][2]);
				tmp_address		= RGB(rgbColor[WHITE][0],  rgbColor[WHITE][1],  rgbColor[WHITE][2]);
				tmp_modetypebit	= RGB(rgbColor[RED][0],	   rgbColor[RED][1],    rgbColor[RED][2]);
				tmp_timestamp	= RGB(rgbColor[LTBLUE][0], rgbColor[LTBLUE][1], rgbColor[LTBLUE][2]);
				tmp_numeric		= RGB(rgbColor[RED][0],    rgbColor[RED][1],    rgbColor[RED][2]);
				tmp_message		= RGB(rgbColor[LTCYAN][0], rgbColor[LTCYAN][1], rgbColor[LTCYAN][2]);
				tmp_misc		= RGB(rgbColor[BROWN][0],  rgbColor[BROWN][1],  rgbColor[BROWN][2]);
				tmp_biterrors	= RGB(rgbColor[LTGRAY][0], rgbColor[LTGRAY][1], rgbColor[LTGRAY][2]);
				tmp_filtermatch	= RGB(rgbColor[LTGREEN][0],rgbColor[LTGREEN][1],rgbColor[LTGREEN][2]);
				tmp_filterlabel	= RGB(rgbColor[LTBLUE][0], rgbColor[LTBLUE][1], rgbColor[LTBLUE][2]);
				tmp_misc		= RGB(rgbColor[LTCYAN][0], rgbColor[LTCYAN][1], rgbColor[LTCYAN][2]);

				DeleteObject(hboxbr);
				hboxbr = CreateSolidBrush(tmp_background);

				SetClassLong(hColorWnd, GCL_HBRBACKGROUND, (LONG) hboxbr);
				InvalidateRect(hColorWnd, NULL, TRUE);
			}
			break;

			case IDOK:

			Profile.color_address		= tmp_address;
			Profile.color_modetypebit	= tmp_modetypebit;
			Profile.color_timestamp		= tmp_timestamp;
			Profile.color_biterrors		= tmp_biterrors;
			Profile.color_numeric		= tmp_numeric;
			Profile.color_message		= tmp_message;
			Profile.color_misc			= tmp_misc;
			Profile.color_filtermatch	= tmp_filtermatch;
			Profile.color_filterlabel[0]= tmp_filterlabel;

			if (Profile.color_background != tmp_background)
			{
				Profile.color_background  = tmp_background;
				DeleteObject(hbr);
				hbr = CreateSolidBrush(Profile.color_background);

				SetClassLong(Pane1.hWnd, GCL_HBRBACKGROUND, (LONG) hbr);
				SetClassLong(Pane2.hWnd, GCL_HBRBACKGROUND, (LONG) hbr);
			}

			InvalidateRect(Pane1.hWnd, NULL, TRUE);
			InvalidateRect(Pane2.hWnd, NULL, TRUE);

			WriteSettings();

			EndDialog(hDlg, TRUE);
			return (TRUE);

			case IDCANCEL:
				EndDialog(hDlg, TRUE);
			return (TRUE);
		}
		break;
	}
	return (FALSE);
} // end of ColorsDlgProc


LRESULT FAR PASCAL ColorWndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	HDC			hDC;
	HFONT		hOldFont;
	TEXTMETRIC	tm;
	PAINTSTRUCT	ps;
	POINT		lpPoint;
	int			tmp_cyChar, tmp_cxChar;

	switch (uMsg)
	{
		case WM_CREATE:
		break;

		case WM_PAINT:
		hDC = BeginPaint (hWnd, &ps);
		hOldFont = (HFONT)SelectObject(hDC, hboxfont);

		GetTextMetrics(hDC, &tm);

		tmp_cyChar = tm.tmHeight + tm.tmExternalLeading;
		tmp_cxChar = tm.tmAveCharWidth;

		SetBkMode(hDC, OPAQUE);
		SetBkColor(hDC, tmp_background);

		// let Windows handle aligning the characters as TextOut goes along
		SetTextAlign(hDC, TA_UPDATECP);

		// POCSAG/Numeric
		MoveToEx(hDC, 0, 0, &lpPoint);
		SetTextColor(hDC, tmp_address);
		TextOut(hDC, 0, 0, " 1234567", 8);
		SetTextColor(hDC, tmp_timestamp);
		TextOut(hDC, 0, 0, "  14:13:33 17-08-00", 19);
		SetTextColor(hDC, tmp_modetypebit);
		TextOut(hDC, 0, 0, "  POCSAG-1   512", 16);
		SetTextColor(hDC, tmp_numeric);
		TextOut(hDC, 0, 0, "  TONE-ONLY / 0123456789", 24);

		// POCSAG/Alphanumeric
		MoveToEx(hDC, 0, tmp_cyChar, &lpPoint);
		SetTextColor(hDC, tmp_address);
		TextOut(hDC, 0, 0, " 7654321", 8);
		SetTextColor(hDC, tmp_timestamp);
		TextOut(hDC, 0, 0, "  15:14:40 18-08-00", 19);
		SetTextColor(hDC, tmp_modetypebit);
		TextOut(hDC, 0, 0, "  POCSAG-2  1200", 16);
		SetTextColor(hDC, tmp_message);
		TextOut(hDC, 0, 0, "  Alphanumeric", 14);

		// POCSAG/Alphanumeric/Bit Errors
		MoveToEx(hDC, 0, tmp_cyChar*2, &lpPoint);
		SetTextColor(hDC, tmp_address);
		TextOut(hDC, 0, 0, " 1234567", 8);
		SetTextColor(hDC, tmp_timestamp);
		TextOut(hDC, 0, 0, "  16:15:20 19-08-00", 19);
		SetTextColor(hDC, tmp_modetypebit);
		TextOut(hDC, 0, 0, "  POCSAG-3  2400", 16);
		SetTextColor(hDC, tmp_message);
		TextOut(hDC, 0, 0, "  Bit", 5);
		SetTextColor(hDC, tmp_biterrors);
		TextOut(hDC, 0, 0, "errors", 6);

		// POCSAG/Alphanumeric/Filter Match
		MoveToEx(hDC, 0, tmp_cyChar*3, &lpPoint);
		SetTextColor(hDC, tmp_address);
		TextOut(hDC, 0, 0, " 7654321", 8);
		SetTextColor(hDC, tmp_timestamp);
		TextOut(hDC, 0, 0, "  17:12:14 20-08-00", 19);
		SetTextColor(hDC, tmp_modetypebit);
		TextOut(hDC, 0, 0, "  FLEX-A    1600", 16);
		SetTextColor(hDC, tmp_message);
		TextOut(hDC, 0, 0, "  Text", 6);
		SetTextColor(hDC, tmp_filtermatch);
		TextOut(hDC, 0, 0, " match", 6);

		// FLEX/Binary
		MoveToEx(hDC, 0, tmp_cyChar*4, &lpPoint);
		SetTextColor(hDC, tmp_address);
		TextOut(hDC, 0, 0, " 1234567", 8);
		SetTextColor(hDC, tmp_timestamp);
		TextOut(hDC, 0, 0, "  20:20:14 23-08-00", 19);
		SetTextColor(hDC, tmp_modetypebit);
		TextOut(hDC, 0, 0, "  FLEX-B    3200", 16);
		SetTextColor(hDC, tmp_misc);
		TextOut(hDC, 0, 0, "  FLEX Binary", 13);

		// FLEX/Short Instructions
		MoveToEx(hDC, 0, tmp_cyChar*5, &lpPoint);
		SetTextColor(hDC, tmp_address);
		TextOut(hDC, 0, 0, " 7654321", 8);
		SetTextColor(hDC, tmp_timestamp);
		TextOut(hDC, 0, 0, "  19:40:21 22-08-00", 19);
		SetTextColor(hDC, tmp_modetypebit);
		TextOut(hDC, 0, 0, "  FLEX-C    6400", 16);
		SetTextColor(hDC, tmp_misc);
		TextOut(hDC, 0, 0, "  TEMPORARY ADDRESS: 2029568", 28);

		// FLEX/Alphanumeric
		MoveToEx(hDC, tmp_cxChar*45, tmp_cyChar*6, &lpPoint);
		SetTextColor(hDC, tmp_filterlabel);
		TextOut(hDC, 0, 0, "- Filter Label -", 16);

		SelectObject(hDC, hOldFont);
		EndPaint (hWnd, &ps);

		break; 

		case WM_CLOSE:	// fall through
		default:
		return (DefWindowProc(hWnd, uMsg, wParam, lParam));
	}
	return 0L;
} // end of ColorWndProc


BOOL FAR PASCAL ACARSColorsDlgProc(HWND hDlg, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	static HWND hWndBox;
	static HWND hColorWnd;
	static RECT rect;
	static RECT client_rect;
	int			xPos, yPos;
	int			xSize, ySize;
	HDC			hDC;
	static CHOOSECOLOR cc;

	switch (uMsg)
	{
		case WM_INITDIALOG:
		if (!CenterWindow(hDlg)) return (FALSE);

		tmp_address		= Profile.color_ac_message_nr;
		tmp_misc		= Profile.color_ac_dbi;
		tmp_modetypebit	= Profile.color_modetypebit;
		tmp_misc		= Profile.color_misc;
		tmp_background	= Profile.color_background;
		tmp_filtermatch	= Profile.color_filtermatch;
		tmp_filterlabel	= Profile.color_filterlabel[0];

		hWndBox = GetDlgItem(hDlg, IDC_AC_COLORWIN);
		GetClientRect (hWndBox, &client_rect);
		xSize = client_rect.right;
		ySize = client_rect.bottom;

		GetWindowRect (hDlg, &rect);
		GetWindowRect (hWndBox, &client_rect);
		xPos = client_rect.left - rect.left -
			   GetSystemMetrics(SM_CXDLGFRAME);

		yPos = client_rect.top - rect.top -
			   GetSystemMetrics(SM_CYMENU) -
			   GetSystemMetrics(SM_CYDLGFRAME);

		hColorWnd = CreateWindow(gszACARSColorClass, NULL,	WS_CHILD | WS_VISIBLE, xPos, yPos,
			xSize, ySize, hDlg, NULL, (HINSTANCE) GetWindowLong(hDlg, GWL_HINSTANCE), 0);

		DeleteObject(hboxbr);

		hboxbr = CreateSolidBrush(tmp_background);
		SetClassLong(hColorWnd, GCL_HBRBACKGROUND, (LONG) hboxbr);
		InvalidateRect(hColorWnd, NULL, TRUE);

		memset(&cc, 0, sizeof(CHOOSECOLOR));

		cc.lStructSize		= sizeof(CHOOSECOLOR);
		cc.hwndOwner		= hDlg;
		cc.rgbResult		= 0;
		cc.lpCustColors		= custom_colors;
		cc.Flags			= CC_RGBINIT | CC_ENABLEHOOK;
		cc.lCustData		= 0;
		cc.lpfnHook			= CenterOpenDlgBox;
		cc.lpTemplateName	= NULL;

		return (TRUE);

		case WM_COMMAND:
		switch (LOWORD(wParam))
		{
			case IDC_AC_COLORBACKGND:
			lstrcpy(szCenterOpenDlgMsg, TEXT("ACARS Background"));
			cc.rgbResult = tmp_background;

			if(ChooseColor(&cc))
			{
				hDC = GetDC(hDlg);
				tmp_background = GetNearestColor(hDC, cc.rgbResult);
				ReleaseDC(hDlg, hDC);

				DeleteObject(hboxbr);

				hboxbr = CreateSolidBrush(tmp_background);

				SetClassLong(hColorWnd, GCL_HBRBACKGROUND, (LONG) hboxbr);

				InvalidateRect(hColorWnd, NULL, TRUE);
			}
			break;

			case IDC_AC_COLORMSGNO:
			lstrcpy(szCenterOpenDlgMsg, TEXT("ACARS Message Number"));
			cc.rgbResult = tmp_address;

			if(ChooseColor(&cc))
			{
				hDC = GetDC(hDlg);
				tmp_address = GetNearestColor(hDC, cc.rgbResult);
				ReleaseDC(hDlg, hDC);
				InvalidateRect(hColorWnd, NULL, TRUE);
			}
			break;

			case IDC_AC_COLORDBI:
			lstrcpy(szCenterOpenDlgMsg, TEXT("ACARS DBI"));
			cc.rgbResult = tmp_misc;

			if(ChooseColor(&cc))
			{
				hDC = GetDC(hDlg);
				tmp_misc = GetNearestColor(hDC, cc.rgbResult);
				ReleaseDC(hDlg, hDC);
				InvalidateRect(hColorWnd, NULL, TRUE);
			}
			break;

			case IDC_AC_COLORTIMESTAMP:
			lstrcpy(szCenterOpenDlgMsg, TEXT("ACARS Time/Date"));
			cc.rgbResult = tmp_timestamp;

			if(ChooseColor(&cc))
			{
				hDC = GetDC(hDlg);
				tmp_timestamp = GetNearestColor(hDC, cc.rgbResult);
				ReleaseDC(hDlg, hDC);
				InvalidateRect(hColorWnd, NULL, TRUE);
			}
			break;

			case IDC_AC_COLORMODE:
			lstrcpy(szCenterOpenDlgMsg, TEXT("ACARS Mode"));
			cc.rgbResult = tmp_modetypebit;

			if(ChooseColor(&cc))
			{
				hDC = GetDC(hDlg);
				tmp_modetypebit = GetNearestColor(hDC, cc.rgbResult);
				ReleaseDC(hDlg, hDC);
				InvalidateRect(hColorWnd, NULL, TRUE);
			}
			break;

			case IDC_AC_COLORTAGS:
			lstrcpy(szCenterOpenDlgMsg, TEXT("ACARS Labels"));
			cc.rgbResult = tmp_misc;

			if(ChooseColor(&cc))
			{
				hDC = GetDC(hDlg);
				tmp_misc = GetNearestColor(hDC, cc.rgbResult);
				ReleaseDC(hDlg, hDC);
				InvalidateRect(hColorWnd, NULL, TRUE);
			}
			break;

			case IDC_AC_COLORMSG:
			lstrcpy(szCenterOpenDlgMsg, TEXT("ACARS Message"));
			cc.rgbResult = tmp_message;

			if(ChooseColor(&cc))
			{
				hDC = GetDC(hDlg);
				tmp_message = GetNearestColor(hDC, cc.rgbResult);
				ReleaseDC(hDlg, hDC);
				InvalidateRect(hColorWnd, NULL, TRUE);
			}
			break;

			case IDC_AC_COLORBITERRORS:
			lstrcpy(szCenterOpenDlgMsg, TEXT("ACARS Bit Errors"));
			cc.rgbResult = tmp_biterrors;

			if(ChooseColor(&cc))
			{
				hDC = GetDC(hDlg);
				tmp_biterrors = GetNearestColor(hDC, cc.rgbResult);
				ReleaseDC(hDlg, hDC);
				InvalidateRect(hColorWnd, NULL, TRUE);
			}
			break;

			case IDC_AC_COLORFILTMATCH:
			lstrcpy(szCenterOpenDlgMsg, TEXT("ACARS Filter Match"));
			cc.rgbResult = tmp_filtermatch;

			if(ChooseColor(&cc))
			{
				hDC = GetDC(hDlg);
				tmp_filtermatch = GetNearestColor(hDC, cc.rgbResult);
				ReleaseDC(hDlg, hDC);
				InvalidateRect(hColorWnd, NULL, TRUE);
			}
			break;

			case IDC_AC_COLORFILTERLABEL:
			lstrcpy(szCenterOpenDlgMsg, TEXT("ACARS Filter Label"));
			cc.rgbResult = tmp_filterlabel;

			if(ChooseColor(&cc))
			{
				hDC = GetDC(hDlg);
				tmp_filterlabel = GetNearestColor(hDC, cc.rgbResult);
				ReleaseDC(hDlg, hDC);
				InvalidateRect(hColorWnd, NULL, TRUE);
			}
			break;

			case IDC_AC_COLORDEFAULT:

			if (MessageBox(hDlg, "Reset all colors to Default?", "ACARS Reset colors", MB_ICONQUESTION | MB_OKCANCEL) == IDOK)
			{
				tmp_address		= RGB(rgbColor[WHITE][0], 
										rgbColor[WHITE][1],
										rgbColor[WHITE][2]);
				tmp_timestamp	= RGB(rgbColor[LTBLUE][0],
										rgbColor[LTBLUE][1],
										rgbColor[LTBLUE][2]);
				tmp_misc		= RGB(rgbColor[RED][0], 
										rgbColor[RED][1],
										rgbColor[RED][2]);
				tmp_modetypebit	= RGB(rgbColor[RED][0],
										rgbColor[RED][1],
										rgbColor[RED][2]);
				tmp_misc		= RGB(rgbColor[LTCYAN][0],
										rgbColor[LTCYAN][1],
										rgbColor[LTCYAN][2]);
				tmp_message		= RGB(rgbColor[WHITE][0],
										rgbColor[WHITE][1],
										rgbColor[WHITE][2]);
				tmp_biterrors	= RGB(rgbColor[LTGRAY][0],
										rgbColor[LTGRAY][1],
										rgbColor[LTGRAY][2]);
				tmp_background	= RGB(rgbColor[BLACK][0],
										rgbColor[BLACK][1],
										rgbColor[BLACK][2]);
				tmp_filtermatch	= RGB(rgbColor[LTGREEN][0],
										rgbColor[LTGREEN][1],
										rgbColor[LTGREEN][2]);
				tmp_filterlabel	= RGB(rgbColor[LTBLUE][0],
										rgbColor[LTBLUE][1],
										rgbColor[LTBLUE][2]);
				DeleteObject(hboxbr);
				hboxbr = CreateSolidBrush(tmp_background);
				SetClassLong(hColorWnd, GCL_HBRBACKGROUND, (LONG) hboxbr);
				InvalidateRect(hColorWnd, NULL, TRUE);
			}
			break;

			case IDOK:

			Profile.color_ac_message_nr	= tmp_address;
			Profile.color_timestamp		= tmp_timestamp;
			Profile.color_ac_dbi		= tmp_misc;
			Profile.color_modetypebit	= tmp_modetypebit;
			Profile.color_misc			= tmp_misc;
			Profile.color_message		= tmp_message;
			Profile.color_biterrors		= tmp_biterrors;
			Profile.color_filtermatch	= tmp_filtermatch;
			Profile.color_filterlabel[0]= tmp_filterlabel;


			if (Profile.color_background != tmp_background)
			{
				Profile.color_background  = tmp_background;

				DeleteObject(hbr);

				hbr = CreateSolidBrush(Profile.color_background);

				SetClassLong(Pane1.hWnd, GCL_HBRBACKGROUND, (LONG) hbr);
				SetClassLong(Pane2.hWnd, GCL_HBRBACKGROUND, (LONG) hbr);
			}

			InvalidateRect(Pane1.hWnd, NULL, TRUE);
			InvalidateRect(Pane2.hWnd, NULL, TRUE);

			WriteSettings();

			EndDialog(hDlg, TRUE);
			return (TRUE);

			case IDCANCEL:
			EndDialog(hDlg, TRUE);
			return (TRUE);
		}
	   break;
   }
   return (FALSE);
} // end of ACARSColorsDlgProc

LRESULT FAR PASCAL ACARSColorWndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	HDC			hDC;
	HFONT			hOldFont;
	TEXTMETRIC	tm;
	PAINTSTRUCT	ps;
	POINT			lpPoint;
	int			tmp_cyChar;

	switch (uMsg)
	{
		case WM_CREATE:
		break;

		case WM_PAINT:
		hDC = BeginPaint (hWnd, &ps);
		hOldFont = (HFONT)SelectObject(hDC, hboxfont);

		GetTextMetrics(hDC, &tm);

		tmp_cyChar = tm.tmHeight + tm.tmExternalLeading;

		SetBkMode(hDC, OPAQUE);
		SetBkColor(hDC, tmp_background);

		// let Windows handle aligning the characters as TextOut goes along
		SetTextAlign(hDC, TA_UPDATECP);

		// ACARS example message
		MoveToEx(hDC, 0, 0, &lpPoint);
		SetTextColor(hDC, tmp_address);
		TextOut(hDC, 0, 0, "   MO5A", 7);
		SetTextColor(hDC, tmp_timestamp);
		TextOut(hDC, 0, 0, "  18:37:34 06/04/01", 19);
		SetTextColor(hDC, tmp_misc);
		TextOut(hDC, 0, 0, "  2", 3);
		SetTextColor(hDC, tmp_modetypebit);
		TextOut(hDC, 0, 0, "  R", 3);
		SetTextColor(hDC, tmp_misc);
		TextOut(hDC, 0, 0, "  Msg Label:", 12);
		SetTextColor(hDC, tmp_message);
		TextOut(hDC, 0, 0, " B1...REQUEST OCL", 17);

		// Next line
		MoveToEx(hDC, 0, tmp_cyChar, &lpPoint);
		SetTextColor(hDC, tmp_misc);
		TextOut(hDC, 0, 0, "   Aircraft Reg:", 16);
		SetTextColor(hDC, tmp_message);
		TextOut(hDC, 0, 0, " .D-AIGH", 8);
		SetTextColor(hDC, tmp_misc);
		TextOut(hDC, 0, 0, "  Country:", 10);
		SetTextColor(hDC, tmp_message);
		TextOut(hDC, 0, 0, " GERMANY", 8);

		// Next line
		MoveToEx(hDC, 0, tmp_cyChar*2, &lpPoint);
		SetTextColor(hDC, tmp_misc);
		TextOut(hDC, 0, 0, "   Aircraft Type:", 17);
		SetTextColor(hDC, tmp_message);
		TextOut(hDC, 0, 0, " AIRBUS A.340-311", 17);

		// Next line
		MoveToEx(hDC, 0, tmp_cyChar*3, &lpPoint);
		SetTextColor(hDC, tmp_misc);
		TextOut(hDC, 0, 0, "   Flight No:", 13);
		SetTextColor(hDC, tmp_filtermatch);
		TextOut(hDC, 0, 0, " LH0410", 7);
		SetTextColor(hDC, tmp_misc);
		TextOut(hDC, 0, 0, "  Airline:", 10);
		SetTextColor(hDC, tmp_message);
		TextOut(hDC, 0, 0, " DEUTCHE LUFTHANSA", 18);

		// Next line
		MoveToEx(hDC, 0, tmp_cyChar*4, &lpPoint);
		SetTextColor(hDC, tmp_misc);
		TextOut(hDC, 0, 0, "   Using Ground Station:", 24);
		SetTextColor(hDC, tmp_message);
		TextOut(hDC, 0, 0, " Ronaldsway", 11);

		// Next line
		MoveToEx(hDC, 0, tmp_cyChar*5, &lpPoint);
		SetTextColor(hDC, tmp_misc);
		TextOut(hDC, 0, 0, "   Message:", 11);
		SetTextColor(hDC, tmp_message);
		TextOut(hDC, 0, 0, " /PIKCLYA.OCL/", 14);
		SetTextColor(hDC, tmp_biterrors);
		TextOut(hDC, 0, 0, "RCL 024", 7);
		SetTextColor(hDC, tmp_message);
		TextOut(hDC, 0, 0, " DLH410-54N015W/", 16);

		// Next line
		MoveToEx(hDC, 0, tmp_cyChar*6, &lpPoint);
		SetTextColor(hDC, tmp_message);
		TextOut(hDC, 0, 0, "   1401 M083F360  -RMK/67F5", 27);

		// Next line
		MoveToEx(hDC, 0, tmp_cyChar*7, &lpPoint);
		SetTextColor(hDC, tmp_timestamp);
		TextOut(hDC, 0, 0, "   DOWNLINK NAK", 15);
		SetTextColor(hDC, tmp_filterlabel);
		TextOut(hDC, 0, 0, " <LH>", 5);

		SelectObject(hDC, hOldFont);
		EndPaint (hWnd, &ps);

		break; 

		case WM_CLOSE:	// fall through

		default:
		return (DefWindowProc(hWnd, uMsg, wParam, lParam));
	}
	return 0L;
} // end of ACARSColorWndProc


BOOL FAR PASCAL MOBITEXColorsDlgProc(HWND hDlg, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	static HWND hWndBox;
	static HWND hColorWnd;
	static RECT rect;
	static RECT client_rect;
	int			xPos, yPos;
	int			xSize, ySize;
	HDC			hDC;
	static CHOOSECOLOR cc;

	switch (uMsg)
	{
		case WM_INITDIALOG:
		if (!CenterWindow(hDlg)) return (FALSE);

		tmp_address		= Profile.color_address;
		tmp_timestamp	= Profile.color_timestamp;
		tmp_misc		= Profile.color_misc;
		tmp_modetypebit	= Profile.color_modetypebit;
		tmp_message		= Profile.color_message;
		tmp_misc		= Profile.color_misc;
		tmp_biterrors	= Profile.color_biterrors;
		tmp_filtermatch	= Profile.color_filtermatch;
		tmp_filterlabel	= Profile.color_filterlabel[0];
		tmp_background	= Profile.color_background;

		hWndBox = GetDlgItem(hDlg, IDC_MB_COLORWIN);
		GetClientRect (hWndBox, &client_rect);
		xSize = client_rect.right;
		ySize = client_rect.bottom;

		GetWindowRect (hDlg, &rect);
		GetWindowRect (hWndBox, &client_rect);
		xPos = client_rect.left - rect.left -
				 GetSystemMetrics(SM_CXDLGFRAME);

		yPos = client_rect.top - rect.top -
				 GetSystemMetrics(SM_CYMENU) -
				 GetSystemMetrics(SM_CYDLGFRAME);

		hColorWnd = CreateWindow(gszMOBITEXColorClass, NULL, WS_CHILD | WS_VISIBLE, xPos, yPos,
			xSize, ySize, hDlg, NULL, (HINSTANCE) GetWindowLong(hDlg, GWL_HINSTANCE), 0);

		DeleteObject(hboxbr);

		hboxbr = CreateSolidBrush(tmp_background);

		SetClassLong(hColorWnd, GCL_HBRBACKGROUND, (LONG) hboxbr);

		InvalidateRect(hColorWnd, NULL, TRUE);

		memset(&cc, 0, sizeof(CHOOSECOLOR));

		cc.lStructSize		= sizeof(CHOOSECOLOR);
		cc.hwndOwner		= hDlg;
		cc.rgbResult		= 0;
		cc.lpCustColors		= custom_colors;
		cc.Flags			= CC_RGBINIT | CC_ENABLEHOOK;
		cc.lCustData		= 0;
		cc.lpfnHook			= CenterOpenDlgBox;
		cc.lpTemplateName	= NULL;

		return (TRUE);

		case WM_COMMAND:
		switch (LOWORD(wParam))
		{
			case IDC_MB_COLORBACKGND :
			lstrcpy(szCenterOpenDlgMsg, TEXT("Mobitex Background"));
			cc.rgbResult = tmp_background;

			if(ChooseColor(&cc))
			{
				hDC = GetDC(hDlg);
				tmp_background = GetNearestColor(hDC, cc.rgbResult);
				ReleaseDC(hDlg, hDC);

				DeleteObject(hboxbr);

				hboxbr = CreateSolidBrush(tmp_background);

				SetClassLong(hColorWnd, GCL_HBRBACKGROUND, (LONG) hboxbr);

				InvalidateRect(hColorWnd, NULL, TRUE);
			}
			break;

			case IDC_MB_COLORMAN :
			lstrcpy(szCenterOpenDlgMsg, TEXT("Mobitex MAN's"));
			cc.rgbResult = tmp_address;

			if(ChooseColor(&cc))
			{
				hDC = GetDC(hDlg);
				tmp_address = GetNearestColor(hDC, cc.rgbResult);
				ReleaseDC(hDlg, hDC);
				InvalidateRect(hColorWnd, NULL, TRUE);
			}
			break;

			case IDC_MB_COLORTIMESTAMP :
			lstrcpy(szCenterOpenDlgMsg, TEXT("Mobitex Time/Date"));
			cc.rgbResult = tmp_timestamp;

			if(ChooseColor(&cc))
			{
				hDC = GetDC(hDlg);
				tmp_timestamp = GetNearestColor(hDC, cc.rgbResult);
				ReleaseDC(hDlg, hDC);
				InvalidateRect(hColorWnd, NULL, TRUE);
			}
			break;

			case IDC_MB_COLORSENDER :
			lstrcpy(szCenterOpenDlgMsg, TEXT("Mobitex Sender"));
			cc.rgbResult = tmp_misc;

			if(ChooseColor(&cc))
			{
				hDC = GetDC(hDlg);
				tmp_misc = GetNearestColor(hDC, cc.rgbResult);
				ReleaseDC(hDlg, hDC);
				InvalidateRect(hColorWnd, NULL, TRUE);
			}
			break;

			case IDC_MB_COLORTYPE:
			lstrcpy(szCenterOpenDlgMsg, TEXT("Mobitex Type"));
			cc.rgbResult = tmp_modetypebit;

			if(ChooseColor(&cc))
			{
				hDC = GetDC(hDlg);
				tmp_modetypebit = GetNearestColor(hDC, cc.rgbResult);
				ReleaseDC(hDlg, hDC);
				InvalidateRect(hColorWnd, NULL, TRUE);
			}
			break;

			case IDC_MB_COLORMESSAGE:
			lstrcpy(szCenterOpenDlgMsg, TEXT("Mobitex Message"));
			cc.rgbResult = tmp_message;

			if(ChooseColor(&cc))
			{
				hDC = GetDC(hDlg);
				tmp_message = GetNearestColor(hDC, cc.rgbResult);
				ReleaseDC(hDlg, hDC);
				InvalidateRect(hColorWnd, NULL, TRUE);
			}
			break;

			case IDC_MB_COLORMISC:
			lstrcpy(szCenterOpenDlgMsg, TEXT("Mobitex MISC"));
			cc.rgbResult = tmp_misc;

			if(ChooseColor(&cc))
			{
				hDC = GetDC(hDlg);
				tmp_misc = GetNearestColor(hDC, cc.rgbResult);
				ReleaseDC(hDlg, hDC);
				InvalidateRect(hColorWnd, NULL, TRUE);
			}
			break;

			case IDC_MB_COLORBITERRORS:
			lstrcpy(szCenterOpenDlgMsg, TEXT("Mobitex Bit Errors"));
			cc.rgbResult = tmp_biterrors;

			if(ChooseColor(&cc))
			{
				hDC = GetDC(hDlg);
				tmp_biterrors = GetNearestColor(hDC, cc.rgbResult);
				ReleaseDC(hDlg, hDC);
				InvalidateRect(hColorWnd, NULL, TRUE);
			}
			break;

			case IDC_MB_COLORFILTMATCH:
			lstrcpy(szCenterOpenDlgMsg, TEXT("Mobitex Filter Match"));
			cc.rgbResult = tmp_filtermatch;

			if(ChooseColor(&cc))
			{
				hDC = GetDC(hDlg);
				tmp_filtermatch = GetNearestColor(hDC, cc.rgbResult);
				ReleaseDC(hDlg, hDC);
				InvalidateRect(hColorWnd, NULL, TRUE);
			}
			break;

			case IDC_MB_COLORFILTERLABEL:
			lstrcpy(szCenterOpenDlgMsg, TEXT("Mobitex Filter Label"));
			cc.rgbResult = tmp_filterlabel;

			if(ChooseColor(&cc))
			{
				hDC = GetDC(hDlg);
				tmp_filterlabel = GetNearestColor(hDC, cc.rgbResult);
				ReleaseDC(hDlg, hDC);
				InvalidateRect(hColorWnd, NULL, TRUE);
			}
			break;

			case IDC_MB_COLORDEFAULT:

			if (MessageBox(hDlg, "Reset all colors to Default?", "Mobitex Reset colors", MB_ICONQUESTION | MB_OKCANCEL) == IDOK)
			{
				tmp_address		= RGB(rgbColor[WHITE][0],
										rgbColor[WHITE][1],
										rgbColor[WHITE][2]);
				tmp_timestamp	= RGB(rgbColor[LTBLUE][0],
										rgbColor[LTBLUE][1],
										rgbColor[LTBLUE][2]);
				tmp_misc		= RGB(rgbColor[RED][0],
										rgbColor[RED][1],
										rgbColor[RED][2]);
				tmp_modetypebit	= RGB(rgbColor[WHITE][0], 
										rgbColor[WHITE][1],
										rgbColor[WHITE][2]);
				tmp_message		= RGB(rgbColor[LTCYAN][0],
										rgbColor[LTCYAN][1],
										rgbColor[LTCYAN][2]);
				tmp_misc		= RGB(rgbColor[RED][0], 
										rgbColor[RED][1],
										rgbColor[RED][2]);
				tmp_biterrors	= RGB(rgbColor[LTGRAY][0],
										rgbColor[LTGRAY][1],
										rgbColor[LTGRAY][2]);
				tmp_background	= RGB(rgbColor[BLACK][0],
										rgbColor[BLACK][1],
										rgbColor[BLACK][2]);
				tmp_filtermatch	= RGB(rgbColor[LTGREEN][0],
										rgbColor[LTGREEN][1],
										rgbColor[LTGREEN][2]);
				tmp_filterlabel	= RGB(rgbColor[LTBLUE][0],
										rgbColor[LTBLUE][1],
										rgbColor[LTBLUE][2]);

				DeleteObject(hboxbr);

				hboxbr = CreateSolidBrush(tmp_background);

				SetClassLong(hColorWnd, GCL_HBRBACKGROUND, (LONG) hboxbr);

				InvalidateRect(hColorWnd, NULL, TRUE);
			}
			break;

			case IDOK:

			Profile.color_address		= tmp_address;
			Profile.color_timestamp		= tmp_timestamp;
			Profile.color_misc			= tmp_misc;
			Profile.color_modetypebit	= tmp_modetypebit;
			Profile.color_message		= tmp_message;
			Profile.color_misc			= tmp_misc;
			Profile.color_biterrors		= tmp_biterrors;
			Profile.color_filtermatch	= tmp_filtermatch;
			Profile.color_filterlabel[0]= tmp_filterlabel;

			if (Profile.color_background != tmp_background)
			{
				Profile.color_background  = tmp_background;

				DeleteObject(hbr);

				hbr = CreateSolidBrush(Profile.color_background);

				SetClassLong(Pane1.hWnd, GCL_HBRBACKGROUND, (LONG) hbr);
				SetClassLong(Pane2.hWnd, GCL_HBRBACKGROUND, (LONG) hbr);
			}
			InvalidateRect(Pane1.hWnd, NULL, TRUE);
			InvalidateRect(Pane2.hWnd, NULL, TRUE);

			WriteSettings();

			EndDialog(hDlg, TRUE);
			return (TRUE);

			case IDCANCEL:
			EndDialog(hDlg, TRUE);
			return (TRUE);
		}
		break;
	}
	return (FALSE);
} // end of MOBITEXColorsDlgProc


LRESULT FAR PASCAL MOBITEXColorWndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	HDC			hDC;
	HFONT		hOldFont;
	TEXTMETRIC	tm;
	PAINTSTRUCT	ps;
	POINT		lpPoint;
	int			tmp_cyChar, tmp_cxChar;

	switch (uMsg)
	{
		case WM_CREATE:
		break;

		case WM_PAINT:
		hDC = BeginPaint (hWnd, &ps);
		hOldFont = (HFONT)SelectObject(hDC, hboxfont);

		GetTextMetrics(hDC, &tm);

		tmp_cyChar = tm.tmHeight + tm.tmExternalLeading;
		tmp_cxChar = tm.tmAveCharWidth;

		SetBkMode(hDC, OPAQUE);
		SetBkColor(hDC, tmp_background);

		// let Windows handle aligning the characters as TextOut goes along
		SetTextAlign(hDC, TA_UPDATECP);

		// MOBITEX example message
		MoveToEx(hDC, 0, 0, &lpPoint);
		SetTextColor(hDC, tmp_address);
		TextOut(hDC, 0, 0, "  <ALL> ", 8);
		SetTextColor(hDC, tmp_timestamp);
		TextOut(hDC, 0, 0, " 13:06:46 12-04-08", 18);
		SetTextColor(hDC, tmp_misc);
		TextOut(hDC, 0, 0, " NETWORK", 8);
		SetTextColor(hDC, tmp_modetypebit);
		TextOut(hDC, 0, 0, "  MPAK  8000", 12);
		SetTextColor(hDC, tmp_misc);
		TextOut(hDC, 0, 0, "  [TIME: 08:48 20-04-2008]", 26);

		// Next line
		MoveToEx(hDC, 0, tmp_cyChar, &lpPoint);
		SetTextColor(hDC, tmp_address);
		TextOut(hDC, 0, 0, " 4361264", 8);
		SetTextColor(hDC, tmp_timestamp);
		TextOut(hDC, 0, 0, " 13:06:45 12-04-08", 18);
		SetTextColor(hDC, tmp_misc);
		TextOut(hDC, 0, 0, " NETWORK", 8);
		SetTextColor(hDC, tmp_modetypebit);
		TextOut(hDC, 0, 0, "  MPAK  8000", 12);
		SetTextColor(hDC, tmp_misc);
		TextOut(hDC, 0, 0, "  <ACK>", 7);

		// Next line
		MoveToEx(hDC, 0, tmp_cyChar*2, &lpPoint);
		SetTextColor(hDC, tmp_address);
		TextOut(hDC, 0, 0, " 4360490", 8);
		SetTextColor(hDC, tmp_timestamp);
		TextOut(hDC, 0, 0, " 13:06:47 12-04-08", 18);
		SetTextColor(hDC, tmp_misc);
		TextOut(hDC, 0, 0, " 4300073", 8);
		SetTextColor(hDC, tmp_modetypebit);
		TextOut(hDC, 0, 0, " HPDATA 8000", 12);
		SetTextColor(hDC, tmp_message);
		TextOut(hDC, 0, 0, "  GK!#0W", 8);
		SetTextColor(hDC, tmp_biterrors);
		TextOut(hDC, 0, 0, "Q4E", 3);
		SetTextColor(hDC, tmp_message);
		TextOut(hDC, 0, 0, "#VX·&1$H? Z", 11);

		// Next line
		MoveToEx(hDC, 0, tmp_cyChar*3, &lpPoint);
		SetTextColor(hDC, tmp_address);
		TextOut(hDC, 0, 0, " 4361264", 8);
		SetTextColor(hDC, tmp_timestamp);
		TextOut(hDC, 0, 0, " 13:06:48 12-04-08", 18);
		SetTextColor(hDC, tmp_misc);
		TextOut(hDC, 0, 0, " 4300073", 8);
		SetTextColor(hDC, tmp_modetypebit);
		TextOut(hDC, 0, 0, "  TEXT  8000", 12);
		SetTextColor(hDC, tmp_message);
		TextOut(hDC, 0, 0, "  8Y@B0W6i~<DE2 ", 16);
		SetTextColor(hDC, tmp_filtermatch);
		TextOut(hDC, 0, 0, "ERROR:", 6);
		SetTextColor(hDC, tmp_message);
		TextOut(hDC, 0, 0, "02", 2);

		// Next line         
		MoveToEx(hDC, 0, tmp_cyChar*4, &lpPoint);
		SetTextColor(hDC, tmp_address);
		TextOut(hDC, 0, 0, " 4360507", 8);
		SetTextColor(hDC, tmp_timestamp);
		TextOut(hDC, 0, 0, " 13:06:49 12-04-08", 18);
		SetTextColor(hDC, tmp_misc);
		TextOut(hDC, 0, 0, " 4300073", 8);
		SetTextColor(hDC, tmp_modetypebit);
		TextOut(hDC, 0, 0, "  DATA  8000", 12);
		SetTextColor(hDC, tmp_message);
		TextOut(hDC, 0, 0, "  N51565504", 11);
		SetTextColor(hDC, tmp_biterrors);
		TextOut(hDC, 0, 0, "3400", 4);

		// Next line         
		MoveToEx(hDC, tmp_cxChar*48, tmp_cyChar*5, &lpPoint);
		SetTextColor(hDC, tmp_filterlabel);
		TextOut(hDC, 0, 0, "- Filter Label -", 16);

		SelectObject(hDC, hOldFont);
		EndPaint (hWnd, &ps);

		break; 

      case WM_CLOSE: // fall through

		default:
			return (DefWindowProc(hWnd, uMsg, wParam, lParam));
	}
	return 0L;
} // end of MOBITEXColorWndProc


BOOL FAR PASCAL ERMESColorsDlgProc(HWND hDlg, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	static HWND hWndBox;
	static HWND hColorWnd;
	static RECT rect;
	static RECT client_rect;
	int			xPos, yPos;
	int			xSize, ySize;
	HDC			hDC;
	static CHOOSECOLOR cc;

	switch (uMsg)
	{
		case WM_INITDIALOG:
		if (!CenterWindow(hDlg)) return (FALSE);

		tmp_background	= Profile.color_background;
		tmp_address		= Profile.color_address;
		tmp_modetypebit	= Profile.color_modetypebit;
		tmp_timestamp	= Profile.color_timestamp;
		tmp_biterrors	= Profile.color_biterrors;
		tmp_numeric		= Profile.color_numeric;
		tmp_message		= Profile.color_message;
		tmp_misc		= Profile.color_misc;
		tmp_filtermatch	= Profile.color_filtermatch;
		tmp_filterlabel	= Profile.color_filterlabel[0];

		hWndBox = GetDlgItem(hDlg, IDC_EM_COLORWIN);
		GetClientRect (hWndBox, &client_rect);
		xSize = client_rect.right;
		ySize = client_rect.bottom;

		GetWindowRect (hDlg, &rect);
		GetWindowRect (hWndBox, &client_rect);
		xPos = client_rect.left - rect.left -
				 GetSystemMetrics(SM_CXDLGFRAME);

		yPos = client_rect.top - rect.top -
				 GetSystemMetrics(SM_CYMENU) -
				 GetSystemMetrics(SM_CYDLGFRAME);

		hColorWnd = CreateWindow(gszERMESColorClass, NULL, WS_CHILD | WS_VISIBLE, xPos, yPos,
			xSize, ySize, hDlg, NULL, (HINSTANCE) GetWindowLong(hDlg, GWL_HINSTANCE), 0);

		DeleteObject(hboxbr);

		hboxbr = CreateSolidBrush(tmp_background);
		SetClassLong(hColorWnd, GCL_HBRBACKGROUND, (LONG) hboxbr);

		InvalidateRect(hColorWnd, NULL, TRUE);

		memset(&cc, 0, sizeof(CHOOSECOLOR));

		cc.lStructSize		= sizeof(CHOOSECOLOR);
		cc.hwndOwner		= hDlg;
		cc.rgbResult		= 0;
		cc.lpCustColors		= custom_colors;
		cc.Flags			= CC_RGBINIT | CC_ENABLEHOOK;
		cc.lCustData		= 0;
		cc.lpfnHook			= CenterOpenDlgBox;
		cc.lpTemplateName	= NULL;

		return (TRUE);

		case WM_COMMAND:
		switch (LOWORD(wParam))
		{
			case IDC_EM_COLORBACKGND:
			lstrcpy(szCenterOpenDlgMsg, TEXT("ERMES Background"));
			cc.rgbResult = tmp_background;

			if(ChooseColor(&cc))
			{
				hDC = GetDC(hDlg);
				tmp_background = GetNearestColor(hDC, cc.rgbResult);
				ReleaseDC(hDlg, hDC);

				DeleteObject(hboxbr);

				hboxbr = CreateSolidBrush(tmp_background);

				SetClassLong(hColorWnd, GCL_HBRBACKGROUND, (LONG) hboxbr);

				InvalidateRect(hColorWnd, NULL, TRUE);
			}
			break;

			case IDC_EM_COLORCAPCODE:
			lstrcpy(szCenterOpenDlgMsg, TEXT("ERMES Address"));
			cc.rgbResult = tmp_address;

			if(ChooseColor(&cc))
			{
				hDC = GetDC(hDlg);
				tmp_address = GetNearestColor(hDC, cc.rgbResult);
				ReleaseDC(hDlg, hDC);
				InvalidateRect(hColorWnd, NULL, TRUE);
			}
			break;

			case IDC_EM_COLORFUNCTION:
			lstrcpy(szCenterOpenDlgMsg, TEXT("ERMES Function"));
			cc.rgbResult = tmp_modetypebit;

			if(ChooseColor(&cc))
			{
				hDC = GetDC(hDlg);
				tmp_modetypebit = GetNearestColor(hDC, cc.rgbResult);
				ReleaseDC(hDlg, hDC);
				InvalidateRect(hColorWnd, NULL, TRUE);
			}
			break;

			case IDC_EM_COLORTIMESTAMP:
			lstrcpy(szCenterOpenDlgMsg, TEXT("ERMES Time/Date"));
			cc.rgbResult = tmp_timestamp;

			if(ChooseColor(&cc))
			{
				hDC = GetDC(hDlg);
				tmp_timestamp = GetNearestColor(hDC, cc.rgbResult);
				ReleaseDC(hDlg, hDC);
				InvalidateRect(hColorWnd, NULL, TRUE);
			}
			break;

			case IDC_EM_COLORBITERRORS:
			lstrcpy(szCenterOpenDlgMsg, TEXT("ERMES Bit Errors"));
			cc.rgbResult = tmp_biterrors;

			if(ChooseColor(&cc))
			{
				hDC = GetDC(hDlg);
				tmp_biterrors = GetNearestColor(hDC, cc.rgbResult);
				ReleaseDC(hDlg, hDC);
				InvalidateRect(hColorWnd, NULL, TRUE);
			}
			break;

			case IDC_EM_COLORNUMERIC:
			lstrcpy(szCenterOpenDlgMsg, TEXT("ERMES Numeric/Tone"));
			cc.rgbResult = tmp_numeric;

			if(ChooseColor(&cc))
			{
				hDC = GetDC(hDlg);
				tmp_numeric = GetNearestColor(hDC, cc.rgbResult);
				ReleaseDC(hDlg, hDC);
				InvalidateRect(hColorWnd, NULL, TRUE);
			}
			break;

			case IDC_EM_COLORALPHANUM:
			lstrcpy(szCenterOpenDlgMsg, TEXT("ERMES Message"));
			cc.rgbResult = tmp_message;

			if(ChooseColor(&cc))
			{
				hDC = GetDC(hDlg);
				tmp_message = GetNearestColor(hDC, cc.rgbResult);
				ReleaseDC(hDlg, hDC);
				InvalidateRect(hColorWnd, NULL, TRUE);
			}
			break;

			case IDC_EM_COLORTRANSPARENT:
			lstrcpy(szCenterOpenDlgMsg, TEXT("ERMES Transparent"));
			cc.rgbResult = tmp_misc;

			if(ChooseColor(&cc))
			{
				hDC = GetDC(hDlg);
				tmp_misc = GetNearestColor(hDC, cc.rgbResult);
				ReleaseDC(hDlg, hDC);
				InvalidateRect(hColorWnd, NULL, TRUE);
			}
			break;

			case IDC_EM_COLORFILTMATCH:
			lstrcpy(szCenterOpenDlgMsg, TEXT("ERMES Filter Match"));
			cc.rgbResult = tmp_filtermatch;

			if(ChooseColor(&cc))
			{
				hDC = GetDC(hDlg);
				tmp_filtermatch = GetNearestColor(hDC, cc.rgbResult);
				ReleaseDC(hDlg, hDC);
				InvalidateRect(hColorWnd, NULL, TRUE);
			}
			break;

			case IDC_EM_COLORFILTERLABEL:
			lstrcpy(szCenterOpenDlgMsg, TEXT("ERMES Filter Label"));
			cc.rgbResult = tmp_filterlabel;

			if(ChooseColor(&cc))
			{
				hDC = GetDC(hDlg);
				tmp_filterlabel = GetNearestColor(hDC, cc.rgbResult);
				ReleaseDC(hDlg, hDC);
				InvalidateRect(hColorWnd, NULL, TRUE);
			}
			break;

			case IDC_EM_COLORDEFAULT:

			if (MessageBox(hDlg, "Reset all colors to Default?", "ERMES Reset colors", MB_ICONQUESTION | MB_OKCANCEL) == IDOK)
			{
				tmp_background	= RGB(rgbColor[BLACK][0],
										rgbColor[BLACK][1],
										rgbColor[BLACK][2]);
				tmp_address		= RGB(rgbColor[WHITE][0],
										rgbColor[WHITE][1],
										rgbColor[WHITE][2]);
				tmp_modetypebit	= RGB(rgbColor[RED][0],
										rgbColor[RED][1],
										rgbColor[RED][2]);
				tmp_timestamp	= RGB(rgbColor[LTBLUE][0],
										rgbColor[LTBLUE][1],
										rgbColor[LTBLUE][2]);
				tmp_numeric		= RGB(rgbColor[RED][0],
										rgbColor[RED][1],
										rgbColor[RED][2]);
				tmp_message		= RGB(rgbColor[LTCYAN][0],
										rgbColor[LTCYAN][1],
										rgbColor[LTCYAN][2]);
				tmp_misc		= RGB(rgbColor[BROWN][0],
										rgbColor[BROWN][1],
										rgbColor[BROWN][2]);
				tmp_biterrors	= RGB(rgbColor[LTGRAY][0],
										rgbColor[LTGRAY][1],
										rgbColor[LTGRAY][2]);
				tmp_filtermatch	= RGB(rgbColor[LTGREEN][0],
										rgbColor[LTGREEN][1],
										rgbColor[LTGREEN][2]);
				tmp_filterlabel	= RGB(rgbColor[LTBLUE][0],
										rgbColor[LTBLUE][1],
										rgbColor[LTBLUE][2]);

				DeleteObject(hboxbr);

				hboxbr = CreateSolidBrush(tmp_background);

				SetClassLong(hColorWnd, GCL_HBRBACKGROUND, (LONG) hboxbr);

				InvalidateRect(hColorWnd, NULL, TRUE);
			}
			break;

			case IDOK:

			Profile.color_address		= tmp_address;
			Profile.color_modetypebit	= tmp_modetypebit;
			Profile.color_timestamp		= tmp_timestamp;
			Profile.color_biterrors		= tmp_biterrors;
			Profile.color_numeric		= tmp_numeric;
			Profile.color_message		= tmp_message;
			Profile.color_misc			= tmp_misc;
			Profile.color_filtermatch	= tmp_filtermatch;
			Profile.color_filterlabel[0]= tmp_filterlabel;

			if (Profile.color_background != tmp_background)
			{
				Profile.color_background  = tmp_background;

				DeleteObject(hbr);

				hbr = CreateSolidBrush(Profile.color_background);

				SetClassLong(Pane1.hWnd, GCL_HBRBACKGROUND, (LONG) hbr);
				SetClassLong(Pane2.hWnd, GCL_HBRBACKGROUND, (LONG) hbr);
			}

			InvalidateRect(Pane1.hWnd, NULL, TRUE);
			InvalidateRect(Pane2.hWnd, NULL, TRUE);

			WriteSettings();

			EndDialog(hDlg, TRUE);
			return (TRUE);

			case IDCANCEL:
			EndDialog(hDlg, TRUE);
			return (TRUE);
		}
		break;
	}
	return (FALSE);
} // end of ERMESColorsDlgProc


LRESULT FAR PASCAL ERMESColorWndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	HDC          hDC;
	HFONT        hOldFont;
	TEXTMETRIC   tm;
	PAINTSTRUCT  ps;
	POINT        lpPoint;
	int          tmp_cyChar;

	switch (uMsg)
	{
		case WM_CREATE:
		break;

		case WM_PAINT:
		hDC = BeginPaint (hWnd, &ps);
		hOldFont = (HFONT)SelectObject(hDC, hboxfont);

		GetTextMetrics(hDC, &tm);

		tmp_cyChar = tm.tmHeight + tm.tmExternalLeading;

		SetBkMode(hDC, OPAQUE);
		SetBkColor(hDC, tmp_background);

		// let Windows handle aligning the characters as TextOut goes along
		SetTextAlign(hDC, TA_UPDATECP);

		// TONE-ONLY
		MoveToEx(hDC, 0, 0, &lpPoint);
		SetTextColor(hDC, tmp_address);
		TextOut(hDC, 0, 0, " 1234567", 8);
		SetTextColor(hDC, tmp_timestamp);
		TextOut(hDC, 0, 0, "  15:16:36 19-06-01", 19);
		SetTextColor(hDC, tmp_modetypebit);
		TextOut(hDC, 0, 0, "  ERMES-1   6250", 16);
		SetTextColor(hDC, tmp_numeric);
		TextOut(hDC, 0, 0, "  TONE-ONLY", 11);

		// ALPHA
		MoveToEx(hDC, 0, tmp_cyChar, &lpPoint);
		SetTextColor(hDC, tmp_address);
		TextOut(hDC, 0, 0, " 7654321", 8);
		SetTextColor(hDC, tmp_timestamp);
		TextOut(hDC, 0, 0, "  16:12:42 20-06-01", 19);
		SetTextColor(hDC, tmp_modetypebit);
		TextOut(hDC, 0, 0, "  ERMES-2   6250", 16);
		SetTextColor(hDC, tmp_message);
		TextOut(hDC, 0, 0, "  Please call ASAP", 18);

		// Alphanumeric/Bit Errors
		MoveToEx(hDC, 0, tmp_cyChar*2, &lpPoint);
		SetTextColor(hDC, tmp_address);
		TextOut(hDC, 0, 0, " 1234567", 8);
		SetTextColor(hDC, tmp_timestamp);
		TextOut(hDC, 0, 0, "  19:17:21 21-06-01", 19);
		SetTextColor(hDC, tmp_modetypebit);
		TextOut(hDC, 0, 0, "  ERMES-2   6250", 16);
		SetTextColor(hDC, tmp_message);
		TextOut(hDC, 0, 0, "  Pl", 4);
		SetTextColor(hDC, tmp_biterrors);
		TextOut(hDC, 0, 0, "#'[;", 4);
		SetTextColor(hDC, tmp_message);
		TextOut(hDC, 0, 0, " call ASAP", 10);

		// Alphanumeric/Filter Match
		MoveToEx(hDC, 0, tmp_cyChar*3, &lpPoint);
		SetTextColor(hDC, tmp_address);
		TextOut(hDC, 0, 0, " 7654321", 8);
		SetTextColor(hDC, tmp_timestamp);
		TextOut(hDC, 0, 0, "  20:14:17 22-06-01", 19);
		SetTextColor(hDC, tmp_modetypebit);
		TextOut(hDC, 0, 0, "  ERMES-3   6250", 16);
		SetTextColor(hDC, tmp_message);
		TextOut(hDC, 0, 0, "  Please", 8);
		SetTextColor(hDC, tmp_filtermatch);
		TextOut(hDC, 0, 0, " call", 5);
		SetTextColor(hDC, tmp_message);
		TextOut(hDC, 0, 0, " ASAP", 5);

		// Numeric
		MoveToEx(hDC, 0, tmp_cyChar*4, &lpPoint);
		SetTextColor(hDC, tmp_address);
		TextOut(hDC, 0, 0, " 1234567", 8);
		SetTextColor(hDC, tmp_timestamp);
		TextOut(hDC, 0, 0, "  21:24:55 23-06-01", 19);
		SetTextColor(hDC, tmp_modetypebit);
		TextOut(hDC, 0, 0, "  ERMES-1   6250", 16);
		SetTextColor(hDC, tmp_numeric);
		TextOut(hDC, 0, 0, "  0845-654321", 13);

		// Alphanumeric/Label
		MoveToEx(hDC, 0, tmp_cyChar*5, &lpPoint);
		SetTextColor(hDC, tmp_address);
		TextOut(hDC, 0, 0, " 7654321", 8);
		SetTextColor(hDC, tmp_timestamp);
		TextOut(hDC, 0, 0, "  22:20:14 24-06-01", 19);
		SetTextColor(hDC, tmp_modetypebit);
		TextOut(hDC, 0, 0, "  ERMES-3   6250", 16);
		SetTextColor(hDC, tmp_filtermatch);
		TextOut(hDC, 0, 0, "  Phone", 7);
		SetTextColor(hDC, tmp_message);
		TextOut(hDC, 0, 0, " ASAP", 5);
		SetTextColor(hDC, tmp_filterlabel);
		TextOut(hDC, 0, 0, " <PH>", 5);

		// Transparent
		MoveToEx(hDC, 0, tmp_cyChar*6, &lpPoint);
		SetTextColor(hDC, tmp_address);
		TextOut(hDC, 0, 0, " 1234567", 8);
		SetTextColor(hDC, tmp_timestamp);
		TextOut(hDC, 0, 0, "  23:21:10 25-06-01", 19);
		SetTextColor(hDC, tmp_modetypebit);
		TextOut(hDC, 0, 0, "  ERMES-4   6250", 16);
		SetTextColor(hDC, tmp_misc);
		TextOut(hDC, 0, 0, "  1F34E67E9A24", 14);

		SelectObject(hDC, hOldFont);
		EndPaint (hWnd, &ps);

		break; 

		case WM_CLOSE: // fall through

		default:
		return (DefWindowProc(hWnd, uMsg, wParam, lParam));
	}
	return 0L;
} // end of ERMESColorWndProc


BOOL FAR PASCAL OptionsDlgProc(HWND hDlg, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	int state, len, fnu, both;
	char tbuf[128];

	extern bool bFlexTIME_detected;

	switch (uMsg)
	{
		case WM_INITDIALOG:
		if (!CenterWindow(hDlg)) return (FALSE);

		// Pocsag :
		CheckDlgButton(hDlg, IDC_DECODEPOCSAG, Profile.decodepocsag);
		CheckDlgButton(hDlg, IDC_POCSAG_512,   Profile.pocsag_512);
		CheckDlgButton(hDlg, IDC_POCSAG_1200,  Profile.pocsag_1200);
		CheckDlgButton(hDlg, IDC_POCSAG_2400,  Profile.pocsag_2400);
		CheckDlgButton(hDlg, IDC_POCSAG_FNU,   Profile.pocsag_fnu);
		CheckDlgButton(hDlg, IDC_POCSAG_BOTH,  Profile.pocsag_showboth);

		// Flex :
		CheckDlgButton(hDlg, IDC_DECODEFLEX, Profile.decodeflex);
		CheckDlgButton(hDlg, IDC_FLEX_1600,  Profile.flex_1600);
		CheckDlgButton(hDlg, IDC_FLEX_3200,  Profile.flex_3200);
		CheckDlgButton(hDlg, IDC_FLEX_6400,  Profile.flex_6400);
		CheckDlgButton(hDlg, IDC_SHOWINSTR,	 Profile.showinstr);
		CheckDlgButton(hDlg, IDC_CONVERT_SI, Profile.convert_si);
		CheckDlgButton(hDlg, IDC_FLEXTIME,   Profile.FlexTIME);

		// Mobitex :
		SendDlgItemMessage(hDlg, IDC_MB_FRSYNC,  EM_LIMITTEXT, 4, 0L);
		SendDlgItemMessage(hDlg, IDC_MB_MIN_MSG, EM_LIMITTEXT, 2, 0L);

		// Display Mobitex frame sync
		sprintf(tbuf,"%04X",mb.frsync);
		SetDlgItemText(hDlg, IDC_MB_FRSYNC, (LPCSTR) tbuf);

		if(!mb.cfs)	EnableWindow(GetDlgItem(hDlg, IDC_MB_FRSYNC),FALSE);
		else		CheckDlgButton(hDlg, IDC_MB_USEFRSYNC, BST_CHECKED);
    
		// Show correct Mobitex bit sync (CCCC or 3333)
		SendDlgItemMessage(hDlg, IDC_MB_BITSYNC, CB_ADDSTRING, 0, (LPARAM) (LPSTR) "Base");
		SendDlgItemMessage(hDlg, IDC_MB_BITSYNC, CB_ADDSTRING, 0, (LPARAM) (LPSTR) "Mobile");
		SendDlgItemMessage(hDlg, IDC_MB_BITSYNC, CB_SETCURSEL, (mb.bitsync == 0xCCCC) ? 0 : 1, 0L);

		// Display Min characters.
		sprintf(tbuf, "%d", mb.min_msg_len);
		SetDlgItemText(hDlg, IDC_MB_MIN_MSG, (LPCSTR) tbuf);

		// Display Baudrate.
		SendDlgItemMessage(hDlg, IDC_MB_BITRATE, CB_ADDSTRING, 0, (LPARAM) (LPSTR) "1200");
		SendDlgItemMessage(hDlg, IDC_MB_BITRATE, CB_ADDSTRING, 0, (LPARAM) (LPSTR) "8000");
		SendDlgItemMessage(hDlg, IDC_MB_BITRATE, CB_SETCURSEL, 1, 0L);

		CheckDlgButton(hDlg, IDC_MB_BITSCRAMBLER,mb.bitscr);
		CheckDlgButton(hDlg, IDC_MB_RAMNET,      mb.ramnet);
		CheckDlgButton(hDlg, IDC_MB_SHOWMPAK,   (mb.show & MOBITEX_SHOW_MPAK));
		CheckDlgButton(hDlg, IDC_MB_SHOWTEXT,   (mb.show & MOBITEX_SHOW_TEXT));
		CheckDlgButton(hDlg, IDC_MB_SHOWHPDATA, (mb.show & MOBITEX_SHOW_HPDATA));
		CheckDlgButton(hDlg, IDC_MB_SHOWDATA,   (mb.show & MOBITEX_SHOW_DATA));
		CheckDlgButton(hDlg, IDC_MB_SHOWHPID,   (mb.show & MOBITEX_SHOW_HPID));
		CheckDlgButton(hDlg, IDC_MB_SHOWSWEEP,  (mb.show & MOBITEX_SHOW_SWEEP));
		CheckDlgButton(hDlg, IDC_MB_VERBOSE,    (mb.show & MOBITEX_VERBOSE));

		// ACARS :
		CheckDlgButton(hDlg, Profile.acars_parity_check ? IDC_ACARS_PC_YES : IDC_ACARS_PC_NO, BST_CHECKED);

		// Optional information in titlebar :
		CheckDlgButton(hDlg, IDC_SHOW_CFS, Profile.show_cfs);
		CheckDlgButton(hDlg, IDC_SHOW_REJECTBLOCKED, Profile.show_rejectblocked);

		strcpy(tbuf, "PDW Options - Currently monitoring : ");
		
		if (Profile.monitor_paging)
		{
			EnableWindow(GetDlgItem(hDlg, IDC_DECODEPOCSAG), true);

			if (Profile.decodepocsag)
			{
				EnableWindow(GetDlgItem(hDlg, IDC_POCSAG_512), true);
				EnableWindow(GetDlgItem(hDlg, IDC_POCSAG_1200),true);
				EnableWindow(GetDlgItem(hDlg, IDC_POCSAG_2400),true);
				EnableWindow(GetDlgItem(hDlg, IDC_POCSAG_FNU), !Profile.pocsag_showboth);
				EnableWindow(GetDlgItem(hDlg, IDC_POCSAG_BOTH),!Profile.pocsag_fnu);
			}

			EnableWindow(GetDlgItem(hDlg, IDC_DECODEFLEX), true);

			if (Profile.decodeflex)
			{
				EnableWindow(GetDlgItem(hDlg, IDC_FLEX_1600),true);
				EnableWindow(GetDlgItem(hDlg, IDC_FLEX_3200),true);
				EnableWindow(GetDlgItem(hDlg, IDC_FLEX_6400),true);
				EnableWindow(GetDlgItem(hDlg, IDC_SHOWINSTR),true);
				EnableWindow(GetDlgItem(hDlg, IDC_SHOW_CFS), true);

				if (Profile.showinstr)
				{
					EnableWindow(GetDlgItem(hDlg, IDC_CONVERT_SI),true);
				}

				if (bFlexTIME_detected)
				{
					EnableWindow(GetDlgItem(hDlg, IDC_FLEXTIME), true);
					SetDlgItemText(hDlg, IDC_FLEXTIME, "Use FlexTIME as Systemtime");
				}
			}
			strcat(tbuf, "Paging (Pocsag/Flex)");
		}
		else if (Profile.monitor_mobitex)
		{
			EnableWindow(GetDlgItem(hDlg, IDC_MB_USEFRSYNC), true);
			EnableWindow(GetDlgItem(hDlg, IDC_MB_BITSYNC),   true);
			EnableWindow(GetDlgItem(hDlg, IDC_MB_MIN_MSG),   true);
			EnableWindow(GetDlgItem(hDlg, IDC_MB_BITSCRAMBLER), true);
			EnableWindow(GetDlgItem(hDlg, IDC_MB_RAMNET),    true);
			EnableWindow(GetDlgItem(hDlg, IDC_MB_SHOWMPAK),  true);
			EnableWindow(GetDlgItem(hDlg, IDC_MB_SHOWTEXT),  true);
			EnableWindow(GetDlgItem(hDlg, IDC_MB_SHOWDATA),  true);
			EnableWindow(GetDlgItem(hDlg, IDC_MB_SHOWHPDATA),true);
			EnableWindow(GetDlgItem(hDlg, IDC_MB_SHOWSWEEP), true);
			EnableWindow(GetDlgItem(hDlg, IDC_MB_VERBOSE),   true);

			if (IsDlgButtonChecked(hDlg, IDC_MB_SHOWHPDATA))
			{
				EnableWindow(GetDlgItem(hDlg, IDC_MB_SHOWHPID),  true);
			}

			if (mb.cfs)
			{
				EnableWindow(GetDlgItem(hDlg, IDC_MB_FRSYNC),true);
			}

			strcat(tbuf, "Mobitex");
		}
		else if (Profile.monitor_acars)
		{
			EnableWindow(GetDlgItem(hDlg, IDC_ACARS_PC_YES), true);
			EnableWindow(GetDlgItem(hDlg, IDC_ACARS_PC_NO),  true);

			strcat(tbuf, "ACARS");
		}
		else if (Profile.monitor_ermes)
		{
			EnableWindow(GetDlgItem(hDlg, IDC_SHOW_CFS), true);

			strcat(tbuf, "ERMES");
		}
		SetWindowText(hDlg, (LPSTR) tbuf);	// Set new title

		return (TRUE);

		case WM_COMMAND:

		switch (LOWORD(wParam))
		{
			case IDC_MB_SHOWHPDATA:

			state =  IsDlgButtonChecked(hDlg, IDC_MB_SHOWHPDATA);

			EnableWindow(GetDlgItem(hDlg, IDC_MB_SHOWHPID), state);

			break;

			case IDC_MB_USEFRSYNC:

			EnableWindow(GetDlgItem(hDlg, IDC_MB_FRSYNC), IsDlgButtonChecked(hDlg, IDC_MB_USEFRSYNC));

			break;

			case IDC_DECODEPOCSAG:
			case IDC_POCSAG_BOTH:
			case IDC_POCSAG_FNU:

			state = IsDlgButtonChecked(hDlg, IDC_DECODEPOCSAG);
			both  = IsDlgButtonChecked(hDlg, IDC_POCSAG_BOTH);
			fnu   = IsDlgButtonChecked(hDlg, IDC_POCSAG_FNU);

			EnableWindow(GetDlgItem(hDlg, IDC_POCSAG_512),  state);
			EnableWindow(GetDlgItem(hDlg, IDC_POCSAG_1200), state);
			EnableWindow(GetDlgItem(hDlg, IDC_POCSAG_2400), state);
			EnableWindow(GetDlgItem(hDlg, IDC_POCSAG_FNU), (state && !both));
			EnableWindow(GetDlgItem(hDlg, IDC_POCSAG_BOTH),(state && !fnu));

			break;

			case IDC_DECODEFLEX:

			state = IsDlgButtonChecked(hDlg, IDC_DECODEFLEX);

			EnableWindow(GetDlgItem(hDlg, IDC_FLEX_1600), state);
			EnableWindow(GetDlgItem(hDlg, IDC_FLEX_3200), state);
			EnableWindow(GetDlgItem(hDlg, IDC_FLEX_6400), state);
			EnableWindow(GetDlgItem(hDlg, IDC_SHOWINSTR), state);
			EnableWindow(GetDlgItem(hDlg, IDC_FLEXTIME),  state);

			if (IsDlgButtonChecked(hDlg, IDC_SHOWINSTR))
			{
				 EnableWindow(GetDlgItem(hDlg, IDC_CONVERT_SI), state);
			}
			else EnableWindow(GetDlgItem(hDlg, IDC_CONVERT_SI), false);

			break;

			case IDC_SHOWINSTR:

			state = IsDlgButtonChecked(hDlg, IDC_SHOWINSTR);

			EnableWindow(GetDlgItem(hDlg, IDC_CONVERT_SI), state);

			if (!state) CheckDlgButton(hDlg, IDC_CONVERT_SI, BST_UNCHECKED);

			break;
			
			case IDC_ACARS_PC_YES:
			case IDC_ACARS_PC_NO:
			break;

			case IDC_GENERALOPTIONS:

			GoModalDialogBoxParam(ghInstance, MAKEINTRESOURCE(GENERALOPTIONSDLGBOX),
										 hDlg, (DLGPROC) GeneralOptionsDlgProc, 0L);
			break;

			case IDOK:

			Profile.decodepocsag       = IsDlgButtonChecked(hDlg, IDC_DECODEPOCSAG);
			Profile.pocsag_512         = IsDlgButtonChecked(hDlg, IDC_POCSAG_512);
			Profile.pocsag_1200        = IsDlgButtonChecked(hDlg, IDC_POCSAG_1200);
			Profile.pocsag_2400        = IsDlgButtonChecked(hDlg, IDC_POCSAG_2400);
			Profile.pocsag_showboth    = IsDlgButtonChecked(hDlg, IDC_POCSAG_BOTH);
			Profile.pocsag_fnu         = IsDlgButtonChecked(hDlg, IDC_POCSAG_FNU);
			Profile.decodeflex         = IsDlgButtonChecked(hDlg, IDC_DECODEFLEX);
			Profile.flex_1600          = IsDlgButtonChecked(hDlg, IDC_FLEX_1600);
			Profile.flex_3200          = IsDlgButtonChecked(hDlg, IDC_FLEX_3200);
			Profile.flex_6400          = IsDlgButtonChecked(hDlg, IDC_FLEX_6400);
			Profile.showinstr          = IsDlgButtonChecked(hDlg, IDC_SHOWINSTR);
			Profile.convert_si         = IsDlgButtonChecked(hDlg, IDC_CONVERT_SI);
			Profile.FlexTIME           = IsDlgButtonChecked(hDlg, IDC_FLEXTIME);
			Profile.show_cfs           = IsDlgButtonChecked(hDlg, IDC_SHOW_CFS);
			Profile.show_rejectblocked = IsDlgButtonChecked(hDlg, IDC_SHOW_REJECTBLOCKED);
			Profile.acars_parity_check = IsDlgButtonChecked(hDlg, IDC_ACARS_PC_YES);

			if (!Profile.decodeflex || !Profile.decodepocsag)
			{
				if (!Profile.monitor_acars)
				{
					pd_reset_all();         // see decode.cpp.
					Reset_ATB();            // see sound_in.cpp.
				}
			}

			// Mobitex options from here
			mb.cfs    = IsDlgButtonChecked(hDlg, IDC_MB_USEFRSYNC);
			mb.bitscr = IsDlgButtonChecked(hDlg, IDC_MB_BITSCRAMBLER);
			mb.ramnet = IsDlgButtonChecked(hDlg, IDC_MB_RAMNET);

			mb.show  = IsDlgButtonChecked(hDlg, IDC_MB_SHOWMPAK)   ? MOBITEX_SHOW_MPAK   : 0;
			mb.show += IsDlgButtonChecked(hDlg, IDC_MB_SHOWTEXT)   ? MOBITEX_SHOW_TEXT   : 0;
			mb.show += IsDlgButtonChecked(hDlg, IDC_MB_SHOWHPDATA) ? MOBITEX_SHOW_HPDATA : 0;
			mb.show += IsDlgButtonChecked(hDlg, IDC_MB_SHOWDATA)   ? MOBITEX_SHOW_DATA   : 0;
			mb.show += IsDlgButtonChecked(hDlg, IDC_MB_SHOWHPID)   ? MOBITEX_SHOW_HPID   : 0;
			mb.show += IsDlgButtonChecked(hDlg, IDC_MB_SHOWSWEEP)  ? MOBITEX_SHOW_SWEEP  : 0;
			mb.show += IsDlgButtonChecked(hDlg, IDC_MB_VERBOSE)    ? MOBITEX_VERBOSE     : 0;

			state = SendDlgItemMessage(hDlg, IDC_MB_BITSYNC, CB_GETCURSEL, 0, 0L);

			if (state == CB_ERR)
			{
				mb.bitsync = 0xCCCC;
			}
			else
			{
				if (state > 0) mb.bitsync = 0x3333;                         
				else		   mb.bitsync = 0xCCCC;
			}
			mb.bitsync_rev = ~mb.bitsync;
 			
			len = GetDlgItemText(hDlg, IDC_MB_FRSYNC,(LPSTR) tbuf, 5);
			if (len > 1)
			{
				sscanf(tbuf, "%04X", &mb.frsync);
				mb.frsync_rev = ~mb.frsync;
			}
                  
			len = GetDlgItemText(hDlg, IDC_MB_MIN_MSG, (LPSTR) tbuf, 3);
			if (len) sscanf(tbuf, "%d", &mb.min_msg_len);

			WriteSettings();

			EndDialog(hDlg, TRUE);
			return (TRUE);

			case IDCANCEL:
			EndDialog(hDlg, TRUE);
			return (TRUE);
		}
		break;
   }
   return (FALSE);
} // end of OptionsDlgProc


BOOL FAR PASCAL GeneralOptionsDlgProc(HWND hDlg, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	char tmp_path[MAX_PATH];
	int test=0;

	LPITEMIDLIST pidl;
	BROWSEINFO bi = {0};

	switch (uMsg)
	{
		case WM_INITDIALOG:
		if (!CenterWindow(hDlg)) return (FALSE);

		// General :
		CheckDlgButton(hDlg, IDC_CONFIRMEXIT,    Profile.confirmExit);
		CheckDlgButton(hDlg, IDC_BLOCKDUPLICATE, Profile.BlockDuplicate);
		CheckDlgButton(hDlg, IDC_BLOCKEDTXT,    (Profile.BlockDuplicate & 0x04));
		SetDlgItemInt(hDlg,	 IDC_BLOCKDUPTIMER,  Profile.BlockDuplicate >> 4, false);
		SendDlgItemMessage(hDlg, IDC_BLOCKDUPTIMER, EM_LIMITTEXT, 1, 0L);

		EnableWindow(GetDlgItem(hDlg, IDC_BLOCKDUPOPTION), Profile.BlockDuplicate & 0x03);
		EnableWindow(GetDlgItem(hDlg, IDC_BLOCKEDTXT)    , Profile.BlockDuplicate & 0x03);
		EnableWindow(GetDlgItem(hDlg, IDC_BLOCKDUPTIMER), (Profile.BlockDuplicate & 0x03) == 3);

		SendDlgItemMessage(hDlg, IDC_BLOCKDUPOPTION, CB_ADDSTRING, 0, (LPARAM) (LPSTR) "Address+message is the same as previous");
		SendDlgItemMessage(hDlg, IDC_BLOCKDUPOPTION, CB_ADDSTRING, 0, (LPARAM) (LPSTR) "Message is the same as previous");
		SendDlgItemMessage(hDlg, IDC_BLOCKDUPOPTION, CB_ADDSTRING, 0, (LPARAM) (LPSTR) "Address(+function)+message is repeated within timer");
		SendDlgItemMessage(hDlg, IDC_BLOCKDUPOPTION, CB_SETCURSEL, (Profile.BlockDuplicate & 0x03)-1, 0L);

		CheckDlgButton(hDlg, IDC_LINEFEEDS, Profile.Linefeed);

		if (Profile.FlexGroupMode)
		{
			EnableWindow(GetDlgItem(hDlg, IDC_SEP_SCREEN), false);

			if (Profile.FlexGroupMode & FLEXGROUPMODE_LOGGING)
			{
				EnableWindow(GetDlgItem(hDlg, IDC_SEP_LOGFILE),    false);
				EnableWindow(GetDlgItem(hDlg, IDC_SEP_FILTERFILE), false);
				EnableWindow(GetDlgItem(hDlg, IDC_SEP_SEPFILES),   false);
			}
		}

		CheckDlgButton(hDlg, Profile.MonthNumber ? IDC_MONTHNUMBER : IDC_MONTHNUMBER2, BST_CHECKED);
		CheckDlgButton(hDlg, IDC_SEPARATOR, Profile.Separator);

		SendDlgItemMessage(hDlg, IDC_DATEFORMAT, CB_ADDSTRING, 0, (LPARAM) (LPSTR) "DD-MM-YY");
		SendDlgItemMessage(hDlg, IDC_DATEFORMAT, CB_ADDSTRING, 0, (LPARAM) (LPSTR) "MM-DD-YY");
		SendDlgItemMessage(hDlg, IDC_DATEFORMAT, CB_ADDSTRING, 0, (LPARAM) (LPSTR) "YY-MM-DD");
		SendDlgItemMessage(hDlg, IDC_DATEFORMAT, CB_SETCURSEL, Profile.DateFormat, 0L);

		// PH: Display Logfile Path
		SetDlgItemText(hDlg, IDC_LOGFILEPATH, (LPCSTR) Profile.LogfilePath);

		if (stricmp(Profile.LogfilePath, szLogPathName) == 0) // PH: If default
		{
			SetDlgItemText(hDlg, IDC_LOGFILEPATHTEXT, "Path for logfiles (default)");
		}
		return (TRUE);

		case WM_COMMAND:

		switch (LOWORD(wParam))
		{
			case IDC_LOGFILEPATHBROWSE:

			bi.lpszTitle = "Select a directory...";
			bi.hwndOwner = hDlg;
			pidl = SHBrowseForFolder(&bi);

			if (pidl != 0)
			{
				if (SHGetPathFromIDList(pidl, tmp_path))
				{
					if (tmp_path[strlen(tmp_path)-1] != '\\') strcat(tmp_path, "\\");
					SetDlgItemText(hDlg, IDC_LOGFILEPATH, tmp_path);
				}

				IMalloc * imalloc = NULL;

				if (SUCCEEDED(SHGetMalloc(&imalloc)))
				{
					imalloc->Free(pidl);
					imalloc->Release();
				}
			}

			break;

			case IDC_BLOCKDUPLICATE:

			EnableWindow(GetDlgItem(hDlg, IDC_BLOCKDUPOPTION), IsDlgButtonChecked(hDlg, IDC_BLOCKDUPLICATE));
			EnableWindow(GetDlgItem(hDlg, IDC_BLOCKEDTXT)    , IsDlgButtonChecked(hDlg, IDC_BLOCKDUPLICATE));
			EnableWindow(GetDlgItem(hDlg, IDC_BLOCKDUPTIMER), (IsDlgButtonChecked(hDlg, IDC_BLOCKDUPLICATE) && SendDlgItemMessage(hDlg, IDC_BLOCKDUPOPTION, CB_GETCURSEL, 0, 0L) == 2));
			if (SendDlgItemMessage(hDlg, IDC_BLOCKDUPOPTION, CB_GETCURSEL, 0, 0L) < 0)
			{
				SendDlgItemMessage(hDlg, IDC_BLOCKDUPOPTION, CB_SETCURSEL, 0, 0L);
			}
			break;

			case IDC_BLOCKDUPOPTION:
			
			EnableWindow(GetDlgItem(hDlg, IDC_BLOCKDUPTIMER), SendDlgItemMessage(hDlg, IDC_BLOCKDUPOPTION, CB_GETCURSEL, 0, 0L) == 2);

			break;

			case IDC_LOGFILEPATH:

			GetDlgItemText(hDlg, IDC_LOGFILEPATH, tmp_path, sizeof(tmp_path));
			SetDlgItemText(hDlg, IDC_LOGFILEPATHTEXT, (stricmp(tmp_path, szLogPathName) == 0) ? "Path for logfiles (default)" : "Path for logfiles");
			EnableWindow(GetDlgItem(hDlg, IDC_LOGFILEPATHDEFAULT), stricmp(tmp_path, szLogPathName));
				
			break;

			case IDC_LOGFILEPATHDEFAULT:

			SetDlgItemText(hDlg, IDC_LOGFILEPATH, szLogPathName);
				
			break;

			case IDOK:

			if (Profile.FlexGroupMode)
			{
				if (SendDlgItemMessage(hDlg, IDC_BLOCKDUPOPTION, CB_GETCURSEL, 0, 0L) == 1)
				{
					MessageBox(hDlg, "When using Flex Group Mode, it's not possible\nto block messages only on the previous text", "PDW Block Duplicate Messages", MB_ICONERROR);
					SetFocus(GetDlgItem(hDlg, IDC_BLOCKDUPOPTION));
					return (FALSE);
				}
			}
			Profile.confirmExit		= IsDlgButtonChecked(hDlg, IDC_CONFIRMEXIT);
			Profile.MonthNumber		= IsDlgButtonChecked(hDlg, IDC_MONTHNUMBER);
			Profile.BlockDuplicate	= IsDlgButtonChecked(hDlg, IDC_BLOCKDUPLICATE);
			if (Profile.BlockDuplicate)
			{
				if (SendDlgItemMessage(hDlg, IDC_BLOCKDUPOPTION, CB_GETCURSEL, 0, 0L) == 2)
				{
					if ((GetDlgItemInt(hDlg, IDC_BLOCKDUPTIMER, NULL, FALSE) << 4) == 0)
					{
						MessageBox(hDlg, "0 is not a valid value!", "PDW Block Duplicate Timer", MB_ICONERROR);
						SendDlgItemMessage(hDlg,  IDC_BLOCKDUPTIMER, EM_SETSEL, 0, -1);
						SetFocus(GetDlgItem(hDlg, IDC_BLOCKDUPTIMER));
						return (FALSE);
					}
				}
				Profile.BlockDuplicate += SendDlgItemMessage(hDlg, IDC_BLOCKDUPOPTION, CB_GETCURSEL, 0, 0L);
				Profile.BlockDuplicate += IsDlgButtonChecked(hDlg, IDC_BLOCKEDTXT) ? 4 : 0;
				Profile.BlockDuplicate += GetDlgItemInt(hDlg, IDC_BLOCKDUPTIMER, NULL, FALSE) << 4;
			}
			Profile.Linefeed	= IsDlgButtonChecked(hDlg, IDC_LINEFEEDS);
			Profile.Separator	= IsDlgButtonChecked(hDlg, IDC_SEPARATOR);
			Profile.DateFormat	= SendDlgItemMessage(hDlg, IDC_DATEFORMAT, CB_GETCURSEL, 0, 0L);
                              
			GetDlgItemText(hDlg, IDC_LOGFILEPATH, (LPTSTR)Profile.LogfilePath, sizeof(Profile.LogfilePath) - 1);

			WriteSettings();

			EndDialog(hDlg, TRUE);
			return (TRUE);

			case IDCANCEL:
			EndDialog(hDlg, TRUE);
			return (TRUE);
		}
		break;
   }
   return (FALSE);
} // end of OptionsDlgProc


BOOL FAR PASCAL ScreenOptionsDlgProc(HWND hDlg, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	int i, j, cursel, nextsel, lastsel, state, mode=0;
	char tbuf[100];
//								 PAGING		 MOBITEX	 ACARS
	char columns[8][3][10] = {	"-",		"-",		"-",
								"Address",	"MAN",		"Aircr.Reg",
								"Time",		"Time",		"Time",
								"Date",		"Date",		"Date",
								"Mode",		"Sender",	"Msg.No.",
								"Type",		"Type",		"DBI",
								"Bitrate",	"Bitrate",	"Mode",
								"Message",	"Message",	"Message" };

	switch (uMsg)
	{
		case WM_INITDIALOG:
		if (!CenterWindow(hDlg)) return (FALSE);

		if (Profile.monitor_mobitex)    mode=1;
		else if (Profile.monitor_acars) mode=2;

		for (i=1; i<=7; i++)	// i = column
		{
			for (j=0; j<8; j++)	// j = item
			{
				SendDlgItemMessage(hDlg, IDC_SCREEN_COLUMN+i, CB_ADDSTRING, 0, (LPARAM) (LPSTR) columns[j][mode]);
			}
			SendDlgItemMessage(hDlg, IDC_SCREEN_COLUMN+i, CB_SETCURSEL, Profile.ScreenColumns[i-1], 0L);
		}
		CheckDlgButton(hDlg, IDC_FLEXGROUPMODE,      Profile.FlexGroupMode);
		CheckDlgButton(hDlg, IDC_FGM_LOGGING,        Profile.FlexGroupMode & FLEXGROUPMODE_LOGGING);
		CheckDlgButton(hDlg, IDC_FGM_COMBINE,        Profile.FlexGroupMode & FLEXGROUPMODE_COMBINE);
		CheckDlgButton(hDlg, IDC_FGM_HIDEGROUPCODES, Profile.FlexGroupMode & FLEXGROUPMODE_HIDEGROUPCODES);

		if (Profile.monitor_paging)
		{
			for (i=1; i<=7; i++)
			{
				EnableWindow(GetDlgItem(hDlg, IDC_SCREEN_COLUMN+i),  Profile.FlexGroupMode ? false : true);
			}
			EnableWindow(GetDlgItem(hDlg, IDC_FLEXGROUPMODE), true);
			EnableWindow(GetDlgItem(hDlg, IDC_FGM_LOGGING),        Profile.FlexGroupMode ? true : false);
			EnableWindow(GetDlgItem(hDlg, IDC_FGM_COMBINE),        Profile.FlexGroupMode ? true : false);
			EnableWindow(GetDlgItem(hDlg, IDC_FGM_HIDEGROUPCODES), Profile.FlexGroupMode ? true : false);
		}

		return (TRUE);

		case WM_COMMAND:

		switch (LOWORD(wParam))
		{
			case IDC_SCREEN_COLUMN+1:
			case IDC_SCREEN_COLUMN+2:
			case IDC_SCREEN_COLUMN+3:
			case IDC_SCREEN_COLUMN+4:
			case IDC_SCREEN_COLUMN+5:
			case IDC_SCREEN_COLUMN+6:
			case IDC_SCREEN_COLUMN+7:

			for (i=1; i<=7; i++)
			{
				cursel = SendDlgItemMessage(hDlg, IDC_SCREEN_COLUMN+i, CB_GETCURSEL, 0, 0L);

				if (cursel == 7) // "Message" is selected
				{
					while(i++ <= 7)
					{
						SendDlgItemMessage(hDlg, IDC_SCREEN_COLUMN+i, CB_SETCURSEL, 0, 0L);
					}
					break;
				}
				else if (cursel == 0) // "-" is selected
				{
					nextsel = SendDlgItemMessage(hDlg, IDC_SCREEN_COLUMN+i+1, CB_GETCURSEL, 0, 0L);

					if ((i<7) && (nextsel!=0))
					{
						SendDlgItemMessage(hDlg, IDC_SCREEN_COLUMN+i,   CB_SETCURSEL, nextsel, 0L);
						SendDlgItemMessage(hDlg, IDC_SCREEN_COLUMN+i+1, CB_SETCURSEL, 0, 0L);
					}
				}
			}
			break;

			case IDC_FLEXGROUPMODE:

			state = IsDlgButtonChecked(hDlg, IDC_FLEXGROUPMODE);

			SendDlgItemMessage(hDlg, IDC_SCREEN_COLUMN+1, CB_SETCURSEL, state ? 2 : 1, 0L);
			SendDlgItemMessage(hDlg, IDC_SCREEN_COLUMN+2, CB_SETCURSEL, state ? 3 : 2, 0L);
			SendDlgItemMessage(hDlg, IDC_SCREEN_COLUMN+3, CB_SETCURSEL, state ? 1 : 3, 0L);
			SendDlgItemMessage(hDlg, IDC_SCREEN_COLUMN+4, CB_SETCURSEL, state ? 7 : 4, 0L);
			SendDlgItemMessage(hDlg, IDC_SCREEN_COLUMN+5, CB_SETCURSEL, state ? 0 : 5, 0L);
			SendDlgItemMessage(hDlg, IDC_SCREEN_COLUMN+6, CB_SETCURSEL, state ? 0 : 6, 0L);
			SendDlgItemMessage(hDlg, IDC_SCREEN_COLUMN+7, CB_SETCURSEL, state ? 0 : 7, 0L);

			for (i=1; i<=7; i++)
			{
				EnableWindow(GetDlgItem(hDlg, IDC_SCREEN_COLUMN+i),  state ? false : true);
			}
			EnableWindow(GetDlgItem(hDlg, IDC_FGM_LOGGING),        state ? true : false);
			EnableWindow(GetDlgItem(hDlg, IDC_FGM_COMBINE),        state ? true : false);
			EnableWindow(GetDlgItem(hDlg, IDC_FGM_HIDEGROUPCODES), state ? true : false);

			break;

			case IDOK:

			for (i=1; i<7; i++)
			{
				cursel = SendDlgItemMessage(hDlg, IDC_SCREEN_COLUMN+i, CB_GETCURSEL, 0, 0L);

				if (!cursel)
				{
					if (lastsel != 7)
					{
						strcpy(tbuf, "The last column always has to be 'message'!");
						MessageBox(hDlg, tbuf, "PDW Screen Options", MB_ICONERROR);
						SetFocus(GetDlgItem(hDlg, IDC_SCREEN_COLUMN+i));
						return (FALSE);
					}
					else break;
				}
				lastsel = cursel;

				for (j=i+1; i<7&&j<7; j++)
				{
					nextsel = SendDlgItemMessage(hDlg, IDC_SCREEN_COLUMN+j, CB_GETCURSEL, 0, 0L);

					if (cursel == nextsel)
					{
						sprintf(tbuf, "Item '%s' has been selected more than once!", columns[j]);
						MessageBox(hDlg, tbuf, "PDW Screen Options", MB_ICONERROR);
						SetFocus(GetDlgItem(hDlg, IDC_SCREEN_COLUMN+j));
						return (FALSE);
					}
				}
			}
			for (i=1; i<=7; i++)	// i = column
			{
				Profile.ScreenColumns[i-1] = SendDlgItemMessage(hDlg, IDC_SCREEN_COLUMN+i, CB_GETCURSEL, 0, 0L);
			}
			SetMessageItemPositionsWidth();
			DrawTitleBarGfx(ghWnd);		// Draw pane1/pane2 title bars

			Profile.FlexGroupMode = IsDlgButtonChecked(hDlg, IDC_FLEXGROUPMODE);
			if (Profile.FlexGroupMode)
			{
				Profile.FlexGroupMode += IsDlgButtonChecked(hDlg, IDC_FGM_LOGGING) ? FLEXGROUPMODE_LOGGING : 0;
				Profile.FlexGroupMode += IsDlgButtonChecked(hDlg, IDC_FGM_COMBINE) ? FLEXGROUPMODE_COMBINE : 0;
				Profile.FlexGroupMode += IsDlgButtonChecked(hDlg, IDC_FGM_HIDEGROUPCODES) ? FLEXGROUPMODE_HIDEGROUPCODES : 0;
			}

			WriteSettings();

			EndDialog(hDlg, TRUE);
			return (TRUE);

			case IDCANCEL:
			EndDialog(hDlg, TRUE);
			return (TRUE);
		}
		break;
   }
   return (FALSE);
} // end of ScreenOptionsDlgProc


BOOL FAR PASCAL ScrollDlgProc(HWND hDlg, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	int pane1_size = Profile.pane1_size;
	int pane2_size = Profile.pane2_size;
	int percent_state = Profile.percent;
	int pane2_percent = (100-Profile.percent);
	int value;

	BOOL	  errTrans;
	size_t	  mem_size;
	char tbuf[128]="";	// PH

	switch (uMsg)
	{
		case WM_INITDIALOG:
		if (!CenterWindow(hDlg)) return (FALSE);

		SendDlgItemMessage(hDlg, IDC_SCROLLPANE1, EM_LIMITTEXT, 4, 0L);
		SetDlgItemInt(hDlg, IDC_SCROLLPANE1, pane1_size, FALSE);

		SendDlgItemMessage(hDlg, IDC_SCROLLPANE2, EM_LIMITTEXT, 4, 0L);
		SetDlgItemInt(hDlg, IDC_SCROLLPANE2, pane2_size, FALSE);

		SendDlgItemMessage(hDlg, IDC_SCROLLSPEED, CB_ADDSTRING, 0, (LPARAM) (LPSTR) "1 Line");
		SendDlgItemMessage(hDlg, IDC_SCROLLSPEED, CB_ADDSTRING, 0, (LPARAM) (LPSTR) "5 Lines");
		SendDlgItemMessage(hDlg, IDC_SCROLLSPEED, CB_ADDSTRING, 0, (LPARAM) (LPSTR) "10 Lines");
		SendDlgItemMessage(hDlg, IDC_SCROLLSPEED, CB_ADDSTRING, 0, (LPARAM) (LPSTR) "Page");

		switch(Profile.ScrollSpeed)
		{
			case 0 :
			SendDlgItemMessage(hDlg, IDC_SCROLLSPEED, CB_SETCURSEL, 3, 0L);
			break;

			case 1 :
			SendDlgItemMessage(hDlg, IDC_SCROLLSPEED, CB_SETCURSEL, 0, 0L);
			break;

			default:
			case 5 :
			SendDlgItemMessage(hDlg, IDC_SCROLLSPEED, CB_SETCURSEL, 1, 0L);
			break;

			case 10 :
			SendDlgItemMessage(hDlg, IDC_SCROLLSPEED, CB_SETCURSEL, 2, 0L);
			break;
		}

		SendDlgItemMessage(hDlg, IDC_PERCENTPANE1, EM_LIMITTEXT, 2, 0L);
		SetDlgItemInt(hDlg, IDC_PERCENTPANE1, Profile.percent, FALSE);

		SetDlgItemInt(hDlg, IDC_PERCENTPANE2, pane2_percent, FALSE);

		return (TRUE);

		case WM_COMMAND:
		switch (LOWORD(wParam))
		{
			case IDC_PERCENTPANE1:

			value = GetDlgItemInt(hDlg, IDC_PERCENTPANE1, NULL, FALSE);
			SetDlgItemInt(hDlg, IDC_PERCENTPANE2, (100-value), FALSE);

			return (TRUE);

			case IDOK:

			value = GetDlgItemInt(hDlg, IDC_SCROLLPANE1, &errTrans, FALSE);

			if (value < PANE1_SIZE)
			{
				sprintf(tbuf, "Message scrollback minimum is %d!", PANE1_SIZE);
				MessageBox(hDlg, tbuf, "PDW Scrollback", MB_ICONERROR);
				SendDlgItemMessage(hDlg,  IDC_SCROLLPANE1, EM_SETSEL, 0, -1);
				SetFocus(GetDlgItem(hDlg, IDC_SCROLLPANE1));
				return (FALSE);
			}

			if (value != pane1_size)  // did it change?
			{
				if (Pane1.buff_char != NULL)	free(Pane1.buff_char);
				if (Pane1.buff_color != NULL)	free(Pane1.buff_color);

				mem_size = (value+1) * (LINE_SIZE+1);
				Pane1.buff_char = (char *) malloc(mem_size);
				Pane1.buff_color = (BYTE *)malloc(mem_size);

				if ((Pane1.buff_char == NULL) || (Pane1.buff_color == NULL))
				{
					if (Pane1.buff_char != NULL)	free(Pane1.buff_char);
					if (Pane1.buff_color != NULL)	free(Pane1.buff_color);

					value = PANE1_SIZE;
					mem_size = (value+1) * (LINE_SIZE+1);
					Pane1.buff_char = (char *) malloc(mem_size);
					Pane1.buff_color = (BYTE *)malloc(mem_size);

					MessageBox(hDlg,"Error Allocating Memory!","PDW Scrollback",MB_ICONWARNING);
				}
				Profile.pane1_size = value;
				Pane1.buff_lines = Profile.pane1_size;

				InitializePane(&Pane1);
				InvalidateRect(Pane1.hWnd, NULL, TRUE);
			}
			value = GetDlgItemInt(hDlg, IDC_SCROLLPANE2, &errTrans, FALSE);

			if (value < PANE2_SIZE)
			{
				sprintf(tbuf, "Message scrollback minimum is %d!", PANE2_SIZE);
				MessageBox(hDlg, tbuf, "PDW Scrollback",MB_ICONERROR);
				SendDlgItemMessage(hDlg,  IDC_SCROLLPANE2, EM_SETSEL, 0, -1);
				SetFocus(GetDlgItem(hDlg, IDC_SCROLLPANE2));
				return (FALSE);
			}

			if (value != pane2_size)  // did it change?
			{
				if (Pane2.buff_char != NULL)	free(Pane2.buff_char);
				if (Pane2.buff_color != NULL)	free(Pane2.buff_color);

				mem_size = (value+1) * (LINE_SIZE+1);
				Pane2.buff_char = (char *)malloc(mem_size);
				Pane2.buff_color = (BYTE *)malloc(mem_size);

				if ((Pane2.buff_char == NULL) || (Pane2.buff_color == NULL))
				{
					if (Pane2.buff_char != NULL)	free(Pane2.buff_char);
					if (Pane2.buff_color != NULL)	free(Pane2.buff_color);

					value = PANE2_SIZE;
					mem_size = (value+1) * (LINE_SIZE+1);
					Pane2.buff_char = (char *)malloc(mem_size);
					Pane2.buff_color = (BYTE *)malloc(mem_size);

					MessageBox(hDlg,"Error Allocating Memory!","PDW Scrollback",MB_ICONWARNING);
				}
				Profile.pane2_size = value;
				Pane2.buff_lines = Profile.pane2_size;

				InitializePane(&Pane2);
				InvalidateRect(Pane2.hWnd, NULL, TRUE);
			}
			value = GetDlgItemInt(hDlg, IDC_PERCENTPANE1, NULL, FALSE);
			
			if ((value < 10) || (value > 90))
			{
				sprintf(tbuf, "Please enter a percentage between 10 and 90!", PANE1_SIZE);
				MessageBox(hDlg, tbuf, "PDW Scrollback", MB_ICONERROR);
				SendDlgItemMessage(hDlg,  IDC_PERCENTPANE1, EM_SETSEL, 0, -1);
				SetFocus(GetDlgItem(hDlg, IDC_PERCENTPANE1));
				return (FALSE);
			}
			else Profile.percent = value;

			switch(SendDlgItemMessage(hDlg, IDC_SCROLLSPEED, CB_GETCURSEL, 0, 0L))
			{
				case 0: Profile.ScrollSpeed = 1;
				break;
				case 1: Profile.ScrollSpeed = 5;
				break;
				case 2: Profile.ScrollSpeed = 10;
				break;
				default:Profile.ScrollSpeed = 0;
				break;
			}

			WriteSettings();

			EndDialog(hDlg, TRUE);

			// Update panes?
			if (Profile.percent != percent_state)	// Both panes?
			{
				SetWindowPaneSize(PANE_SIZES);
			}
			return (TRUE);

			case IDCANCEL:
			EndDialog(hDlg, TRUE);
			return (TRUE);
		}
		break;
	}
	return (FALSE);
} // end of ScrollDlgProc


void Copy_Filter_Fields(FILTER *out_filter, FILTER in_filter)
{
	out_filter->type = in_filter.type;

	if (in_filter.capcode[0]) strcpy(out_filter->capcode, in_filter.capcode);
	else							  out_filter->capcode[0] = 0;

	out_filter->reject        = in_filter.reject;
	out_filter->wave_number   = in_filter.wave_number;
	out_filter->label_enabled = in_filter.label_enabled;
	out_filter->label_color   = in_filter.label_color;

	if (in_filter.label[0]) strcpy(out_filter->label, in_filter.label);
	else						   out_filter->label[0] = 0;

	if (in_filter.text[0]) strcpy(out_filter->text, in_filter.text);
	else						  out_filter->text[0] = 0;

	out_filter->cmd_enabled		= in_filter.cmd_enabled;
	out_filter->monitor_only	= in_filter.monitor_only;
	out_filter->smtp			= in_filter.smtp;
	out_filter->match_exact_msg	= in_filter.match_exact_msg;

	for (int i=0; i<3; i++)
	{
		if (in_filter.sep_filterfile[i][0]) strcpy(out_filter->sep_filterfile[i], in_filter.sep_filterfile[i]);
		else out_filter->sep_filterfile[i][0] = 0;
	}

	out_filter->sep_filterfiles     = in_filter.sep_filterfiles;
	out_filter->sep_filterfile_en   = in_filter.sep_filterfile_en;

	out_filter->hitcounter = in_filter.hitcounter;	// PH: Number of hits

	strcpy(out_filter->lasthit_date, in_filter.lasthit_date);
	strcpy(out_filter->lasthit_time, in_filter.lasthit_time);

	return;
} // end of Copy_Filter_Fields()


void BuildFilterString(char *temp_str, FILTER filter)
{
	char *filter_types[7] = {"UNUSED", "FLEX","POCSAG","TEXT","ERMES ","ACARS ", "MOBITX"};
	char *wave_names[11]  = {"Default","Sound-0","Sound-1","Sound-2","Sound-3","Sound-4",
									   "Sound-5","Sound-6","Sound-7","Sound-8","Sound-9"};

	strcpy(temp_str, filter_types[filter.type]);

	if (filter.type == ACARS_FILTER) strcat(temp_str, " ");
	
	strcat(temp_str, " ");
	strcat(temp_str, filter.capcode[0] ? filter.capcode : "         ");

	if ((filter.type == MOBITEX_FILTER) && filter.capcode[strlen(filter.capcode)-1] == 'X')
	{
		temp_str[strlen(temp_str)-2] = '\0';
	}
	else if ((filter.type == POCSAG_FILTER) && filter.capcode[strlen(filter.capcode)-2] == '-')
	{
		temp_str[strlen(temp_str)-2] = '\0';
	}

	if (filter.reject)
	{
		strcat(temp_str, " | REJECT");
		if (filter.text[0] && !filter.capcode[0])
		{
			strcat(temp_str, " | \"");
			strcat(temp_str, filter.text);
			strcat(temp_str, "\"");
		}
		return;
	}

	if (Profile.FilterWindowExtra)
	{
		strcat(temp_str, " | ");
		strcat(temp_str, filter.cmd_enabled ? "CMD" : "cmd");
		strcat(temp_str, " | ");
		strcat(temp_str, filter.label_enabled ? "LAB" : "lab");
		strcat(temp_str, " | ");
		strcat(temp_str, filter.sep_filterfile_en ? "SEP" : "sep");
		strcat(temp_str, " | ");

		if (filter.monitor_only)
		{
			strcat(temp_str, filter.wave_number ? "M-ONLY " : "NoSound");
		}
		else
		{
			strcat(temp_str, filter.wave_number == -1 ? "NoSound" : wave_names[filter.wave_number]);
		}
	}

	if (filter.label[0])
	{
		strcat(temp_str, " | ");
		strcat(temp_str, filter.label);
	}

	if (filter.text[0])
	{
		strcat(temp_str, " | \"");
		strcat(temp_str, filter.text);
		strcat(temp_str, "\"");
	}
	return;
} // end of BuildFilterString()


BOOL FAR PASCAL FilterDlgProc(HWND hDlg, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	int index, more;
	int x, y, w, h, buttonspace, start, next;

	static int iFiltersColors = Profile.FilterWindowColors;
	static HFONT hf ;

	RECT rect;
	DWORD  dwStyle;
	HBRUSH hb ;
	UINT   idCtl;				// control identifier 
	LPDRAWITEMSTRUCT lpdis;		// item-drawing information 
	LOGFONT font_listview;
		
	switch (uMsg)
	{
		case WM_DRAWITEM:
		{
			idCtl = (UINT) wParam;						// control identifier 

			if (idCtl != IDC_FILTERS) break;

			lpdis = (LPDRAWITEMSTRUCT) lParam;			// item-drawing information 
			rect  = lpdis->rcItem ;
			rect.left=1;

			if (lpdis->itemAction == ODA_DRAWENTIRE)
			{
				SetTextColor(lpdis->hDC, GetColorRGB(COLOR_FILTERLABEL+Profile.filters[lpdis->itemID].label_color));

				if (ListView_GetItemState(hListView, lpdis->itemID, LVIS_SELECTED))
				{
					FillRect(lpdis->hDC, &rect, hb = CreateSolidBrush(GetColorRGB(COLOR_FILTERLABEL+11)));
					DeleteObject(hb) ;
					SetTextColor(lpdis->hDC, GetColorRGB(COLOR_FILTERLABEL+6));// WHITE
				}
				else
				{
					if (!Profile.filters[lpdis->itemID].label_enabled)
					{
						SetTextColor(lpdis->hDC, RGB(0x44, 0x44, 0x44));	// GRAY
					}
					FillRect(lpdis->hDC, &rect, hb = CreateSolidBrush(Profile.color_background));
					DeleteObject(hb) ;
				}

				BuildFilterString(szTEMP, Profile.filters[lpdis->itemID]);
				DrawText(lpdis->hDC, szTEMP, strlen(szTEMP), &rect, DT_SINGLELINE | DT_LEFT | DT_NOPREFIX | DT_END_ELLIPSIS);

				if (ListView_GetItemState(hListView, lpdis->itemID, LVIS_FOCUSED))
				{
					DrawFocusRect(lpdis->hDC, &rect) ;
				}
			}
			if (lpdis->itemAction == ODA_SELECT)
			{
				DrawFocusRect(lpdis->hDC, &rect) ;
			}

			return(TRUE) ;
		}
		break ;

		case WM_INITDIALOG:

		if (!CenterWindow(hDlg)) return (FALSE);

		sprintf(szTEMP, "PDW Filters (%u)", Profile.filters.size());
		SetWindowText(hDlg, (LPSTR) szTEMP);

		hFilterDlg = hDlg;

		// Get the screen sizes
		x = GetSystemMetrics(SM_CXMAXIMIZED) ;
		y = GetSystemMetrics(SM_CYMAXIMIZED) ;
		MoveWindow(hDlg, 76, 69, x-84, y-77, TRUE) ;

		// Get the size of the Filter Window
		GetClientRect(hDlg, &rect) ;
		x = rect.right ;
		y = rect.bottom ;

		// Get the size of a button 
		// All buttons are assumed to be the same size!
		GetClientRect(GetDlgItem(hDlg, IDOK) , &rect) ;
		w = x/12;	// PH: Button-width is resolution dependant
		h = rect.bottom ;

		start=12;
		next=(x-20)/10;

		buttonspace = (x/15)+10; // PH: Variable space between buttons, depending on resolution

		MoveWindow(GetDlgItem(hDlg, IDC_FILTERS), 10, 10, x - 10, y - (30 + h) , TRUE) ;
		MoveWindow(GetDlgItem(hDlg, IDC_FILTERADD), start, y - (10 + h), w, h, TRUE);
		MoveWindow(GetDlgItem(hDlg, IDC_FILTEREDIT), start+next, y - (10 + h), w, h, TRUE);
		MoveWindow(GetDlgItem(hDlg, IDC_FILTERDEL), start+(2*next), y - (10 + h), w, h, TRUE);
		MoveWindow(GetDlgItem(hDlg, IDC_FILTEROPTIONS), start+(5*next), y - (10 + h), w, h, TRUE);
		MoveWindow(GetDlgItem(hDlg, IDC_FILTERFIND),   start+(6*next), y - (10 + h), w, h, TRUE);
		MoveWindow(GetDlgItem(hDlg, IDOK), start+(9*next), y - (10 + h), w, h, TRUE);

		InitListControl(hDlg);

		font_listview.lfHeight			= -11;
		font_listview.lfWidth			= 0;
		font_listview.lfEscapement		= 0;
		font_listview.lfOrientation		= 0;
		font_listview.lfWeight			= FW_BOLD;
		font_listview.lfItalic			= 0;
		font_listview.lfUnderline		= 0;
		font_listview.lfStrikeOut		= 0;
		font_listview.lfCharSet			= OEM_CHARSET;
		font_listview.lfOutPrecision	= OUT_STROKE_PRECIS;
		font_listview.lfClipPrecision	= CLIP_DEFAULT_PRECIS;
		font_listview.lfQuality			= DEFAULT_QUALITY;
		font_listview.lfPitchAndFamily	= FIXED_PITCH | FF_MODERN;
		lstrcpy(font_listview.lfFaceName, "MS Sans Serif");

		SendDlgItemMessage(hDlg, IDC_FILTERS, WM_SETFONT, (WPARAM) CreateFontIndirect(&font_listview), 0);

		if (Profile.FilterWindowColors)
		{
			dwStyle = GetWindowLong(hListView, GWL_STYLE);
			SetWindowLong(hListView, GWL_STYLE, dwStyle | LVS_OWNERDRAWFIXED);
			SendMessage(hListView, LVM_SETBKCOLOR, 0, Profile.color_background);
		}
		else
		{
			SendMessage(hListView, LVM_SETBKCOLOR, 0, GetColorRGB(COLOR_FILTERLABEL+6)); // WHITE
		}
   
		for (index=0; index<Profile.filters.size(); index++)
		{
			BuildFilterString(szTEMP, Profile.filters[index]);
			InsertListViewItem(szTEMP, index);
		}
		ListView_SetItemState(hListView, 0, LVIS_SELECTED | LVIS_FOCUSED, LVIS_SELECTED | LVIS_FOCUSED);

		return (TRUE);

		case WM_TIMER:
		OnTimer(wParam) ;
		break ;

		default :
//			OUTPUTDEBUGMSG((("WM_%04X lParam = %08lX wParam %d\n"), uMsg, lParam, wParam));
		break ;

		case WM_CONTEXTMENU:
		{
			ShowContextMenu(HMENU_FILTERS, hFilterDlg);
		}
		break ;

		case WM_NOTIFY:

		switch (((LPNMHDR)lParam)->code)
		{
			case LVN_ITEMCHANGED :

			if (bSortingFilters || bDeletingFilters) break;

			more = ListView_GetSelectedCount(hListView) > 1 ; 

			EnableWindow(GetDlgItem(hDlg, IDC_FILTERADD),  !more);
			EnableWindow(GetDlgItem(hDlg, IDC_FILTERFIND), (!more && Profile.filters.size()));

			if (more)
			{
				if (bEditFilter) break;

				sprintf(szTEMP, "PDW Filters (%u) - %i filters selected", Profile.filters.size(), ListView_GetSelectedCount(hListView));
				SetWindowText(hDlg, (LPSTR) szTEMP);
			}
			else if ((index = ListView_GetNextItem(hListView, -1, LVNI_SELECTED)) != CB_ERR)
			{
				sprintf(szTEMP, "PDW Filters (%u) - ", Profile.filters.size());

				if (filter.type == TEXT_FILTER)
				{
					strcat(szTEMP, "\"");
					strcat(szTEMP, Profile.filters[index].text);
					strcat(szTEMP, "\"");

					if (Profile.filters[index].label[0])
					{
						strcat(szTEMP, " | ");
						strcat(szTEMP, Profile.filters[index].label);
					}
				}
				else
				{
					if ( index < Profile.filters.size() )
					{
						strcat(szTEMP, Profile.filters[index].capcode);

						if (Profile.filters[index].label[0])
						{
							strcat(szTEMP, " | ");
							strcat(szTEMP, Profile.filters[index].label);
						}
						if (Profile.filters[index].text[0])
						{
							strcat(szTEMP, " | \"");
							strcat(szTEMP, Profile.filters[index].text);
							strcat(szTEMP, "\"");
						}
					}
				}
				SetWindowText(hDlg, (LPSTR) szTEMP);
			}
			break;
	
			case LVN_KEYDOWN:
//			OUTPUTDEBUGMSG((("LVN_KEYDOWN lParam = %08lX Key %04X flags %04X\n"), lParam, ((LPNMLVKEYDOWN)lParam)->wVKey, ((LPNMLVKEYDOWN)lParam)->flags)) ;

			if (GetKeyState(VK_CONTROL) < 0)
			{
				switch(((LPNMLVKEYDOWN)lParam)->wVKey)
				{
					case 'F' :	// Find filter

						PostMessage(hFilterDlg, WM_COMMAND, IDC_FILTERFIND, 0L) ;

					break ;

					case 'C' :	// Copy current selection

					if(CheckSelection(false))
					{
						PostMessage(hFilterDlg, WM_COMMAND, IDT_MENU_COPY, 0L) ;
					}
					break ;

					case 'A' :	// Select all filters

						PostMessage(hFilterDlg, WM_COMMAND, IDT_MENU_SELECTALL, 0L) ;

					break ;

					case 'V' :	// Paste copied filters

					if(CheckSelection(false))
					{
						PostMessage(hFilterDlg, WM_COMMAND, IDT_MENU_PASTE, 0L) ;
					}

					break ;

					case 'O' :	// Show context menu

						ShowContextMenu(HMENU_FILTERS, hFilterDlg);

					break ;
				}
			}
			else
			{
				switch(((LPNMLVKEYDOWN)lParam)->wVKey)
				{
					case VK_DELETE :	// Delete current selection
					case VK_F8 :

						PostMessage(hFilterDlg, WM_COMMAND, IDC_FILTERDEL, 0L) ;

					break ;

					case VK_INSERT :	// Insert filter

					if(CheckSelection(false))
					{
						PostMessage(hFilterDlg, WM_COMMAND, IDC_FILTERADD, 0L) ;
					}
					break ;

					case VK_SPACE :		// Page down

						PostMessage(hListView, WM_KEYDOWN, VK_NEXT, 0L) ;

					break ;

					case VK_F3:			// Find next filter

					if (Profile.filters.size()) FindFilter(-1, "", false, (GetKeyState(VK_SHIFT) & 0x80) ? true : false);

					break ;

					case VK_F4:			// Edit current selection

					if (GetKeyState(VK_MENU) < 0)	// If ALT=pressed -> exit
					{
						 PostMessage(hFilterDlg, WM_COMMAND, IDCANCEL, 0L) ;
					}
					else PostMessage(hFilterDlg, WM_COMMAND, IDC_FILTEREDIT, 0L) ;

					break ;

					case VK_F7:			// Find filter

						PostMessage(hFilterDlg, WM_COMMAND, IDC_FILTERFIND, 0L) ;

					break;

					case VK_F5:			// Copy current selection

					if(CheckSelection(false))
					{
						PostMessage(hFilterDlg, WM_COMMAND, IDT_MENU_COPY, 0L) ;
					}
					break;
				}
			}
			break ;

			case NM_CLICK:
				SetFocus(GetDlgItem(hFilterDlg, IDC_FILTERS)) ;

				more = ListView_GetSelectedCount(hListView) > 1 ; 
				
				EnableWindow(GetDlgItem(hDlg, IDC_FILTERADD), !more);
				break ;

			case NM_DBLCLK:
				PostMessage(hFilterDlg, WM_COMMAND, IDC_FILTEREDIT, 0L) ;
				break ;

			case LVN_BEGINDRAG:
				OnBeginDrag((LPNMHDR)lParam) ;
				break;

			case NM_CUSTOMDRAW :
				SetWindowLong(hFilterDlg, DWL_MSGRESULT, OnCustomDraw((LPNMLVCUSTOMDRAW) lParam)) ;
				return(TRUE) ;
//			default :
//				OUTPUTDEBUGMSG((("NM_%04X lParam = %08lX\n"), ((LPNMHDR)lParam)->code, lParam));
//				return(TRUE) ;
		}
		break ;

		case WM_MOUSEMOVE:
			if (bDragging) OnMouseMove(wParam, LOWORD(lParam), HIWORD(lParam)) ;
		break;

		case WM_LBUTTONUP:
			OnLButtonUp(wParam, LOWORD(lParam), HIWORD(lParam)) ;
		break;

		case WM_COMMAND:

		switch (LOWORD(wParam))
		{
			case IDT_MENU_SORT_ADDRESS :
				SortFilter(hDlg, true);
			break;

			case IDT_MENU_SORT_LABEL :
				SortFilter(hDlg, false);
			break;

			case IDT_MENU_COPY :
				CopyFilter() ;
			break;

			case IDT_MENU_PASTE :
				PasteFilter() ;
				sprintf(szTEMP,"PDW Filters (%u)", Profile.filters.size());
				SetWindowText(hDlg, (LPSTR) szTEMP);
			break;

			case IDT_MENU_RESET :
				ResetHitcounters(false) ;
			break;

			case IDT_MENU_SELECTALL :
				ListView_SetItemState(hListView, -1, LVIS_SELECTED, LVIS_SELECTED) ;
			break;

			case IDC_FILTERFIND:

			if (Profile.filters.size())
			{
				GoModalDialogBoxParam(ghInstance, MAKEINTRESOURCE(FILTERFINDDLGBOX),
										 hDlg, (DLGPROC) FilterFindDlgProc, 0L);
				SetFocus(GetDlgItem(hFilterDlg, IDC_FILTERS));
			}
			break;

			case IDT_FILTERCHECKDUPLICATE:
				GoModalDialogBoxParam(ghInstance, MAKEINTRESOURCE(FILTERCHECKDUPLICATEDLGBOX),
										 hDlg, (DLGPROC) FilterCheckDuplicateDlgProc, 0L);

				if (iDuplicateFilter != -1)
				{
					ListView_SetItemState (hListView, -1, 0, LVIS_SELECTED | LVIS_FOCUSED);
					ListView_SetItemState (hListView, iDuplicateFilter, LVIS_SELECTED | LVIS_FOCUSED, LVIS_SELECTED | LVIS_FOCUSED);
					SendMessage(hListView, LVM_SETSELECTIONMARK, 0, iDuplicateFilter);
					ListView_EnsureVisible(hListView, iDuplicateFilter, TRUE);
				}
				SetFocus(GetDlgItem(hFilterDlg, IDC_FILTERS));

			break;

			case IDM_FILTEROPTIONS:

			GoModalDialogBoxParam(ghInstance, MAKEINTRESOURCE(FILTEROPTIONSDLGBOX),
								  hDlg, (DLGPROC) FilterOptionsDlgProc, 0L);
			break;

			case IDC_FILTERADD:

				more = ListView_GetSelectedCount(hListView) > 1 ; 
				if (more) break;

				//HWi
				memset(&filter, 0, sizeof(filter)) ;

				SetFocus(GetDlgItem(hFilterDlg, IDC_FILTERS));

				GoModalDialogBoxParam(ghInstance, MAKEINTRESOURCE(FILTEREDITDLGBOX),
									 hDlg, (DLGPROC) FilterEditDlgProc, 0L);

				sprintf(szTEMP, "PDW Filters (%u)",Profile.filters.size());
				SetWindowText(hDlg, (LPSTR) szTEMP);

			break;

			case IDC_FILTERS:

			if (HIWORD(wParam) != LBN_DBLCLK) break; // fall through to IDC_FILTEREDIT

			case IDC_FILTEREDIT:

			more = ListView_GetSelectedCount(hListView) > 1 ; 

			SetFocus(GetDlgItem(hFilterDlg, IDC_FILTERS));

			if ((index = ListView_GetNextItem(hListView, -1, LVNI_SELECTED)) != CB_ERR)
			{
				bEditFilter = true;

				Copy_Filter_Fields(&filter, Profile.filters[index]);

				GoModalDialogBoxParam(ghInstance, MAKEINTRESOURCE(FILTEREDITDLGBOX),
											 hDlg, (DLGPROC) FilterEditDlgProc, 0L);
				bEditFilter = false;
			}
			else MessageBox(hDlg, "You must select a filter to edit!", "PDW Filter", MB_ICONERROR);

			break;

			case IDC_FILTERDEL:

			more = ListView_GetSelectedCount(hListView) > 1 ; 

			bDeletingFilters = true;

			SetWindowText(hDlg, (LPSTR) "PDW Filters - Please wait while removing filters...");

			if ((index = ListView_GetNextItem(hListView, -1, LVNI_SELECTED)) != CB_ERR)
			{
				Copy_Filter_Fields(&filter, Profile.filters[index]);

				if (more) strcpy(szTEMP, "Remove selected filters ?");
				else
				{
					sprintf(szTEMP, "Remove filter '%s' ?",
						    filter.type != TEXT_FILTER ? filter.capcode : filter.text);
				}

				SetFocus(GetDlgItem(hFilterDlg, IDC_FILTERS));

				if (MessageBox(hDlg, szTEMP, more ? "PDW Remove filters" : "PDW Remove filter", MB_ICONQUESTION | MB_OKCANCEL) == IDOK)
				{
					ListView_EnsureVisible(hListView, index, TRUE);	// Ensure visability of first selected filter

					while (index != CB_ERR)
					{
						Profile.filters.erase(Profile.filters.begin()+index);
						ListView_DeleteItem(hListView, index) ;
						index = ListView_GetNextItem(hListView, index-1, LVNI_SELECTED) ;
						pumpMessages();	// Process messages
					}

					if ((index = ListView_GetNextItem(hListView, -1, LVNI_FOCUSED)) != CB_ERR)
					{
						ListView_SetItemState(hListView, index, LVIS_SELECTED, LVIS_SELECTED);
						SendMessage(hListView, LVM_SETSELECTIONMARK, 0, index);
					}
					nCopyStart = nCopyEnd = -1 ;
				}
			}
			else MessageBox(hDlg, "You must select a filter to remove!", "PDW Filter", MB_ICONERROR);

			bDeletingFilters = false;

			sprintf(szTEMP, "PDW Filters (%u)",Profile.filters.size());
			SetWindowText(hDlg, (LPSTR) szTEMP);
            
			break;

			case IDC_FILTEROPTIONS:

			GoModalDialogBoxParam(ghInstance, MAKEINTRESOURCE(FILTEROPTIONSDLGBOX),
										 hDlg, (DLGPROC) FilterOptionsDlgProc, 0L);
			SetFocus(GetDlgItem(hFilterDlg, IDC_FILTERS));

			if (iFiltersColors == Profile.FilterWindowColors) break;

			else
			{
				ListView_DeleteAllItems(hListView);

				dwStyle = GetWindowLong(hListView, GWL_STYLE);

				if (Profile.FilterWindowColors)
				{
					SetWindowLong(hListView, GWL_STYLE, dwStyle | LVS_OWNERDRAWFIXED);
					SendMessage(hListView, LVM_SETBKCOLOR, 0, Profile.color_background);
				}
				else
				{
					SetWindowLong(hListView, GWL_STYLE, dwStyle - LVS_OWNERDRAWFIXED);
					SendMessage(hListView, LVM_SETBKCOLOR, 0, GetColorRGB(COLOR_FILTERLABEL+6)); // WHITE
				}
   
				for (index=0; index<Profile.filters.size(); index++)
				{
					BuildFilterString(szTEMP, Profile.filters[index]);
					InsertListViewItem(szTEMP, index);
				}
				ListView_SetItemState(hListView, 0, LVIS_SELECTED | LVIS_FOCUSED, LVIS_SELECTED | LVIS_FOCUSED);
				iFiltersColors ^= 0x01;
			}

			break;

			case IDOK:
			case IDCANCEL:

			if (GetKeyState(VK_RETURN) & 0x80)	// If RETURN/ENTER has been pressed, edit current filter
			{
				PostMessage(hFilterDlg, WM_COMMAND, IDC_FILTEREDIT, 0L);
				break;
			}
			bUpdateFilters = true;	// PH: Update filters.ini in UpdateFilters() when IDLE
			bFilterFindCASE = false;

			hFilterDlg = NULL;
			EndDialog(hDlg, TRUE);

			return (TRUE);
		}
		break;
	}
	return (FALSE);
}  // end of FilterDlgProc()


BOOL FAR PASCAL FilterOptionsDlgProc(HWND hDlg, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	static char szFilterLog[] =
		{"Filter Log Files (*.flt)\0*.flt\0All Files (*.*)\0*.*\0\0"};
	static char szFilterCmd[] =
		{"Command Files (*.exe)\0*.exe\0All Files (*.*)\0*.*\0\0"};
	static char *types[6] = { "FLEX Capcode", "POCSAG Riccode",
		                      "TEXT",		  "ERMES Address",
							  "ACARS Reg.no", "Mobitex MAN" };
	OPENFILENAME ofn = {0};
	char szFileFilter[FILTER_FILE_LEN+1], szFileTitle[256], szFileFilterExt[5]="flt";
	char szFileCmd[MAX_FILE_LEN+1], szCmdArgs[MAX_FILE_LEN+1];
	char tmp_message[256];			// PH: Buffer for error messages
	char temp_filename[MAX_PATH]="";
	char temp[MAX_FILE_LEN+1];
	int  i, state, date_state, cmd_state, index;

	FILE *pFileFilter = NULL;

	CreateDateFilename(".flt", NULL);		// PH: Create date for filename

	switch (uMsg)
	{
		case WM_INITDIALOG:
		if (!CenterWindow(hDlg)) return (FALSE);

		CheckDlgButton(hDlg, IDC_LABELLOG,     Profile.LabelLog);
		CheckDlgButton(hDlg, IDC_LABELNEWLINE, Profile.LabelNewline);
		CheckDlgButton(hDlg, IDC_FILTERFILEEN, Profile.filterfile_enabled);
		CheckDlgButton(hDlg, IDC_FILTERFILEDATE, Profile.filterfile_use_date);
		CheckDlgButton(hDlg, IDC_FILTERTONE, Profile.showtone);
		CheckDlgButton(hDlg, IDC_FILTERNUMERIC, Profile.shownumeric);
		CheckDlgButton(hDlg, IDC_FILTERBINARYHEX, Profile.showmisc);
		CheckDlgButton(hDlg, IDC_FILTERBEEP,  Profile.filterbeep);
		CheckDlgButton(hDlg, IDC_FILTERWINDOWONLY, Profile.filterwindowonly);
		CheckDlgButton(hDlg, IDC_FILTERSCOLORS, Profile.FilterWindowColors);
		CheckDlgButton(hDlg, IDC_FILTERSEXTRA, Profile.FilterWindowExtra);

		for (i=1; i<=7; i++)
		{
			CheckDlgButton(hDlg, IDC_FILTERLOGCOLUMN+i, strchr(Profile.ColFilterfile, '0'+i) != 0);
		}

		if (Profile.monitor_mobitex)
		{
			SendDlgItemMessage(hDlg, IDC_FILTERLOGCOLUMN+1, WM_SETTEXT, 0, (LPARAM) "MAN");
			SendDlgItemMessage(hDlg, IDC_FILTERLOGCOLUMN+4, WM_SETTEXT, 0, (LPARAM) "Sender");
		}
		else if (Profile.monitor_acars)
		{
			SendDlgItemMessage(hDlg, IDC_FILTERLOGCOLUMN+1, WM_SETTEXT, 0, (LPARAM) "Reg.No.");
			SendDlgItemMessage(hDlg, IDC_FILTERLOGCOLUMN+4, WM_SETTEXT, 0, (LPARAM) "Msg.No.");
			SendDlgItemMessage(hDlg, IDC_FILTERLOGCOLUMN+5, WM_SETTEXT, 0, (LPARAM) "DBI");
			SendDlgItemMessage(hDlg, IDC_FILTERLOGCOLUMN+6, WM_SETTEXT, 0, (LPARAM) "Mode");
		}
		
		if (Profile.filterfile_enabled)
		{
			if (Profile.filterfile_use_date)
			{
				EnableWindow(GetDlgItem(hDlg, IDC_FILTERFILE),   false);
				EnableWindow(GetDlgItem(hDlg, IDC_FILTERBROWSE), false);
				SetDlgItemText(hDlg, IDC_FILTERFILE, szFilenameDate);
			}
			else if (Profile.filterfile[0]) // Show filename if exists.
			{
				SetDlgItemText(hDlg, IDC_FILTERFILE, Profile.filterfile);
			}
		}
		else
		{
			EnableWindow(GetDlgItem(hDlg, IDC_FILTERFILE),     false);
			EnableWindow(GetDlgItem(hDlg, IDC_FILTERFILEDATE), false);
			EnableWindow(GetDlgItem(hDlg, IDC_FILTERBROWSE),   false);
		}

		// Filter command-file stuff
		CheckDlgButton(hDlg, IDC_FILTERCMDEN, Profile.filter_cmd_file_enabled);

		if (Profile.filter_cmd_file_enabled)
		{
			EnableWindow(GetDlgItem(hDlg, IDC_FILTERCMDBROWSE), true);
			EnableWindow(GetDlgItem(hDlg, IDC_FILTERCMDFILE),   true);
		}

		SendDlgItemMessage(hDlg, IDC_FILTERCMDARGS, EM_LIMITTEXT, MAX_FILE_LEN, 0L);

		if (Profile.filter_cmd[0])
		{
			SetDlgItemText(hDlg, IDC_FILTERCMDFILE, Profile.filter_cmd);
			EnableWindow(GetDlgItem(hDlg, IDC_FILTERCMDARGS), Profile.filter_cmd_file_enabled);

			if (Profile.filter_cmd_args[0])
			{
				SetDlgItemText(hDlg, IDC_FILTERCMDARGS, Profile.filter_cmd_args);
			}
		}

		for (index=0; index<6; index++) // show filter types
		{
			SendDlgItemMessage(hDlg, IDC_FILTERDEFTYPE, CB_ADDSTRING, 0, (LPARAM) (LPSTR) types[index]);
		}
		SendDlgItemMessage(hDlg, IDC_FILTERDEFTYPE, CB_SETCURSEL, (WPARAM) Profile.filter_default_type, 0L);

		SendMessage(hDlg, WM_WININICHANGE, NULL, 0L);

		return (TRUE);

		case WM_WININICHANGE:

		switch (IsDlgButtonChecked(hDlg, IDC_FILTERBEEP))
		{
			case BST_CHECKED:

			SetDlgItemText(hDlg, IDC_FILTERBEEP, "Filter Beep / Wavfiles (enabled)");

			break;

			case BST_UNCHECKED:

			SetDlgItemText(hDlg, IDC_FILTERBEEP, "Filter Beep / Wavfiles (disabled)");

			break;

			case BST_INDETERMINATE:

			SetDlgItemText(hDlg, IDC_FILTERBEEP, "Filter Beep / Wavfiles (enabled for filtered messages)");

			break;
		}

		break;

		case WM_COMMAND:

		switch (LOWORD(wParam))
		{
			case IDC_FILTERFILEEN:

			state      = IsDlgButtonChecked(hDlg, IDC_FILTERFILEEN);
			date_state = IsDlgButtonChecked(hDlg, IDC_FILTERFILEDATE);

			EnableWindow(GetDlgItem(hDlg, IDC_FILTERFILEDATE), state);

			SetDlgItemText(hDlg, IDC_FILTERFILE, (state && date_state) ? szFilenameDate : "");

			if (state)
			{
				EnableWindow(GetDlgItem(hDlg, IDC_FILTERFILE),   !date_state);
				EnableWindow(GetDlgItem(hDlg, IDC_FILTERBROWSE), !date_state);

				if (!date_state) SetFocus(GetDlgItem(hDlg, IDC_FILTERFILE));

			}
			else
			{
				EnableWindow(GetDlgItem(hDlg, IDC_FILTERFILE),   false);
				EnableWindow(GetDlgItem(hDlg, IDC_FILTERBROWSE), false);
			}
			break;

			case IDC_FILTERBROWSE:

			strcpy(szFileFilter, "*.flt");

			ofn.lStructSize = sizeof(OPENFILENAME);
			ofn.hwndOwner = hDlg;
			ofn.lpstrFilter = szFilterLog;
			ofn.lpstrCustomFilter = NULL;
			ofn.nMaxCustFilter = 0L;
			ofn.nFilterIndex = 1;
			ofn.lpstrFile = szFileFilter;
			ofn.nMaxFile = sizeof(szFileFilter);
			ofn.lpstrFileTitle = szFileTitle;
			ofn.nMaxFileTitle = sizeof(szFileTitle);
			ofn.lpstrInitialDir = Profile.LogfilePath;
			ofn.lpstrTitle = "Open Filter File...";
			ofn.Flags = OFN_HIDEREADONLY;
			ofn.nFileOffset = 0;
			ofn.nFileExtension = 0;
			ofn.lpstrDefExt = szFileFilterExt;

			if (GetOpenFileName(&ofn))
			{
				SendDlgItemMessage(hDlg, IDC_FILTERFILE, WM_SETTEXT, 0, (LPARAM) szFileFilter);
			}
			SetFocus(GetDlgItem(hDlg, IDOK));

			break;

			case IDC_FILTERFILEDATE:

			state = IsDlgButtonChecked(hDlg, IDC_FILTERFILEDATE);

			EnableWindow(GetDlgItem(hDlg, IDC_FILTERFILE),   !state);
			EnableWindow(GetDlgItem(hDlg, IDC_FILTERBROWSE), !state);

			SetDlgItemText(hDlg, IDC_FILTERFILE, state ? szFilenameDate : "");

			break;

			case IDC_FILTERCMDEN:

			state = IsDlgButtonChecked(hDlg, IDC_FILTERCMDEN);

			if (state)
			{
				strcpy(tmp_message, "Syntax :\n\n");
				strcat(tmp_message, "%1 : Capcode\n");
				strcat(tmp_message, "%2 : Time\n");
				strcat(tmp_message, "%3 : Date\n");
				strcat(tmp_message, "%4 : Mode\n");
				strcat(tmp_message, "%5 : Type\n");
				strcat(tmp_message, "%6 : Bitrate\n");
				strcat(tmp_message, "%7 : Message\n");
				strcat(tmp_message, "%8 : Label");

				MessageBox(hDlg, tmp_message, "PDW Command file arguments", MB_ICONINFORMATION);
			}

			EnableWindow(GetDlgItem(hDlg, IDC_FILTERCMDBROWSE), state);
			EnableWindow(GetDlgItem(hDlg, IDC_FILTERCMDFILE),   state);

			if (state) SetFocus(GetDlgItem(hDlg, IDC_FILTERCMDFILE));
			else   EnableWindow(GetDlgItem(hDlg, IDC_FILTERCMDARGS), false);

			break;

			case IDC_FILTERCMDBROWSE:

			strcpy(szFileCmd, Profile.filter_cmd[0] ? Profile.filter_cmd : "*.exe");

			ofn.lStructSize = sizeof(OPENFILENAME);
			ofn.hwndOwner = hDlg;
			ofn.lpstrFilter = szFilterCmd;
			ofn.lpstrCustomFilter = NULL;
			ofn.nMaxCustFilter = 0L;
			ofn.nFilterIndex = 1;
			ofn.lpstrFile = szFileCmd;
			ofn.nMaxFile = sizeof(szFileCmd);
			ofn.lpstrFileTitle = szFileTitle;
			ofn.nMaxFileTitle = sizeof(szFileTitle);
			ofn.lpstrInitialDir = szPath;
			ofn.lpstrTitle = "Select Command File...";
			ofn.Flags = OFN_HIDEREADONLY;
			ofn.nFileOffset = 0;
			ofn.nFileExtension = 0;
			ofn.lpstrDefExt = NULL;

			if (GetOpenFileName(&ofn))
			{
				SetDlgItemText(hDlg, IDC_FILTERCMDFILE, szFileCmd);
			}
			SetFocus(GetDlgItem(hDlg, IDOK));

			break;

			case IDC_FILTERBEEP:

			SendMessage(hDlg, WM_WININICHANGE, NULL, 0L);

			break;

			case IDC_FILTERCMDFILE:

				GetDlgItemText(hDlg, IDC_FILTERCMDFILE, temp, MAX_FILE_LEN+1);

				EnableWindow(GetDlgItem(hDlg, IDC_FILTERCMDARGS), strlen(temp));

			break;

			case IDOK:

			GetDlgItemText(hDlg, IDC_FILTERFILE, szFileFilter, FILTER_FILE_LEN+1);

			state = IsDlgButtonChecked(hDlg, IDC_FILTERFILEEN);
			date_state = IsDlgButtonChecked(hDlg, IDC_FILTERFILEDATE);

			if (state && !date_state)
			{
				if (!szFileFilter[0])
				{
					MessageBox(hDlg,"You haven't entered a file name!","PDW Filterfile",MB_ICONERROR);
					SetFocus(GetDlgItem(hDlg, IDC_FILTERFILE));
					return (FALSE);
				}
				else
				{
					if (Need_Ext(szFileFilter)) strcat(szFileFilter,".flt");
					if (!FileExists(szLogPathName)) CreateDirectory(szLogPathName, NULL);
				}

				if (strcmp(szFileFilter, Profile.logfile) == 0)
				{
					sprintf(tmp_message, "Filterfile can't be same as Logfile!\n( %s )", szFileFilter);
					MessageBox(hDlg, tmp_message, "PDW Filterfile",MB_ICONERROR);
					SendDlgItemMessage(hDlg,  IDC_FILTERFILE, EM_SETSEL, 0, -1);
					SetFocus(GetDlgItem(hDlg, IDC_FILTERFILE));
					return (FALSE);
				}

				if (strstr(szFileFilter, "\\") != 0)
				{
					if (szFileFilter[1] == ':')
					{
						strcpy(temp_filename, szFileFilter); // Full path
					}
					else sprintf(temp_filename, "%s\\%s", szPath, szFileFilter);
				}
				else sprintf(temp_filename, "%s\\%s", szLogPathName, szFileFilter);

				if ((pFileFilter = fopen(temp_filename, "a")) == NULL)
				{
					sprintf(tmp_message, "Error opening filterfile!\n( %s )", szFileFilter);
					MessageBox(hDlg, tmp_message, "PDW Filterfile",MB_ICONERROR);
					SetFocus(GetDlgItem(hDlg, IDC_FILTERFILE));
					return (FALSE);
				}
				else
				{
					strcpy(Profile.filterfile, szFileFilter);
					fclose(pFileFilter);
				}
			}
			else strcpy(Profile.filterfile, "");

			strcpy(Profile.ColFilterfile, "");

			if (IsDlgButtonChecked(hDlg, IDC_FILTERLOGCOLUMN+1)) strcat(Profile.ColFilterfile, "1");
			if (IsDlgButtonChecked(hDlg, IDC_FILTERLOGCOLUMN+2)) strcat(Profile.ColFilterfile, "2");
			if (IsDlgButtonChecked(hDlg, IDC_FILTERLOGCOLUMN+3)) strcat(Profile.ColFilterfile, "3");
			if (IsDlgButtonChecked(hDlg, IDC_FILTERLOGCOLUMN+4)) strcat(Profile.ColFilterfile, "4");
			if (IsDlgButtonChecked(hDlg, IDC_FILTERLOGCOLUMN+5)) strcat(Profile.ColFilterfile, "5");
			if (IsDlgButtonChecked(hDlg, IDC_FILTERLOGCOLUMN+6)) strcat(Profile.ColFilterfile, "6");
			if (IsDlgButtonChecked(hDlg, IDC_FILTERLOGCOLUMN+7)) strcat(Profile.ColFilterfile, "7");

			Profile.filterfile_enabled  = state;
			Profile.filterfile_use_date = date_state;

			Profile.showtone		   = IsDlgButtonChecked(hDlg, IDC_FILTERTONE);		 // Show Tone-Only
			Profile.shownumeric		   = IsDlgButtonChecked(hDlg, IDC_FILTERNUMERIC);	 // Show Numeric
			Profile.showmisc		   = IsDlgButtonChecked(hDlg, IDC_FILTERBINARYHEX); // Show FLEX Binary/Hex data
			Profile.filterbeep		   = IsDlgButtonChecked(hDlg, IDC_FILTERBEEP);		 // Enable filterbeep
			Profile.filterwindowonly   = IsDlgButtonChecked(hDlg, IDC_FILTERWINDOWONLY);// Show filtered messages only in filterwindow
			Profile.FilterWindowColors = IsDlgButtonChecked(hDlg, IDC_FILTERSCOLORS);	// Show label colors in filterwindow
			Profile.FilterWindowExtra  = IsDlgButtonChecked(hDlg, IDC_FILTERSEXTRA);	// Show CMD/DESC/SEP/etc in filterwindow

			// Command file stuff
			GetDlgItemText(hDlg, IDC_FILTERCMDFILE, szFileCmd, MAX_FILE_LEN+1);

			cmd_state = IsDlgButtonChecked(hDlg, IDC_FILTERCMDEN);

			if (cmd_state && !szFileCmd[0])
			{
				MessageBox(hDlg,"You must enter a command file!","PDW Filter command file",MB_ICONERROR);
				SetFocus(GetDlgItem(hDlg, IDC_FILTERCMDFILE));
				return (FALSE);
			}

			// Enable filter command file?
			Profile.filter_cmd_file_enabled = cmd_state;
			strcpy(Profile.filter_cmd, szFileCmd);

			// Get filter Command args
			GetDlgItemText(hDlg, IDC_FILTERCMDARGS, szCmdArgs, MAX_FILE_LEN+1);

			strcpy(Profile.filter_cmd_args, szCmdArgs);

			Profile.LabelLog = IsDlgButtonChecked(hDlg, IDC_LABELLOG);

			Profile.LabelNewline = IsDlgButtonChecked(hDlg, IDC_LABELNEWLINE);
			Profile.filter_default_type = SendDlgItemMessage(hDlg, IDC_FILTERDEFTYPE, CB_GETCURSEL, 0, 0L);

			WriteSettings();

			EndDialog(hDlg, TRUE);

			return (TRUE);

			case IDCANCEL:

			EndDialog(hDlg, TRUE);

			return (TRUE);
		}
		break;
	}
	return (FALSE);
}	// end of FilterOptionsDlgProc()


BOOL FAR PASCAL FilterEditDlgProc(HWND hDlg, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	static char szFilterLog[] = {"Text File (*.txt)\0*.txt\0All Files (*.*)\0*.*\0\0"};

	static char *types[6] = { "FLEX Capcode", "POCSAG Riccode", "TEXT",
							  "ERMES Address","ACARS Reg.no", "Mobitex MAN" };

	static char *label_colors[18] = { "Default",   "Yellow",   "Red",     "Orange",    "Light Blue",
									  "Cyan",      "White",    "Green",   "Gray",      "Brown",
									  "Light Cyan","Blue",     "Magenta", "Sea Green", "Pink",
									  "Ice Blue",  "Turqoise", "Don't Change"};

	static int capcode_len[7] = { 0, Profile.FlexGroupMode ? 7 : 9, 7, 0, 7, 6, 7 };

	int i, index=-1, str_len, pos, idcControl=IDC_SEPFILTERFILE1;
	int capcode=0, text=0, captxt=0, label=0;

	int sepfile[3] = { 0, 0, 0 };

	static int offset=0, sep_filterfiles=0, initializing=1, focus, lastfocus;

	static bool bApplying=false, bFilterJump=false;

	int type=0, monitor_only=0, reject=0, match_exact=0, label_en=0, smtp=0;
	int filter_cmd=0, color=0, audio=0, iHits=0;
	int sep_en=0, sep1=0, sep2=0, sep3=0;
	
	char temp_cap[FILTER_CAPCODE_LEN+1]="",
		 temp[MAX_PATH],
		 tmp_text[FILTER_TEXT_LEN+1],
		 tmp_sepfile[3][MAX_PATH];

	char szFileFilter[MAX_PATH]="",
		 szFilename[MAX_PATH]="",
		 szPathname[MAX_PATH]="",
		 szFileTitle[256],
		 szFileFilterExt[5]="txt",
		 szFilterEditHelpText[128];

	FILE *pSepFile = NULL;

	OPENFILENAME ofn = {0};

	int multiple_edit = ListView_GetSelectedCount(hListView) > 1;

	RECT rect ;
	HBRUSH hb ;

	LOGFONT font_labelcolor;

	font_labelcolor.lfHeight			= -11;
	font_labelcolor.lfWidth				= 0;
	font_labelcolor.lfEscapement		= 0;
	font_labelcolor.lfOrientation		= 0;
	font_labelcolor.lfWeight			= Profile.fontInfo.lfWeight;
	font_labelcolor.lfItalic			= Profile.fontInfo.lfItalic;
	font_labelcolor.lfUnderline			= 0;
	font_labelcolor.lfStrikeOut			= 0;
	font_labelcolor.lfCharSet			= OEM_CHARSET;
	font_labelcolor.lfOutPrecision		= OUT_STROKE_PRECIS;
	font_labelcolor.lfClipPrecision		= CLIP_DEFAULT_PRECIS;
	font_labelcolor.lfQuality			= DEFAULT_QUALITY;
	font_labelcolor.lfPitchAndFamily	= FIXED_PITCH | FF_MODERN;
	lstrcpy(font_labelcolor.lfFaceName, Profile.fontInfo.lfFaceName);

	static int x_screen = GetSystemMetrics(SM_CXMAXIMIZED),
			   y_screen = GetSystemMetrics(SM_CYMAXIMIZED);

	int x_box=8, y_box=280, box_width=400, box_height=55;

	int button_width = 66, button_height = 23;

	GetClientRect(hDlg, &rect) ;

	static int x_window = rect.right + 6,
			   y_window = rect.bottom + 25,
			   window_left = (x_screen/2 - x_window/2)+10,
			   window_top  =  y_screen/2 - y_window/2,
			   button_x_right  = rect.right,
			   button_x_middle = rect.right/2;

	focus = GetDlgCtrlID(GetFocus());

	if (focus != lastfocus)
	{
		switch(focus)
		{
			case IDC_FILTERTYPE :

			strcpy(szFilterEditHelpText, " Select the type of filter, depending on the type of network");

			break;

			case IDC_FILTERCAPCODE :

			strcpy(szFilterEditHelpText, " Enter the capcode/riccode to filter. Use ? as wildcard.");

			break;

			case IDC_FILTERFNU :

			strcpy(szFilterEditHelpText, " Whether a specific function number has to be filtered.");

			break;

			case IDC_FILTERTEXT :

			strcpy(szFilterEditHelpText, " Text to filter. Use & for multiple texts, like 'ambulance&urgent'");

			break;

			case IDC_FILTERRXTXMAN :

			strcpy(szFilterEditHelpText, " Whether only RX- or TX-MAN will be filtered, or both");

			break;

			case IDC_FILTERMATCHEXACT :

			strcpy(szFilterEditHelpText, " Whether the complete message must match the filter text");

			break;

			case IDC_FILTERAUDIO :

			strcpy(szFilterEditHelpText, " The type of sound to be played when this filter is triggered");

			break;

			case IDC_FILTERLABEL :

			strcpy(szFilterEditHelpText, " The label (description) which will be shown below the filtered message");

			break;

			case IDC_FILTERLABELEN :

			strcpy(szFilterEditHelpText, " Whether the filter label will be shown below the filtered message");

			break;

			case IDC_FILTERLABELCOLOR :

			strcpy(szFilterEditHelpText, " The filter label's color");

			break;

			case IDC_FILTERCMD :

			strcpy(szFilterEditHelpText, " Whether the (external) command file will be executed");

			break;

			case IDC_FILTERSMTP :

			strcpy(szFilterEditHelpText, " Whether PDW has to send an email when this filter is triggered");

			break;

			case IDC_FILTERREJECT :

			strcpy(szFilterEditHelpText, " Whether messages matching this filter will be ignored");

			break;

			case IDC_SEPFILTERFILEEN :

			strcpy(szFilterEditHelpText, " Whether you want PDW to log messages to a separate filter file");

			break;

			case IDC_FILTER_MONITOR_ONLY :

			strcpy(szFilterEditHelpText, " Show filtered messages only in the upper (monitor) window");

			break;

			case IDC_FILTERRESET :

			strcpy(szFilterEditHelpText, " Reset filter's hitcounter");

			break;

			case IDC_SEPFILTERFILEBROWSE1:
			case IDC_SEPFILTERFILEBROWSE2:
			case IDC_SEPFILTERFILEBROWSE3:

			GetDlgItemText(hDlg, focus, temp, 10);

			if (strcmp(temp, "Browse") == 0)
			{
				strcpy(szFilterEditHelpText, " Select a separate filter file");
			}
			else
			{
				strcpy(szFilterEditHelpText, " Delete current separate filter file");
			}
			break;

			case IDOK :

			strcpy(szFilterEditHelpText, " Save changes and close window");

			break;

			case IDC_FILTER_PREVIOUS :

			strcpy(szFilterEditHelpText, " Jump to previous filter");

			break;

			case IDC_FILTER_APPLY :

			strcpy(szFilterEditHelpText, " Save changes but don't close window");

			break;

			case IDC_FILTER_NEXT :

			strcpy(szFilterEditHelpText, " Jump to next filter");

			break;

			case IDCANCEL :

			strcpy(szFilterEditHelpText, " Close window, don't save changes");

			break;

			default :

			strcpy(szFilterEditHelpText, "");

			break;
		}
		lastfocus=focus;

		SetDlgItemText(hDlg, IDC_FILTEREDITHELP, szFilterEditHelpText);
	}

	switch (uMsg)
	{
		case WM_DRAWITEM:
		{
			LPDRAWITEMSTRUCT lpdis = (LPDRAWITEMSTRUCT) lParam;		// item-drawing information 
			UINT idCtl = (UINT) wParam;								// control identifier 
			RECT rect  = lpdis->rcItem ;

			if (idCtl != IDC_FILTERLABELCOLOR) break;

			if (lpdis->itemAction == ODA_DRAWENTIRE)
			{
				FillRect(lpdis->hDC, &rect, hb = CreateSolidBrush(Profile.color_background)) ;
				DeleteObject(hb) ;
				SetBkColor(lpdis->hDC, Profile.color_background) ;
				SetTextColor(lpdis->hDC, GetColorRGB(COLOR_FILTERLABEL+lpdis->itemID));
				DrawText(lpdis->hDC,label_colors[lpdis->itemID], strlen(label_colors[lpdis->itemID]), &rect, DT_SINGLELINE | DT_CENTER | DT_VCENTER);

				if (lpdis->itemState == ODS_SELECTED)
				{
					DrawFocusRect(lpdis->hDC, &rect) ;
				}
				if (lpdis->itemState == ODS_FOCUS)
				{
					DrawFocusRect(lpdis->hDC, &rect) ;
				}
			}
			if (lpdis->itemAction == ODA_SELECT)
			{
				DrawFocusRect(lpdis->hDC, &rect) ;
			}
			return(TRUE);
		}
		break ;

		case WM_WINDOWPOSCHANGED: // Give focus to the Text edit control

		SetFocus(GetDlgItem(hDlg, (filter.type == TEXT_FILTER) ? IDC_FILTERTEXT : IDC_FILTERCAPCODE));

		break;   

		case WM_INITDIALOG:

		if (!bFilterJump)
		{
			if (!CenterWindow(hDlg)) return (FALSE);

			SendDlgItemMessage(hDlg, IDC_FILTERLABELCOLOR, WM_SETFONT, (WPARAM) CreateFontIndirect(&font_labelcolor), 0);

			for (i=0; i<6; i++)		// show filter types
			{
				SendDlgItemMessage(hDlg, IDC_FILTERTYPE, CB_ADDSTRING, 0, (LPARAM) (LPSTR) types[i]);
			}

			SendMessage(hListView, LVM_SETBKCOLOR, 0, Profile.color_background);

			for (i=0; i< (multiple_edit ? 18 : 17); i++)	// Add/show list of colors.
			{
				SendDlgItemMessage(hDlg, IDC_FILTERLABELCOLOR, CB_ADDSTRING, 0, (LPARAM) (LPSTR) label_colors[i]);
			}
		}
		else
		{
			for (i=0; i<3; i++)
			{
				SetDlgItemText(hDlg, IDC_SEPFILTERFILE1+i, "");
			}
		}

		if (Profile.SMTP)
		{
			EnableWindow(GetDlgItem(hDlg, IDC_FILTERSMTP), true);
			SendDlgItemMessage(hDlg, IDC_FILTERSMTP, WM_SETTEXT, 0, (LPARAM) "Send email");
		}

		if (filter.type == UNUSED_FILTER) // Add Filter
		{
			EnableWindow(GetDlgItem(hDlg, IDC_FILTERTYPE), true);
			filter.type = (FILTER_TYPE) (Profile.filter_default_type+1);
			SendDlgItemMessage(hDlg, IDC_FILTERAUDIO, CB_SETCURSEL, (WPARAM) 1, 0L);
			SendDlgItemMessage(hDlg, IDC_FILTERLABELCOLOR, CB_SETCURSEL, (WPARAM) 0, 0L);

			SetWindowText(hDlg, (LPSTR) "PDW Add Filter");
		}
		else	 // Edit Filter
		{
			SetWindowText(hDlg, (LPSTR) multiple_edit ? "PDW (multiple) Edit Filter" : "PDW Edit Filter");

			if (multiple_edit)	// If more than one filter is selected
			{
				while ((index = ListView_GetNextItem(hListView, index, LVNI_SELECTED)) != CB_ERR)
				{
					if (strcmp(Profile.filters[index].capcode, filter.capcode) != 0) capcode=1;
					if (strcmp(Profile.filters[index].text, filter.text) != 0) text=1;
					if (strcmp(Profile.filters[index].label, filter.label) != 0) label=1;

					for (i=0; i<3; i++)
					{
						if (strcmp(Profile.filters[index].sep_filterfile[i], filter.sep_filterfile[i]) != 0) sepfile[i]=1;
					}

					if (Profile.filters[index].monitor_only != filter.monitor_only) monitor_only=1;
					if (Profile.filters[index].reject != filter.reject) reject=1;
					if (Profile.filters[index].match_exact_msg != filter.match_exact_msg) match_exact=1;
					if (Profile.filters[index].label_enabled != filter.label_enabled) label_en=1;
					if (Profile.filters[index].cmd_enabled != filter.cmd_enabled) filter_cmd=1;
					if (Profile.filters[index].label_color != filter.label_color) color=1;
					if (Profile.filters[index].wave_number != filter.wave_number) audio=1;
					if (Profile.filters[index].smtp != filter.smtp) smtp=1;
					if (Profile.filters[index].sep_filterfile_en != filter.sep_filterfile_en) sep_en=1;
					if (Profile.filters[index].sep_filterfiles > sep_filterfiles) sep_filterfiles = Profile.filters[index].sep_filterfiles;

					iHits += Profile.filters[index].hitcounter;
				}

				if (monitor_only) SendDlgItemMessage(hDlg, IDC_FILTER_MONITOR_ONLY, BM_SETSTYLE, BS_AUTO3STATE, TRUE);
				if (reject)       SendDlgItemMessage(hDlg, IDC_FILTERREJECT,        BM_SETSTYLE, BS_AUTO3STATE, TRUE);
				if (match_exact)  SendDlgItemMessage(hDlg, IDC_FILTERMATCHEXACT,    BM_SETSTYLE, BS_AUTO3STATE, TRUE);
				if (label_en)     SendDlgItemMessage(hDlg, IDC_FILTERLABELEN,       BM_SETSTYLE, BS_AUTO3STATE, TRUE);
				if (filter_cmd)   SendDlgItemMessage(hDlg, IDC_FILTERCMD,           BM_SETSTYLE, BS_AUTO3STATE, TRUE);
				if (sep_en)       SendDlgItemMessage(hDlg, IDC_SEPFILTERFILEEN,     BM_SETSTYLE, BS_AUTO3STATE, TRUE);
				if (smtp)         SendDlgItemMessage(hDlg, IDC_FILTERSMTP,          BM_SETSTYLE, BS_AUTO3STATE, TRUE);

				sprintf(temp, "Total number of hits: %i", iHits);
				SetDlgItemText(hDlg, IDC_FILTERHITS, temp);
				SetDlgItemText(hDlg, IDC_FILTERLASTHIT, "(total of selection)");

				ShowWindow(GetDlgItem(hDlg, IDC_FILTERRESET), SW_HIDE);	// Hide reset button
				ShowWindow(GetDlgItem(hDlg, IDC_DONTCHANGE),  SW_SHOW);	// Show "don't change"

				ShowWindow(GetDlgItem(hDlg, IDC_FILTER_PREVIOUS), SW_HIDE);
				ShowWindow(GetDlgItem(hDlg, IDC_FILTER_APPLY),    SW_HIDE);
				ShowWindow(GetDlgItem(hDlg, IDC_FILTER_NEXT),     SW_HIDE);
			}
			else	// If editing only one filter
			{
				sprintf(temp, "Number of hits: %i", filter.hitcounter);
				SetDlgItemText(hDlg, IDC_FILTERHITS, temp);

				if (filter.hitcounter)	// If filter has been 'hit' at least one time
				{
					sprintf(temp, "Last hit: %s  %s", filter.lasthit_date, filter.lasthit_time);
					SetDlgItemText(hDlg, IDC_FILTERLASTHIT, temp);
					EnableWindow(GetDlgItem(hDlg, IDC_FILTERRESET), true);
				}
				else
				{
					SetDlgItemText(hDlg, IDC_FILTERLASTHIT, "");
				}
			}
		}

		for (i=0; i<3; i++)
		{
			SendDlgItemMessage(hDlg, (IDC_SEPFILTERFILE1+i), EM_LIMITTEXT, FILTER_FILE_LEN,  0L);
			SetDlgItemText(hDlg, IDC_SEPFILTERFILE1+i, sepfile[i] ? "Don't change" : filter.sep_filterfile[i]);
		}
		SendDlgItemMessage(hDlg, IDC_FILTERCAPCODE,  EM_LIMITTEXT, capcode_len[filter.type], 0L);
		SendDlgItemMessage(hDlg, IDC_FILTERLABEL,    EM_LIMITTEXT, FILTER_LABEL_LEN, 0L);
		SendDlgItemMessage(hDlg, IDC_FILTERTEXT,     EM_LIMITTEXT, FILTER_TEXT_LEN,  0L);

		for (i=0; i<capcode_len[filter.type]; i++)
		{
			temp_cap[i] = filter.capcode[i];
		}

		SetDlgItemText(hDlg, IDC_FILTERCAPCODE, capcode ? "Don't change" : temp_cap);
		SetDlgItemText(hDlg, IDC_FILTERTEXT,    text    ? "Don't change" : filter.text);
		SetDlgItemText(hDlg, IDC_FILTERLABEL,   label   ? "Don't change" : filter.label);

		SendDlgItemMessage(hDlg, IDC_FILTERLABELCOLOR, CB_SETCURSEL, (WPARAM) color ? 17 : filter.label_color, 0L);

		CheckDlgButton(hDlg, IDC_FILTER_MONITOR_ONLY, monitor_only ? BST_INDETERMINATE : filter.monitor_only);
		CheckDlgButton(hDlg, IDC_FILTERREJECT,        reject       ? BST_INDETERMINATE : filter.reject);
		CheckDlgButton(hDlg, IDC_FILTERMATCHEXACT,    match_exact  ? BST_INDETERMINATE : filter.match_exact_msg);
		CheckDlgButton(hDlg, IDC_FILTERLABELEN,       label_en     ? BST_INDETERMINATE : filter.label_enabled);
		CheckDlgButton(hDlg, IDC_FILTERCMD,           filter_cmd   ? BST_INDETERMINATE : filter.cmd_enabled);
		CheckDlgButton(hDlg, IDC_FILTERSMTP,          smtp         ? BST_INDETERMINATE : filter.smtp);
		CheckDlgButton(hDlg, IDC_SEPFILTERFILEEN,     sep_en       ? BST_INDETERMINATE : filter.sep_filterfile_en);

		SendDlgItemMessage(hDlg, IDC_FILTERAUDIO, CB_RESETCONTENT, 0, 0);
		SendDlgItemMessage(hDlg, IDC_FILTERAUDIO, CB_ADDSTRING, 0, (LPARAM) (LPSTR) "No sound");

		if (!monitor_only)				// If Monitor-Only != BST_INDETERMINATE
		{
			if (!filter.monitor_only)	// If all selected filters =! Monitor-Only
			{
				SendDlgItemMessage(hDlg, IDC_FILTERAUDIO, CB_ADDSTRING, 0, (LPARAM) (LPSTR) "Default");
				for (i=0; i<10; i++)
				{
					sprintf(temp, "Sound%i.wav", i);
					SendDlgItemMessage(hDlg, IDC_FILTERAUDIO, CB_ADDSTRING, 0, (LPARAM) (LPSTR) temp);
				}
				SendDlgItemMessage(hDlg, IDC_FILTERAUDIO, CB_SETCURSEL, (WPARAM) filter.wave_number+1, 0L);
			}
			else						// If all selected filters == Monitor-Only
			{
				SendDlgItemMessage(hDlg, IDC_FILTERAUDIO, CB_ADDSTRING, 0, (LPARAM)(LPCTSTR) "Monitor_only.wav");
				SendDlgItemMessage(hDlg, IDC_FILTERAUDIO, CB_SETCURSEL, (WPARAM) filter.wave_number, 0L);
			}
			if (multiple_edit && audio)	// If we are editing multiple filters &&
										// audio == BST_INDETERMINATE
			{
				SendDlgItemMessage(hDlg, IDC_FILTERAUDIO, CB_ADDSTRING, 0, (LPARAM)(LPCTSTR) "Don't change");
				SendDlgItemMessage(hDlg, IDC_FILTERAUDIO, CB_SETCURSEL, (WPARAM) filter.monitor_only ? 2 : 12, 0L);
			}
		}
		else EnableWindow(GetDlgItem(hDlg, IDC_FILTERAUDIO), FALSE); // If Monitor-Only == BST_INDETERMINATE

		SendDlgItemMessage(hDlg, IDC_FILTERTYPE, CB_SETCURSEL, (WPARAM) filter.type-1, 0L);

		if (filter.type == MOBITEX_FILTER)
		{
			switch (filter.capcode[strlen(filter.capcode)-2])
			{
				case 'R' :
				CheckDlgButton(hDlg, IDC_FILTERRXTXMAN, BST_UNCHECKED);
				break;

				case 'T' :
				CheckDlgButton(hDlg, IDC_FILTERRXTXMAN, BST_CHECKED);
				break;

				default :
				CheckDlgButton(hDlg, IDC_FILTERRXTXMAN, BST_INDETERMINATE);
				break;
			}
		}
		else if (filter.type == POCSAG_FILTER)
		{
			SendDlgItemMessage(hDlg, IDC_FILTERFNU, CB_ADDSTRING, 0, (LPARAM)(LPCTSTR) "All");
			SendDlgItemMessage(hDlg, IDC_FILTERFNU, CB_ADDSTRING, 0, (LPARAM)(LPCTSTR) "1");
			SendDlgItemMessage(hDlg, IDC_FILTERFNU, CB_ADDSTRING, 0, (LPARAM)(LPCTSTR) "2");
			SendDlgItemMessage(hDlg, IDC_FILTERFNU, CB_ADDSTRING, 0, (LPARAM)(LPCTSTR) "3");
			SendDlgItemMessage(hDlg, IDC_FILTERFNU, CB_ADDSTRING, 0, (LPARAM)(LPCTSTR) "4");

			if (filter.capcode[strlen(filter.capcode)-2] == '-')	// Contains function number
			{
				 SendDlgItemMessage(hDlg, IDC_FILTERFNU, CB_SETCURSEL, (WPARAM) atoi(&filter.capcode[8]), 0L);
			}
			else SendDlgItemMessage(hDlg, IDC_FILTERFNU, CB_SETCURSEL, (WPARAM) 0, 0L);
		}

		initializing=0;
		SendMessage(hDlg, WM_WININICHANGE, initializing, 0L);
	
		index = ListView_GetNextItem(hListView, -1, LVNI_SELECTED);

		if (!bEditFilter)
		{
			ShowWindow(GetDlgItem(hDlg, IDC_FILTER_PREVIOUS), SW_HIDE);	// Hide '-' button
			ShowWindow(GetDlgItem(hDlg, IDC_FILTER_APPLY), SW_HIDE);	// Hide 'Apply' button
			ShowWindow(GetDlgItem(hDlg, IDC_FILTER_NEXT), SW_HIDE);		// Hide '+' button
		}
		else
		{
			EnableWindow(GetDlgItem(hDlg, IDC_FILTER_NEXT), (index == Profile.filters.size()-1) ? false : true);
			EnableWindow(GetDlgItem(hDlg, IDC_FILTER_PREVIOUS), (index == 0) ? false : true);
		}

		SetFocus(GetDlgItem(hDlg, IDC_FILTERCAPCODE));

		return (TRUE);

		case WM_WININICHANGE:

		if (wParam) break;	// Set if initializing dialog

		type		  = (FILTER_TYPE) (SendDlgItemMessage(hDlg, IDC_FILTERTYPE, CB_GETCURSEL, 0, 0L)+1);
		reject		  = IsDlgButtonChecked(hDlg, IDC_FILTERREJECT);
		monitor_only  = IsDlgButtonChecked(hDlg, IDC_FILTER_MONITOR_ONLY);
		multiple_edit = ListView_GetSelectedCount(hListView) > 1;
		capcode		  = SendDlgItemMessage(hDlg, IDC_FILTERCAPCODE, WM_GETTEXTLENGTH, 0, 0);
		text		  = SendDlgItemMessage(hDlg, IDC_FILTERTEXT,    WM_GETTEXTLENGTH, 0, 0);
		captxt		  = capcode || text;
		label		  = SendDlgItemMessage(hDlg, IDC_FILTERLABEL,   WM_GETTEXTLENGTH, 0, 0);
		sep_en		  = IsDlgButtonChecked(hDlg, IDC_SEPFILTERFILEEN);
		sep1		  = SendDlgItemMessage(hDlg, IDC_SEPFILTERFILE1,WM_GETTEXTLENGTH, 0, 0);
		sep2		  = SendDlgItemMessage(hDlg, IDC_SEPFILTERFILE2,WM_GETTEXTLENGTH, 0, 0);
		sep3		  = SendDlgItemMessage(hDlg, IDC_SEPFILTERFILE3,WM_GETTEXTLENGTH, 0, 0);

		SendDlgItemMessage(hDlg, IDC_FILTERCAPCODE, EM_LIMITTEXT, capcode_len[filter.type], 0L);

		GetDlgItemText(hDlg, IDC_FILTERTEXT, temp, FILTER_TEXT_LEN+1);

		EnableWindow(GetDlgItem(hDlg, IDC_FILTERREJECT), captxt);
		EnableWindow(GetDlgItem(hDlg, IDC_FILTER_MONITOR_ONLY), captxt);

		if (type != MOBITEX_FILTER)
		{
			EnableWindow(GetDlgItem(hDlg, IDC_FILTERMATCHEXACT), (text && temp[0] != '^'));

			if (type == POCSAG_FILTER)	// Monitoring POCSAG? Show function number combo box
			{
				ShowWindow(GetDlgItem(hDlg,   IDC_FILTERFNU), SW_SHOW);
				SetWindowPos(GetDlgItem(hDlg, IDC_FILTERCAPCODE), NULL, 0, 0, 75, 21, SWP_NOMOVE);
				EnableWindow(GetDlgItem(hDlg, IDC_FILTERFNU), capcode);
		}
			else
		{
				ShowWindow(GetDlgItem(hDlg,   IDC_FILTERFNU), SW_HIDE);
				SetWindowPos(GetDlgItem(hDlg, IDC_FILTERCAPCODE), NULL, 0, 0, 125, 21, SWP_NOMOVE);
			}
		}
		else	// If Mobitex filter
		{
			EnableWindow(GetDlgItem(hDlg, IDC_FILTERRXTXMAN), capcode);

			switch (IsDlgButtonChecked(hDlg, IDC_FILTERRXTXMAN))
			{
				case BST_UNCHECKED :
				SetDlgItemText(hDlg, IDC_FILTERRXTXMAN, "Only Receiving MAN");
				break;

				case BST_CHECKED:
				SetDlgItemText(hDlg, IDC_FILTERRXTXMAN, "Only Sending MAN");
				break;

				case BST_INDETERMINATE:
				default :
				SetDlgItemText(hDlg, IDC_FILTERRXTXMAN, "Both RX and TX MAN");
				break;
			}
		}
		ShowWindow(GetDlgItem(hDlg, IDC_FILTERRXTXMAN),    (type == MOBITEX_FILTER) ? SW_SHOW : SW_HIDE);	// Show/hide RX/TX Address
		ShowWindow(GetDlgItem(hDlg, IDC_FILTERMATCHEXACT), (type == MOBITEX_FILTER) ? SW_HIDE : SW_SHOW);	// Show/hide Match Exact Text

		if (filter.type != TEXT_FILTER)
		{
			if (!multiple_edit) EnableWindow(GetDlgItem(hDlg, IDC_FILTERCAPCODE), true);
			EnableWindow(GetDlgItem(hDlg, IDC_FILTERTEXT), capcode);
		}
		else	// if TEXT filter
		{
			EnableWindow(GetDlgItem(hDlg, IDC_FILTERCAPCODE), false);
			EnableWindow(GetDlgItem(hDlg, IDC_FILTERTEXT),    true);
		}

		if (IsDlgButtonChecked(hDlg, IDC_FILTER_MONITOR_ONLY) != BST_INDETERMINATE)
		{
			 EnableWindow(GetDlgItem(hDlg, IDC_FILTERAUDIO), (!reject && captxt));
		}
		else EnableWindow(GetDlgItem(hDlg, IDC_FILTERAUDIO), false);

		EnableWindow(GetDlgItem(hDlg, IDC_FILTERLABEL),      (!reject && captxt));
		EnableWindow(GetDlgItem(hDlg, IDC_FILTERLABELEN),    (!reject && label));
		EnableWindow(GetDlgItem(hDlg, IDC_FILTERLABELCOLOR), (!reject && label));
		EnableWindow(GetDlgItem(hDlg, IDC_SEPFILTERFILEEN),  (!reject && captxt));

		EnableWindow(GetDlgItem(hDlg, IDC_SEPFILTERFILEBROWSE1), sep_en ? true    : false);
		ShowWindow  (GetDlgItem(hDlg, IDC_SEPFILTERFILEBROWSE2), sep1   ? SW_SHOW : SW_HIDE);
		ShowWindow  (GetDlgItem(hDlg, IDC_SEPFILTERFILEBROWSE3), sep2   ? SW_SHOW : SW_HIDE);

		SetDlgItemText(hDlg, IDC_SEPFILTERFILEBROWSE1, sep1 ? "Clear":"Browse");
		SetDlgItemText(hDlg, IDC_SEPFILTERFILEBROWSE2, sep2 ? "Clear":"Browse");
		SetDlgItemText(hDlg, IDC_SEPFILTERFILEBROWSE3, sep3 ? "Clear":"Browse");

		for (i=0; i<2; i++)
		{
			if  (SendDlgItemMessage(hDlg, IDC_SEPFILTERFILE3-i, WM_GETTEXTLENGTH, 0, 0) &&
				!SendDlgItemMessage(hDlg, IDC_SEPFILTERFILE2-i, WM_GETTEXTLENGTH, 0, 0))
			{
				GetDlgItemText(hDlg, IDC_SEPFILTERFILE3-i, temp, FILTER_FILE_LEN+1);
				SetDlgItemText(hDlg, IDC_SEPFILTERFILE2-i, temp);
				SetDlgItemText(hDlg, IDC_SEPFILTERFILE3-i, "");
			}
		}
		sep_filterfiles = (sep1!=0)+(sep2!=0)+(sep3!=0); // Number of sepfiles

		EnableWindow(GetDlgItem(hDlg, IDC_FILTERCMD),  multiple_edit ? true : (captxt && !reject));
		EnableWindow(GetDlgItem(hDlg, IDC_FILTERSMTP), captxt ? true : false);

		return (TRUE);

		case WM_COMMAND:

		switch (LOWORD(wParam))
		{
			case IDC_FILTERTYPE:

			if (HIWORD(wParam) == CBN_SELENDOK)
			{
				filter.type = (FILTER_TYPE) (SendDlgItemMessage(hDlg, IDC_FILTERTYPE, CB_GETCURSEL, 0, 0L)+1);

				if (filter.type == TEXT_FILTER)  // if TEXT filter
				{
					SetDlgItemText(hDlg, IDC_FILTERLABEL, filter.label);
					SetDlgItemText(hDlg, IDC_FILTERTEXT, filter.text);
				}
				else if (filter.type == POCSAG_FILTER)  // if POCSAG filter
				{
					SendDlgItemMessage(hDlg, IDC_FILTERFNU, CB_RESETCONTENT, 0, 0);
					SendDlgItemMessage(hDlg, IDC_FILTERFNU, CB_ADDSTRING, 0, (LPARAM)(LPCTSTR) "All");
					SendDlgItemMessage(hDlg, IDC_FILTERFNU, CB_ADDSTRING, 0, (LPARAM)(LPCTSTR) "1");
					SendDlgItemMessage(hDlg, IDC_FILTERFNU, CB_ADDSTRING, 0, (LPARAM)(LPCTSTR) "2");
					SendDlgItemMessage(hDlg, IDC_FILTERFNU, CB_ADDSTRING, 0, (LPARAM)(LPCTSTR) "3");
					SendDlgItemMessage(hDlg, IDC_FILTERFNU, CB_ADDSTRING, 0, (LPARAM)(LPCTSTR) "4");
					SendDlgItemMessage(hDlg, IDC_FILTERFNU, CB_SETCURSEL, (WPARAM) 0, 0L);
				}
				SendMessage(hDlg, WM_WININICHANGE, initializing, 0L);
			}
			break;

			case IDC_FILTERREJECT:

			SetDlgItemText(hDlg, IDC_SEPFILTERFILE1, "");

			if (!multiple_edit)
			{
				if (IsDlgButtonChecked(hDlg, IDC_FILTERREJECT))
				{
					CheckDlgButton(hDlg, IDC_FILTER_MONITOR_ONLY,  BST_UNCHECKED);
					CheckDlgButton(hDlg, IDC_SEPFILTERFILEEN,      BST_UNCHECKED);
					SendDlgItemMessage(hDlg, IDC_FILTERAUDIO,      WM_CLEAR, (WPARAM) 0, 0L);
					SendDlgItemMessage(hDlg, IDC_FILTERLABELCOLOR, WM_CLEAR, (WPARAM) 0, 0L);
				}
				else	// if not rejecting
				{
					SendDlgItemMessage(hDlg, IDC_FILTERLABELCOLOR, CB_SETCURSEL, (WPARAM) 0, 0L);
				}
				SendMessage(hDlg, WM_WININICHANGE, initializing, 0L);
			}

			break;

			case IDC_FILTERRESET:

			filter.hitcounter = 0 ;
			filter.lasthit_date[0] = '\0' ;
			filter.lasthit_time[0] = '\0' ;

			SetDlgItemText(hDlg, IDC_FILTERHITS,    "Number of hits: 0");
			SetDlgItemText(hDlg, IDC_FILTERLASTHIT, "");
			EnableWindow(GetDlgItem(hDlg, IDC_FILTERRESET), false);

			SetFocus(GetDlgItem(hDlg, IDOK));

			break;

			case IDC_FILTER_MONITOR_ONLY:

			SendDlgItemMessage(hDlg, IDC_FILTERAUDIO, CB_RESETCONTENT, 0, 0);

			monitor_only = (IsDlgButtonChecked(hDlg, IDC_FILTER_MONITOR_ONLY) & BST_CHECKED);

			if (monitor_only)
			{
				CheckDlgButton(hDlg, IDC_FILTERREJECT, BST_UNCHECKED);

				SendDlgItemMessage(hDlg, IDC_FILTERAUDIO, CB_ADDSTRING, 0, (LPARAM)(LPCTSTR) "No sound");
				SendDlgItemMessage(hDlg, IDC_FILTERAUDIO, CB_ADDSTRING, 0, (LPARAM)(LPCTSTR) "Monitor_only.wav");
			}
			else // if !monitor_only
			{
				SendDlgItemMessage(hDlg, IDC_FILTERAUDIO, CB_ADDSTRING, 0, (LPARAM) (LPSTR) "No sound");
				SendDlgItemMessage(hDlg, IDC_FILTERAUDIO, CB_ADDSTRING, 0, (LPARAM) (LPSTR) "Default");

				for (i=0; i<11; i++)
				{
					sprintf(temp, "Sound%i.wav", i);
					SendDlgItemMessage(hDlg, IDC_FILTERAUDIO, CB_ADDSTRING, 0, (LPARAM) (LPSTR) temp);
				}
			}
			if (multiple_edit)
			{
				SendDlgItemMessage(hDlg, IDC_FILTERAUDIO, CB_ADDSTRING, 0, (LPARAM)(LPCTSTR) "Don't change");
				SendDlgItemMessage(hDlg, IDC_FILTERAUDIO, CB_SETCURSEL, (WPARAM) monitor_only ? 2 : 12, 0L);
			}
			else SendDlgItemMessage(hDlg, IDC_FILTERAUDIO, CB_SETCURSEL, (WPARAM) 1, 0L);

			SendMessage(hDlg, WM_WININICHANGE, initializing, 0L);

			break;

			case IDC_SEPFILTERFILEEN:

			for (i=0; i<3; i++)
			{
				SetDlgItemText(hDlg, IDC_SEPFILTERFILE1+i, "");
			}
			SendMessage(hDlg, WM_WININICHANGE, initializing, 0L);

			break;

			case IDC_FILTERCAPCODE:
			case IDC_FILTERTEXT:
			case IDC_FILTERLABEL:
			case IDC_FILTERLABELEN:
			case IDC_SEPFILTERFILE1:
			case IDC_SEPFILTERFILE2:
			case IDC_SEPFILTERFILE3:
			case IDC_FILTERRXTXMAN:
			case IDC_FILTERFNU:

			SendMessage(hDlg, WM_WININICHANGE, initializing, 0L);

			break;

			case IDC_SEPFILTERFILEBROWSE3: idcControl += 1;
			case IDC_SEPFILTERFILEBROWSE2: idcControl += 1;
			case IDC_SEPFILTERFILEBROWSE1:

			if (SendDlgItemMessage(hDlg, idcControl, WM_GETTEXTLENGTH, 0, 0))
			{
				SetDlgItemText(hDlg, idcControl, "");
				break;
			}

			strcpy(szFileFilter, "*.txt");

			ofn.lStructSize = sizeof(OPENFILENAME);
			ofn.hwndOwner = hDlg;
			ofn.lpstrFilter = szFilterLog;
			ofn.lpstrCustomFilter = (LPSTR) NULL;
			ofn.nMaxCustFilter = 0L;
			ofn.nFilterIndex = 1;
			ofn.lpstrFile = szFileFilter;
			ofn.nMaxFile = sizeof(szFileFilter);
			ofn.lpstrFileTitle = szFileTitle;
			ofn.nMaxFileTitle = sizeof(szFileTitle);
			ofn.lpstrInitialDir = Profile.LogfilePath;
			ofn.lpstrTitle = "Open Separate Filter File...";
			ofn.Flags = OFN_HIDEREADONLY;
			ofn.nFileOffset = 0;
			ofn.nFileExtension = 0;
			ofn.lpstrDefExt = szFileFilterExt;

			if (GetOpenFileName(&ofn))
			{
				GetFilePath(szFileFilter, szPathname, sizeof(szPathname));

				if (stricmp(Profile.LogfilePath, szPathname) == 0)
				{
					GetFileName(szFileFilter, szFilename, sizeof(szFilename));
					SetDlgItemText(hDlg, idcControl, szFilename);
				}
				else
				{
					SetDlgItemText(hDlg, idcControl, szFileFilter);
				}
			}
			SetFocus(GetDlgItem(hDlg, IDOK));

			break;

			case IDC_FILTER_APPLY:

			bApplying=true;

			case IDOK:

			filter.reject = IsDlgButtonChecked(hDlg, IDC_FILTERREJECT);

			if (filter.type != TEXT_FILTER) // If no TEXT filter
			{
				GetDlgItemText(hDlg, IDC_FILTERCAPCODE, temp_cap, FILTER_CAPCODE_LEN+1);

				if (strncmp(temp_cap, "Don't cha", 9))
				{
					str_len = strlen(temp_cap);

					if (!str_len)
					{
						MessageBox(hDlg,"You must enter an Address!","PDW Filter address",MB_ICONERROR);
						SetFocus(GetDlgItem(hDlg, IDC_FILTERCAPCODE));

						return (FALSE);
					}
					filter.capcode[0] = 0;

					while (str_len < capcode_len[(int)(filter.type)])
					{
						if ((filter.type == ACARS_FILTER) ||
							(filter.type == MOBITEX_FILTER))
						{
							MessageBox(hDlg,"The address is too short!","PDW Filter address",MB_ICONERROR);
							SetFocus(GetDlgItem(hDlg, IDC_FILTERCAPCODE));
							return (FALSE);
						}
						else if (filter.type == FLEX_FILTER)
						{
							if (str_len != 7)
							{
								MessageBox(hDlg,"A FLEX address needs to contain 7 or 9 digits!","PDW Filter address",MB_ICONERROR);
								SetFocus(GetDlgItem(hDlg, IDC_FILTERCAPCODE));
								return (FALSE);
							}
							else break;
						}
						else	// If ERMES/POCSAG
						{
							strcat(filter.capcode, "0");
							str_len++;
						}
					}
					strcat(filter.capcode, temp_cap);

					// check capcode for valid characters
					if (filter.type == ACARS_FILTER)
					{
						for (i=0; i<str_len; i++)
						{
							filter.capcode[i] = toupper(filter.capcode[i]);
						}
						
						for (i=0; i<str_len; i++)
						{
							if (((filter.capcode[i] <  'A') || (filter.capcode[i] >  'Z')) &&
								 (filter.capcode[i] != '-') && (filter.capcode[i] != '?'))
							break;
						}
					}
					else	// If ERMES/POCSAG/FLEX
					{
						for (i=0; i<str_len; i++)
						{
							if (((filter.capcode[i] <  '0') ||
								 (filter.capcode[i] >  '9')) &&
								 (filter.capcode[i] != '?'))
							break;
						}
					}

					if (i < str_len)
					{
						MessageBox(hDlg, "Invalid character(s) in Address!", "PDW Filter address", MB_ICONERROR);
						SendDlgItemMessage(hDlg, IDC_FILTERCAPCODE, EM_SETSEL, i, i+1);
						SetFocus(GetDlgItem(hDlg, IDC_FILTERCAPCODE));
						return (FALSE);
					}
					if (filter.type == MOBITEX_FILTER)
					{
						if (IsDlgButtonChecked(hDlg, IDC_FILTERRXTXMAN) == BST_CHECKED)
						{
							strcat(filter.capcode, "TX");
						}
						else if (IsDlgButtonChecked(hDlg, IDC_FILTERRXTXMAN) == BST_UNCHECKED)
						{
							strcat(filter.capcode, "RX");
						}
					}
					else if (filter.type == POCSAG_FILTER)
					{
						if (SendDlgItemMessage(hDlg, IDC_FILTERFNU, CB_GETCURSEL, 0, 0L))
						{
							sprintf(temp, "-%i", SendDlgItemMessage(hDlg, IDC_FILTERFNU, CB_GETCURSEL, 0, 0L));
							strcat(filter.capcode, temp);
						}
					}
					else if (filter.type == FLEX_FILTER)
					{
						// Convert 9-digit short addresses to 7-digits
						if ((strlen(filter.capcode) == 9) && memcmp(filter.capcode, "00", 2) == 0)
						{
							if (CompareCapcodes(filter.capcode, "002101249") < 0) memmove(filter.capcode, &filter.capcode[2], strlen(filter.capcode));
//							if (atoi(filter.capcode) < 2101249) memmove(filter.capcode, &filter.capcode[2], strlen(filter.capcode));
						}
					}
				}
			}

			GetDlgItemText(hDlg, IDC_FILTERTEXT, filter.text, FILTER_TEXT_LEN+1);

			if (filter.text[0] && strcmp(filter.text, "Don't change"))
			{
				if (strstr(filter.text, "\"") != 0)
				{
					MessageBox(hDlg,"The character \" is not allowed here!","PDW Filter Text", MB_ICONERROR);
					SetFocus(GetDlgItem(hDlg, IDC_FILTERTEXT));
					pos = strchr(filter.text, '"') - filter.text;
					SendDlgItemMessage(hDlg, IDC_FILTERTEXT, EM_SETSEL, pos, pos+1);
					return (FALSE);
				}

				if ((strchr(filter.text, '^') - filter.text) > 0)
				{
					MessageBox(hDlg, "The character ^ can only be used\nat the beginning of the line","PDW Filter Text", MB_ICONERROR);
					SetFocus(GetDlgItem(hDlg, IDC_FILTERTEXT));
					pos = strchr(filter.text, '^') - filter.text;
					SendDlgItemMessage(hDlg, IDC_FILTERTEXT, EM_SETSEL, pos, pos+1);
					return (FALSE);
				}
				filter.match_exact_msg = IsDlgButtonChecked(hDlg, IDC_FILTERMATCHEXACT);
			}
			else if (filter.type == TEXT_FILTER && (strcmp(filter.text, "Don't change")))
			{
				MessageBox(hDlg, "You must enter a text string!", "PDW Filter Text", MB_ICONERROR);
				SetFocus(GetDlgItem(hDlg, IDC_FILTERTEXT));
				return (FALSE);
			}
			GetDlgItemText(hDlg, IDC_FILTERTEXT, tmp_text, FILTER_TEXT_LEN+1);

			filter.label_enabled = IsDlgButtonChecked(hDlg, IDC_FILTERLABELEN);

			GetDlgItemText(hDlg, IDC_FILTERLABEL, filter.label, FILTER_LABEL_LEN+1);

			if (strcmp(filter.label, "Don't change"))
			{
				if (!filter.label[0] && filter.label_enabled)
				{
					filter.label_enabled = 0;
				}
				else if (strstr(filter.label, "\""))
				{
					MessageBox(hDlg,"The character \" is not allowed here!","PDW Filter Label",MB_ICONERROR);
					SetFocus(GetDlgItem(hDlg, IDC_FILTERLABEL));
					pos = strchr(filter.label, '"') - filter.label;
					SendDlgItemMessage(hDlg, IDC_FILTERLABEL, EM_SETSEL, pos, pos+1);
					return (FALSE);
				}
			}

			if (IsDlgButtonChecked(hDlg, IDC_SEPFILTERFILEEN) == BST_CHECKED)
			{
				if (!SendDlgItemMessage(hDlg, IDC_SEPFILTERFILE1, WM_GETTEXTLENGTH, 0, 0) &&
					!SendDlgItemMessage(hDlg, IDC_SEPFILTERFILE2, WM_GETTEXTLENGTH, 0, 0) &&
					!SendDlgItemMessage(hDlg, IDC_SEPFILTERFILE3, WM_GETTEXTLENGTH, 0, 0))
				{
					MessageBox(hDlg,"If you enable the separate filter file\nyou must enter at least one filename!","PDW Separate filterfile",MB_ICONERROR);
					return (FALSE);
				}

				for (int j=0; j<3; j++)
				{
					if (j >= sep_filterfiles)
					{
						strcpy(tmp_sepfile[j], "");
						continue;
					}

					GetDlgItemText(hDlg, IDC_SEPFILTERFILE1+j, szFileFilter, FILTER_FILE_LEN+1);

					if (strstr(szFileFilter, ";") != 0)
					{
						MessageBox(hDlg,"The character ; is not allowed here!","PDW Separate filterfile",MB_ICONERROR);
						SetFocus(GetDlgItem(hDlg, (IDC_SEPFILTERFILE1+j)));
						pos = strchr(szFileFilter, ';') - szFileFilter;
						SendDlgItemMessage(hDlg, (IDC_SEPFILTERFILE1+j), EM_SETSEL, pos, pos+1);
						return (FALSE);
					}
					else
					{
						strcpy(szFilenameDate, "");

						if (strstr(szFileFilter, "\\") != 0)
						{
							if (szFileFilter[1] == ':')
							{
								strcpy(szFilenameDate, szFileFilter); // Full path
							}
							else sprintf(szFilenameDate, "%s\\%s", szPath, szFileFilter);
						}
						else sprintf(szFilenameDate, "%s\\%s", szLogPathName, szFileFilter);

						if (!FileExists(szLogPathName)) CreateDirectory(szLogPathName, NULL);

						if (!FileExists(szFilenameDate))
						{
							if ((pSepFile = fopen(szFilenameDate, "a")) == NULL)
							{
								MessageBox(hDlg,"Error opening separate filter file!","PDW Separate Filterfile",MB_ICONERROR);
								filter.sep_filterfile_en = 0;
								SetFocus(GetDlgItem(hDlg, IDC_SEPFILTERFILE1));
								return (FALSE);
							}
							else
							{
								fclose(pSepFile);
								DeleteFile(szFilenameDate);
							}
						}
					}
					strcpy(tmp_sepfile[j], szFileFilter);

					if (j == 1)			// If we have 2 sepfiles
					{
						if ((stricmp(tmp_sepfile[0], tmp_sepfile[1]) == 0) && (strcmp(tmp_sepfile[0], "Don't change")))
						{
							MessageBox(hDlg,"Separate filterfiles 1 and 2 are equal.\nPlease change one of them.","PDW Separate Filterfile",MB_ICONERROR);
							SetFocus(GetDlgItem(hDlg, IDC_SEPFILTERFILEBROWSE2));
							return (FALSE);
						}
					}
					else if (j == 2)	// If we have 3 sepfiles
					{
						if ((stricmp(tmp_sepfile[0], tmp_sepfile[2]) == 0) && (strcmp(tmp_sepfile[0], "Don't change")))
						{
							MessageBox(hDlg,"Separate filterfiles 1 and 3 are equal.\nPlease change one of them.","PDW Separate Filterfile",MB_ICONERROR);
							SetFocus(GetDlgItem(hDlg, IDC_SEPFILTERFILEBROWSE3));
							return (FALSE);
						}
						if ((stricmp(tmp_sepfile[1], tmp_sepfile[2]) == 0) && (strcmp(tmp_sepfile[1], "Don't change")))
						{
							MessageBox(hDlg,"Separate filterfiles 2 and 3 are equal.\nPlease change one of them.","PDW Separate Filterfile",MB_ICONERROR);
							SetFocus(GetDlgItem(hDlg, IDC_SEPFILTERFILEBROWSE3));
							return (FALSE);
						}
					}
				}
				filter.sep_filterfiles=sep_filterfiles;
			}

			SetCursor(LoadCursor(NULL, IDC_WAIT));

			while (((index = ListView_GetNextItem(hListView, index, LVNI_SELECTED)) != CB_ERR) || !Profile.filters.size())
			{
				if (!bEditFilter)	// If we are adding a new filter
				{
					ListView_SetItemState(hListView, index++, 0, LVIS_SELECTED | LVIS_FOCUSED); // Deselect current
					Profile.filters.insert(Profile.filters.begin() + index, filter);
				}

				if (strncmp(temp_cap, "Don't cha", 9))	// If not "Don't cha(nge)"
				{
					strcpy(Profile.filters[index].capcode, filter.capcode);
				}

				if (IsDlgButtonChecked(hDlg, IDC_FILTERREJECT) != BST_INDETERMINATE)
				{
					Profile.filters[index].reject = filter.reject;
				}

				if (!filter.reject)
				{
					if (IsDlgButtonChecked(hDlg, IDC_FILTER_MONITOR_ONLY) != BST_INDETERMINATE)
					{
						Profile.filters[index].monitor_only = IsDlgButtonChecked(hDlg, IDC_FILTER_MONITOR_ONLY);

						if (SendDlgItemMessage(hDlg, IDC_FILTERAUDIO, CB_GETCURSEL, 0, 0L) != (Profile.filters[index].monitor_only ? 2 : 12))
						{
							Profile.filters[index].wave_number = SendDlgItemMessage(hDlg, IDC_FILTERAUDIO, CB_GETCURSEL, 0, 0L);
							if (!Profile.filters[index].monitor_only) Profile.filters[index].wave_number -= 1;
						}
					}

					if (strcmp(tmp_text, "Don't change"))	// If not "Don't change"
					{
						strcpy(Profile.filters[index].text, tmp_text);
					}

					if (IsDlgButtonChecked(hDlg, IDC_FILTERLABELEN) != BST_INDETERMINATE)
					{
						Profile.filters[index].label_enabled = filter.label_enabled;
					}

					GetDlgItemText(hDlg, IDC_FILTERLABEL, filter.label, FILTER_LABEL_LEN+1);

					if (!filter.label[0])
					{
						Profile.filters[index].label_color = 0;
						strcpy(Profile.filters[index].label, "");
					}
					else
					{
						if (SendDlgItemMessage(hDlg, IDC_FILTERLABELCOLOR, CB_GETCURSEL, 0, 0L) != 17)
						{
							Profile.filters[index].label_color = SendDlgItemMessage(hDlg, IDC_FILTERLABELCOLOR, CB_GETCURSEL, 0, 0L);
						}
						if (strcmp(filter.label, "Don't change"))	// If not "Don't change"
						{
							strcpy(Profile.filters[index].label, filter.label);
						}
					}

					if (IsDlgButtonChecked(hDlg, IDC_FILTERMATCHEXACT) != BST_INDETERMINATE)
					{
						Profile.filters[index].match_exact_msg = IsDlgButtonChecked(hDlg, IDC_FILTERMATCHEXACT);
					}

					if (IsDlgButtonChecked(hDlg, IDC_FILTERCMD) != BST_INDETERMINATE)
					{
						Profile.filters[index].cmd_enabled = IsDlgButtonChecked(hDlg, IDC_FILTERCMD);
					}

					if (IsDlgButtonChecked(hDlg, IDC_FILTERSMTP) != BST_INDETERMINATE)
					{
						Profile.filters[index].smtp = IsDlgButtonChecked(hDlg, IDC_FILTERSMTP);
					}

					if (IsDlgButtonChecked(hDlg, IDC_SEPFILTERFILEEN) != BST_INDETERMINATE)
					{
						Profile.filters[index].sep_filterfile_en = IsDlgButtonChecked(hDlg, IDC_SEPFILTERFILEEN);
						Profile.filters[index].sep_filterfiles   = filter.sep_filterfiles;

						if (IsDlgButtonChecked(hDlg, IDC_SEPFILTERFILEEN) == BST_UNCHECKED)
						{
							for (int j=0; j<3; j++)
							{
								strcpy(Profile.filters[index].sep_filterfile[j], "");
							}
						}
						else
						{
							if (filter.sep_filterfiles)
							{
								for (int j=0; j<3; j++)
								{
									if (strcmp(tmp_sepfile[j], "Don't change"))
									{
										if (Need_Ext(tmp_sepfile[j]) && tmp_sepfile[j][0]) strcat(tmp_sepfile[j],".txt");
										strcpy(Profile.filters[index].sep_filterfile[j], tmp_sepfile[j]);
									}
								}
							}
						}
					}
				}

				if (bEditFilter)
				{
					if (!multiple_edit)
					{
						Profile.filters[index].hitcounter = filter.hitcounter;
						strcpy(Profile.filters[index].lasthit_date, filter.lasthit_date);
						strcpy(Profile.filters[index].lasthit_time, filter.lasthit_time);
					}
					Copy_Filter_Fields(&filter, Profile.filters[index]);
					BuildFilterString(temp, filter);
					ListView_DeleteItem(hListView, index) ;
					InsertListViewItem(temp, index) ;
					SendMessage(hListView, LVM_SETSELECTIONMARK, 0, index);
				}
				else
				{
					Copy_Filter_Fields(&filter, Profile.filters[index]);
					BuildFilterString(temp, filter);
					InsertListViewItem(temp, index) ;
					ListView_SetItemState(hListView, index, LVIS_SELECTED | LVIS_FOCUSED, LVIS_SELECTED | LVIS_FOCUSED);
					SendMessage(hListView, LVM_SETSELECTIONMARK, 0, index);
					break;
				}
				ListView_SetItemState(hListView, index, LVIS_SELECTED | LVIS_FOCUSED, LVIS_SELECTED | LVIS_FOCUSED);
			}
			ListView_SetItemState(hListView, index+1, 0, LVIS_SELECTED | LVIS_FOCUSED); // Deselect current

			SetCursor(LoadCursor(NULL, IDC_ARROW));

			if (bApplying)
			{
				bApplying=false;
			}
			else EndDialog(hDlg, TRUE);

			return (TRUE);

			case IDC_FILTER_NEXT:
			case IDC_FILTER_PREVIOUS:

			index = ListView_GetNextItem(hListView, -1, LVNI_SELECTED);
			index += (LOWORD(wParam) == IDC_FILTER_NEXT) ? 1 : -1;

			bFilterJump=true;
			Copy_Filter_Fields(&filter, Profile.filters[index]);
			ListView_SetItemState (hListView, -1, 0, LVIS_SELECTED | LVIS_FOCUSED);
			ListView_SetItemState (hListView, index, LVIS_SELECTED | LVIS_FOCUSED, LVIS_SELECTED | LVIS_FOCUSED);
			ListView_EnsureVisible(hListView, index, TRUE);
			SendMessage(hDlg, WM_INITDIALOG, NULL, 0L);
			bFilterJump=false;

			return (TRUE);

			case IDCANCEL:

			filter.type = UNUSED_FILTER;

			EndDialog(hDlg, TRUE);

			return (TRUE);
		}
		break;
	}
	return (FALSE);
} // end of FilterEditDlgBox


BOOL FAR PASCAL FilterFindDlgProc(HWND hDlg, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	static char search_string[FILTER_LABEL_LEN+1]="";
	static char old_search_string[FILTER_LABEL_LEN+1]="";

	int nHits;
	static int index=0;
	static bool bInit=true;

	switch (uMsg)
	{
		case WM_INITDIALOG:

		bInit=true;

		if (!CenterWindow(hDlg)) return (FALSE);

		SendDlgItemMessage(hDlg, IDC_FILTERFIND, EM_LIMITTEXT, FILTER_LABEL_LEN, 0L);

		if (search_string[0])
		{
			SetDlgItemText(hDlg, IDC_FILTERFIND, search_string);
		}
		else
		{
			SetDlgItemText(hDlg, IDC_FILTERFIND, "address / text / label");
			SendDlgItemMessage(hDlg, IDC_FILTERFIND, EM_SETSEL, 0, -1);
		}
		CheckDlgButton(hDlg, IDC_FILTERFIND_CASE, bFilterFindCASE);

		bInit=false;

		return (TRUE);

		case WM_COMMAND:

		// A typical stupid Hunter-workaround
		if ((HIWORD(wParam)) & 0x300) break;

		switch (LOWORD(wParam))
		{
			case IDC_FILTERFIND_CASE:

			bFilterFindCASE = IsDlgButtonChecked(hDlg, IDC_FILTERFIND_CASE) ? true : false;

			case IDOK:
			case IDC_FILTERFIND:

			if (bInit) break;	// Skip if we are in WM_INITDIALOG

			if (!SendDlgItemMessage(hDlg, IDC_FILTERFIND, WM_GETTEXTLENGTH, 0, 0))
			{
				SetDlgItemInt(hDlg, IDC_FILTERFIND_HITS, 0, FALSE);
				break;
			}
			GetDlgItemText(hDlg, IDC_FILTERFIND, search_string, FILTER_LABEL_LEN+1);

			// If the search string is different than previous,
			// or if we just have (un)checked Case Sensitive
			if ((strcmp(old_search_string, search_string) != 0) ||
				(LOWORD(wParam) == IDC_FILTERFIND_CASE))
			{
				index = ListView_GetSelectionMark(hListView);
			}
			else	// If search string hasn't changed (ENTER / F3)
			{
				if (index == Profile.filters.size()) index = 0;
				else index = ListView_GetSelectionMark(hListView)+1;
			}

			nHits = FindFilter(index, search_string, true, false);
			SetDlgItemInt(hDlg, IDC_FILTERFIND_HITS, nHits, FALSE);

			strcpy(old_search_string, search_string);

			break;

			case IDCANCEL:

			index=0;
			EndDialog(hDlg, TRUE);

			return (TRUE);
		}
		break;
	}
	return (FALSE);
} // end of FilterFindDlgProc


BOOL FAR PASCAL FilterCheckDuplicateDlgProc(HWND hDlg, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	static char szFilter1[MAX_STR_LEN]="";
	static char szFilter2[MAX_STR_LEN]="";

	char szPercentage[8];
	char szFirstCapcode[FILTER_CAPCODE_LEN+1];
	char szFirstText[FILTER_TEXT_LEN+1];
	int   iFirstType;

	static int index1, index2, oldpercentage=0;

	int cursel, percentage;
	double calculate;

	unsigned int nFilters, nFilters2;

	static bool bDuplicate;

	HWND hResults = GetDlgItem(hDlg, IDC_FILTERFIND_DUPLICATE);
	HWND hProgress= GetDlgItem(hDlg, IDC_PROGRESS1);

	switch (uMsg)
	{
		case WM_INITDIALOG:

		if (!CenterWindow(hDlg)) return (FALSE);

		if (!bDuplicate)
		{
			index1=0;
			index2=1;
		}
		else
		{
			if ((strstr(szFilter1, Profile.filters[index1].label) == 0)   ||
				(strstr(szFilter2, Profile.filters[index2].label) == 0)   ||
				(strstr(szFilter1, Profile.filters[index1].capcode) == 0) ||
				(strstr(szFilter2, Profile.filters[index2].capcode) == 0))
			{
				bDuplicate=false;
			}
			else
			{
				SendMessage(hResults, LB_ADDSTRING, 0, (LPARAM) (LPSTR) szFilter1);
				SendMessage(hResults, LB_ADDSTRING, 0, (LPARAM) (LPSTR) szFilter2);
			}
		}
		EnableWindow(GetDlgItem(hDlg, IDC_FILTERFIND_DUPLICATE), bDuplicate);

		SendMessage(hProgress,PBM_SETRANGE,0,MAKELONG(0,100));	// Set PB range	(0-100)
		SendMessage(hProgress,PBM_SETPOS,0,0);					// Set PB start position (0)
		SendMessage(hProgress,PBM_SETSTEP,1,0);					// Set PB step (1)

		return (TRUE);

		case WM_KEYDOWN:

		if (wParam == VK_RETURN)
		{
			cursel = SendMessage(hResults, LB_GETCURSEL, 0, 0);

			iDuplicateFilter = cursel ? index2 : index1;
			EndDialog(hDlg, TRUE);
			return (TRUE);
		}
		break ;

		case WM_COMMAND:

		switch (LOWORD(wParam))
		{
			case IDOK:

			SendMessage(hResults, LB_ADDSTRING, 0, (LPARAM) (LPSTR) "Searching for duplicate filters");
			SendMessage(hResults, LB_ADDSTRING, 0, (LPARAM) (LPSTR) "Please be patient...");

			nFilters=Profile.filters.size();
			nFilters2=nFilters*nFilters;

			EnableWindow(GetDlgItem(hDlg, IDOK),     false);
			EnableWindow(GetDlgItem(hDlg, IDCANCEL), false);

			if (bDuplicate)
			{
				EnableWindow(hResults, false);
				bDuplicate=false;
				index2++;
			}
			while (index1<nFilters && !bDuplicate)
			{
				calculate= nFilters2-((nFilters-index1)*(nFilters-index1));
				calculate/=nFilters2;

				percentage=(calculate*100)+1;

				if (percentage != oldpercentage)
				{
					sprintf(szPercentage, "%i%%", percentage);
					SetDlgItemText(hDlg, IDC_FILTERDUP_PCT, szPercentage);
					oldpercentage = percentage;

					SendMessage(hProgress,PBM_STEPIT,0,0);	// Increase PB one step
					pumpMessages();	// Process messages
				}

				iFirstType = Profile.filters[index1].type;
				strcpy(szFirstCapcode, Profile.filters[index1].capcode);
				strcpy(szFirstText,    Profile.filters[index1].text);

				while (index2<nFilters && !bDuplicate)
				{
					if (iFirstType == Profile.filters[index2].type)
					{
						if (strcmp(szFirstCapcode, Profile.filters[index2].capcode) == 0)
						{
							if (strcmp(szFirstText, Profile.filters[index2].text) == 0)
							{
								bDuplicate=true;
							}
						}
					}
					if (!bDuplicate) index2++;
				}
				if (!bDuplicate)
				{
					index1++;
					index2=index1+1;
				}
			}
			EnableWindow(hResults, bDuplicate);

			if (bDuplicate)
			{
				strcpy(szFilter1, "1. ");
				strcpy(szFilter2, "2. ");

				if (Profile.filters[index1].capcode[0])
				{
					strcat(szFilter1, Profile.filters[index1].capcode);
					strcat(szFilter2, Profile.filters[index2].capcode);
				}

				if (Profile.filters[index1].text[0])
				{
					if (Profile.filters[index1].capcode[0])
					{
						strcat(szFilter1, " - ");
						strcat(szFilter2, " - ");
					}
					strcat(szFilter1, Profile.filters[index1].text);
					strcat(szFilter2, Profile.filters[index2].text);
				}
				if (Profile.filters[index1].label[0])
				{
					strcat(szFilter1, " - ");
					strcat(szFilter1, Profile.filters[index1].label);
					strcat(szFilter2, " - ");
					strcat(szFilter2, Profile.filters[index2].label);
				}

				SendMessage(hResults, LB_RESETCONTENT, 0, 0);
				SendMessage(hResults, LB_ADDSTRING, 0, (LPARAM) (LPSTR) szFilter1);
				SendMessage(hResults, LB_ADDSTRING, 0, (LPARAM) (LPSTR) szFilter2);
			}
			else
			{
				SendMessage(hResults, LB_ADDSTRING, 0, (LPARAM) (LPSTR) "No (more) duplicate filters found");
				index1=0;
				index2=1;
			}
			EnableWindow(GetDlgItem(hDlg, IDOK), true);
			EnableWindow(GetDlgItem(hDlg, IDCANCEL), true);
			MessageBeep(MB_OK);
			
			break;

			case IDC_FILTERFIND_DUPLICATE:

			if (!bDuplicate)
			{
				SetFocus(GetDlgItem(hDlg, IDOK));
			}
			else if (HIWORD(wParam) == LBN_DBLCLK)
			{
				cursel = SendMessage(hResults, LB_GETCURSEL, 0, 0);

				iDuplicateFilter = cursel ? index2 : index1;
				EndDialog(hDlg, TRUE);
				return (TRUE);
			}
			break;

			case IDCANCEL:
				iDuplicateFilter=-1;
				EndDialog(hDlg, TRUE);
			return (TRUE);
		}
		break;
	}
	return (FALSE);
} // end of FilterCheckDuplicateDlgProc


void SetMailOptions(HWND hDlg, int nOptions)
{
	int tmp ;

	CheckDlgButton(hDlg, IDC_SMTP,         ((nOptions & MAIL_OPTION_ENABLE)  != 0)) ;
	CheckDlgButton(hDlg, IDC_SMTP_ADDRESS, ((nOptions & MAIL_OPTION_ADDRESS) != 0)) ;
	CheckDlgButton(hDlg, IDC_SMTP_TIME,    ((nOptions & MAIL_OPTION_TIME)    != 0)) ;
	CheckDlgButton(hDlg, IDC_SMTP_DATE,    ((nOptions & MAIL_OPTION_DATE)    != 0)) ;
	CheckDlgButton(hDlg, IDC_SMTP_MODE,    ((nOptions & MAIL_OPTION_MODE)    != 0)) ;
	CheckDlgButton(hDlg, IDC_SMTP_TYPE,    ((nOptions & MAIL_OPTION_TYPE)    != 0)) ;
	CheckDlgButton(hDlg, IDC_SMTP_BITRATE, ((nOptions & MAIL_OPTION_BITRATE) != 0)) ;
	CheckDlgButton(hDlg, IDC_SMTP_MESSAGE, ((nOptions & MAIL_OPTION_MESSAGE) != 0)) ;
	CheckDlgButton(hDlg, IDC_SMTP_LABEL,   ((nOptions & MAIL_OPTION_LABEL)   != 0)) ;
	CheckDlgButton(hDlg, IDC_SMTP_AUTH,    ((nOptions & MAIL_OPTION_AUTH)    != 0)) ;
	CheckDlgButton(hDlg, IDC_SMTP_SSL,    ((nOptions & MAIL_OPTION_SSL)    != 0)) ;

	SendDlgItemMessage(hDlg, IDC_SMTP_SETTING, CB_SETCURSEL, nOptions & MAIL_OPTION_MODES, 0L) ;
	tmp = ((nOptions & 0x3000) >> 12) - 1 ;
	SendDlgItemMessage(hDlg, IDC_SMTP_SENDIN, CB_SETCURSEL, tmp, 0L) ;

	tmp = ((nOptions & 0x1F0000) >> 16) - 1 ;
	if (tmp < 0)
	{
		tmp = 1 ;  // default to ISO-8859-1
	}
	SendDlgItemMessage(hDlg, IDC_SMTP_CHARSET, CB_SETCURSEL, tmp, 0L) ; 

}

int GetMailOptions(HWND hDlg)
{
	int ret, nOptions ;

	ret = IsDlgButtonChecked(hDlg, IDC_SMTP) ? MAIL_OPTION_ENABLE : 0 ;
	nOptions = SendDlgItemMessage(hDlg, IDC_SMTP_SETTING, CB_GETCURSEL, 0, 0L);
	EnableWindow(GetDlgItem(hDlg, IDC_SMTP_SETTING), ret) ;
	EnableWindow(GetDlgItem(hDlg, IDC_SMTP_PORT), ret) ;
	EnableWindow(GetDlgItem(hDlg, IDC_SMTP_ADDRESS), ret) ;
	EnableWindow(GetDlgItem(hDlg, IDC_SMTP_TIME), ret) ;
	EnableWindow(GetDlgItem(hDlg, IDC_SMTP_DATE), ret) ;
	EnableWindow(GetDlgItem(hDlg, IDC_SMTP_MODE), ret) ;
	EnableWindow(GetDlgItem(hDlg, IDC_SMTP_TYPE), ret) ;
	EnableWindow(GetDlgItem(hDlg, IDC_SMTP_BITRATE), ret) ;
	EnableWindow(GetDlgItem(hDlg, IDC_SMTP_MESSAGE), ret) ;
	EnableWindow(GetDlgItem(hDlg, IDC_SMTP_LABEL), ret) ;
	EnableWindow(GetDlgItem(hDlg, IDC_SMTP_HOST), ret) ;
	EnableWindow(GetDlgItem(hDlg, IDC_SMTP_HELO), ret) ;
	EnableWindow(GetDlgItem(hDlg, IDC_SMTP_TO), ret) ;
	EnableWindow(GetDlgItem(hDlg, IDC_SMTP_FROM), ret) ;
	EnableWindow(GetDlgItem(hDlg, IDC_SMTP_TEST), ret) ;
	EnableWindow(GetDlgItem(hDlg, IDC_SMTP_RESPONSE), ret) ;
	EnableWindow(GetDlgItem(hDlg, IDC_SMTP_USER), ret) ;
	EnableWindow(GetDlgItem(hDlg, IDC_SMTP_PASSWORD), ret) ;
	EnableWindow(GetDlgItem(hDlg, IDC_SMTP_AUTH), ret) ;
	EnableWindow(GetDlgItem(hDlg, IDC_SMTP_SENDIN), ret) ;
	EnableWindow(GetDlgItem(hDlg, IDC_SMTP_CHARSET), ret) ;
	EnableWindow(GetDlgItem(hDlg, IDC_SMTP_SSL), ret) ;

	if(ret)
	{
		ret= IsDlgButtonChecked(hDlg, IDC_SMTP_AUTH) ;
		EnableWindow(GetDlgItem(hDlg, IDC_SMTP_USER), ret) ;
		EnableWindow(GetDlgItem(hDlg, IDC_SMTP_PASSWORD), ret) ;
	}

	nOptions += IsDlgButtonChecked(hDlg, IDC_SMTP)         ? MAIL_OPTION_ENABLE  : 0 ;
	nOptions += IsDlgButtonChecked(hDlg, IDC_SMTP_ADDRESS) ? MAIL_OPTION_ADDRESS : 0 ;
	nOptions += IsDlgButtonChecked(hDlg, IDC_SMTP_TIME)    ? MAIL_OPTION_TIME	 : 0 ;
	nOptions += IsDlgButtonChecked(hDlg, IDC_SMTP_DATE)    ? MAIL_OPTION_DATE	 : 0 ;
	nOptions += IsDlgButtonChecked(hDlg, IDC_SMTP_MODE)    ? MAIL_OPTION_MODE	 : 0 ;
	nOptions += IsDlgButtonChecked(hDlg, IDC_SMTP_TYPE)    ? MAIL_OPTION_TYPE	 : 0 ;
	nOptions += IsDlgButtonChecked(hDlg, IDC_SMTP_BITRATE) ? MAIL_OPTION_BITRATE : 0 ;
	nOptions += IsDlgButtonChecked(hDlg, IDC_SMTP_MESSAGE) ? MAIL_OPTION_MESSAGE : 0 ;
	nOptions += IsDlgButtonChecked(hDlg, IDC_SMTP_LABEL)   ? MAIL_OPTION_LABEL	 : 0 ;
	nOptions += IsDlgButtonChecked(hDlg, IDC_SMTP_AUTH)    ? MAIL_OPTION_AUTH	 : 0 ;
	nOptions += IsDlgButtonChecked(hDlg, IDC_SMTP_SSL)    ? MAIL_OPTION_SSL		 : 0 ;

	ret = SendDlgItemMessage(hDlg, IDC_SMTP_SENDIN, CB_GETCURSEL, 0, 0L);
	if(ret == CB_ERR) ret = 0 ;
	ret++ ;
	nOptions += (ret << 12) ;

	ret = SendDlgItemMessage(hDlg, IDC_SMTP_CHARSET, CB_GETCURSEL, 0, 0L);
	if(ret == CB_ERR) ret = 0 ;
	ret++ ;
	nOptions += (ret << 16) ;

	return(nOptions) ;
}


char *GetTestMail(int nLine)
{
	static char szBuf[9][128] ;
	time_t lTime ;
	struct tm *recTm ; 
	lTime = time(NULL) ;
	recTm = localtime(&lTime) ;
	int		len = 0 ;
	wsprintf(szBuf[1], "012345678") ;
	wsprintf(szBuf[2], "%02d:%02d", recTm->tm_hour, recTm->tm_min) ;
	wsprintf(szBuf[3], "%02d-%02d-%04d", recTm->tm_mday, recTm->tm_mon+1, recTm->tm_year + 1900) ;
	wsprintf(szBuf[4], "FLEX") ;
	wsprintf(szBuf[5], "ALPHA") ;
	wsprintf(szBuf[6], "1600") ;
	wsprintf(szBuf[7], "Message") ;
	wsprintf(szBuf[8], "Label") ;
	return(szBuf[nLine]) ;
}


BOOL FAR PASCAL MailDlgProc(HWND hDlg, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	static int nOldOptions ;
	int i;

	switch (uMsg)
	{
		case WM_INITDIALOG:

		if (!CenterWindow(hDlg)) return (FALSE);

		CheckDlgButton(hDlg, IDC_SMTP, Profile.SMTP) ;

		SendDlgItemMessage(hDlg, IDC_SMTP_HOST,     EM_LIMITTEXT, MAIL_TEXT_LEN, 0L) ; 
		SendDlgItemMessage(hDlg, IDC_SMTP_HELO,     EM_LIMITTEXT, MAIL_TEXT_LEN, 0L) ; 
		SendDlgItemMessage(hDlg, IDC_SMTP_TO,       EM_LIMITTEXT, MAIL_TEXT_LEN * 5, 0L) ; 
		SendDlgItemMessage(hDlg, IDC_SMTP_FROM,     EM_LIMITTEXT, MAIL_TEXT_LEN, 0L) ; 
		SendDlgItemMessage(hDlg, IDC_SMTP_USER,     EM_LIMITTEXT, MAIL_TEXT_LEN, 0L) ; 
		SendDlgItemMessage(hDlg, IDC_SMTP_PASSWORD, EM_LIMITTEXT, MAIL_TEXT_LEN, 0L) ; 

		SendDlgItemMessage(hDlg, IDC_SMTP_SETTING, CB_ADDSTRING, 0, (LPARAM)(LPCTSTR) "All messages") ; 
		SendDlgItemMessage(hDlg, IDC_SMTP_SETTING, CB_ADDSTRING, 0, (LPARAM)(LPCTSTR) "Filtered messages") ; 
		SendDlgItemMessage(hDlg, IDC_SMTP_SETTING, CB_ADDSTRING, 0, (LPARAM)(LPCTSTR) "Filtered & Monitor-Only messages") ; 
		SendDlgItemMessage(hDlg, IDC_SMTP_SETTING, CB_ADDSTRING, 0, (LPARAM)(LPCTSTR) "Selected filters only") ; 

		SetDlgItemText(hDlg, IDC_SMTP_HOST,     Profile.szMailHost);
		SetDlgItemText(hDlg, IDC_SMTP_HELO,     Profile.szMailHeloDomain);
		SetDlgItemText(hDlg, IDC_SMTP_TO,       Profile.szMailTo);
		SetDlgItemText(hDlg, IDC_SMTP_FROM,     Profile.szMailFrom);
		SetDlgItemText(hDlg, IDC_SMTP_USER,     Profile.szMailUser);
		SetDlgItemText(hDlg, IDC_SMTP_PASSWORD, Profile.szMailPassword);
		SetDlgItemInt (hDlg, IDC_SMTP_PORT,     Profile.iMailPort, FALSE);

		SendDlgItemMessage(hDlg, IDC_SMTP_SENDIN, CB_ADDSTRING, 0, (LPARAM)(LPCTSTR) "Subject") ; 
		SendDlgItemMessage(hDlg, IDC_SMTP_SENDIN, CB_ADDSTRING, 0, (LPARAM)(LPCTSTR) "Message") ; 
		SendDlgItemMessage(hDlg, IDC_SMTP_SENDIN, CB_ADDSTRING, 0, (LPARAM)(LPCTSTR) "Subject & Message") ; 

		for (i=0; i<MAX_SMTP_CHARSETS; i++)
		{
			SendDlgItemMessage(hDlg, IDC_SMTP_CHARSET, CB_ADDSTRING, 0, (LPARAM)(LPCTSTR) szSmtpCharSets[i]) ; 
		}

		SetDlgItemInt(hDlg, IDC_SMTP_SESSIONS,  nSMTPsessions, false);
		SetDlgItemInt(hDlg, IDC_SMTP_EMAILS,    nSMTPemails,   false);
		SetDlgItemInt(hDlg, IDC_SMTP_ERRORS,    nSMTPerrors,   false);
		SetDlgItemInt(hDlg, IDC_SMTP_LASTERROR, iSMTPlastError,false);
		
		SetMailOptions(hDlg, Profile.nMailOptions) ;
		nOldOptions = GetMailOptions(hDlg) ;
		return (TRUE);

		case WM_COMMAND:

		switch (LOWORD(wParam))
		{
			case IDOK:
				SendDlgItemMessage(hDlg, IDC_SMTP_HOST,     WM_GETTEXT, MAIL_TEXT_LEN, (LPARAM)(LPCTSTR) Profile.szMailHost) ;
				SendDlgItemMessage(hDlg, IDC_SMTP_HELO,     WM_GETTEXT, MAIL_TEXT_LEN, (LPARAM)(LPCTSTR) Profile.szMailHeloDomain) ;
				SendDlgItemMessage(hDlg, IDC_SMTP_TO,       WM_GETTEXT, MAIL_TEXT_LEN * 5, (LPARAM)(LPCTSTR) Profile.szMailTo) ;
				SendDlgItemMessage(hDlg, IDC_SMTP_FROM,     WM_GETTEXT, MAIL_TEXT_LEN, (LPARAM)(LPCTSTR) Profile.szMailFrom) ;
				SendDlgItemMessage(hDlg, IDC_SMTP_USER,     WM_GETTEXT, MAIL_TEXT_LEN, (LPARAM)(LPCTSTR) Profile.szMailUser) ;
				SendDlgItemMessage(hDlg, IDC_SMTP_PASSWORD, WM_GETTEXT, MAIL_TEXT_LEN, (LPARAM)(LPCTSTR) Profile.szMailPassword) ;
				Profile.iMailPort = GetDlgItemInt(hDlg, IDC_SMTP_PORT, NULL, FALSE);
				Profile.nMailOptions = GetMailOptions(hDlg) ;
				Profile.SMTP = Profile.nMailOptions & MAIL_OPTION_ENABLE ? 1 : 0 ;
				Profile.ssl =  Profile.nMailOptions & MAIL_OPTION_SSL ? 1 : 0 ;
				MailInit(Profile.szMailHost, Profile.szMailHeloDomain, Profile.szMailFrom, Profile.szMailTo, Profile.szMailUser, Profile.szMailPassword, Profile.iMailPort, Profile.nMailOptions);
				SendMail(0, 0, 0, 0, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL) ;

				WriteSettings();
				EndDialog(hDlg, TRUE);
				return (TRUE);

			case IDCANCEL:
				EndDialog(hDlg, TRUE);
				return (TRUE);

			case IDC_SMTP_TEST:
				SendDlgItemMessage(hDlg, IDC_SMTP_HOST,     WM_GETTEXT, MAIL_TEXT_LEN, (LPARAM)(LPCTSTR) Profile.szMailHost) ;
				SendDlgItemMessage(hDlg, IDC_SMTP_HELO,     WM_GETTEXT, MAIL_TEXT_LEN, (LPARAM)(LPCTSTR) Profile.szMailHeloDomain) ;
				SendDlgItemMessage(hDlg, IDC_SMTP_TO,       WM_GETTEXT, MAIL_TEXT_LEN * 5, (LPARAM)(LPCTSTR) Profile.szMailTo) ;
				SendDlgItemMessage(hDlg, IDC_SMTP_FROM,     WM_GETTEXT, MAIL_TEXT_LEN, (LPARAM)(LPCTSTR) Profile.szMailFrom) ;
				SendDlgItemMessage(hDlg, IDC_SMTP_USER,     WM_GETTEXT, MAIL_TEXT_LEN, (LPARAM)(LPCTSTR) Profile.szMailUser) ;
				SendDlgItemMessage(hDlg, IDC_SMTP_PASSWORD, WM_GETTEXT, MAIL_TEXT_LEN, (LPARAM)(LPCTSTR) Profile.szMailPassword) ;
				Profile.iMailPort = GetDlgItemInt(hDlg, IDC_SMTP_PORT, NULL, FALSE);
				Profile.nMailOptions = GetMailOptions(hDlg) ;
				Profile.ssl =  Profile.nMailOptions & MAIL_OPTION_SSL ? 1 : 0 ;
				if((Profile.nMailOptions & MAIL_OPTION_MODES) && (!(Profile.nMailOptions & ~MAIL_OPTION_MODES)))
				{
					break ;
				}
				MailInit(Profile.szMailHost, Profile.szMailHeloDomain, Profile.szMailFrom, Profile.szMailTo, Profile.szMailUser, Profile.szMailPassword, Profile.iMailPort, Profile.nMailOptions);
				SendMail(GetDlgItem(hDlg, IDC_SMTP_RESPONSE), 1, 0, 1, GetTestMail(1), GetTestMail(2), GetTestMail(3), GetTestMail(4), GetTestMail(5), GetTestMail(6), GetTestMail(7), GetTestMail(8)) ;
				break ;
			case IDC_SMTP_SETTING :
				nOldOptions = GetMailOptions(hDlg) ;
				break ;
			case IDC_SMTP :
				nOldOptions = GetMailOptions(hDlg) ;
				break ;
			case IDC_SMTP_AUTH :
				nOldOptions = GetMailOptions(hDlg) ;
				break ;
			case IDC_SMTP_SENDIN :
				nOldOptions = GetMailOptions(hDlg) ;
				break ;
			case IDC_SMTP_ADDRESS :
			case IDC_SMTP_TIME    :
			case IDC_SMTP_DATE    :
			case IDC_SMTP_MODE    :
			case IDC_SMTP_TYPE    :
			case IDC_SMTP_MESSAGE :
			case IDC_SMTP_BITRATE :
			case IDC_SMTP_SSL	  :
				Profile.nMailOptions = GetMailOptions(hDlg) ;
				if((Profile.nMailOptions & MAIL_OPTION_MODES) && (!(Profile.nMailOptions & ~MAIL_OPTION_MODES)))
				{
					MessageBox(hDlg, "You must select at least one of the Mail Options!", "Mail Settings Error", MB_ICONWARNING);
					SetMailOptions(hDlg, nOldOptions) ;
				}
				nOldOptions = GetMailOptions(hDlg) ;
				break ;
		}
		break;
	}
	return (FALSE);
} // end of MailDlgProc


BOOL FAR PASCAL MonStatDlgProc(HWND hDlg, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	static int idc_stat_hr_n[NUM_STAT] =
	{
		IDC_STATHRF64N, IDC_STATHRF32N, IDC_STATHRF16N,
		IDC_STATHRP24N, IDC_STATHRP12N, IDC_STATHRP512N, 
		IDC_STATHR_ACARS_N, IDC_STATHR_MB_N, IDC_STATHR_EM_N
	};
	static int idc_stat_hr_a[NUM_STAT] =
	{
		IDC_STATHRF64A, IDC_STATHRF32A, IDC_STATHRF16A,
		IDC_STATHRP24A, IDC_STATHRP12A, IDC_STATHRP512A,
		IDC_STATHR_ACARS_A,IDC_STATHR_MB_A, IDC_STATHR_EM_A
	};
	static int idc_stat_dl_n[NUM_STAT] =
	{
		IDC_STATDLF64N, IDC_STATDLF32N, IDC_STATDLF16N,
		IDC_STATDLP24N, IDC_STATDLP12N, IDC_STATDLP512N, 
		IDC_STATDL_ACARS_N, IDC_STATDL_MB_N, IDC_STATDL_EM_N
	};
	static int idc_stat_dl_a[NUM_STAT] =
	{
		IDC_STATDLF64A, IDC_STATDLF32A, IDC_STATDLF16A,
		IDC_STATDLP24A, IDC_STATDLP12A, IDC_STATDLP512A,
		IDC_STATDL_ACARS_A, IDC_STATDL_MB_A, IDC_STATDL_EM_A
	};

	char value[10];
	long stat;
	int loop;
	static char szStatTxt[] =
		{"Statistics Files (*.st)\0*.st\0All Files (*.*)\0*.*\0\0"};

	OPENFILENAME ofn = {0};
	char szStatFile[MAX_FILE_LEN+1], szFileTitle[256], szStatExt[3]="st";
	int state, date_state;
	FILE *pFileLog = NULL;

	CreateDateFilename(".st", NULL);

	switch (uMsg)
	{
		case WM_INITDIALOG:

		if (!CenterWindow(hDlg)) return (FALSE);

		for (loop=0; loop < NUM_STAT; loop++)
		{
			stat = hourly_stat[loop][0];	// numeric

			if (stat > 999999L) strcpy(value, "******");
			else				sprintf(value, "%ld", stat);

			SetDlgItemText(hDlg, idc_stat_hr_n[loop], value);
		}

		for (loop=0; loop < NUM_STAT; loop++)
		{
			stat = hourly_stat[loop][1];  // alphanumeric
	
			if (stat > 999999L) strcpy(value, "******");
			else				sprintf(value, "%ld", stat);

			SetDlgItemText(hDlg, idc_stat_hr_a[loop], value);
		}

		for (loop=0; loop < NUM_STAT; loop++)
		{
			stat = daily_stat[loop][0];  // numeric
			
			if (stat > 9999999L) strcpy(value, "******");
			else				 sprintf(value, "%ld", stat);

			SetDlgItemText(hDlg, idc_stat_dl_n[loop], value);
		}

		for (loop=0; loop < NUM_STAT; loop++)
		{
			stat = daily_stat[loop][1];  // alphanumeric

			if (stat > 9999999L) strcpy(value, "******");
			else				 sprintf(value, "%ld", stat);

			SetDlgItemText(hDlg, idc_stat_dl_a[loop], value);
		}

		CheckDlgButton(hDlg, IDC_STATFILEEN,   Profile.stat_file_enabled);
		CheckDlgButton(hDlg, IDC_STATFILEDATE, Profile.stat_file_use_date);

		EnableWindow(GetDlgItem(hDlg, IDC_STATFILEDATE), Profile.stat_file_enabled);
		
		if (Profile.stat_file_enabled)
		{
			if (Profile.stat_file_use_date)
			{
				SetDlgItemText(hDlg, IDC_STATFILE, szFilenameDate);
			}
			else
			{
				EnableWindow(GetDlgItem(hDlg, IDC_STATFILE),   true);
				EnableWindow(GetDlgItem(hDlg, IDC_STATBROWSE), true);
				SetDlgItemText(hDlg, IDC_STATFILE, Profile.stat_file);
			}
		}

		return (TRUE);

		case WM_COMMAND:

		switch (LOWORD(wParam))
		{
			case IDC_STATFILEEN:

			state      = IsDlgButtonChecked(hDlg, IDC_STATFILEEN);
			date_state = IsDlgButtonChecked(hDlg, IDC_STATFILEDATE);

			EnableWindow(GetDlgItem(hDlg, IDC_STATFILEDATE), state);

			if (state)
			{
				EnableWindow(GetDlgItem(hDlg, IDC_STATFILE),   !date_state);
				EnableWindow(GetDlgItem(hDlg, IDC_STATBROWSE), !date_state);

				if (!date_state)
				{
					SetFocus(GetDlgItem(hDlg, IDC_STATFILE));
				}
			}
			else
			{
				EnableWindow(GetDlgItem(hDlg, IDC_STATFILE),   false);
				EnableWindow(GetDlgItem(hDlg, IDC_STATBROWSE), false);
				SetDlgItemText(hDlg, IDC_STATFILE, "");
				CheckDlgButton(hDlg, IDC_STATFILEDATE, BST_UNCHECKED);
			}
			break;

			case IDC_STATBROWSE:

			strcpy(szStatFile, "*.st");

			ofn.lStructSize = sizeof(OPENFILENAME);
			ofn.hwndOwner = hDlg;
			ofn.lpstrFilter = szStatTxt;
			ofn.lpstrCustomFilter = NULL;
			ofn.nMaxCustFilter = 0L;
			ofn.nFilterIndex = 1;
			ofn.lpstrFile = szStatFile;
			ofn.nMaxFile = sizeof(szStatFile)-5;
			ofn.lpstrFileTitle = szFileTitle;
			ofn.nMaxFileTitle = sizeof(szFileTitle);
			ofn.lpstrInitialDir = szPath;
			ofn.lpstrTitle = "Open Statistics File";
			ofn.Flags = OFN_HIDEREADONLY;
			ofn.nFileOffset = 0;
			ofn.nFileExtension = 0;
			ofn.lpstrDefExt = szStatExt;

			if (GetOpenFileName(&ofn))
			{
				SetDlgItemText(hDlg, IDC_STATFILE, szStatFile);
			}
			SetFocus(GetDlgItem(hDlg, IDOK));

			break;

			case IDC_STATFILEDATE:

			state	= IsDlgButtonChecked(hDlg, IDC_STATFILEDATE);

			EnableWindow(GetDlgItem(hDlg, IDC_STATFILE)  , !state);
			EnableWindow(GetDlgItem(hDlg, IDC_STATBROWSE), !state);

			SetDlgItemText(hDlg, IDC_STATFILE, state ? szFilenameDate : "");

			break;

			case IDOK:

			state      = IsDlgButtonChecked(hDlg, IDC_STATFILEEN);
			date_state = IsDlgButtonChecked(hDlg, IDC_STATFILEDATE);

			if (state)
			{
				GetDlgItemText(hDlg, IDC_STATFILE, szStatFile, MAX_FILE_LEN+1);

				if (date_state)
				{
					strcpy(Profile.stat_file, "");
				}
				else if (!szStatFile[0])
				{
					MessageBox(hDlg,"You haven't entered a file name!","PDW Statistics",MB_ICONERROR);
					SetFocus(GetDlgItem(hDlg, IDC_STATFILE));
					return (FALSE);
				}
				else
				{
					if (Need_Ext(szStatFile)) strcat(szStatFile,".st");

					if ((pFileLog = fopen(szStatFile, "a")) == NULL)
					{
						MessageBox(hDlg,"Error opening statfile!","PDW Logfile",MB_ICONERROR);
						SetFocus(GetDlgItem(hDlg, IDC_STATFILE));
						return (FALSE);
					}
					else
					{
						fclose(pFileLog);
						strcpy(Profile.stat_file, szStatFile);
					}
				}
			}

			Profile.stat_file_enabled  = state;
			Profile.stat_file_use_date = date_state;

			WriteSettings();

			EndDialog(hDlg, TRUE);
			return (TRUE);

			case IDCANCEL:
			EndDialog(hDlg, TRUE);
			return (TRUE);
		}
		break;
	}
	return (FALSE);
} // end of MonStatDlgProc


UINT CALLBACK CenterOpenDlgBox(HWND hDlg, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	switch (uMsg)
	{
		case WM_INITDIALOG:
		SetWindowText(hDlg, (LPSTR) szCenterOpenDlgMsg);
		if (!CenterWindow(hDlg)) return (FALSE);
	}
	return (FALSE);
} // end of CenterOpenDlgBox


BOOL CenterWindow(HWND hWnd)
{
	RECT rRect, rParentRect;
	HWND hParentWnd;
	int  wParent, hParent, xNew, yNew, w, h;

	GetWindowRect (hWnd, &rRect);
	w = rRect.right  - rRect.left;
	h = rRect.bottom - rRect.top;

	if (NULL == (hParentWnd = GetParent(hWnd))) return (FALSE);

	GetWindowRect(hParentWnd, &rParentRect);

	wParent = rParentRect.right  - rParentRect.left;
	hParent = rParentRect.bottom - rParentRect.top;

	xNew = wParent/2 - w/2 + rParentRect.left;
	yNew = hParent/2 - h/2 + rParentRect.top;

	xNew = max(xNew, 0);
	yNew = max(yNew, 0);

	// Get the limits of the 'workarea' (exclude tray area for WIN95)
	if (!SystemParametersInfo(SPI_GETWORKAREA, sizeof(RECT), &rParentRect, 0))
	{
		rParentRect.left   = rParentRect.top = 0;
		rParentRect.right  = GetSystemMetrics(SM_CXSCREEN);
		rParentRect.bottom = GetSystemMetrics(SM_CYSCREEN);
	}

	xNew = min(xNew, rParentRect.right  - w);
	yNew = min(yNew, rParentRect.bottom - h);

	return(SetWindowPos(hWnd, NULL, xNew, yNew, 0, 0, SWP_NOSIZE | SWP_NOZORDER));
} // end of CenterWindow


BOOL ErrorMessageBox(LPCTSTR lpszText, LPCTSTR lpszTitle, LPCTSTR lpszFile, INT Line)
{
	#define ERROR_BUFFER_SIZE 512

	static TCHAR Format[] =
	TEXT("%s\n\n"                                 )
	TEXT("-- Error Information --\n"              )
	TEXT("File : %s\n"                            )
	TEXT("Line : %d\n"                            )
	TEXT("Error Number : %d\n"                    )
	TEXT("Error Message : %s\n"                   )
	TEXT("\n"                                     )
	TEXT("Press OK to terminate this application.");

	LPTSTR	lpFormatMessageBuffer;
	DWORD	dwFormatMessage;
	DWORD	dwGetLastError;
	HLOCAL	hMessageBoxBuffer;
	LPVOID	lpMessageBoxBuffer;

	//-- perform a simple check on the needed buffer size
	if (lstrlen(lpszText) > (ERROR_BUFFER_SIZE - lstrlen(Format))) return (FALSE);

	//-- allocate the message box buffer
	hMessageBoxBuffer  = LocalAlloc(LMEM_FIXED, ERROR_BUFFER_SIZE);
	lpMessageBoxBuffer = LocalLock(hMessageBoxBuffer);

	//-- get the system error and system error message
	dwGetLastError = GetLastError();
	dwFormatMessage = FormatMessage(FORMAT_MESSAGE_ALLOCATE_BUFFER |
		FORMAT_MESSAGE_FROM_SYSTEM, NULL, dwGetLastError, LANG_NEUTRAL,
		(LPTSTR) &lpFormatMessageBuffer, 0, NULL);

	if (!dwFormatMessage) lpFormatMessageBuffer = TEXT("FormatMessage() Failed!");

	//-- format the error messge box string
	wsprintf((char *)lpMessageBoxBuffer, Format, lpszText, lpszFile, Line,
											dwGetLastError, lpFormatMessageBuffer);

	// -- display the error and allow the user to terminate or continue
	if (IDOK == MessageBox(NULL, (const char *)lpMessageBoxBuffer, lpszTitle,
											MB_APPLMODAL | MB_ICONSTOP | MB_OKCANCEL))
	ExitProcess(0);

	//-- free all buffers
	if (dwFormatMessage) LocalFree((HLOCAL) lpFormatMessageBuffer);

	LocalFree((HLOCAL) hMessageBoxBuffer);

	return (TRUE);
} // end of ErrorMessageBox


BOOL GetPrivateProfileSettings(LPCTSTR lpszAppTitle, LPCTSTR lpszIniPathName, PPROFILE pProfile)
{
	TCHAR ValBuf[16];
	TCHAR color_str[13];
	int red, green, blue;

	pProfile->xPos  = (INT) GetPrivateProfileInt(lpszAppTitle, TEXT("xPos"), 0, lpszIniPathName);
	pProfile->yPos  = (INT) GetPrivateProfileInt(lpszAppTitle, TEXT("yPos"), 0, lpszIniPathName);
	pProfile->xSize = (INT) GetPrivateProfileInt(lpszAppTitle, TEXT("xSize"), Profile.xSize, lpszIniPathName);
	pProfile->ySize = (INT) GetPrivateProfileInt(lpszAppTitle, TEXT("ySize"), Profile.ySize, lpszIniPathName);

	pProfile->maximize_flg		= (INT) GetPrivateProfileInt(lpszAppTitle, TEXT("Maximize"), Profile.maximize_flg, lpszIniPathName);
	Profile.maximize_tmp		= Profile.maximize_flg;

	pProfile->lang_mi_index		= (INT) GetPrivateProfileInt(lpszAppTitle, TEXT("LangIndex"), Profile.lang_mi_index, lpszIniPathName);
	pProfile->confirmExit		= (INT) GetPrivateProfileInt(lpszAppTitle, TEXT("ConfirmExit"), Profile.confirmExit, lpszIniPathName);
	pProfile->filterbeep		= (INT) GetPrivateProfileInt(lpszAppTitle, TEXT("FilterBeep"), Profile.filterbeep, lpszIniPathName);
	pProfile->filterwindowonly	= (INT) GetPrivateProfileInt(lpszAppTitle, TEXT("FilterWindowOnly"), Profile.filterwindowonly, lpszIniPathName);
	pProfile->decodepocsag		= (INT) GetPrivateProfileInt(lpszAppTitle, TEXT("DecodePocsag"), Profile.decodepocsag, lpszIniPathName);
	pProfile->pocsag_512		= (INT) GetPrivateProfileInt(lpszAppTitle, TEXT("Pocsag512"), Profile.pocsag_512, lpszIniPathName);
	pProfile->pocsag_1200		= (INT) GetPrivateProfileInt(lpszAppTitle, TEXT("Pocsag1200"), Profile.pocsag_1200, lpszIniPathName);
	pProfile->pocsag_2400		= (INT) GetPrivateProfileInt(lpszAppTitle, TEXT("Pocsag2400"), Profile.pocsag_2400, lpszIniPathName);
	pProfile->pocsag_fnu		= (INT) GetPrivateProfileInt(lpszAppTitle, TEXT("PocsagFNU"), Profile.pocsag_fnu, lpszIniPathName);
	pProfile->pocsag_showboth	= (INT) GetPrivateProfileInt(lpszAppTitle, TEXT("PocsagShowBoth"), Profile.pocsag_showboth, lpszIniPathName);
	pProfile->decodeflex		= (INT) GetPrivateProfileInt(lpszAppTitle, TEXT("DecodeFlex"), Profile.decodeflex, lpszIniPathName);
	pProfile->showinstr			= (INT) GetPrivateProfileInt(lpszAppTitle, TEXT("ShowInstr"), Profile.showinstr, lpszIniPathName);
	pProfile->convert_si		= (INT) GetPrivateProfileInt(lpszAppTitle, TEXT("ConvertInstr"), Profile.convert_si, lpszIniPathName);
	pProfile->show_cfs			= (INT) GetPrivateProfileInt(lpszAppTitle, TEXT("ShowCFS"), Profile.show_cfs, lpszIniPathName);
	pProfile->show_rejectblocked= (INT) GetPrivateProfileInt(lpszAppTitle, TEXT("ShowRejectBlocked"), Profile.show_rejectblocked, lpszIniPathName);
	pProfile->flex_1600			= (INT) GetPrivateProfileInt(lpszAppTitle, TEXT("Flex1600"), Profile.flex_1600, lpszIniPathName);
	pProfile->flex_3200			= (INT) GetPrivateProfileInt(lpszAppTitle, TEXT("Flex3200"), Profile.flex_3200, lpszIniPathName);
	pProfile->flex_6400			= (INT) GetPrivateProfileInt(lpszAppTitle, TEXT("Flex6400"), Profile.flex_6400, lpszIniPathName);
	pProfile->acars_parity_check= (INT) GetPrivateProfileInt(lpszAppTitle, TEXT("AcarsParityCheck"), Profile.acars_parity_check, lpszIniPathName);
 	pProfile->monitor_paging	= (INT) GetPrivateProfileInt(lpszAppTitle, TEXT("PocsagFlex"), Profile.monitor_paging, lpszIniPathName);
	pProfile->monitor_acars		= (INT) GetPrivateProfileInt(lpszAppTitle, TEXT("Acars"), Profile.monitor_acars, lpszIniPathName);
	pProfile->monitor_mobitex	= (INT) GetPrivateProfileInt(lpszAppTitle, TEXT("Mobitex"), Profile.monitor_mobitex, lpszIniPathName);
	pProfile->monitor_ermes		= (INT) GetPrivateProfileInt(lpszAppTitle, TEXT("Ermes"), Profile.monitor_ermes, lpszIniPathName);

	mb.cfs			= (INT) GetPrivateProfileInt(lpszAppTitle, TEXT("CFS"),    mb.cfs, lpszIniPathName);
	mb.frsync		= (INT) GetPrivateProfileInt(lpszAppTitle, TEXT("FRSYNC"), mb.frsync, lpszIniPathName);
	mb.frsync_rev	= ~mb.frsync;
	mb.bitsync		= (INT) GetPrivateProfileInt(lpszAppTitle, TEXT("BTSYNC"), mb.bitsync, lpszIniPathName);
	mb.bitsync_rev	= ~mb.bitsync;
	mb.bitrate		= (INT) GetPrivateProfileInt(lpszAppTitle, TEXT("BRATE"),  mb.bitrate, lpszIniPathName);
	mb.bitscr		= (INT) GetPrivateProfileInt(lpszAppTitle, TEXT("BITSCR"), mb.bitscr, lpszIniPathName);
	mb.ramnet		= (INT) GetPrivateProfileInt(lpszAppTitle, TEXT("RAMNET"), mb.ramnet, lpszIniPathName);
	mb.min_msg_len	= (INT) GetPrivateProfileInt(lpszAppTitle, TEXT("MINMSG"), mb.min_msg_len, lpszIniPathName);
	mb.show			= (INT) GetPrivateProfileInt(lpszAppTitle, TEXT("SHOW"),   mb.show, lpszIniPathName);

	pProfile->showtone			= (INT) GetPrivateProfileInt(lpszAppTitle, TEXT("ShowTone"), Profile.showtone, lpszIniPathName);
	pProfile->shownumeric		= (INT) GetPrivateProfileInt(lpszAppTitle, TEXT("ShowNumeric"), Profile.shownumeric, lpszIniPathName);
	pProfile->showmisc			= (INT) GetPrivateProfileInt(lpszAppTitle, TEXT("ShowBinary"), Profile.showmisc, lpszIniPathName);
	pProfile->FilterWindowColors= (INT) GetPrivateProfileInt(lpszAppTitle, TEXT("FilterWindowColors"), Profile.FilterWindowColors, lpszIniPathName);
	pProfile->FilterWindowExtra	= (INT) GetPrivateProfileInt(lpszAppTitle, TEXT("FilterWindowExtra"), Profile.FilterWindowExtra, lpszIniPathName);
	pProfile->fourlevel			= (INT) GetPrivateProfileInt(lpszAppTitle, TEXT("4LevelInterface"), Profile.fourlevel, lpszIniPathName);
	pProfile->invert			= (INT) GetPrivateProfileInt(lpszAppTitle, TEXT("InvertData"), Profile.invert, lpszIniPathName);
	pProfile->percent			= (INT) GetPrivateProfileInt(lpszAppTitle, TEXT("Percent"), Profile.percent, lpszIniPathName);
	pProfile->comPortEnabled	= (INT) GetPrivateProfileInt(lpszAppTitle, TEXT("ComPortEnabled"), Profile.comPortEnabled, lpszIniPathName);
	pProfile->comPortRS232		= (INT) GetPrivateProfileInt(lpszAppTitle, TEXT("ComPortRS232"), pProfile->comPortRS232, lpszIniPathName);

	GetPrivateProfileString(lpszAppTitle, TEXT("ComPort"), NULL, ValBuf, 16, lpszIniPathName);

	if  (strcmp(ValBuf, "Custom") == 0) Profile.comPort = 5;
	else sscanf(ValBuf, "COM%d", &Profile.comPort);

	if (!GetPrivateProfileString(lpszAppTitle, TEXT("ComPortAddr"),
			NULL, ValBuf, 16, lpszIniPathName))
	{
		Profile.comPort = 2;
	}
	else sscanf(ValBuf, "0x%x", &Profile.comPortAddr);

	if (!GetPrivateProfileString(lpszAppTitle, TEXT("ComPortIRQ"),
			NULL, ValBuf, 16, lpszIniPathName))
	{
		Profile.comPort = 2;
	}
	else sscanf(ValBuf, "%d", &Profile.comPortIRQ);

	pProfile->fontInfo.lfHeight			= (INT) GetPrivateProfileInt(lpszAppTitle, TEXT("Font.Height"), pProfile->fontInfo.lfHeight, lpszIniPathName);
	pProfile->fontInfo.lfWidth			= (INT) GetPrivateProfileInt(lpszAppTitle, TEXT("Font.Width"), pProfile->fontInfo.lfWidth, lpszIniPathName);
	pProfile->fontInfo.lfEscapement		= (INT) GetPrivateProfileInt(lpszAppTitle, TEXT("Font.Escapement"), pProfile->fontInfo.lfEscapement, lpszIniPathName);
	pProfile->fontInfo.lfOrientation	= (INT) GetPrivateProfileInt(lpszAppTitle, TEXT("Font.Orientation"), pProfile->fontInfo.lfOrientation, lpszIniPathName);
	pProfile->fontInfo.lfWeight			= (INT) GetPrivateProfileInt(lpszAppTitle, TEXT("Font.Weight"), pProfile->fontInfo.lfWeight, lpszIniPathName);
	pProfile->fontInfo.lfItalic			=(BYTE) GetPrivateProfileInt(lpszAppTitle, TEXT("Font.Italic"), (INT) pProfile->fontInfo.lfItalic, lpszIniPathName);
	pProfile->fontInfo.lfCharSet		=(BYTE) GetPrivateProfileInt(lpszAppTitle, TEXT("Font.CharSet"), (INT) pProfile->fontInfo.lfCharSet, lpszIniPathName);
	pProfile->fontInfo.lfOutPrecision	=(BYTE) GetPrivateProfileInt(lpszAppTitle, TEXT("Font.OutPrecision"), (INT) pProfile->fontInfo.lfOutPrecision, lpszIniPathName);
	pProfile->fontInfo.lfClipPrecision	=(BYTE) GetPrivateProfileInt(lpszAppTitle, TEXT("Font.ClipPrecision"), (INT) pProfile->fontInfo.lfClipPrecision, lpszIniPathName);
	pProfile->fontInfo.lfQuality		=(BYTE) GetPrivateProfileInt(lpszAppTitle, TEXT("Font.Quality"), (INT) pProfile->fontInfo.lfQuality, lpszIniPathName);
	pProfile->fontInfo.lfPitchAndFamily	=(BYTE) GetPrivateProfileInt(lpszAppTitle, TEXT("Font.PitchAndFamily"), (INT) pProfile->fontInfo.lfPitchAndFamily, lpszIniPathName);

	GetPrivateProfileString(lpszAppTitle, TEXT("Font.FaceName"),"Courier New", pProfile->fontInfo.lfFaceName, LF_FACESIZE, lpszIniPathName);

	GetPrivateProfileString(lpszAppTitle, TEXT("Color.Background"), "", color_str, 13, lpszIniPathName);
	if (sscanf(color_str, "%d,%d,%d", &red, &green, &blue) == 3) pProfile->color_background = RGB(red, green, blue);

	GetPrivateProfileString(lpszAppTitle, TEXT("Color.Capcode"), "", color_str, 13, lpszIniPathName);
	if (sscanf(color_str, "%d,%d,%d", &red, &green, &blue) == 3) pProfile->color_address = RGB(red, green, blue);

	GetPrivateProfileString(lpszAppTitle, TEXT("Color.FLEXPhase"), "", color_str, 13, lpszIniPathName);
	if (sscanf(color_str, "%d,%d,%d", &red, &green, &blue) == 3) pProfile->color_modetypebit = RGB(red, green, blue);

	GetPrivateProfileString(lpszAppTitle, TEXT("Color.Timestamp"), "", color_str, 13, lpszIniPathName);
	if (sscanf(color_str, "%d,%d,%d", &red, &green, &blue) == 3) pProfile->color_timestamp = RGB(red, green, blue);

	GetPrivateProfileString(lpszAppTitle, TEXT("Color.Numeric"), "", color_str, 13, lpszIniPathName);
	if (sscanf(color_str, "%d,%d,%d", &red, &green, &blue) == 3) pProfile->color_numeric = RGB(red, green, blue);

	GetPrivateProfileString(lpszAppTitle, TEXT("Color.Alphanumeric"), "", color_str, 13, lpszIniPathName);
	if (sscanf(color_str, "%d,%d,%d", &red, &green, &blue) == 3) pProfile->color_message = RGB(red, green, blue);

	GetPrivateProfileString(lpszAppTitle, TEXT("Color.FLEXBinary"), "", color_str, 13, lpszIniPathName);
	if (sscanf(color_str, "%d,%d,%d", &red, &green, &blue) == 3) pProfile->color_misc = RGB(red, green, blue);

	GetPrivateProfileString(lpszAppTitle, TEXT("Color.BitErrors"), "", color_str, 13, lpszIniPathName);
	if (sscanf(color_str, "%d,%d,%d", &red, &green, &blue) == 3) pProfile->color_biterrors = RGB(red, green, blue);

	GetPrivateProfileString(lpszAppTitle, TEXT("Color.FilterMatch"), "", color_str, 13, lpszIniPathName);
	if (sscanf(color_str, "%d,%d,%d", &red, &green, &blue) == 3) pProfile->color_filtermatch = RGB(red, green, blue);

	GetPrivateProfileString(lpszAppTitle, TEXT("Color.FilterLabel"), "", color_str, 13, lpszIniPathName);
	if (sscanf(color_str, "%d,%d,%d", &red, &green, &blue) == 3) pProfile->color_filterlabel[0] = RGB(red, green, blue);

	GetPrivateProfileString(lpszAppTitle, TEXT("Color.ACSEQNO"), "", color_str, 13, lpszIniPathName);
	if (sscanf(color_str, "%d,%d,%d", &red, &green, &blue) == 3) pProfile->color_ac_message_nr = RGB(red, green, blue);

	GetPrivateProfileString(lpszAppTitle, TEXT("Color.ACDBI"), "", color_str, 13, lpszIniPathName);
	if (sscanf(color_str, "%d,%d,%d", &red, &green, &blue) == 3) pProfile->color_ac_dbi = RGB(red, green, blue);

	GetPrivateProfileString(lpszAppTitle, TEXT("Color.MbSender"), "", color_str, 13, lpszIniPathName);
	if (sscanf(color_str, "%d,%d,%d", &red, &green, &blue) == 3) pProfile->color_mb_sender = RGB(red, green, blue);

	GetPrivateProfileString(lpszAppTitle, TEXT("LogFile"), "", pProfile->logfile, MAX_FILE_LEN, lpszIniPathName);
	pProfile->logfile_enabled = (INT) GetPrivateProfileInt(lpszAppTitle, TEXT("LogFileEnabled"), pProfile->logfile_enabled, lpszIniPathName);
	pProfile->logfile_use_date = (INT) GetPrivateProfileInt(lpszAppTitle, TEXT("LogFileUseDate"), pProfile->logfile_use_date, lpszIniPathName);

	pProfile->audioEnabled				= (INT) GetPrivateProfileInt(lpszAppTitle, TEXT("AudioEnabled"), pProfile->audioEnabled, lpszIniPathName);
	pProfile->audioDevice				= (INT) GetPrivateProfileInt(lpszAppTitle, TEXT("AudioDevice"), pProfile->audioDevice, lpszIniPathName);
	pProfile->audioSampleRate			= (INT) GetPrivateProfileInt(lpszAppTitle, TEXT("AudioSampleRate"), pProfile->audioSampleRate, lpszIniPathName);
	pProfile->audioConfig				= (INT) GetPrivateProfileInt(lpszAppTitle, TEXT("AudioConfiguration"), pProfile->audioConfig, lpszIniPathName);
	pProfile->audioThreshold[INDEX512]	= (INT) GetPrivateProfileInt(lpszAppTitle, TEXT("Threshold512"), pProfile->audioThreshold[INDEX512], lpszIniPathName);
	pProfile->audioThreshold[INDEX1200]	= (INT) GetPrivateProfileInt(lpszAppTitle, TEXT("Threshold1200"), pProfile->audioThreshold[INDEX1200], lpszIniPathName);
	pProfile->audioThreshold[INDEX1600]	= (INT) GetPrivateProfileInt(lpszAppTitle, TEXT("Threshold1600"), pProfile->audioThreshold[INDEX1600], lpszIniPathName);
	pProfile->audioThreshold[INDEX2400]	= (INT) GetPrivateProfileInt(lpszAppTitle, TEXT("Threshold2400"), pProfile->audioThreshold[INDEX2400], lpszIniPathName);
	pProfile->audioResync[INDEX512]		= (INT) GetPrivateProfileInt(lpszAppTitle, TEXT("Resync512"), pProfile->audioResync[INDEX512], lpszIniPathName);
	pProfile->audioResync[INDEX1200]	= (INT) GetPrivateProfileInt(lpszAppTitle, TEXT("Resync1200"), pProfile->audioResync[INDEX1200], lpszIniPathName);
	pProfile->audioResync[INDEX1600]	= (INT) GetPrivateProfileInt(lpszAppTitle, TEXT("Resync1600"), pProfile->audioResync[INDEX1600], lpszIniPathName);
	pProfile->audioResync[INDEX2400]	= (INT) GetPrivateProfileInt(lpszAppTitle, TEXT("Resync2400"), pProfile->audioResync[INDEX2400], lpszIniPathName);
	pProfile->audioCentering[INDEX512]	= (INT) GetPrivateProfileInt(lpszAppTitle, TEXT("Centering512"), pProfile->audioCentering[INDEX512], lpszIniPathName);
	pProfile->audioCentering[INDEX1200]	= (INT) GetPrivateProfileInt(lpszAppTitle, TEXT("Centering1200"), pProfile->audioCentering[INDEX1200], lpszIniPathName);
	pProfile->audioCentering[INDEX1600]	= (INT) GetPrivateProfileInt(lpszAppTitle, TEXT("Centering1600"), pProfile->audioCentering[INDEX1600], lpszIniPathName);
	pProfile->audioCentering[INDEX2400]	= (INT) GetPrivateProfileInt(lpszAppTitle, TEXT("Centering2400"), pProfile->audioCentering[INDEX2400], lpszIniPathName);

	SetAudioConfig(pProfile->audioConfig);	// set audio input configuration

	GetPrivateProfileString(lpszAppTitle, TEXT("EditFile"), "", pProfile->edit_save_file, MAX_FILE_LEN, lpszIniPathName);
	pProfile->pane1_size		= (INT) GetPrivateProfileInt(lpszAppTitle, TEXT("Pane1Size"), pProfile->pane1_size, lpszIniPathName);
	pProfile->pane2_size		= (INT) GetPrivateProfileInt(lpszAppTitle, TEXT("Pane2Size"), pProfile->pane2_size, lpszIniPathName);
	pProfile->ScrollSpeed		= (INT) GetPrivateProfileInt(lpszAppTitle, TEXT("ScrollSpeed"), pProfile->ScrollSpeed, lpszIniPathName);

	pProfile->ScreenColumns[0]	= (INT) GetPrivateProfileInt(lpszAppTitle, TEXT("ScreenCol1"), 1, lpszIniPathName);
	pProfile->ScreenColumns[1]	= (INT) GetPrivateProfileInt(lpszAppTitle, TEXT("ScreenCol2"), 2, lpszIniPathName);
	pProfile->ScreenColumns[2]	= (INT) GetPrivateProfileInt(lpszAppTitle, TEXT("ScreenCol3"), 3, lpszIniPathName);
	pProfile->ScreenColumns[3]	= (INT) GetPrivateProfileInt(lpszAppTitle, TEXT("ScreenCol4"), 4, lpszIniPathName);
	pProfile->ScreenColumns[4]	= (INT) GetPrivateProfileInt(lpszAppTitle, TEXT("ScreenCol5"), 5, lpszIniPathName);
	pProfile->ScreenColumns[5]	= (INT) GetPrivateProfileInt(lpszAppTitle, TEXT("ScreenCol6"), 6, lpszIniPathName);
	pProfile->ScreenColumns[6]	= (INT) GetPrivateProfileInt(lpszAppTitle, TEXT("ScreenCol7"), 7, lpszIniPathName);

	pProfile->stat_file_enabled = (INT) GetPrivateProfileInt(lpszAppTitle, TEXT("StatFileEnabled"), pProfile->stat_file_enabled, lpszIniPathName);

	GetPrivateProfileString(lpszAppTitle, TEXT("StatFile"), "", pProfile->stat_file, MAX_FILE_LEN, lpszIniPathName);

	pProfile->stat_file_use_date = (INT) GetPrivateProfileInt(lpszAppTitle, TEXT("StatFileUseDate"), pProfile->stat_file_use_date, lpszIniPathName);

	GetPrivateProfileString(lpszAppTitle, TEXT("ColLogfile"), "1234567", pProfile->ColLogfile, sizeof(pProfile->ColLogfile), lpszIniPathName);
	GetPrivateProfileString(lpszAppTitle, TEXT("ColFilterfile"), "1234567", pProfile->ColFilterfile, sizeof(pProfile->ColFilterfile), lpszIniPathName);
	GetPrivateProfileString(lpszAppTitle, TEXT("LogfilePath"), szLogPathName, pProfile->LogfilePath, MAX_PATH, lpszIniPathName);

	pProfile->LabelLog		= (INT) GetPrivateProfileInt(lpszAppTitle, TEXT("LabelLog"), pProfile->LabelLog, lpszIniPathName);
	pProfile->LabelNewline	= (INT) GetPrivateProfileInt(lpszAppTitle, TEXT("LabelNewline"), pProfile->LabelNewline, lpszIniPathName);
//	Notification			= (INT) GetPrivateProfileInt(lpszAppTitle, TEXT("Notification"), Notification, lpszIniPathName);
	pProfile->Linefeed		= (INT) GetPrivateProfileInt(lpszAppTitle, TEXT("Linefeed"), pProfile->Linefeed, lpszIniPathName);
	pProfile->Separator		= (INT) GetPrivateProfileInt(lpszAppTitle, TEXT("Separator"), pProfile->Separator, lpszIniPathName);
	pProfile->MonthNumber	= (INT) GetPrivateProfileInt(lpszAppTitle, TEXT("MonthNumber"), pProfile->MonthNumber, lpszIniPathName);
	pProfile->DateFormat	= (INT) GetPrivateProfileInt(lpszAppTitle, TEXT("DateFormat"), pProfile->DateFormat, lpszIniPathName);
	pProfile->BlockDuplicate= (INT) GetPrivateProfileInt(lpszAppTitle, TEXT("BlockDuplicate"), pProfile->BlockDuplicate, lpszIniPathName);
	pProfile->FlexTIME		= (INT) GetPrivateProfileInt(lpszAppTitle, TEXT("FlexTIME"), pProfile->FlexTIME, lpszIniPathName);
	pProfile->FlexGroupMode	= (INT) GetPrivateProfileInt(lpszAppTitle, TEXT("FlexGroupMode"), pProfile->FlexGroupMode, lpszIniPathName);
	pProfile->SystemTray	= (INT) GetPrivateProfileInt(lpszAppTitle, TEXT("SystemTray"), pProfile->SystemTray, lpszIniPathName);
	pProfile->SystemTrayRestore = (INT) GetPrivateProfileInt(lpszAppTitle, TEXT("SystemTrayRestore"), pProfile->SystemTrayRestore, lpszIniPathName);

	/***** Get SMTP settings *****/

	pProfile->SMTP = (INT) GetPrivateProfileInt("SMTP", TEXT("Enable"), 0, lpszIniPathName);
	GetPrivateProfileString("SMTP", TEXT("Host"), "", pProfile->szMailHost, MAIL_TEXT_LEN, lpszIniPathName);
	GetPrivateProfileString("SMTP", TEXT("HeloDomain"), "", pProfile->szMailHeloDomain, MAIL_TEXT_LEN, lpszIniPathName);
	GetPrivateProfileString("SMTP", TEXT("To"), "", pProfile->szMailTo, MAIL_TEXT_LEN * 5, lpszIniPathName);
	GetPrivateProfileString("SMTP", TEXT("From"), "", pProfile->szMailFrom, MAIL_TEXT_LEN, lpszIniPathName);
	GetPrivateProfileString("SMTP", TEXT("User"), "", pProfile->szMailUser, MAIL_TEXT_LEN, lpszIniPathName);
	GetPrivateProfileString("SMTP", TEXT("Password"), "", pProfile->szMailPassword, MAIL_TEXT_LEN, lpszIniPathName);
	pProfile->iMailPort = (INT) GetPrivateProfileInt("SMTP", TEXT("Port"), 25, lpszIniPathName);
	pProfile->nMailOptions = (INT) GetPrivateProfileInt("SMTP", TEXT("Options"), (MAIL_OPTION_ADDRESS | MAIL_OPTION_SUBJECT), lpszIniPathName);
	pProfile->ssl = (INT) GetPrivateProfileInt("SMTP", TEXT("SSL"), 0, lpszIniPathName);
	
	MailInit(Profile.szMailHost, Profile.szMailHeloDomain, Profile.szMailFrom, Profile.szMailTo, Profile.szMailUser, Profile.szMailPassword, Profile.iMailPort, Profile.nMailOptions);

	/***** Get Filter settings *****/

	pProfile->filterfile_enabled = (INT) GetPrivateProfileInt("Filter", TEXT("FilterFileEnabled"), pProfile->filterfile_enabled, lpszIniPathName);
	GetPrivateProfileString("Filter", TEXT("FilterFile"), "", pProfile->filterfile, FILTER_FILE_LEN, lpszIniPathName);
	pProfile->filterfile_use_date = (INT) GetPrivateProfileInt("Filter", TEXT("FilterFileUseDate"), pProfile->filterfile_use_date, lpszIniPathName);
	pProfile->filter_cmd_file_enabled = (INT) GetPrivateProfileInt("Filter", TEXT("FilterCmdFileEnabled"), pProfile->filter_cmd_file_enabled, lpszIniPathName);
	GetPrivateProfileString("Filter", TEXT("FilterCmdFile"), "", pProfile->filter_cmd, MAX_FILE_LEN, lpszIniPathName);
	GetPrivateProfileString("Filter", TEXT("FilterCmdArgs"), "", pProfile->filter_cmd_args, MAX_FILE_LEN, lpszIniPathName);
	pProfile->filter_default_type = (INT) GetPrivateProfileInt("Filter", TEXT("FilterDefaultType"), pProfile->filter_default_type, lpszIniPathName);

	if (ReadFilters(szFilterPathName, &Profile, false) == false)
	{
		if (MessageBox(ghWnd, "An error occured while loading filters.ini.\nTry to load a backup instead?", "PDW Filters", MB_ICONQUESTION | MB_OKCANCEL) == IDOK)
		{
			if (ReadFilters(szFilterBackup, &Profile, false) == false)
			{
				MessageBox(ghWnd, "Also failed, starting without filters...", "PDW Filters", MB_ICONINFORMATION);
			}
		}
	}
	return (TRUE);
} // end of GetPrivateProfileSettings


bool ReadFilters(char *szFilters, PPROFILE pProfile, bool bNew)
{
	char *pSearch=NULL;
	char *token;
	char szSepfiles[MAX_STR_LEN];
	char szLine[MAX_STR_LEN];				// Buffer for current line
	bool bError=false;
	int  i=0, pos, nLines=0, iFilterCount=0, iFilter=0;

	FILTER filter;

	#define FILTER_TYPE			1
	#define FILTER_CAPCODE		2
	#define FILTER_LABEL		3
	#define FILTER_TEXT			4
	#define FILTER_REJECT		5
	#define FILTER_WAVE			6
	#define FILTER_CMD			7
	#define FILTER_LABEL_COLOR	8
	#define FILTER_SEP			9
	#define FILTER_SEPFILES		10
	#define FILTER_HITCOUNTER	11

	FILE *pFile = fopen(szFilters ,"r");	// Check if FILTERS.INI exists

	if (pFile)
	{
		if (bNew) Profile.filters.clear();

		while (fgets(szLine, sizeof(szLine), pFile) != NULL)
		{
			szLine[strlen(szLine)-1] = '\0';	// Remove linebreaks

			if (nLines == 0)
			{
				if (strcmp(szLine, "[Filter]") != 0)
				{
					bError=true;
				}
			}
			else if ((nLines == 1) || (nLines == 3))
			{
				if (strcmp(szLine, "") != 0)
				{
					bError=true;
				}
			}
			else if (nLines == 2)
			{
				if (strncmp(szLine, "FilterCount=", 12) != 0)
				{
					bError=true;
				}
				else
				{
					memmove(szLine, &szLine[12], strlen(szLine));
					iFilterCount=atoi(szLine);
				}
			}
			else
			{
				// Filter1=1,"CAPCODE","LABEL","TEXT",2,0,0,0,1,"sep1.txt;sep2.txt",hit,date,time

				if (strncmp(szLine, "Filter", 6) == 0)
				{
					pos = strchr(szLine, '=') - szLine+1;	// Find first item
				}
				else pos=0;

				if (!isdigit(szLine[pos]) || szLine[pos+1] != ',')
				{
					bError=true;
				}
				else
				{
					ZeroMemory(&filter, sizeof(filter));

					for (int item=1; item<=FILTER_HITCOUNTER; item++, i=0)	// Loop trough items
					{
						switch (item)
						{
							case FILTER_TYPE:			// Get filter type

							switch (szLine[pos])
							{
								case '1': filter.type = FLEX_FILTER;	break;
								case '2': filter.type = POCSAG_FILTER;	break;
								case '3': filter.type = TEXT_FILTER;	break;
								case '4': filter.type = ERMES_FILTER;	break;
								case '5': filter.type = ACARS_FILTER;	break;
								case '6': filter.type = MOBITEX_FILTER;	break;
								default : filter.type = UNUSED_FILTER;	break;
							}
							break;

							case FILTER_CAPCODE:		// Get capcode

							if (szLine[pos+1] != '"')
							{
								strncpy(filter.capcode, &szLine[pos+1], strlen(szLine));
								filter.capcode[strchr(filter.capcode, '"') - filter.capcode] = 0;
								pos += strlen(filter.capcode) + 1;	// + 1 to start at last "

								// Convert 9-digit short addresses to 7-digits
								if ((strlen(filter.capcode) == 9) && memcmp(filter.capcode, "00", 2) == 0)
								{
									if (CompareCapcodes(filter.capcode, "002101249") < 0) memmove(filter.capcode, &filter.capcode[2], strlen(filter.capcode));
//									if (atoi(filter.capcode) < 2101249) memmove(filter.capcode, &filter.capcode[2], strlen(filter.capcode));
//									memmove(filter.capcode, &filter.capcode[2], strlen(filter.capcode));
								}
							}
							else filter.capcode[0] = 0;

							break;

							case FILTER_LABEL:			// Get label

							if (szLine[pos+1] != '"')
							{
								strncpy(filter.label, &szLine[pos+1], strlen(szLine));
								filter.label[strchr(filter.label, '"') - filter.label] = 0;
								pos += strlen(filter.label) + 1;	// + 1 to start at last "
							}
							else filter.label[0] = 0;

							break;

							case FILTER_TEXT:			// Get text

							if (szLine[pos+1] != '"')
							{
								strncpy(filter.text, &szLine[pos+1], strlen(szLine));
								filter.text[strchr(filter.text, '"') - filter.text] = 0;
								pos += strlen(filter.text) + 1;	// + 1 to start at last "
							}
							else filter.text[0] = 0;

							break;

							case FILTER_REJECT:		// Get reject / monitor_only

							iTEMP = szLine[pos] - '0';

							filter.reject		= (iTEMP & 0x01) != 0;
							filter.monitor_only	= (iTEMP & 0x02) != 0;

							break;

							case FILTER_WAVE:		// Get wave_number

							while (szLine[pos] != ',') szTEMP[i++] = szLine[pos++];

							szTEMP[i] = 0;
							filter.wave_number = atoi(szTEMP);

							break;

							case FILTER_CMD:		// Get cmd_enabled

							filter.cmd_enabled = szLine[pos] - '0';

							break;

							case FILTER_LABEL_COLOR:	// Get label_enabled / color

							while (szLine[pos] != ',') szTEMP[i++] = szLine[pos++];

							szTEMP[i] = 0;
							iTEMP = atoi(szTEMP);

							filter.label_enabled = iTEMP ? 1 : 0;
							filter.label_color   = iTEMP ? iTEMP-1 : 0;

							break;

							case FILTER_SEP:		// Get sep_filterfile_en / smtp / match_exact_msg

							while (szLine[pos] != ',') szTEMP[i++] = szLine[pos++];

							szTEMP[i] = 0;
							iTEMP = atoi(szTEMP);

							filter.sep_filterfile_en   = (iTEMP & 0x01) != 0;
							filter.smtp                = (iTEMP & 0x08) != 0;
							filter.match_exact_msg     = (iTEMP & 0x10) != 0;

							break;
							
							case FILTER_SEPFILES:	// Get separate filterfile(s)

							if (szLine[pos+1] != '"')
							{
								strncpy(szSepfiles, &szLine[pos+1], strlen(szLine));
								szSepfiles[strchr(szSepfiles, '"') - szSepfiles] = 0;
								pos += strlen(szSepfiles);
								token = strtok(szSepfiles, ";");

								do
								{
									strcpy(filter.sep_filterfile[filter.sep_filterfiles++], token);
								}
								while (token = strtok(NULL, ";"));
							}
							break;

							case FILTER_HITCOUNTER:	// Get hitcounter

							if (token = strtok(&szLine[pos], ",")) filter.hitcounter = atoi(token);
							if (token = strtok(NULL, ",")) strcpy(filter.lasthit_date, token);
							if (token = strtok(NULL, ",")) strcpy(filter.lasthit_time, token);

							break;
						}
	
						token = strchr(&szLine[pos], ',');

						if (token)
						{
							pos = token - szLine+1; // set position to next item
						}
						else break;
					}
					Profile.filters.insert(Profile.filters.begin() + iFilter, filter);
					iFilter++;
				}
			}
			if (bError) return(false);
			else nLines++;
		}
		fclose(pFile);
		pFile=NULL;
	}
	else
	{
		if (bNew) return(true);

		MessageBox(ghWnd, "FILTERS.INI not found!", "PDW Filters", MB_ICONINFORMATION);
		return(false);
	}

	if (bNew)
	{
		DeleteFile(szFilters);
		return(true);
	}
	else if (iFilterCount)
	{
		CopyFile(szFilters, szFilterBackup, false);
		bUpdateFilters = true;	// PH: Update filters.ini in UpdateFilters() when IDLE
	}
	return(true);
}

void WriteSettings()
{
	if (GetFileAttributes(szIniPathName) & FILE_ATTRIBUTE_READONLY)
	{
		SetFileAttributes(szIniPathName, FILE_ATTRIBUTE_NORMAL);
	}

	FILE* pFile = fopen(szIniPathName, "w+");

	if (pFile)
	{
		fprintf(pFile, "[PDW]\n");
		fprintf(pFile, "xPos=%i\n",						Profile.xPos);
		fprintf(pFile, "yPos=%i\n",						Profile.yPos);
		fprintf(pFile, "xSize=%i\n",					Profile.xSize);
		fprintf(pFile, "ySize=%i\n",					Profile.ySize);
		fprintf(pFile, "Maximize=%i\n",					Profile.maximize_flg);
		fprintf(pFile, "LangIndex=%i\n",				Profile.lang_mi_index);
		fprintf(pFile, "ConfirmExit=%i\n",				Profile.confirmExit);
		fprintf(pFile, "FilterBeep=%i\n",				Profile.filterbeep);
		fprintf(pFile, "FilterWindowOnly=%i\n",			Profile.filterwindowonly);
		fprintf(pFile, "DecodePocsag=%i\n",				Profile.decodepocsag);
		fprintf(pFile, "Pocsag512=%i\n",				Profile.pocsag_512);
		fprintf(pFile, "Pocsag1200=%i\n",				Profile.pocsag_1200);
		fprintf(pFile, "Pocsag2400=%i\n",				Profile.pocsag_2400);
		fprintf(pFile, "PocsagFNU=%i\n",				Profile.pocsag_fnu);
		fprintf(pFile, "PocsagShowBoth=%i\n",			Profile.pocsag_showboth);
		fprintf(pFile, "DecodeFlex=%i\n",				Profile.decodeflex);
		fprintf(pFile, "ShowInstr=%i\n",				Profile.showinstr);
		fprintf(pFile, "ConvertInstr=%i\n",				Profile.convert_si);
		fprintf(pFile, "ShowCFS=%i\n",					Profile.show_cfs);
		fprintf(pFile, "ShowRejectBlocked=%i\n",		Profile.show_rejectblocked);
		fprintf(pFile, "Flex1600=%i\n",					Profile.flex_1600);
		fprintf(pFile, "Flex3200=%i\n",					Profile.flex_3200);
		fprintf(pFile, "Flex6400=%i\n",					Profile.flex_6400);
		fprintf(pFile, "AcarsParityCheck=%i\n",			Profile.acars_parity_check);
		fprintf(pFile, "PocsagFlex=%i\n",				Profile.monitor_paging);
		fprintf(pFile, "Acars=%i\n",					Profile.monitor_acars);
		fprintf(pFile, "Mobitex=%i\n",					Profile.monitor_mobitex);
		fprintf(pFile, "Ermes=%i\n",					Profile.monitor_ermes);
		fprintf(pFile, "CFS=%i\n",						mb.cfs);
		fprintf(pFile, "FRSYNC=%i\n",					mb.frsync);
		fprintf(pFile, "BTSYNC=%i\n",					mb.bitsync);
		fprintf(pFile, "BRATE=%i\n",					mb.bitrate);
		fprintf(pFile, "BITSCR=%i\n",					mb.bitscr);
		fprintf(pFile, "RAMNET=%i\n",					mb.ramnet);
		fprintf(pFile, "MINMSG=%i\n",					mb.min_msg_len);
		fprintf(pFile, "SHOW=%i\n",						mb.show);
		fprintf(pFile, "ShowTone=%i\n",					Profile.showtone);
		fprintf(pFile, "ShowNumeric=%i\n",				Profile.shownumeric);
		fprintf(pFile, "ShowBinary=%i\n",				Profile.showmisc);
		fprintf(pFile, "FilterWindowColors=%i\n",		Profile.FilterWindowColors);
		fprintf(pFile, "FilterWindowExtra=%i\n",		Profile.FilterWindowExtra);
		fprintf(pFile, "4LevelInterface=%i\n",			Profile.fourlevel);
		fprintf(pFile, "InvertData=%i\n",				Profile.invert);
		fprintf(pFile, "Percent=%i\n",					Profile.percent);
		fprintf(pFile, "ComPortEnabled=%i\n",			Profile.comPortEnabled);
		fprintf(pFile, "ComPortRS232=%i\n",				Profile.comPortRS232);
		fprintf(pFile, "ComPort=%s%d\n",				(bWin9x && (Profile.comPort == 5)) ? "Custom", "" : "COM", Profile.comPort);
		fprintf(pFile, "ComPortAddr=0x%X\n",			Profile.comPortAddr);
		fprintf(pFile, "ComPortIRQ=%d\n",				Profile.comPortIRQ);
		fprintf(pFile, "Font.Height=%i\n",				Profile.fontInfo.lfHeight);
		fprintf(pFile, "Font.Width=%i\n",				Profile.fontInfo.lfWidth);
		fprintf(pFile, "Font.Escapement=%i\n",			Profile.fontInfo.lfEscapement);
		fprintf(pFile, "Font.Orientation=%i\n",			Profile.fontInfo.lfOrientation);
		fprintf(pFile, "Font.Weight=%i\n",				Profile.fontInfo.lfWeight);
		fprintf(pFile, "Font.Italic=%i\n",				Profile.fontInfo.lfItalic);
		fprintf(pFile, "Font.CharSet=%i\n",				Profile.fontInfo.lfCharSet);
		fprintf(pFile, "Font.OutPrecision=%i\n",		Profile.fontInfo.lfOutPrecision);
		fprintf(pFile, "Font.ClipPrecision=%i\n",		Profile.fontInfo.lfClipPrecision);
		fprintf(pFile, "Font.Quality=%i\n",				Profile.fontInfo.lfQuality);
		fprintf(pFile, "Font.PitchAndFamily=%i\n",		Profile.fontInfo.lfPitchAndFamily);
		fprintf(pFile, "Font.FaceName=%s\n",			Profile.fontInfo.lfFaceName);

		fprintf(pFile, "Color.Background=%d,%d,%d\n",	GetRValue(Profile.color_background),
														GetGValue(Profile.color_background),
														GetBValue(Profile.color_background));
		fprintf(pFile, "Color.Capcode=%d,%d,%d\n",		GetRValue(Profile.color_address),
														GetGValue(Profile.color_address),
														GetBValue(Profile.color_address));
		fprintf(pFile, "Color.FLEXPhase=%d,%d,%d\n",	GetRValue(Profile.color_modetypebit),
														GetGValue(Profile.color_modetypebit),
														GetBValue(Profile.color_modetypebit));
		fprintf(pFile, "Color.Timestamp=%d,%d,%d\n",	GetRValue(Profile.color_timestamp),
														GetGValue(Profile.color_timestamp),
														GetBValue(Profile.color_timestamp));
		fprintf(pFile, "Color.Numeric=%d,%d,%d\n",		GetRValue(Profile.color_numeric),
														GetGValue(Profile.color_numeric),
														GetBValue(Profile.color_numeric));
		fprintf(pFile, "Color.Alphanumeric=%d,%d,%d\n",	GetRValue(Profile.color_message),
														GetGValue(Profile.color_message),
														GetBValue(Profile.color_message));
		fprintf(pFile, "Color.FLEXBinary=%d,%d,%d\n",	GetRValue(Profile.color_misc),
														GetGValue(Profile.color_misc),
														GetBValue(Profile.color_misc));
		fprintf(pFile, "Color.BitErrors=%d,%d,%d\n",	GetRValue(Profile.color_biterrors),
														GetGValue(Profile.color_biterrors),
														GetBValue(Profile.color_biterrors));
		fprintf(pFile, "Color.FilterMatch=%d,%d,%d\n",	GetRValue(Profile.color_filtermatch),
														GetGValue(Profile.color_filtermatch),
														GetBValue(Profile.color_filtermatch));
		fprintf(pFile, "Color.FilterLabel=%d,%d,%d\n",	GetRValue(Profile.color_filterlabel[0]),
														GetGValue(Profile.color_filterlabel[0]),
														GetBValue(Profile.color_filterlabel[0]));
		fprintf(pFile, "Color.ACSEQNO=%d,%d,%d\n",		GetRValue(Profile.color_ac_message_nr),
														GetGValue(Profile.color_ac_message_nr),
														GetBValue(Profile.color_ac_message_nr));
		fprintf(pFile, "Color.ACDBI=%d,%d,%d\n",		GetRValue(Profile.color_ac_dbi),
														GetGValue(Profile.color_ac_dbi),
														GetBValue(Profile.color_ac_dbi));
		fprintf(pFile, "Color.MbSender=%d,%d,%d\n",		GetRValue(Profile.color_mb_sender),
														GetGValue(Profile.color_mb_sender),
														GetBValue(Profile.color_mb_sender));

		fprintf(pFile, "LogFileEnabled=%i\n",			Profile.logfile_enabled);
		fprintf(pFile, "LogFile=%s\n",					Profile.logfile);
		fprintf(pFile, "LogFileUseDate=%i\n",			Profile.logfile_use_date);
		fprintf(pFile, "AudioEnabled=%i\n",				Profile.audioEnabled);
		fprintf(pFile, "AudioDevice=%i\n",				Profile.audioDevice);
		fprintf(pFile, "AudioSampleRate=%i\n",			Profile.audioSampleRate);
		fprintf(pFile, "AudioConfiguration=%i\n",		Profile.audioConfig);
		fprintf(pFile, "Threshold512=%i\n",				Profile.audioThreshold[INDEX512]);
		fprintf(pFile, "Threshold1200=%i\n",			Profile.audioThreshold[INDEX1200]);
		fprintf(pFile, "Threshold1600=%i\n",			Profile.audioThreshold[INDEX1600]);
		fprintf(pFile, "Threshold2400=%i\n",			Profile.audioThreshold[INDEX2400]);
		fprintf(pFile, "Resync512=%i\n",				Profile.audioResync[INDEX512]);
		fprintf(pFile, "Resync1200=%i\n",				Profile.audioResync[INDEX1200]);
		fprintf(pFile, "Resync1600=%i\n",				Profile.audioResync[INDEX1600]);
		fprintf(pFile, "Resync2400=%i\n",				Profile.audioResync[INDEX2400]);
		fprintf(pFile, "Centering512=%i\n",				Profile.audioCentering[INDEX512]);
		fprintf(pFile, "Centering1200=%i\n",			Profile.audioCentering[INDEX1200]);
		fprintf(pFile, "Centering1600=%i\n",			Profile.audioCentering[INDEX1600]);
		fprintf(pFile, "Centering2400=%i\n",			Profile.audioCentering[INDEX2400]);
		fprintf(pFile, "EditFile=%s\n",					Profile.edit_save_file);
		fprintf(pFile, "Pane1Size=%i\n",				Profile.pane1_size);
		fprintf(pFile, "Pane2Size=%i\n",				Profile.pane2_size);
		fprintf(pFile, "ScrollSpeed=%i\n",				Profile.ScrollSpeed);
		fprintf(pFile, "ScreenCol1=%i\n",				Profile.ScreenColumns[0]);
		fprintf(pFile, "ScreenCol2=%i\n",				Profile.ScreenColumns[1]);
		fprintf(pFile, "ScreenCol3=%i\n",				Profile.ScreenColumns[2]);
		fprintf(pFile, "ScreenCol4=%i\n",				Profile.ScreenColumns[3]);
		fprintf(pFile, "ScreenCol5=%i\n",				Profile.ScreenColumns[4]);
		fprintf(pFile, "ScreenCol6=%i\n",				Profile.ScreenColumns[5]);
		fprintf(pFile, "ScreenCol7=%i\n",				Profile.ScreenColumns[6]);
		fprintf(pFile, "StatFileEnabled=%i\n",			Profile.stat_file_enabled);
		fprintf(pFile, "StatFile=%s\n",					Profile.stat_file);
		fprintf(pFile, "StatFileUseDate=%i\n",			Profile.stat_file_use_date);
		fprintf(pFile, "ColLogfile=%s\n",				Profile.ColLogfile);
		fprintf(pFile, "ColFilterfile=%s\n",			Profile.ColFilterfile);
		if (strcmp(Profile.LogfilePath, szLogPathName) != 0) // PH: Only if != default
		{
			fprintf(pFile, "LogfilePath=%s\n",			Profile.LogfilePath);
		}
		fprintf(pFile, "LabelLog=%i\n",					Profile.LabelLog);
		fprintf(pFile, "LabelNewline=%i\n",				Profile.LabelNewline);
//		fprintf(pFile, "Notification=%i\n",				Notification);
		fprintf(pFile, "Linefeed=%i\n",					Profile.Linefeed);
		fprintf(pFile, "Separator=%i\n",				Profile.Separator);
		fprintf(pFile, "MonthNumber=%i\n",				Profile.MonthNumber);
		fprintf(pFile, "DateFormat=%i\n",				Profile.DateFormat);
		fprintf(pFile, "BlockDuplicate=%i\n",			Profile.BlockDuplicate);
		fprintf(pFile, "FlexTIME=%i\n",					Profile.FlexTIME);
		fprintf(pFile, "FlexGroupMode=%i\n",			Profile.FlexGroupMode);
		fprintf(pFile, "SystemTray=%i\n",				Profile.SystemTray);
		fprintf(pFile, "SystemTrayRestore=%i\n",		Profile.SystemTrayRestore);

		fprintf(pFile, "\n[SMTP]\n");
		fprintf(pFile, "Enable=%i\n",					Profile.SMTP);
		fprintf(pFile, "Host=%s\n",						Profile.szMailHost);
		fprintf(pFile, "HeloDomain=%s\n",				Profile.szMailHeloDomain);
		fprintf(pFile, "To=%s\n",						Profile.szMailTo);
		fprintf(pFile, "From=%s\n",						Profile.szMailFrom);
		fprintf(pFile, "User=%s\n",						Profile.szMailUser);
		fprintf(pFile, "Password=%s\n",					Profile.szMailPassword);
		fprintf(pFile, "Port=%i\n",						Profile.iMailPort);
		fprintf(pFile, "Options=%i\n",					Profile.nMailOptions);
		fprintf(pFile, "SSL=%i\n",                      Profile.ssl);

		fprintf(pFile, "\n[Filter]\n");
		fprintf(pFile, "FilterFileEnabled=%i\n",		Profile.filterfile_enabled);
		fprintf(pFile, "FilterFile=%s\n",				Profile.filterfile);
		fprintf(pFile, "FilterFileUseDate=%i\n",		Profile.filterfile_use_date);
		fprintf(pFile, "FilterCmdFileEnabled=%i\n",		Profile.filter_cmd_file_enabled);
		fprintf(pFile, "FilterCmdFile=%s\n",			Profile.filter_cmd);
		sprintf(szTEMP,"FilterCmdArgs=\"%s\"\n",		Profile.filter_cmd_args);
		fwrite(szTEMP, strlen(szTEMP), 1, pFile);
		fprintf(pFile, "FilterDefaultType=%i\n",		Profile.filter_default_type);

		fclose(pFile);
		pFile=NULL;
	}
} // end of WritePrivateProfileSettings


void WriteFilters(PPROFILE pProfile, int backup)
{
	char szLine[256];
	char szFilename[MAX_PATH];
	char szPathname[MAX_PATH];
	char ext[10];

	if (GetFileAttributes(szFilterPathName) & FILE_ATTRIBUTE_READONLY)
	{
		SetFileAttributes(szFilterPathName, FILE_ATTRIBUTE_NORMAL);
	}
	
	if (backup)
	{
		strcpy(szFilename, szFilterPathName);
		sprintf(ext, ".%03i", backup);
		ChangeFileExtension(szFilename, ext);
		CopyFile(szFilterPathName, szFilename, false);
	}
	if ((pFiltersFile = fopen(szFilterPathName, "w+")) != NULL)
	{
		fprintf(pFiltersFile, "[Filter]\n\nFilterCount=%i\n\n", pProfile->filters.size());
		
		for (int index=0; index<pProfile->filters.size(); index++)
		{
			sprintf(szLine, "%1d", pProfile->filters[index].type);

			strcat(szLine, ",\"");

			if (pProfile->filters[index].type != 3)
			{
				strcat(szLine, pProfile->filters[index].capcode);
			}
			strcat(szLine, "\",\"");

			strcat(szLine, pProfile->filters[index].label);

			strcat(szLine, "\",\"");

			strcat(szLine, pProfile->filters[index].text);

			strcat(szLine, "\",");

			if (pProfile->filters[index].reject) strcat(szLine, "1");
			else if (pProfile->filters[index].monitor_only) strcat(szLine, "2");
			else strcat(szLine, "0");

			strcat(szLine, ",");

			sprintf(szTEMP, "%i", pProfile->filters[index].wave_number);
			strcat(szLine, szTEMP);

			strcat(szLine, ",");

			sprintf(szTEMP, "%i", pProfile->filters[index].cmd_enabled);
			strcat(szLine, szTEMP);

			strcat(szLine, ",");

			if (pProfile->filters[index].label_enabled)
			{
				sprintf(szTEMP, "%i", pProfile->filters[index].label_color+1);
				strcat(szLine, szTEMP);
			}
			else strcat(szLine, "0");

			strcat(szLine, ",");

			iTEMP=0;

			if (!pProfile->filters[index].reject)
			{
				if (pProfile->filters[index].sep_filterfile_en)	iTEMP += 1;
				if (pProfile->filters[index].smtp)				iTEMP += 8;
				if (pProfile->filters[index].match_exact_msg)	iTEMP += 16;
			}

			sprintf(szTEMP, "%i", iTEMP);
			strcat(szLine, szTEMP);

			strcat(szLine, ",\"");

			if (pProfile->filters[index].sep_filterfile_en)
			{
				for (int j=0; j<pProfile->filters[index].sep_filterfiles; j++)
				{
					if (!pProfile->filters[index].sep_filterfile[j][0]) continue;
					else if (j) strcat(szLine, ";");

					GetFilePath(pProfile->filters[index].sep_filterfile[j], szPathname, sizeof(szPathname));

					if (stricmp(Profile.LogfilePath, szPathname) == 0)
					{
						GetFileName(pProfile->filters[index].sep_filterfile[j], szFilename, sizeof(szFilename));
						strcat(szLine, szFilename);
					}
					else
					{
						strcat(szLine, pProfile->filters[index].sep_filterfile[j]);
					}
				}
			}

			strcat(szLine, "\"");

			if (pProfile->filters[index].hitcounter)
			{
				strcat(szLine, ",");

				sprintf(szTEMP, "%i,%s,%s", pProfile->filters[index].hitcounter,
											pProfile->filters[index].lasthit_date,
											pProfile->filters[index].lasthit_time);
				strcat(szLine, szTEMP);
			}
			fprintf(pFiltersFile, "%s\n", szLine);
		}
		fclose(pFiltersFile);
	}
}


bool LoadDriver(void)	// HWi
{
	SLICER_IN_STR  slicer_in;
	SLICER_OUT_STR slicer_out;
	DWORD bytesReturned;

	char szErrorMSG[128];
	
	if (bWin9x && !Profile.comPortRS232)	// Using Windows 95/98/ME
	{
		hDriver = CreateFile("\\\\.\\COMPRT.VXD", 0, 0, 0, 0, FILE_FLAG_DELETE_ON_CLOSE, 0);

		if (hDriver == INVALID_HANDLE_VALUE)
		{
			MessageBox(ghWnd, "Unable to Load Comport Driver!", "PDW Driver", MB_ICONWARNING);
			return(false);
		}
		slicer_in.irq = Profile.comPortIRQ;
		slicer_in.com_port = Profile.comPortAddr;
		memset(&slicer_out, 0, sizeof(slicer_out));

		if (!DeviceIoControl(hDriver, 1, (LPVOID)&slicer_in, 1, (LPVOID)&slicer_out, 1, &bytesReturned, NULL))
		{
			MessageBox(ghWnd,"Unknown Error from Comport Driver!", "PDW Driver", MB_ICONWARNING);
			CloseHandle(hDriver); // Dynamically UNLOAD the Virtual device driver
			nDriverLoaded = DRIVER_NOT_LOADED;
			return(false);
		}
		nDriverLoaded = DRIVER_VXD_LOADED;	    // HWi must be only TRUE when all is OK
	}
	else	// Using RS232 or Windows 2000/XP/Vista/7
	{
		slicer_in.irq = Profile.comPortIRQ;
//		Use comport number instead of comport address
		slicer_in.com_port = Profile.comPort;
		hDriver = (HANDLE) rs232_connect(&slicer_in, &slicer_out);

		if (hDriver != RS232_SUCCESS)
		{
			MessageBox(ghWnd,"Unable to open the selected Comport", "PDW Driver", MB_ICONWARNING);
//			OUTPUTDEBUGMSG((("Error: rs232_connect() = %d\n"), hDriver));		
			nDriverLoaded = DRIVER_NOT_LOADED;
			return(false);
		}
		nDriverLoaded = DRIVER_COM_LOADED;	    // HWi must be only TRUE when all is OK
	}

	if (slicer_out.bufsize < 0L)  // error?
	{
		switch (slicer_out.bufsize)
		{
			case -1L:
				strcpy(szErrorMSG, "Comport driver not previously unloaded!");
			break;

			case -2L:
				strcpy(szErrorMSG, "Error attaching to Comport!");
			break;

			case -3L:
				strcpy(szErrorMSG, "Error Allocating Memory for Comport driver!");
			break;

			case -4L:
				strcpy(szErrorMSG, "Error Allocating Memory for Comport driver!");
			break;

			default:
				strcpy(szErrorMSG, "Unknown Error from Comport driver!");
			break;
		}
		MessageBox(ghWnd, szErrorMSG, "PDW Driver", MB_ICONWARNING);

		UnloadDriver(); // HWi

		return(false);
	}

	freqdata	= slicer_out.freqdata;
	linedata	= slicer_out.linedata;
	cpstn		= slicer_out.cpstn;
	bufsize		= slicer_out.bufsize;

//	bDriverLoaded = true;	    // HWi must be only TRUE when all is OK

	return(true);
} // end of LoadDriver


void UnloadDriver(void)	//HWi made this a function to clean up the code
{
	if (nDriverLoaded)
	{
		KillTimer(ghWnd, PDW_TIMER);

		if (nDriverLoaded == DRIVER_VXD_LOADED)
		{
			CloseHandle(hDriver);
		}
		else 
		{
			rs232_disconnect();		// HWi 
		}
		nDriverLoaded=DRIVER_NOT_LOADED;
	}
}


// This is used to get the Edit menu - 'Save' filename.
bool GetEditSaveName(HWND hWnd)
{
	char szFileSave[MAX_FILE_LEN], szFileTitle[256];
	static char szLog[] = {"TXT Files (*.txt)\0*.txt\0All Files (*.*)\0*.*\0\0"};

	OPENFILENAME ofn = {0};

	strcpy(szFileSave, "*.txt");

	ofn.lStructSize = sizeof(OPENFILENAME);
	ofn.hwndOwner = hWnd;
	ofn.lpstrFilter = szLog;
	ofn.lpstrCustomFilter = NULL;
	ofn.nMaxCustFilter = 0L;
	ofn.nFilterIndex = 1;
	ofn.lpstrFile = szFileSave;
	ofn.nMaxFile = sizeof(szFileSave)-5;
	ofn.lpstrFileTitle = szFileTitle;
	ofn.nMaxFileTitle = sizeof(szFileTitle);
	ofn.lpstrInitialDir = szPath;
	ofn.lpstrTitle = "Save Clipboard";
	ofn.Flags = OFN_HIDEREADONLY;
	ofn.nFileOffset = 0;
	ofn.nFileExtension = 0;
	ofn.lpstrDefExt = NULL;

	if (GetSaveFileName(&ofn))
	{
		strcpy(Profile.edit_save_file,szFileSave);
		return(true);
	}
	return(false);
}


// HWi added listview Drag&Drop stuff
void InitListControl(HWND hDlg)
{
	int  width ;
	RECT rect ;
	LVCOLUMN lvc = { 0 } ;

	hListView = GetDlgItem(hDlg, IDC_FILTERS) ;

	// Initialize the LVCOLUMN structure. 
 	lvc.mask = LVCF_FMT | LVCF_WIDTH ; // | LVCF_TEXT /*| LVCF_SUBITEM*/ ;
	lvc.fmt  = LVCFMT_LEFT; 

	GetWindowRect(hListView, &rect) ;

	width = rect.right - rect.left;
	width-= GetSystemMetrics(SM_CXVSCROLL);
	width-= GetSystemMetrics(SM_CYFIXEDFRAME) * 2;

	lvc.cx = width ; // * 8/16 ;

	ListView_InsertColumn(hListView, 0, &lvc) ;
#if 0
	lvc.pszText = "Address" ; 
	lvc.cx = width * 1/16 ;
	ListView_InsertColumn(hListView, 1, &lvc) ;
	lvc.pszText = "Cmd" ; 
	lvc.cx = width * 1/16 ;
	ListView_InsertColumn(hListView, 2, &lvc) ;
	lvc.pszText = "Desc" ; 
	lvc.cx = width * 1/16 ;
	ListView_InsertColumn(hListView, 3, &lvc) ;
	lvc.pszText = "Sep" ; 
	lvc.cx = width * 1/16 ;
	ListView_InsertColumn(hListView, 4, &lvc) ;
	lvc.pszText = "Html" ; 
	lvc.cx = width * 1/16 ;
	ListView_InsertColumn(hListView, 5, &lvc) ;
	lvc.pszText = "Wave" ; 
	lvc.cx = width * 3/16 ;
	ListView_InsertColumn(hListView, 6, &lvc) ;
#endif // 0
}


bool WINAPI InsertListViewItem(char *szLine, int nItem)
{ 
	LVITEM lvitem = { 0 } ;
 
	// Add Type
	lvitem.mask = LVIF_TEXT |LVIF_PARAM;
	lvitem.iItem = nItem;
	lvitem.iSubItem = 0;
	lvitem.lParam = nItem;
	lvitem.pszText = szLine;
	ListView_InsertItem(hListView, &lvitem);
	return(true);
} 


void OnBeginDrag(NMHDR* pNMHDR) 
{
	int			iPos, iHeight, items=0;
	bool		bFirst=true;
	POINT		p, pt;
	IMAGEINFO	imf;
	HIMAGELIST	hOneImageList, hTempImageList;

	if (!CheckSelection(false)) return;

	p.x = 8;
	p.y = 8;

	// Create a drag-image for all selected items
	iPos = ListView_GetNextItem(hListView, -1, LVNI_SELECTED);

	while (iPos != -1)
	{
		if (ListView_EnsureVisible(hListView, iPos, TRUE))
		{
			items++ ;
			if (bFirst)
			{
				// For the first selected item, we simply create a single-line drag image
				hDragImageList = ListView_CreateDragImage(hListView, iPos, &p);
				ImageList_GetImageInfo(hDragImageList, 0, &imf);
				iHeight = imf.rcImage.bottom;
				bFirst = false;
			}
			else 
			{	// For the rest selected items, we create a single-line drag image,
				// then append it to the bottom of the complete drag image
				hOneImageList  = ListView_CreateDragImage(hListView, iPos, &p);
				hTempImageList = ImageList_Merge(hDragImageList, 0, hOneImageList, 0, 0, iHeight);
				ImageList_Destroy(hDragImageList);
				ImageList_Destroy(hOneImageList);
				hDragImageList = hTempImageList;
				ImageList_GetImageInfo(hDragImageList, 0, &imf);
				iHeight = imf.rcImage.bottom;
			}
		}
		iPos = ListView_GetNextItem(hListView, iPos, LVNI_SELECTED);
		// Don't make the image larger then 5 items.
		if (items > 5) break;
	}

	// Now we can initialize then start the drag action
	ImageList_BeginDrag(hDragImageList, 0, 0, 0);

	pt = ((NM_LISTVIEW*) (pNMHDR))->ptAction;
	ClientToScreen(hListView, &pt);
	ImageList_DragEnter(GetDesktopWindow(), pt.x, pt.y);
	bDragging = true;

	// Don't forget to capture the mouse
	SetCapture(hFilterDlg);
}

void OnMouseMove(UINT nFlags, int x, int y)
{
	int nIndex ;
	POINT p, pt;
	HWND pDropWnd ;

	p.x = x;
	p.y = y;

	ClientToScreen(hFilterDlg, &p);
	ImageList_DragMove(p.x, p.y);

	// Get the HWND pointer of the window that is under the mouse cursor
	pDropWnd = WindowFromPoint(p);
	// If we are hovering over a ListView we need to adjust the highlights

	// Get the item that is below cursor
	LVHITTESTINFO lvhti ;
	lvhti.pt.x = x;
	lvhti.pt.y = y;
	ClientToScreen(hFilterDlg, &lvhti.pt);
	ScreenToClient(hListView, &lvhti.pt);
	nIndex = ListView_HitTest(hListView, &lvhti);

	if (m_nDropIndex != LB_ERR)
	{
		if (m_nDropIndex != nIndex)
		{
			ImageList_DragShowNolock(FALSE);
//			OUTPUTDEBUGMSG((("Kill HighLite on item %d\n"), m_nDropIndex));
			// Turn off hilight for previous drop target
			ListView_SetItemState(hListView, m_nDropIndex, 0, LVIS_DROPHILITED);
			// Redraw previous item
			ListView_RedrawItems(hListView, m_nDropIndex, m_nDropIndex);
			UpdateWindow(pDropWnd);
			ImageList_DragShowNolock(TRUE);
		}
	}
	
	if(nIndex != LB_ERR)
	{
//		OUTPUTDEBUGMSG((("Redraw item %d\n"), nIndex));
		ImageList_DragShowNolock(FALSE);
		//Note that we can drop here
		SetCursor(LoadCursor(NULL, IDC_ARROW));
		// Redraw item
		bDragLine = true;
		ListView_RedrawItems(pDropWnd, nIndex, nIndex);
		UpdateWindow(pDropWnd);
		bDragLine = false;
		ImageList_DragShowNolock(TRUE);

		KillScrollTimer();
	}
	else
	{
		// If we are not hovering over a CListCtrl, change the cursor
		// to note that we cannot drop here
		SetCursor(LoadCursor(NULL, IDC_NO));
		RECT rectClient;
		int top, bottom ;

		GetClientRect(hListView, &rectClient);

 		p.x = rectClient.left;
		p.y = rectClient.top;
		ClientToScreen(hListView, &p) ;
		top = p.y ;

		p.x = rectClient.left;
		p.y = rectClient.bottom;
		ClientToScreen(hListView, &p) ;
		bottom = p.y ;

		pt.x = x ;
		pt.y = y ;
		ClientToScreen(hFilterDlg, &pt) ;
		pt.y &= 0x7fff ;

		if (pt.y < top)
		{
			// Mouse is above the list control - scroll up.
			SetScrollTimer(scrollUp) ;
			if((top - pt.y) < 15) m_nScrollOffset = 0 ;
			else m_nScrollOffset = (top - 15 - pt.y) / 5 ;
		}
		else if (pt.y > bottom)
		{
			// Mouse is below the list control - scroll down.
			SetScrollTimer(scrollDown);
			if((pt.y - bottom) < 15)  m_nScrollOffset = 0 ;
			else m_nScrollOffset = (pt.y - 15 - bottom) / 5;
		}
		else KillScrollTimer();
	}
	m_nDropIndex = nIndex ;
}


void OnMouseWheel(WPARAM wParam, int x, int y, RECT g_rect)
{
	bool bScrollingUp;
	POINT p;
	HWND hScrollWND;

	// First check if we are inside the PDW window
	if ((x >= g_rect.left) && (x <= g_rect.right) && (y >= g_rect.top) && (y <= g_rect.bottom))
	{
		p.x = x;
		p.y = y;

		hScrollWND   = WindowFromPoint(p);
		bScrollingUp = (HIWORD(wParam) == 120);

		if (Profile.ScrollSpeed)
		{
			 SendMessage(hScrollWND, WM_VSCROLL, bScrollingUp ? SB_LINEUP : SB_LINEDOWN, Profile.ScrollSpeed);
		}
		else SendMessage(hScrollWND, WM_VSCROLL, bScrollingUp ? SB_PAGEUP : SB_PAGEDOWN, 0L);
	}
}


void OnLButtonUp(UINT nFlags, int x, int y) 
{
	#define MAX_TARGET_LEN   256
	char	buf[MAX_TARGET_LEN] ;

	LVITEM lvi ;
	int iPos, iRet;

	// End the drag-and-drop process
	bDragging = false;
	ImageList_DragLeave(hListView);
	ImageList_EndDrag();
	ImageList_Destroy(hDragImageList);

	hDragImageList = NULL ;

	ReleaseCapture();

	if(m_nDropIndex != LB_ERR)
	{
		ImageList_DragShowNolock(FALSE);
		// Turn off hilight for previous drop target
		ListView_SetItemState(hListView, m_nDropIndex, 0, LVIS_DROPHILITED);
		// Redraw previous item
		ListView_RedrawItems(hListView, m_nDropIndex, m_nDropIndex);
		ImageList_DragShowNolock(TRUE);
	}

	// Determine the dropped item
	lvhti.pt.x = x;
	lvhti.pt.y = y;
	ClientToScreen(hFilterDlg, &lvhti.pt);
	ScreenToClient(hListView, &lvhti.pt);
	ListView_HitTest(hListView, &lvhti);

	// Out of the ListView?
	if (lvhti.iItem == -1) return;
	// Not in an item?
	if ((lvhti.flags & LVHT_ONITEM) == 0) return;

	// Dropped item is selected?
	lvi.iItem = lvhti.iItem;
	lvi.iSubItem = 0;
	lvi.mask = LVIF_STATE;
	lvi.stateMask = LVIS_SELECTED;
	ListView_GetItem(hListView, &lvi);

	// On origional item?
	if (lvi.state & LVIS_SELECTED) return;

	// Rearrange the items
	iPos = ListView_GetNextItem(hListView, -1, LVNI_SELECTED);
	while (iPos != -1)
	{
		// First, copy one item
		lvi.iItem = iPos;
		lvi.iSubItem = 0;
		lvi.cchTextMax = MAX_TARGET_LEN;
		lvi.pszText = buf;
		lvi.stateMask = ~LVIS_SELECTED;
		lvi.mask = LVIF_STATE | LVIF_IMAGE | LVIF_INDENT | LVIF_PARAM | LVIF_TEXT;
		ListView_GetItem(hListView, &lvi);
		lvi.iItem = lvhti.iItem;

		// Insert the main item
		iRet = ListView_InsertItem(hListView, &lvi);

		Copy_Filter_Fields(&filter, Profile.filters[iPos]);
		Profile.filters.insert(Profile.filters.begin() + lvhti.iItem, filter);

		if (lvi.iItem < iPos)  lvhti.iItem++;
		if (iRet <= iPos) iPos++;
		// Delete from original position
		ListView_DeleteItem(hListView, iPos);
		Profile.filters.erase(Profile.filters.begin() + iPos);
		iPos = ListView_GetNextItem(hListView, -1, LVNI_SELECTED);
		nCopyStart = nCopyEnd = -1 ;
	}
}


void SetScrollTimer(EScrollDirection ScrollDirection)
{
	if (m_uScrollTimer == 0)
	{
		m_uScrollTimer = SetTimer(hFilterDlg, SCROLL_TIMER, 100, NULL);
	}
	m_ScrollDirection = ScrollDirection;
}

void KillScrollTimer()
{
	if (m_uScrollTimer != 0)
	{
		KillTimer(hFilterDlg, SCROLL_TIMER);
	}
	m_uScrollTimer		= 0;
	m_ScrollDirection	= scrollNull;
	m_nScrollOffset = 0 ;
}


void OnTimer(UINT nIDEvent) 
{
	if (nIDEvent == SCROLL_TIMER && hDragImageList)
	{
		ImageList_DragShowNolock(FALSE);

		for (int i=0; i<1+(m_nScrollOffset*m_nScrollOffset); i++) SendMessage(hListView, WM_VSCROLL, (m_ScrollDirection == scrollUp) ? SB_LINEUP : SB_LINEDOWN , NULL);

		ImageList_DragShowNolock(TRUE);
	}
}

void Marker(LONG x, LONG y, HDC hdc) 
{ 
	if (Profile.FilterWindowColors) SelectObject(hdc,SysPEN[WHITE]);

	MoveToEx(hdc, (int) 0, (int) y + 1, (LPPOINT) NULL); 
	LineTo(hdc,   (int) x, (int) y + 1); 

	MoveToEx(hdc, (int) 0, (int) y - 2, (LPPOINT) NULL); 
	LineTo(hdc,   (int) 0, (int) y + 4); 

	MoveToEx(hdc, (int) x - 1, (int) y - 2, (LPPOINT) NULL); 
	LineTo(hdc,   (int) x - 1, (int) y + 4); 
} 

LONG OnCustomDraw(LPNMLVCUSTOMDRAW pLVCD)
{
	LONG ret = CDRF_DODEFAULT ;

	// If not dragging and not redraw a item needed, then return
	if (!bDragging || !bDragLine) return(ret);

	if (pLVCD->nmcd.dwDrawStage == CDDS_PREPAINT)
	{
		// Return POSTPAINT because we want to draw the line on top of all
		ret = CDRF_NOTIFYPOSTPAINT ;
	}
	else if (pLVCD->nmcd.dwDrawStage == CDDS_POSTPAINT) 
	{
		Marker(pLVCD->nmcd.rc.right, pLVCD->nmcd.rc.top, pLVCD->nmcd.hdc) ;
	}
	return(ret) ;
}


BOOL TestIfBigger(int nHigher, int nLower, bool bAddress)
{
	int nCapcodeCmp;

	if (bAddress)
	{
		nCapcodeCmp = CompareCapcodes(Profile.filters[nHigher].capcode, Profile.filters[nLower].capcode) ;

		if (nCapcodeCmp != 0)
		{
			return (nCapcodeCmp == -1) ;
		}

		if (!Profile.filters[nLower].text[0] && Profile.filters[nHigher].text[0])
		{
			return (1);
		}
		if (Profile.filters[nLower].text[0] && !Profile.filters[nHigher].text[0])
		{
			return (0);
		}

		if (Profile.filters[nLower].text[0] == '^')
		{
			if (Profile.filters[nHigher].text[0] == '^')
			{
				return(stricmp(Profile.filters[nHigher].text, Profile.filters[nLower].text) < 0);
			}
			return (1);
		}
		if (Profile.filters[nHigher].text[0] == '^')
		{
			return (0);
		}

		if (strnicmp(&Profile.filters[nLower].text[0], Profile.filters[nHigher].text, strlen(Profile.filters[nHigher].text)) == 0)
		{
			return (0);
		}
		if (strnicmp(&Profile.filters[nHigher].text[0], Profile.filters[nLower].text, strlen(Profile.filters[nLower].text)) == 0)
		{
			return (1);
		}
		return(stricmp(Profile.filters[nHigher].text, Profile.filters[nLower].text) < 0);
	}
	else return(stricmp(Profile.filters[nHigher].label, Profile.filters[nLower].label) < 0);
}


void SortFilter(HWND hDlg, bool bAddress)
{
	int index, min, i ;	
				
	bSortingFilters = true;

	if ((index = ListView_GetNextItem(hListView, -1, LVNI_SELECTED)) != CB_ERR)
	{
		min = index ;
		
		SetWindowText(hDlg, (LPSTR) "PDW Filters - Please wait while sorting filters...");

		while ((index = ListView_GetNextItem(hListView, index, LVNI_SELECTED)) != CB_ERR)
		{
			for (i=min; i<index; i++)
			{
				if (TestIfBigger(index, i, bAddress)) break;
			}
			if (i == index) continue;

			ListView_EnsureVisible(hListView, index, TRUE);

			Copy_Filter_Fields(&filter, Profile.filters[index]);
			BuildFilterString(szTEMP, Profile.filters[index]);

			Profile.filters.erase (Profile.filters.begin() + index);
			Profile.filters.insert(Profile.filters.begin() + i, filter);
			ListView_DeleteItem(hListView, index);
			InsertListViewItem(szTEMP, i);

			ListView_SetItemState(hListView, i, LVIS_SELECTED, LVIS_SELECTED);

			pumpMessages();	// Process messages
		} 
		ListView_SetItemState(hListView, i, LVIS_FOCUSED, LVIS_FOCUSED);

		if (sprintf(szTEMP, "PDW Filters (%u)", Profile.filters.size()) != EOF)
		SetWindowText(hDlg, (LPSTR) szTEMP);
	}
	bSortingFilters = false;
}


bool CheckSelection(bool bMore)
{
	int index = -1, old = -1;	
				
	if (bMore)
	{
		if(ListView_GetSelectedCount(hListView) <= 1) return(false);
	}

	while ((index = ListView_GetNextItem(hListView, index, LVNI_SELECTED)) != CB_ERR)
	{
		if(old == -1)
		{
			old = index ;
		}
		else
		{
			if(old != index-1)
			{
				return(false) ;
			}
		}
		old = index ;
	} 
	return(true) ;
}


int CompareCapcodes(char *szCapcode1, char *szCapcode2)
{
	for (int i=0; i<9; i++) 
	{
		if(szCapcode1[i] > szCapcode2[i]) return(1) ;
		if(szCapcode1[i] < szCapcode2[i]) return(-1) ;
	}
	return(0) ;
}


void CopyFilter(void)
{
	int nIndex ;

	if((nIndex = ListView_GetNextItem(hListView, -1, LVNI_SELECTED)) != CB_ERR)
	{
		nCopyStart = nCopyEnd = nIndex ;
		while((nIndex = ListView_GetNextItem(hListView, nIndex, LVNI_SELECTED)) != CB_ERR)
		{
			nCopyEnd = nIndex ;
		}
	}
}

void PasteFilter(void) 
{
	int i, nIndex, item;

	if (nCopyEnd == -1)
	{
		return ; 
	}

	if ((nIndex = ListView_GetNextItem(hListView, -1, LVNI_SELECTED)) != CB_ERR)
	{
		ListView_SetItemState(hListView, -1, 0, LVIS_SELECTED | LVIS_FOCUSED); // Deselect all

		for (i = item = nCopyStart; i <= nCopyEnd; i++)
		{
 			Copy_Filter_Fields(&filter, Profile.filters[item]);
			BuildFilterString(szTEMP, filter);
			Profile.filters.insert(Profile.filters.begin() + nIndex, filter);

			if (nIndex <= nCopyStart)
			{
				item += 2 ;
				nCopyStart++ ;
				nCopyEnd++ ;
				i++ ;
			}
			else item++;

			InsertListViewItem(szTEMP, nIndex);
			ListView_SetItemState(hListView, nIndex, LVIS_SELECTED | LVIS_FOCUSED, LVIS_SELECTED | LVIS_FOCUSED);
			SendMessage(hListView, LVM_SETSELECTIONMARK, 0, nIndex);

			nIndex++ ;
		}
	}
}

void ResetHitcounters(bool bAll)
{
	int index=-1, i=0;

	char filename[MAX_PATH];
	char ext[10];
				
	if (MessageBox(ghWnd, "Are you sure?", "Reset Hitcounters", MB_ICONQUESTION | MB_OKCANCEL) == IDCANCEL) return;

	if (bAll)
	{
		for (int index=0; index<Profile.filters.size(); index++)
		{
			Profile.filters[index].hitcounter = 0 ;
			Profile.filters[index].lasthit_date[0] = '\0' ;
			Profile.filters[index].lasthit_time[0] = '\0' ;
		}
		strcpy(filename, szFilterPathName);
		while (FileExists(filename))
		{
			i++;
			sprintf(ext, ".%03i", i);
			ChangeFileExtension(filename, ext);
		}
		WriteFilters(&Profile, i);
		sprintf(filename, "The old filters.ini has been backed up as 'filters%s'", ext);
		MessageBox(ghWnd, filename, "PDW Filters", MB_ICONINFORMATION);
	}
	else
	{
		while((index = ListView_GetNextItem(hListView, index, LVNI_SELECTED)) != CB_ERR)
		{
			Profile.filters[index].hitcounter = 0 ;
			Profile.filters[index].lasthit_date[0] = '\0' ;
			Profile.filters[index].lasthit_time[0] = '\0' ;
		}
		bUpdateFilters = true;	// PH: Update filters.ini in UpdateFilters() when IDLE
	}
}

void SetWindowPaneSize(int param)
{
	int maximize_state;
	
	RECT rect;

	GetClientRect(ghWnd, &rect);

	if (param)
	{
		if (param == 2) Profile.percent = (100-Profile.percent);

		maximize_state = Profile.maximize_flg;

		SendMessage(ghWnd, WM_SIZE, SIZENORMAL, (LPARAM) MAKELONG(rect.right - rect.left, rect.bottom - rect.top));
		SendMessage(Pane1.hWnd, WM_VSCROLL, SB_BOTTOM, 0L);
		SendMessage(Pane2.hWnd, WM_VSCROLL, SB_BOTTOM, 0L);

		Profile.maximize_flg = maximize_state;
	}
	else
	{
		Max_X_Client = GetSystemMetrics(SM_CXFULLSCREEN);	// Get max X client size
		iMaxWidth = rect.right;								// PH: Set to window-width
		iMaxWidth-= GetSystemMetrics(SM_CXVSCROLL);			// PH: Subtract scrollbar width	
		iMaxWidth-= (cxChar*2);								// PH: Subtract 1 character+1px
		NewLinePoint = (iMaxWidth/cxChar);					// PH: We have the max. chars
	}
}


void SystemTrayWindow(bool bHideWindow)
{
	bool bAnimationInfo=false;

	ANIMATIONINFO ani = { 0 };

	RECT rcWindow;
	RECT rcSystemTray;

	HWND hShellTrayWnd = NULL;
	HWND hTrayNotifyWnd= NULL;

	ani.cbSize = sizeof(ani);
	SystemParametersInfo(SPI_GETANIMATION,sizeof(ani),&ani,0);

	if (ani.iMinAnimate) bAnimationInfo=true;

	if (!bTrayed)
	{
		if (bAnimationInfo)
		{
			hShellTrayWnd=FindWindowEx(NULL, NULL, TEXT("Shell_TrayWnd"), NULL);

			if (hShellTrayWnd)
			{
				hTrayNotifyWnd=FindWindowEx(hShellTrayWnd, NULL, TEXT("TrayNotifyWnd"), NULL);

				if (hTrayNotifyWnd)
				{
					GetWindowRect(hTrayNotifyWnd, &rcSystemTray);
				}
			}
			if (!hShellTrayWnd || !hTrayNotifyWnd)
			{
				SystemParametersInfo(SPI_GETWORKAREA, 0, &rcSystemTray, 0);
				rcSystemTray.left=rcSystemTray.right-150;
				rcSystemTray.top =rcSystemTray.bottom-30;
			}
			GetWindowRect(ghWnd, &rcWindow);
			DrawAnimatedRects(ghWnd, IDANI_CAPTION,	&rcWindow, &rcSystemTray);
		}
		ShowWindow(ghWnd, SW_HIDE);

		SystemTrayIcon(false);

		Profile.minimize_flg=1;
		bTrayed=true;
	}
	else if (bTrayed)	// PH: Restore PDW-window from systemtray
	{
		ShowWindow(ghWnd, Profile.maximize_flg ? SW_SHOWMAXIMIZED : SW_SHOWNORMAL);
		SetForegroundWindow(ghWnd);
		SetFocus(ghWnd);

		Profile.minimize_flg=0;
		bTrayed=false;
	}
}


void SystemTrayIcon(bool bRemoveIcon)
{
	static bool bIcon=false;

	NOTIFYICONDATA nid = { 0 };

	nid.cbSize           = sizeof(NOTIFYICONDATA);
	nid.hWnd             = ghWnd;
	nid.uID              = 110;	// Per WinCE SDK docs, values from 0 to 12 are reserved and should not be used.
	nid.uFlags           = NIF_MESSAGE | NIF_ICON | NIF_TIP;
	nid.hIcon            = LoadIcon(ghInstance, MAKEINTRESOURCE(PDWICON));
	nid.uCallbackMessage = SYSTEMTRAY_ICON_MESSAGE;

	strcpy(nid.szTip, pdw_version);

	if (bRemoveIcon)	// PH: Remove PDW-icon from systemtray
	{
		if (bIcon) Shell_NotifyIcon(NIM_DELETE, &nid);
		bIcon=false;
		return;
	}
	else if (!bIcon)
	{
		Shell_NotifyIcon(NIM_ADD, &nid);
		bIcon=true;
	}
}


void SetNewWindowText(char *text)
{
//	extern bool bMode_IDLE;			// Set if no signal

	strcpy(szTEMP, szWindowText[0]);

	if (text[0]) strcat(szTEMP, text);
	else
	{
		for (int i=1; i<6; i++)
		{
			if (szWindowText[i][0])
			{
				strcat(szTEMP, " - ");
				strcat(szTEMP, szWindowText[i]);
			}
		}
		strcpy(szWindowText[5], "");
	}
	SetWindowText(ghWnd, (LPSTR) szTEMP);
}


void AutoRecording()
{
	if (strstr(szPath, "DEBUG")) return;

	char szPathname[MAX_PATH];
	char szFilename[MAX_PATH]="";
	static char szTMPfile[MAX_PATH]="";

	sprintf(szPathname, "%s\\Recordings", szPath);

	if (!FileExists(szPathname)) CreateDirectory(szPathname, NULL);

	Get_Date_Time();

	bAutoRecording=true;

	if (bRecording)
	{
		Stop_Recording();

		strncpy(szFilename, szTMPfile, strlen(szTMPfile)-5);
		 strcat(szFilename, "-");
		strncat(szFilename, &szCurrentTime[0], 2);
		strncat(szFilename, &szCurrentTime[3], 2);
		strncat(szFilename, &szCurrentTime[6], 2);
		 strcat(szFilename, "].rec");

		CopyFile(szTMPfile, szFilename, false);
		DeleteFile(szTMPfile);
	}

	CreateDateFilename("", NULL);

	sprintf(szTMPfile, "%s\\%s [", szPathname, szFilenameDate);
	strncat(szTMPfile, &szCurrentTime[0], 2);
	strncat(szTMPfile, &szCurrentTime[3], 2);
	strncat(szTMPfile, &szCurrentTime[6], 2);
	 strcat(szTMPfile, "].rec");

	Start_Recording(szTMPfile);
}


void ShowContextMenu(int menu, HWND hWindow)
{
	int more;
	HMENU hMenu = CreatePopupMenu();
	POINT pt;
	GetCursorPos(&pt);

	switch(menu)
	{
		case HMENU_FILTERS:
		more = ListView_GetSelectedCount(hListView) > 1;
		AppendMenu(hMenu, MF_STRING | CheckSelection(true) ? 0 : MF_GRAYED, IDT_MENU_SORT_ADDRESS, "Sort by Address") ;
		AppendMenu(hMenu, MF_STRING | CheckSelection(true) ? 0 : MF_GRAYED, IDT_MENU_SORT_LABEL,   "Sort by Label") ;
		AppendMenu(hMenu, MF_SEPARATOR, NULL, NULL);
		AppendMenu(hMenu, MF_STRING,										IDT_MENU_SELECTALL, "Select All") ;
		AppendMenu(hMenu, MF_STRING | CheckSelection(false) ? 0 : MF_GRAYED, IDT_MENU_COPY, "Copy") ;
		AppendMenu(hMenu, MF_STRING | CheckSelection(false) && (nCopyEnd != -1) ? 0 : MF_GRAYED, IDT_MENU_PASTE, "Paste") ;
		AppendMenu(hMenu, MF_SEPARATOR, NULL, NULL);
		AppendMenu(hMenu, MF_STRING | more ? MF_GRAYED : 0,					IDC_FILTERADD, "Add...") ;
		AppendMenu(hMenu, MF_STRING,										IDC_FILTEREDIT,"Edit...");
		AppendMenu(hMenu, MF_STRING,										IDC_FILTERDEL, "Delete...") ;
		AppendMenu(hMenu, MF_SEPARATOR, NULL, NULL);
		AppendMenu(hMenu, MF_STRING,										IDT_MENU_RESET, more ? "Reset Hitcounters" : "Reset Hitcounter") ;
		AppendMenu(hMenu, MF_STRING | (more || !Profile.filters.size()) ? MF_GRAYED : 0, IDC_FILTERFIND,"Find Filter") ;
		AppendMenu(hMenu, MF_STRING,						                IDT_FILTERCHECKDUPLICATE,"Check for duplicate filters") ;
		AppendMenu(hMenu, MF_STRING,						                IDC_FILTEROPTIONS,"Filter Options...") ;
		break;

		case HMENU_MAIN:

		AppendMenu(hMenu, MF_STRING  | selected ? 0 : MF_GRAYED, IDM_COPY_SELECTION, "Copy Selection");
		AppendMenu(hMenu, MF_SEPARATOR, NULL, NULL);
		AppendMenu(hMenu, MF_STRING,    IDM_OPTIONS,      "Options...") ;
		AppendMenu(hMenu, MF_STRING,    IDM_FILTERS,      "Filter List...") ;
		AppendMenu(hMenu, MF_STRING,	IDM_FILTEROPTIONS,"Filter Options...") ;
		AppendMenu(hMenu, MF_SEPARATOR, NULL, NULL);
		AppendMenu(hMenu, MF_STRING,    IDM_SCROLLBACK,   "Scrollback...") ;
		AppendMenu(hMenu, MF_STRING,    IDM_CLEARDISPLAY, "Clear Screen...") ;
		AppendMenu(hMenu, MF_SEPARATOR, NULL, NULL);
		AppendMenu(hMenu, MF_STRING,    IDM_EXIT,         "Exit PDW");
		break;

		case HMENU_SYSTRAY:
		AppendMenu(hMenu, MF_STRING | bTrayed ? 0 : MF_GRAYED, IDT_MENU_RESTORE, "Restore");
		AppendMenu(hMenu, MF_STRING,	IDM_SYSTEMTRAY,   "Settings...");
		AppendMenu(hMenu, MF_SEPARATOR, NULL, NULL);
		AppendMenu(hMenu, MF_STRING,    IDM_FILTERS,      "Filter List...");
		AppendMenu(hMenu, MF_STRING,    IDM_EXIT,         "Exit PDW");
		break;
	}
	TrackPopupMenu(hMenu, TPM_RIGHTBUTTON, pt.x, pt.y, 0, hWindow, NULL);
	DestroyMenu(hMenu);
}


void SelectByDoubleClick(HWND hWnd, PaneStruct *pane, int iPosition, int StartRow)
{
	int line_no = pane->iVscrollPos + iSelectionStartRow;
	int offset  = line_no * (LINE_SIZE+1);
	int min_col=0, max_col=0;
	static LONG LastSelection=0;
	static int iLastRow=0;

	char *pchar;
	
	pchar = pane->buff_char;

	if (pchar[offset + iPosition] > 32)	// If user clicked on a character bigger than ASCII(32)
	{
		if ((GetKeyState(VK_SHIFT) & 0x80) && (StartRow == iLastRow))
		{
			iSelectionStartCol = min(iPosition, LOWORD(LastSelection));	// We have found the first character
			iSelectionEndCol   = max(iPosition, HIWORD(LastSelection));	// We have found the last character
		}
		else
		{
			iSelectionStartCol = iPosition;
			iSelectionEndCol   = iPosition;
		}

		while (pchar[offset + iSelectionStartCol-1] > 32) iSelectionStartCol--;
		while (pchar[offset + iSelectionEndCol+1] > 32)   iSelectionEndCol++;

		LastSelection=MAKELONG(iSelectionStartCol, iSelectionEndCol);

		iSelectionStartRow = StartRow;
		iSelectionEndRow   = StartRow;
		iLastRow           = StartRow;

		InvertSelection();					// Invert selection

		SetCapture(hWnd);
		select_pane = pane;
		selected=1;
	}
}

void GoogleMaps(int iPosition)
{
	char szTMP[1024];
	char szString[32];
	char *pchar;

	int i, j, xx;

	int line_no = Pane1.iVscrollPos + iSelectionStartRow;
	int offset  = line_no * (LINE_SIZE+1);
	pchar = Pane1.buff_char;

	if (Profile.monitor_mobitex && iPosition == iItemPositions[MSG_MESSAGE])
	{
		for (i=0, xx = iSelectionStartCol; xx <= (iSelectionStartCol+18); i++, xx++)
		{
			if (pchar[offset + xx]) szTMP[i] = pchar[offset + xx];
			else break;
		}
		szTMP[i] = '\0';

		if ((szTMP[0] == '+') && (strlen(szTMP) == 18))		// + 4 ] v ^ v # c ·"
		{
			for (i=2; i<13; i+=2)
			{
				if (szTMP[i] > 89) szTMP[i] -= 90;
			}
				sprintf(szString, "%02i+%02i+%02i+%02i+%02i+%02i", szTMP[2], szTMP[4], szTMP[6], szTMP[8], szTMP[10], szTMP[12]);
		}
		else if ((szTMP[0] == 'N') && (strlen(szTMP) == 13))	// N515915043140
		{
			for (i=1, j=0; szTMP[i] != 0; i++)
			{
					szString[j++] = szTMP[i];
					if ((i%2) == 0) szString[j++] = '+';
			}
		}
		else return;

		szString[17] = '\0';
	}
	else if ((iSelectionEndCol - iSelectionStartCol) == 5)
	{
		for (i=0, xx = iSelectionStartCol; i<6; i++, xx++)
		{
			if (i < 4)
			{
				if (!isdigit(pchar[offset + xx])) return;
			}
			else if (i < 6)
			{
				if (isdigit(pchar[offset + xx])) return;
			}

			if (pchar[offset + xx]) szString[i] = pchar[offset + xx];
			else break;
		}
		szString[i] = '\0';
	}
	else return;

	sprintf(szTMP, "http://maps.google.nl/maps?q=%s", szString);

	ShellExecute(NULL, "open", szTMP, NULL, NULL, SW_SHOWNORMAL);
}

int FindFilter(int index, char *szSearchString, bool bShowHits, bool bBackwards)
{
	int i, nHits=0, iFilter, nFilters, iMatch;
	bool bFound=false, bAddress=true;
	char szAddress[FILTER_CAPCODE_LEN+1];
	char szText[FILTER_TEXT_LEN+1];
	char szLabel[FILTER_LABEL_LEN+1];
	char szString[FILTER_LABEL_LEN+1];
	static char szLastSearchString[FILTER_LABEL_LEN+1]="";

	if (!szSearchString[0])
	{
		strcpy(szString, szLastSearchString);
	}
	else
	{
		strcpy(szLastSearchString, szSearchString);
		strcpy(szString, szSearchString);
	}

	if (!bFilterFindCASE)	// Not case sensitive, make the strings upper case
	{
		for (i=0; i<strlen(szString); i++)
		{
			szString[i] = toupper(szString[i]);
		}
	}

	if (index == -1)
	{
		index = ListView_GetSelectionMark(hListView)+1;
		if (bBackwards) index -= 2;
	}

	nFilters = Profile.filters.size();

	for (iFilter=0; iFilter < nFilters; iFilter++, bBackwards ? index-- : index++)
	{
		if (index == nFilters)	// Reached the last filter
		{
			index=0;
		}
		else if (index < 0)		// Searching backwards
		{
			index=nFilters-1;
		}

		strcpy(szAddress, Profile.filters[index].capcode);
		strcpy(szText,    Profile.filters[index].text);
		strcpy(szLabel,   Profile.filters[index].label);

		if (!bFilterFindCASE)	// Not case sensitive; make both strings upper case
		{
			for (i=0; szLabel[i] != 0; i++)
			{
				// ((x & 0x40) == 0x02) checks whether the current character is lower case
				if ((szLabel[i] > 96) && (szLabel[i] < 123))
				{
					szLabel[i] -= 32;
				}
			}
			for (i=0; szText[i] != 0; i++)
			{
				if ((szText[i] > 96) && (szText[i] < 123))
				{
					szText[i] -= 32;
				}
			}
		}

		if ((strstr(szAddress, szString)) ||
			(strstr(szLabel,   szString)) ||
		    (strstr(szText,    szString)))
		{
			if (!bFound)
			{
				iMatch=index;
				bFound=true;	// We have a match!
			}
			nHits++;		// Keep track of the number of hits
			if (!bShowHits) break;
		}
	}

	if (bFound)
	{
		ListView_SetItemState (hListView, -1, 0,  LVIS_SELECTED | LVIS_FOCUSED);
		ListView_SetItemState (hListView, iMatch, LVIS_SELECTED | LVIS_FOCUSED, LVIS_SELECTED | LVIS_FOCUSED);
		ListView_EnsureVisible(hListView, iMatch, TRUE);
		SendMessage(hListView, LVM_SETSELECTIONMARK, 0, iMatch);
	}
	else MessageBeep(MB_OK);

	return(nHits);
}


void pumpMessages(void)
{
    MSG  msg;

    while (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
	{
        TranslateMessage(&msg);
        DispatchMessage(&msg);
	}
}
