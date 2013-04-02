#ifndef PDW_GFX_H
#define PDW_GFX_H

#define TITLE_BAR_SIZE   18   // Size of pane1/pane2 title bars.
#define DIVIDER_OFFSET   12

/*
#define ADDRESS_TXT_POS			DIVIDER_OFFSET
#define DATETIME_TXT_POS		70+DIVIDER_OFFSET		// PH: was 80
#define MODETYPEBAUD_TXT_POS	215+DIVIDER_OFFSET	// PH: was 225
#define MSG_TXT_POS				390+DIVIDER_OFFSET	// PH: was 385

#define ADDRESS_BOX_SIZE		 75	// PH: was 80
#define DATETIME_BOX_SIZE		145	// PH: was 145
#define MODETYPEBAUD_BOX_SIZE	175	// PH: was 80 (was 70)
#define MSG_BOX_SIZE			 80	// PH: was 80
*/

#define BLACK		0
#define BLUE		1
#define GREEN		2
#define CYAN		3
#define RED			4
#define MAGENTA		5
#define BROWN		6
#define LTGRAY		7
#define DARKGRAY	8
#define LTBLUE		9
#define LTGREEN		10
#define LTCYAN		11
#define LTRED		12
#define LTMAGENTA	13
#define YELLOW		14
#define WHITE		15
#define ORANGE		16

/*
#define FONT_COURIERNEW      0
#define FONT_TIMESNEWROMAN   1
#define FONT_ARIAL           2
#define FONT_VERDANA         3
#define FONT_COUNT           4	// Number of fonts above
*/
#define FONT_LABELS		0
#define FONT_RXQUAL		1
#define FONT_COUNT		2	// Number of fonts above

extern HBRUSH  hbr;
extern HBRUSH  hboxbr;
extern HFONT   hfont;
extern HFONT   hboxfont;
extern HFONT   pdw_font[FONT_COUNT];
extern LOGFONT boxfontInfo;

extern HPEN   null_pen;     // Stock Object - General purpose
extern HBRUSH gray_brush;   // Stock Object - General purpose
extern HBRUSH lgray_brush;  // Stock Object - General purpose
extern HBRUSH black_brush;  // Stock Object - General purpose
extern DWORD  rgbColor[17][3];
extern HPEN   SysPEN[16];   // This holds general purpose system pens
extern int Max_X_Client;
extern int NewLinePoint;
extern int PL1_SCount;
extern int PL2_SCount;

extern int iItemPositions[8];
extern int iItemWidths[7];

// Functions
bool SetBoxFONT(void);
bool Get_Drawing_Objects(void);
bool GetSysObjects(HWND hwnd);
bool GetLogFONTS(void);
void FreeLogFONTS(void);
void FreeSysObjects(void);
void Free_Drawing_Objects(void);
void Draw3D_Box(HDC hdc, int x, int y, int w, int h, int item);
void Draw3D_Box_INV(HDC hdc,int x, int y, int w, int h);
void DrawTitleBarGfx(HWND hwnd);
void SetAPEN(HDC hdc,int fpen, int bpen);
void SetAPEN_SYS(HDC hdc, int fpen);
void DrawPaneLabels(HWND hwnd, int pane);
void copy_logfont(LOGFONT *from,LOGFONT *to);
void SetMessageItemPositionsWidth();

HWND TmpDlgChildWin(HWND phwnd, HINSTANCE hInstance, int rc_index, int x, int y);
LRESULT CALLBACK TmpDlgChildWinProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam);

#endif
