// Gfx.cpp
// All graphics related stuff
//
// This file contains functions for the following:
//
//   1.Creating general perpose system pens/brushes.
//   2.Creating general perpose font objects.
//   3.Creating title bar gfx for pane1 and pane2.
//   4.Creating font/brush objects for pane1/pane2.
//   5.Functions for use with display_show_char() in misc.cpp.

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
#include "headers\misc.h"
#include "headers\acars.h"

int PL1_SCount=0;		// pane1 label scroll position
int PL2_SCount=0;		// pane2 label scroll position

int Max_X_Client=0;		// Holds the max clientX screen size in pixel. e.g. 800/640 etc
int NewLinePoint=0;		// Holds the max characters that can fit in client area when full screen

int iItemPositions[8] = { 0,
						  DIVIDER_OFFSET,		// Address
						  DIVIDER_OFFSET+70,	// Time
						  DIVIDER_OFFSET+120,	// Date
						  DIVIDER_OFFSET+215,	// Mode
						  DIVIDER_OFFSET+250,	// Type
						  DIVIDER_OFFSET+300,	// Bitrate
						  DIVIDER_OFFSET+390 };	// Message

int iItemWidths[7] = { 0,
					  75,		// Address
					  70,		// Time
					  75,		// Date
					  50,		// Mode
					  55,		// Type
					  55 };		// Bitrate

HBRUSH  hbr=NULL;		// background brush for main win, pain1/pain2
HBRUSH  hboxbr=NULL;
HFONT   hfont=NULL;
HFONT   hboxfont=NULL;
HFONT   pdw_font[FONT_COUNT];	// Fonts for any purpose.
LOGFONT boxfontInfo;			// Used for font dialog box etc.
HBITMAP hbm_exclam=NULL;
BITMAP	bme;

HPEN	null_pen=NULL;       // Stock Object - General purpose
HBRUSH lgray_brush=NULL;  // System 3d face brush. Color depends on system color scheme. Default is gray.
HBRUSH black_brush=NULL;  // Stock Object - General purpose
HBRUSH gray_brush=NULL;   // Stock Object - General purpose
COLORREF sys_3d_face_cr;  // Used for setting background text color.

// These are for temp dlg child window functions
char TmpDlgChildClass[17] = "WinTmpChildClass";
HBITMAP hbm_chw=NULL;
BITMAP bm_chw;

// These are used with the color dialog box, for pane1/pane2 text colors
// and for system pen/brush creation.
DWORD rgbColor[17][3] = {{ 0,   0,   0  },	// BLACK
                         { 0,   0,   128},	// BLUE
                         { 0,   128, 0  },	// GREEN
                         { 0,   128, 128},	// CYAN
                         { 255, 0,   0  },	// RED
                         { 64,  0,   128},	// MAGENTA
                         { 128, 128, 64 },	// BROWN
                         { 192, 192, 192},	// LIGHTGRAY
                         { 138, 138, 138},	// DARKGRAY
                         { 0,   0,   255},	// LIGHTBLUE
                         { 0,   255, 0  },	// LIGHTGREEN
                         { 0,   255, 255},	// LIGHTCYAN
                         { 255, 64,  64 },	// LIGHTRED
                         { 255, 0,   255},	// LIGHTMAGENTA
                         { 255, 255, 0  },	// YELLOW
                         { 255, 255, 255},	// WHITE
                         { 255, 165, 0 }};	// ORANGE


HPEN SysPEN[16]; // This holds general purpose system pens


// Draw Pane1/Pane2 Title bar graphics.
// Note: The pane1/pane2 title bars are created on the main window above each pane.
void DrawTitleBarGfx(HWND hwnd)
{
	SetMessageItemPositionsWidth();
	DrawPaneLabels(hwnd, PANE1 | PANE2 | PANERXQUAL);
}

