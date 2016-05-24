#ifndef PDW_H
#define PDW_H

#define VER_MAJOR 1
#define VER_MINOR 0
#define VER_BUILD 0

#define FILTER_CAPCODE_LEN  9	// longest is FLEX long (9 chars)
#define FILTER_LABEL_LEN    70	// PH: was 40
#define FILTER_TEXT_LEN     40	// PH: was 25
#define FILTER_FILE_LEN     128	// PH: was 256

#define MAX_STR_LEN			5120

enum FILTER_TYPE {	UNUSED_FILTER  = 0,
					FLEX_FILTER    = 1,
					POCSAG_FILTER  = 2,
					TEXT_FILTER    = 3,
					ERMES_FILTER   = 4,
					ACARS_FILTER   = 5,
					MOBITEX_FILTER = 6	};

typedef struct
{
	FILTER_TYPE  type;
	char		 capcode[FILTER_CAPCODE_LEN+1];
	char		 label[FILTER_LABEL_LEN+1];
	char		 text[FILTER_TEXT_LEN+1];
	int			 match_exact_msg;
	int			 cmd_enabled;
	int			 reject;
	int			 monitor_only;
	int			 wave_number;
	int			 label_enabled;
	int			 label_color;
	int			 smtp;
	int			 sep_filterfile_en;
	char		 sep_filterfile[3][FILTER_FILE_LEN+1];	// PH: max 3 sepfiles
	int			 sep_filterfiles;						// PH: number of sepfiles
	unsigned int hitcounter;
	char		 lasthit_date[10];
	char		 lasthit_time[10];
} FILTER;

#define MAX_FILE_LEN  256

#define MAIL_TEXT_LEN  100

#define WINDOW_SIZE  0	// PH : Used in SetWindowPaneSize(int panes)
#define PANE_SIZES   1	// PH : Used in SetWindowPaneSize(int panes)
#define PANE_SWITCH  2	// PH : Used in SetWindowPaneSize(int panes)

#define PANE1		 0x01	// PH : Used in DrawPaneLabels(HWND hwnd, int pane)
#define PANE2		 0x02	// PH : Used in DrawPaneLabels(HWND hwnd, int pane)
#define PANERXQUAL	 0x04	// PH : Used in DrawPaneLabels(HWND hwnd, int pane)

#define MOBITEX_SHOW_MPAK	0x01
#define MOBITEX_SHOW_TEXT	0x02
#define MOBITEX_SHOW_HPDATA	0x04
#define MOBITEX_SHOW_DATA	0x08
#define MOBITEX_VERBOSE		0x10
#define MOBITEX_SHOW_HPID	0x20
#define MOBITEX_SHOW_SWEEP	0x40

// COLORS
#define COLOR_UNUSED		0
#define COLOR_BACKGROUND	1
#define COLOR_ADDRESS		2
#define COLOR_TIMESTAMP		3
#define COLOR_MODETYPEBIT	4
#define COLOR_MESSAGE		5
#define COLOR_NUMERIC		6
#define COLOR_MISC			7
#define COLOR_BITERRORS		8
#define COLOR_FILTERMATCH	9
#define COLOR_INSTRUCTIONS	10
#define COLOR_AC_MESSAGE_NR	11
#define COLOR_AC_DBI		12
#define COLOR_MB_SENDER		13

// LABEL COLORS
#define COLOR_FILTERLABEL	20	// 21-36 are reserved for the filter label colors


#define SYSTEMTRAY_ICON_MESSAGE (WM_USER+1) // ID for PDW sytem tray icon

#include <cstring>

#pragma warning(disable : 4786)
#include <vector>
#pragma warning(default : 4786)
#include <algorithm>

using namespace std;

typedef vector<FILTER> FILTERLIST;