void DrawPaneLabels(HWND hwnd, int pane)
{
	int left_edge, top_edge, i, w;
	char qual[10];
	extern double dRX_Quality;
	RECT r;

	HDC hdc = GetDC(hwnd);
	HDC hdc_mem=NULL;

	GetClientRect(hwnd, &r);

	/* Set new font/pen */
	SelectObject(hdc,pdw_font[FONT_LABELS]);
	SetAPEN_SYS(hdc,BLACK);

	if (pane & PANE1)	// DRAW TITLE BAR AND GFX FOR PANE1
	{
		left_edge = r.left + PL1_SCount;	// Work out left edge
		top_edge  = r.top+30;				// Work out top edge for all labels

		// First, draw a grey line above all labels
		SelectObject(hdc,SysPEN[DARKGRAY]);
		MoveToEx(hdc,r.left,top_edge,NULL);
		LineTo(hdc, r.right,top_edge);

		top_edge+=1;		// Move one pixel down for labels

		for (i=0, w=cxChar; i<7; i++, w=0)
		{
			if (Profile.ScreenColumns[i] == 0) break;

			switch(Profile.ScreenColumns[i])
			{
				case 1 :	// Address-box

				w += iItemWidths[1];
				Draw3D_Box(hdc,left_edge,top_edge,w,TITLE_BAR_SIZE, i);

				if (Profile.monitor_acars)
				{
					TextOut(hdc,left_edge+(i ? 1 : 5),top_edge+2,"Aircraft Reg",12);
				}
				else if (Profile.monitor_ermes || Profile.FlexGroupMode)
				{
					TextOut(hdc,left_edge+(i ? 9 : 12),top_edge+2,"Address",7);
				}
				else if (Profile.monitor_mobitex)
				{
					TextOut(hdc,left_edge+20,top_edge+2,"MAN",3);
				}
				else TextOut(hdc,left_edge+(i ? 12 : 14),top_edge+2,"Address",7);

				left_edge += iItemWidths[1];

				break;

				case 2 :	// Time box

				w += iItemWidths[2];
				Draw3D_Box(hdc,left_edge,top_edge,w,TITLE_BAR_SIZE, i);
				TextOut(hdc,left_edge+(i ? 18 : 20),top_edge+2,"Time", 4);
				left_edge += iItemWidths[2];

				break;

				case 3 :	// Date box
				
				w += iItemWidths[3];
				Draw3D_Box(hdc,left_edge,top_edge,w,TITLE_BAR_SIZE, i);
				TextOut(hdc,left_edge+18,top_edge+2,"Date", 4);
				left_edge += iItemWidths[3];

				break;

				case 4 :	// Mode box

				w += iItemWidths[4];
				Draw3D_Box(hdc,left_edge,top_edge,w,TITLE_BAR_SIZE, i);

				if (Profile.monitor_acars)
				{
					TextOut(hdc,left_edge+9,top_edge+2,"Msg.No.", 7);
				}
				else if (Profile.monitor_mobitex)
				{   
					TextOut(hdc,left_edge+13,top_edge+2,"Sender", 6);
				}
				else TextOut(hdc,left_edge+14,top_edge+2,"Mode", 4);

				left_edge += iItemWidths[4];

				break;

				case 5 :	// Type box

				w += iItemWidths[5];
				Draw3D_Box(hdc,left_edge,top_edge,w,TITLE_BAR_SIZE, i);

				if (Profile.monitor_acars)
				{
					TextOut(hdc,left_edge+7,top_edge+2,"DBI", 3);
				}
				else TextOut(hdc,left_edge+14,top_edge+2,"Type", 4);

				left_edge += iItemWidths[5];

				break;

				case 6 :	// Bitrate box

				w += iItemWidths[6];
				Draw3D_Box(hdc,left_edge,top_edge,w,TITLE_BAR_SIZE, i);

				if (Profile.monitor_acars)
				{
					TextOut(hdc,left_edge+9,top_edge+2,"Mode", 4);
				}
				else TextOut(hdc,left_edge+6,top_edge+2,"Bitrate", 7);

				left_edge += iItemWidths[6];

				break;

				case 7 :	// Message box

				w = r.right-left_edge - 46;	// 46px smaller smaller for RX-Quality
				Draw3D_Box(hdc,left_edge,top_edge,w,TITLE_BAR_SIZE, i);
				TextOut(hdc,left_edge+10,top_edge+2,"Monitored Messages",18);

				break;
			}
			if (!i) left_edge += cxChar;
		}

		// Add gray line to bottom edges
		SelectObject(hdc,SysPEN[DARKGRAY]);
		MoveToEx(hdc,r.left,top_edge+TITLE_BAR_SIZE,NULL);
		LineTo(hdc,r.right, top_edge+TITLE_BAR_SIZE);
	}

	if (pane & PANE2)	// DRAW TITLE BAR AND GFX FOR PANE2
	{
		top_edge  = pane1Pos+pane1Height;	// Work out top edge for all labels
		left_edge = r.left + PL2_SCount;	// Work out left edge

		for (i=0, w=cxChar; i<7; i++, w=0)
		{	
			if (Profile.ScreenColumns[i] == 0) break;

			switch(Profile.ScreenColumns[i])
			{
				case 1 :	// Address-box

				w += iItemWidths[1];
				Draw3D_Box(hdc,left_edge,top_edge,w,TITLE_BAR_SIZE, i);

				if (Profile.monitor_acars)
				{
					TextOut(hdc,left_edge+(i ? 1 : 5),top_edge+2,"Aircraft Reg",12);
				}
				else if (Profile.monitor_ermes || Profile.FlexGroupMode)
				{
					TextOut(hdc,left_edge+(i ? 9 : 12),top_edge+2,"Address",7);
				}
				else if (Profile.monitor_mobitex)
				{
					TextOut(hdc,left_edge+20,top_edge+2,"MAN",3);
				}
				else TextOut(hdc,left_edge+(i ? 12 : 14),top_edge+2,"Address",7);

				left_edge += iItemWidths[1];

				break;

				case 2 :	// Time box

				w += iItemWidths[2];
				Draw3D_Box(hdc,left_edge,top_edge,w,TITLE_BAR_SIZE, i);
				TextOut(hdc,left_edge+(i ? 18 : 20),top_edge+2,"Time", 4);
				left_edge += iItemWidths[2];

				break;

				case 3 :	// Date box
				
				w += iItemWidths[3];
				Draw3D_Box(hdc,left_edge,top_edge,w,TITLE_BAR_SIZE, i);
				TextOut(hdc,left_edge+18,top_edge+2,"Date", 4);
				left_edge += iItemWidths[3];

				break;

				case 4 :	// Mode box

				w += iItemWidths[4];
				Draw3D_Box(hdc,left_edge,top_edge,w,TITLE_BAR_SIZE, i);

				if (Profile.monitor_acars)
				{
					TextOut(hdc,left_edge+9,top_edge+2,"Msg.No.", 7);
				}
				else if (Profile.monitor_mobitex)
				{   
					TextOut(hdc,left_edge+13,top_edge+2,"Sender", 6);
				}
				else TextOut(hdc,left_edge+14,top_edge+2,"Mode", 4);

				left_edge += iItemWidths[4];

				break;

				case 5 :	// Type box

				w += iItemWidths[5];
				Draw3D_Box(hdc,left_edge,top_edge,w,TITLE_BAR_SIZE, i);

				if (Profile.monitor_acars)
				{
					TextOut(hdc,left_edge+7,top_edge+2,"DBI", 3);
				}
				else TextOut(hdc,left_edge+14,top_edge+2,"Type", 4);

				left_edge += iItemWidths[5];

				break;

				case 6 :	// Bitrate box

				w += iItemWidths[6];
				Draw3D_Box(hdc,left_edge,top_edge,w,TITLE_BAR_SIZE, i);

				if (Profile.monitor_acars)
				{
					TextOut(hdc,left_edge+9,top_edge+2,"Mode", 4);
				}
				else TextOut(hdc,left_edge+6,top_edge+2,"Bitrate", 7);

				left_edge += iItemWidths[6];

				break;

				case 7 :	// Message box

				w += r.right-left_edge;

				Draw3D_Box(hdc,left_edge,top_edge,w,TITLE_BAR_SIZE, -1);	// -1 so no grey line at the right
				TextOut(hdc,left_edge+10,top_edge+2,"Filtered Messages",17);

				break;
			}
			if (!i) left_edge += cxChar;
		}
		// Add gray line to bottom edges
		SelectObject(hdc,SysPEN[DARKGRAY]);
		MoveToEx(hdc,r.left,top_edge+TITLE_BAR_SIZE,NULL);
		LineTo(hdc,r.right, top_edge+TITLE_BAR_SIZE);
	}

	if (pane & PANERXQUAL)	// DRAW RX-Quality
	{
		if (dRX_Quality && dRX_Quality < 90)
		{
			SelectObject(hdc, black_brush);
			Rectangle(hdc, r.right-75, r.top+5, r.right-50, r.top+27);

			if (hdc_mem = CreateCompatibleDC(hdc))
			{
				SelectObject(hdc_mem, hbm_exclam);
				BitBlt(hdc, r.right-75, r.top+5, bme.bmWidth, bme.bmHeight, hdc_mem, 0, 0, SRCPAINT);
				DeleteDC(hdc_mem);
			}
		}
		else
		{
			SelectObject(hdc, null_pen);
			SelectObject(hdc, lgray_brush);
			Rectangle(hdc, r.right-75, r.top+5, r.right-49, r.top+28);
		}

		/* Set new font/pen */
		SelectObject(hdc,pdw_font[FONT_RXQUAL]);

		if (dRX_Quality == 0)
		{
			strcpy(qual, "RX-Q");
			SetAPEN_SYS(hdc, BLACK);
			left_edge = r.right-39;
		}
		else
		{
			if (dRX_Quality < 96) SetAPEN_SYS(hdc, RED);
			else SetAPEN_SYS(hdc, GREEN);
			
			if (dRX_Quality > 99.9)
			{
				strcpy(qual, "100%");
				left_edge = r.right-42;
			}
			else
			{
				sprintf(qual, "%.1f%%", dRX_Quality);
				left_edge = r.right-44;
			}
		}
		Draw3D_Box(hdc, r.right-46, r.top+31, 46, TITLE_BAR_SIZE, -1);
		TextOut(hdc, left_edge, r.top+33, qual, strlen(qual));
	}
	ReleaseDC(hwnd, hdc);
}


// Get general purpose logfonts
bool GetLogFONTS(void)
{
	bool bOK=true;
	LOGFONT logfont;
/*	
	memset(&logfont, 0, sizeof(LOGFONT));

	logfont.lfHeight			= -11;
	logfont.lfWidth				= 0;
	logfont.lfEscapement		= 0;
	logfont.lfOrientation		= 0;
	logfont.lfWeight			= FW_NORMAL;
	logfont.lfItalic			= 0;
	logfont.lfUnderline			= 0;
	logfont.lfStrikeOut			= 0;
	logfont.lfCharSet			= OEM_CHARSET;
	logfont.lfOutPrecision		= OUT_STROKE_PRECIS;
	logfont.lfClipPrecision		= CLIP_DEFAULT_PRECIS;
	logfont.lfQuality			= DEFAULT_QUALITY;
	logfont.lfPitchAndFamily	= FIXED_PITCH | FF_MODERN;
*/
	for (int i=0; i<FONT_COUNT; i++)
	{
		memset(&logfont, 0, sizeof(LOGFONT));

		switch (i)
		{
			case FONT_LABELS:

			lstrcpy(logfont.lfFaceName, "Verdana");
			logfont.lfHeight = -11;
			logfont.lfWeight = FW_NORMAL;
			
			break;

			case FONT_RXQUAL:

			lstrcpy(logfont.lfFaceName, "Verdana");
			logfont.lfHeight = -11;
			logfont.lfWeight = FW_BOLD;

			break;

/*			case FONT_COURIERNEW:
					lstrcpy(logfont.lfFaceName, "Courier New");
			break;

			default:
			case FONT_TIMESNEWROMAN:
					lstrcpy(logfont.lfFaceName, "Times New Roman");
					logfont.lfHeight = -13;
			break;

			case FONT_ARIAL:
					lstrcpy(logfont.lfFaceName, "Arial");
					logfont.lfHeight = -12;
			break;
*/
		}
		if (!(pdw_font[i] = CreateFontIndirect(&logfont))) bOK=false;
	}
	return(bOK);
}