typedef struct
{
	int xPos;						// application window's horizontal position
	int yPos;						// application window's vertical position
	int xSize;						// application window's horizontal size
	int ySize;						// application window's vertical size
	unsigned int confirmExit;		// confirm exit flag
	unsigned int showtone;			// show tone-only messages flag
	unsigned int shownumeric;		// show numeric messages flag
	unsigned int showmisc;			// show miscellaneous messages flag
	unsigned int filterbeep;		// beep when filter is triggered
	unsigned int filterwindowonly;	// Show filtered messages only in filterwindow
	unsigned int decodepocsag;		// flag to enable POCSAG decoding
	unsigned int decodeflex;		// flag to enable FLEX/ReFLEX decoding
	unsigned int showinstr;			// flag to show Short Instructions
	unsigned int convert_si;		// flag to convert Short Instructions to textmessage
	unsigned int pocsag_512;		// show pocsag-512  flag
	unsigned int pocsag_1200;		// show pocsag-1200 flag
	unsigned int pocsag_2400;		// show pocsag-2400 flag
	unsigned int pocsag_fnu;		// Display pocsag function numbers as default (NTTA)
	unsigned int pocsag_showboth;	// Show "difficult" messages as numeric AND alpha
	unsigned int flex_1600;			// show flex-1600 flag
	unsigned int flex_3200;			// show flex-3200 flag
	unsigned int flex_6400;			// show flex-6400 flag
	unsigned int acars_parity_check;
	unsigned int show_cfs;			// flag to show FLEX cycle/frame information in titlebar
	unsigned int show_rejectblocked;// flag to show rejected/blocked messages in titlebar

	int  Hide_Column;
	int  LabelLog;					// Labels in normal logfile
	int  LabelNewline;				// Labels on new line
	char ColLogfile[10];			// Flag for columns to be logged in logfile
	char ColFilterfile[10];			// Flag for columns to be logged in filterfile
	int  Linefeed;					// Flag for converting ¯ to linefeed
	int  Separator;					// Flag for separating messages (empty line)
	int  MonthNumber;				// Flag for using monthnumber in logfilenames
	int  DateFormat;				// Flag for date format
	int  Date_USA;					// Flag for using MM-DD-YY in stead of DD-MM-YY
	char LogfilePath[MAX_PATH];		// Flag for using monthnumber in logfilenames
	int  BlockDuplicate;			// Flag for blocking duplicate messages
	int  FilterWindowColors;		// Flag for showing label colors in filterwindow
	int  FilterWindowExtra;			// Flag for showing CMD/DESC/SEP/etc in filterwindow

	int  SystemTray;				// Flag for enabeling the system tray
	int  SystemTrayRestore;			// Flag for enabeling auto restore from tray

	int  FlexTIME;					// Flag for FlexTIME as systemtime
	int  FlexGroupMode;				// Flag for FlexGroupMode

	int  SMTP;						// SMTP-email

	bool Trayed;					// TRUE if trayed

	int comPortEnabled;
	int comPort;					// COM port (1-4 or 5 for custom)
	int comPortRS232;				// Special serial converter from Rene
	int comRS232bitrate;
	int comPortAddr;				// COM port I/O address
	int comPortIRQ;					// COM port IRQ vector
	int fourlevel;					// use 4 Level FSK interface flag
	int invert;						// invert COM port/Sound card data
	int invert_option;				// 0=auto,1=Yes,2=No

	int percent;					// percent of total main window area

	double dRX_Quality;

	LOGFONT fontInfo;

	int  logfile_enabled;
	char logfile[MAX_FILE_LEN+1];
	int  logfile_use_date;

	int  filterfile_enabled;
	char filterfile[MAX_FILE_LEN+1];
	int  filterfile_use_date;
	int  filter_cmd_file_enabled;
	char filter_cmd[MAX_FILE_LEN+1];
	char filter_cmd_args[MAX_FILE_LEN+1];
	int  filter_default_type;
	int  filter_searchwhiletyping;

	FILTERLIST filters;

	char szMailHost[MAIL_TEXT_LEN] ;
	char szMailHeloDomain[MAIL_TEXT_LEN] ;
	char szMailTo[MAIL_TEXT_LEN * 5] ;
	char szMailFrom[MAIL_TEXT_LEN] ;
	char szMailUser[MAIL_TEXT_LEN] ;
	char szMailPassword[MAIL_TEXT_LEN] ;
	int	 iMailPort ;
	int	 nMailOptions ;

	COLORREF color_background;
	COLORREF color_address;
	COLORREF color_timestamp;
	COLORREF color_modetypebit;
	COLORREF color_numeric;
	COLORREF color_message;
	COLORREF color_misc;
	COLORREF color_biterrors;
	COLORREF color_filtermatch;
	COLORREF color_instructions;
	COLORREF color_ac_message_nr;
	COLORREF color_ac_dbi;
	COLORREF color_mb_sender;
	COLORREF color_filterlabel[17];	// array for all label colors

	int pane1_size;
	int pane2_size;
	int ScrollSpeed;
	int ScreenColumns[7];

	int  stat_file_enabled;
	int  stat_file_use_date;
	char stat_file[MAX_FILE_LEN+1];
	char edit_save_file[MAX_FILE_LEN+1];
	int maximize_flg;
	int maximize_tmp;
	int minimize_flg;
	int audioEnabled;
	int audioDevice;
	int audioChannels;
	int audioSampleRate;
	int audioConfig;
	int audioThreshold[4];
	int audioResync[4];
	int audioCentering[4];

	BOOL monitor_paging;
	BOOL monitor_acars;
	BOOL monitor_mobitex;
	BOOL monitor_ermes;
	BOOL reverse_msg;
	int lang_mi_index;          // Decides language menu item.
	int lang_tbl_index;         // Decides language character map.
	bool ssl;
} PROFILE, *PPROFILE;