// Free font objects allocated by above
void FreeLogFONTS(void)
{
	for (int i=0; i<FONT_COUNT;i++) if (pdw_font[i]) DeleteObject(pdw_font[i]);
}


// Setup a box font
bool SetBoxFONT(void)
{
	boxfontInfo.lfHeight		= -11;
	boxfontInfo.lfWidth			= 0;
	boxfontInfo.lfEscapement	= 0;
	boxfontInfo.lfOrientation	= 0;
	boxfontInfo.lfWeight		= FW_NORMAL;
	boxfontInfo.lfItalic		= 0;
	boxfontInfo.lfUnderline		= 0;
	boxfontInfo.lfStrikeOut		= 0;
	boxfontInfo.lfCharSet		= OEM_CHARSET;
	boxfontInfo.lfOutPrecision	= OUT_STROKE_PRECIS;
	boxfontInfo.lfClipPrecision	= CLIP_DEFAULT_PRECIS;
	boxfontInfo.lfQuality		= DEFAULT_QUALITY;
	boxfontInfo.lfPitchAndFamily= FIXED_PITCH | FF_MODERN;
	lstrcpy(boxfontInfo.lfFaceName, "Courier New");

	if (!(hboxfont = CreateFontIndirect(&boxfontInfo))) return(false);

	return(true);
}


// General purpse drawing and painting objects
// These use standard system colours
bool GetSysObjects(HWND hwnd)
{
	HDC hdc;
	bool bOK=true;
	LOGBRUSH log_brush;
 
	// General purpose system pens
	hdc = GetDC(hwnd);

	for (int i=0; i<16; i++)
	{
		if (!(SysPEN[i] = CreatePen(PS_SOLID,0,
			  GetNearestColor(hdc, RGB(rgbColor[i][0],
			  rgbColor[i][1],rgbColor[i][2])))))
		{
			bOK=false;
		}
	}

	ReleaseDC(hwnd, hdc);

	if(!bOK) return(false); // Got all pens?

	// Get system brush for drawing 3D surfaces.
	sys_3d_face_cr = (COLORREF)GetSysColor(COLOR_3DFACE);
	log_brush.lbStyle = BS_SOLID;
	log_brush.lbColor = (COLORREF)sys_3d_face_cr;
	log_brush.lbHatch = 0;    
    
	if (!(lgray_brush = (HBRUSH) CreateBrushIndirect(&log_brush))) return(false);

	// Windows stockobjects
	if (!(gray_brush = (HBRUSH) GetStockObject(GRAY_BRUSH))) return(false);
	if (!(black_brush = (HBRUSH) GetStockObject(BLACK_BRUSH))) return(false);
	if (!(null_pen = (HPEN) GetStockObject(NULL_PEN))) return(false);

	hbm_exclam = LoadBitmap(ghInstance,MAKEINTRESOURCE((WORD)IDS_EXCLAM));
	GetObject(hbm_exclam, sizeof(bme), &bme);

	return(true);
}


void FreeSysObjects(void)	// Free objects allocated by GetSysObjects.
{
	// Free general purpose system pens
	for(int i=0; i<16; i++) if (SysPEN[i]) DeleteObject(SysPEN[i]);

	// Free windows stockobjects
	if (gray_brush)	DeleteObject(gray_brush);
	if (lgray_brush)DeleteObject(lgray_brush);
	if (null_pen)	DeleteObject(null_pen);
	if (black_brush)DeleteObject(black_brush);
}

bool Get_Drawing_Objects(void)	// Get pain1/pain2/font drawing objects.
{
	// These 3 are used by main program for background color etc.
	if(!(hbr = CreateSolidBrush(Profile.color_background)))		return(false);
	if(!(hboxbr = CreateSolidBrush(Profile.color_background)))  return(false);
	if(!(SetBoxFONT())) return(false);

	return(true);
}

void Free_Drawing_Objects(void)	// Free drawing objects allocated by Get_Drawing_Objects()
{
	// Free main win/pain1/pain2 related stuff
	if(hbr)		DeleteObject(hbr);
	if(hboxbr)	DeleteObject(hboxbr);
	if(hfont)	DeleteObject(hfont);
	if(hboxfont)DeleteObject(hboxfont);
}

void SetAPEN(HDC hdc, int fpen, int bpen)	// This simplifies pen color selection
{
	SetTextColor(hdc,GetNearestColor(hdc, RGB(rgbColor[fpen][0],
					 rgbColor[fpen][1],rgbColor[fpen][2])));
	SetBkColor(hdc,GetNearestColor(hdc, RGB(rgbColor[bpen][0],
					 rgbColor[bpen][1],rgbColor[bpen][2])));
}