extern PROFILE Profile;     // profile information

#define LINE_SIZE 180	// PH Test : was 160 (problem Colin)

typedef struct
{
	HWND hWnd;
	unsigned int  cyLines, cxClient, cyClient;
	int  iVscrollPos, iVscrollMax, iHscrollPos, iHscrollMax;
	unsigned int  Bottom;
	int  currentPos;
	BYTE currentColor;
	unsigned int buff_lines;
	char *buff_char;
	BYTE *buff_color;
} PaneStruct;

#define MODE_IDLE    0x00

#define MODE_REFLEX  0x10
#define MODE_POCSAG  0x20
#define MODE_ERMES   0x30
#define MODE_MOBITEX 0x40
#define MODE_ACARS   0x80
#define MODE_PAGING  0xF0

#define MODE_P512    0x01
#define MODE_P1200   0x02
#define MODE_P2400   0x04

#define MODE_FLEX_A  0x08
#define MODE_FLEX_B  0x04
#define MODE_FLEX_C  0x02
#define MODE_FLEX_D  0x01

#define STAT_FLEX6400    0
#define STAT_FLEX3200    1
#define STAT_FLEX1600    2
#define STAT_POCSAG2400  3
#define STAT_POCSAG1200  4
#define STAT_POCSAG512   5
#define STAT_ACARS2400   6
#define STAT_MOBITEX     7
#define STAT_ERMES       8
#define NUM_STAT         9

#define STAT_NUMERIC     0
#define STAT_ALPHA       1

class FLEX
{
	private:
				void FlexTIME();
				int  xsumchk(long int l);
				void show_address(long int l, long int l2, bool bLongAddress);
				void showframe(int asa, int vsa);
				void show_phase_speed(int vt);
	public:
				char block[256];		// was 300
				long int frame[200];
				char ppp;
				FLEX();
				~FLEX();
				void showblock(int blknum);
				void showword(int wordnum);
				void showwordhex(int wordnum);
};


class POCSAG
{
	private:
				long int pocaddr;
				int  wordc, nalp, nnum, shown, srca, srcn;
				bool bAddressWord; // Will be set if last word was an address flag
				int  alp[MAX_STR_LEN], num[40];
				int  function;

				void show_addr(bool bAlpha);
				void show_message();
				void logbits(char *text, bool bClose);
				int  GetMessageType();
	public:
				POCSAG();
				~POCSAG();

				void reset();
				void process_word(int fn2);
				void frame(int bit);
};

// ***** Globals ******

// Text editing globals
extern unsigned int start_col, start_row, end_col, end_row;
extern int select_on, selected, selecting;
extern PaneStruct *select_pane;

extern char *label_colors[9]; // PH: Colors for filter labels
extern char *wave_files[11];
extern char *pdw_version;

extern char aNumeric[17];

extern int g_sps;
extern int g_sps2;

extern bool bUpdateFilters;			// PH: Does FILTERS.INI need to be updated?

// ************* Function Prototypes **************

BOOL Need_Ext(char *s);
void Free_Common_Objects(void);
void PrintClip(void);
void display_color(PaneStruct *pane, BYTE ct);
void display_show_char(PaneStruct *pane, char cin);
void display_show_str(PaneStruct *pane, char strin[]);
void ShowMessage();
void display_show_timestamp(PaneStruct *pane);
void display_showmo(int mode);
void display_show_hex21(PaneStruct *pane, long int l);
void display_show_hex16(PaneStruct *pane, int l);
void display_cfstatus(int cy=-1, int fr=-1);
void setupecc();
void frame_flex(char gin);
void flex_reset(void);
int  nOnes(int k);
int  bit10(int gin);
int  ecd();
int  filter_addr(char addr_str[], char filter_str[]);

// macros for easier readability...

#define GETHINST( x )  ((HINSTANCE) GetWindowLong( x, GWL_HINSTANCE ))

// Windows function prototypes...

LRESULT FAR PASCAL PDWWndProc  (HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
LRESULT FAR PASCAL Pane1WndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
LRESULT FAR PASCAL Pane2WndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);