// This simplifies pen color selection.
// This always uses the default 3Dface system color(gray-default) for the background pen color.
void SetAPEN_SYS(HDC hdc, int fpen)
{
	SetTextColor(hdc,GetNearestColor(hdc, RGB(rgbColor[fpen][0], rgbColor[fpen][1],rgbColor[fpen][2])));
	SetBkColor(hdc,  GetNearestColor(hdc, sys_3d_face_cr));
}

// Draw a filled 3D style box inside hwnd.
// x,y are upper left corner of box.
// w,h are width/height of box. Min for these is 6 pixels.
// Fill color is light gray.
void Draw3D_Box(HDC hdc, int x, int y, int w, int h, int item)
{
	// Fill box
	SelectObject(hdc,null_pen);
	SelectObject(hdc,lgray_brush);
	Rectangle(hdc,x,(y+1),(x+w)+1,(y+h)+1);

	// Draw line to go round box
	SelectObject(hdc,SysPEN[WHITE]);

	if (item)	// Only if not first item
	{
		MoveToEx(hdc, x,(y+h),NULL);
		LineTo(hdc,x,y);				// left line
	}

	MoveToEx(hdc,x,y,NULL);				// Starting point
	LineTo(hdc,(x+w),y);				// top line

	SelectObject(hdc,SysPEN[BLACK]);
	LineTo(hdc,(x+w),(y+h));			// right line

	if (item >= 0)	// Only if not last item
	{
		// Add gray line to right edges
		SelectObject(hdc,SysPEN[DARKGRAY]);
		MoveToEx(hdc,(x+w)-1,(y+h)-1,NULL);
		LineTo(hdc,(x+w)-1,(y));
	}
}

// Draw unfilled-inverted 3D style box inside hwnd.
// x,y are upper left corner of box.
// w,h are width/height of box. Min for these is 6 pixels.
// Fill color is light gray.
void Draw3D_Box_INV(HDC hdc,int x, int y, int w, int h)
{
	// Draw line to go round box
	SelectObject(hdc,SysPEN[BLACK]);
	MoveToEx(hdc, x,(y+h),NULL);
	LineTo(hdc,x,y);              // left line
	LineTo(hdc,(x+w),y);          // top line

	SelectObject(hdc,SysPEN[WHITE]);
	LineTo(hdc,(x+w),(y+h));        // right line
	LineTo(hdc,x-1,(y+h));          // bottom line1
}


void SetMessageItemPositionsWidth()
{
	int i, item, iTempPosition = 1;

	iItemWidths[2] = 9*cxChar;	// Time
	iItemWidths[3] = 9*cxChar;	// Date

	if (Profile.FlexGroupMode || Profile.monitor_ermes)
	{
		iItemWidths[1] = 9*cxChar;	// Address
		iItemWidths[4] = 9*cxChar;	// Mode
		iItemWidths[5] = 8*cxChar;	// Type
		iItemWidths[6] = 7*cxChar;	// Bitrate
	}
	else if (Profile.monitor_mobitex)
	{
		iItemWidths[1] = 9*cxChar;	// Address
		iItemWidths[4] = 9*cxChar;	// Sender
		iItemWidths[5] = 8*cxChar;	// Type
		iItemWidths[6] = 7*cxChar;	// Bitrate
	}
	else // if (Profile.monitor_paging || ACARS)
	{
		iItemWidths[1] = 10*cxChar;	// Address / Aircraft Reg
		iItemWidths[4] =  9*cxChar;	// Mode	/ Msg No
		if (Profile.monitor_acars)
		{
			iItemWidths[5] = 5*cxChar;	// DBI
		}
		else iItemWidths[5] = 8*cxChar;	// Type
		iItemWidths[6] =  7*cxChar;	// Bitrate / Mode
	}

	for (i=0; i<7; i++)
	{
		item = Profile.ScreenColumns[i];

		if (item == 0) break;

		iItemPositions[item] = iTempPosition;
		iTempPosition+=iItemWidths[item]/cxChar;

		switch(item)
		{
			case 1 : // Address

			if (Profile.monitor_acars) iItemPositions[1]+=1;	// ACARS FlightNo
			if (Profile.FlexGroupMode) iItemPositions[1]+=1;	// Flex Groupmode

			break;

			case 2 : // Time
			break;

			case 3 : // Date
			break;

			case 4 : // Mode

			if (Profile.monitor_acars)   iItemPositions[4]+=2;	// ACARS Msg.No
			if (Profile.monitor_ermes)   iItemPositions[4]+=1;	// ERMES Mode
			if (Profile.monitor_mobitex) iItemPositions[4]+=1;	// Mobitex Sender

			break;

			case 5 : // Type

			if (Profile.monitor_acars)   iItemPositions[5]+=2;	// ACARS DBI
			if (Profile.monitor_mobitex) iItemPositions[5]+=1;	// Mobitex Type

			break;

			case 6 : // Bitrate

			if (Profile.monitor_acars) iItemPositions[6]+=2;	// ACARS Mode

			break;

			case 7 : // Message

			iItemPositions[7] += 1;

			break;
		}
	}
}


// Tmp child window for displaying bitmap in dialog box..
// Image to display is indicated by rc_index.
// x,y = window position inside dialog box.
// Window size is size of bitmap.
HWND TmpDlgChildWin(HWND phwnd, HINSTANCE hInstance, int rc_index, int x, int y)
{
	HWND hwnd=NULL;
	WNDCLASS wndclass;
	static int registered_class = 0;
 
	// Get bitmap resource.
	if (hbm_chw = LoadBitmap(hInstance, MAKEINTRESOURCE((WORD)rc_index)))
	{
		if(!(GetObject(hbm_chw, sizeof(bm_chw), &bm_chw)))
		{
			DeleteObject(hbm_chw);
			return(NULL);
		}
	}
 
	if(!registered_class)
	{  
		wndclass.style			= 0;
		wndclass.lpfnWndProc	= TmpDlgChildWinProc;
		wndclass.cbClsExtra		= 0;
		wndclass.cbWndExtra		= sizeof(LONG);
		wndclass.hInstance		= hInstance;
		wndclass.hIcon			= NULL;
		wndclass.hCursor		= LoadCursor(NULL, IDC_ARROW);
		wndclass.hbrBackground	= (HBRUSH) GetStockObject(BLACK_BRUSH); //(LTGRAY_BRUSH);
		wndclass.lpszMenuName	= NULL;
		wndclass.lpszClassName	= TmpDlgChildClass;

		if (!RegisterClass(&wndclass)) return (NULL);
		registered_class = 1;
	}
 
	hwnd = CreateWindow(TmpDlgChildClass, NULL,
						WS_CHILD | WS_VISIBLE,
						x,y,bm_chw.bmWidth,bm_chw.bmHeight,
						phwnd, NULL,hInstance, 0);
	return(hwnd);
}

// Procedure for tmp child window.
LRESULT CALLBACK TmpDlgChildWinProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	PAINTSTRUCT ps;
	HDC hdc;
	HDC hdc_mem=NULL;

	switch (message)
	{
		case WM_PAINT:
		BeginPaint(hwnd,&ps);
		hdc = GetDC(hwnd);
		Draw3D_Box_INV(hdc,0,0, bm_chw.bmWidth+1,bm_chw.bmHeight+1);
        
		if(hbm_chw)
		{
			// draw bitmap
			if (hdc_mem = CreateCompatibleDC(hdc))
			{
				SelectObject(hdc_mem, hbm_chw);
				BitBlt(hdc, 0, 0, bm_chw.bmWidth,
				bm_chw.bmHeight, hdc_mem, 0, 0, SRCPAINT);
				DeleteDC(hdc_mem);
			}
		}
		ReleaseDC(hwnd,hdc);
		EndPaint(hwnd,&ps);
		break;

		case WM_CREATE:
		break;

		case WM_CLOSE:                  // Fall through.
		if(hbm_chw != NULL)
		{      // Delete bitmap.
			DeleteObject(hbm_chw);
			hbm_chw = NULL;
		}
		default:                   /* for messages that we don't deal with */

		return DefWindowProc(hwnd, message, wParam, lParam);
	}
	return 0L;
}