bool GetEditSaveName(HWND hWnd);
void ClearPanes(bool bPane1, bool bPane2);
void InvertSelection(void);
void SaveClipToDisk(char *fname);
void CopyToClipboard(PaneStruct *pane, UINT min_col, UINT max_col, UINT min_row, UINT max_row);
void PanePaint(PaneStruct *pane);
void PaneHScroll(PaneStruct *pane, WPARAM wParam);
void PaneVScroll(PaneStruct *pane, WPARAM wParam, LPARAM lParam);
void BuildFilterString(char *temp_str, FILTER filter);
void ChangeDataMode(HWND hWnd, int mode);

VOID NEAR GoModalDialogBoxParam(HINSTANCE hInstance, LPCSTR lpszTemplate, HWND hWnd, DLGPROC lpDlgProc, LPARAM lParam);
UINT CALLBACK CenterOpenDlgBox(HWND hDlg, UINT uMsg, WPARAM wParam, LPARAM lParam);
BOOL CenterWindow(HWND hWnd);

BOOL FAR PASCAL AboutDlgProc(HWND hDlg, UINT uMsg, WPARAM wParam, LPARAM lParam);
BOOL FAR PASCAL ClearScreenDlgProc(HWND hDlg, UINT uMsg, WPARAM wParam, LPARAM lParam);
BOOL FAR PASCAL DebugDlgProc(HWND hDlg, UINT uMsg, WPARAM wParam, LPARAM lParam); // PH
BOOL FAR PASCAL LogFileDlgProc(HWND hDlg, UINT uMsg, WPARAM wParam, LPARAM lParam);
BOOL FAR PASCAL CustomAudioDlgProc(HWND hDlg, UINT uMsg, WPARAM wParam, LPARAM lParam);
BOOL FAR PASCAL SetupDlgProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
BOOL FAR PASCAL SystemTrayDlgProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam); // PH

BOOL NEAR SelectFont(HWND hDlg);

BOOL FAR PASCAL    ColorsDlgProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
LRESULT FAR PASCAL ColorWndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
BOOL FAR PASCAL    ACARSColorsDlgProc(HWND hDlg, UINT uMsg, WPARAM wParam, LPARAM lParam);                                
LRESULT FAR PASCAL ACARSColorWndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
BOOL FAR PASCAL    MOBITEXColorsDlgProc(HWND hDlg, UINT uMsg, WPARAM wParam, LPARAM lParam);
LRESULT FAR PASCAL MOBITEXColorWndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
BOOL FAR PASCAL    ERMESColorsDlgProc(HWND hDlg, UINT uMsg, WPARAM wParam, LPARAM lParam);
LRESULT FAR PASCAL ERMESColorWndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);                                                                                                

BOOL FAR PASCAL OptionsDlgProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
BOOL FAR PASCAL GeneralOptionsDlgProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
BOOL FAR PASCAL ScreenOptionsDlgProc(HWND hWnd, UINT uMsg,	WPARAM wParam, LPARAM lParam);
BOOL FAR PASCAL ScrollDlgProc(HWND hWnd, UINT uMsg,	WPARAM wParam, LPARAM lParam);
BOOL FAR PASCAL FilterDlgProc(HWND hDlg, UINT uMsg, WPARAM wParam, LPARAM lParam);
BOOL FAR PASCAL FilterEditDlgProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
BOOL FAR PASCAL FilterOptionsDlgProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
BOOL FAR PASCAL FilterFindDlgProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
BOOL FAR PASCAL FilterCheckDuplicateDlgProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
BOOL FAR PASCAL MonStatDlgProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
BOOL FAR PASCAL MailDlgProc(HWND hDlg, UINT uMsg, WPARAM wParam, LPARAM lParam);
BOOL NEAR SetTitle(HWND hWnd, TCHAR *cTitle);

BOOL ErrorMessageBox(LPCTSTR lpszText, LPCTSTR lpszTitle, LPCTSTR lpszFile, INT Line);
BOOL GetPrivateProfileSettings(LPCTSTR lpszAppTitle, LPCTSTR lpszIniPathName, PPROFILE pProfile);
void WriteSettings();
void WriteFilters(PPROFILE pProfile, int backup);
bool ReadFilters(char *szFilters, PPROFILE pProfile, bool bNew);

bool LoadDriver(void);
void UnloadDriver(void);

void WriteStatFileHourly(FILE *fp);
void WriteStatFileDaily(FILE *fp);
void CreateDateFilename(char *ext, SYSTEMTIME *yesterday);
void GetFileName(const char*, char*, int);
void SetWindowPaneSize(int param);
void SetNewWindowText(char* text);
void SystemTrayWindow(bool bHideWindow);
void SystemTrayIcon(bool bRemoveIcon);

#endif