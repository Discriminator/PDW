//
// Sigind.cpp
//
// This file contains functions for displaying/updating
// the signal indicator.
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
#include "headers\sigind.h"

#define MAX_SI_POS        20	// 0-12. Max positions available to signal indicator.
#define AUDIO_POINT_VALUE 2	// Used for working out samples per signal
										// position.e.g. sample=20/10, position=2.

BOOL old_rect_flg=FALSE;
BOOL got_sigind=FALSE;
HDC hdcMemory=NULL;

HBITMAP hbm_sigind=NULL;
BITMAP bms;
RECT old_rect;     // used for redrawing signal indicator bitmap
RECT sig_rect;     // used for holding current position of sigind bitmap

// Keep track of signal indicator.
int si_index=0;
int si_old_index=0;
int delay_cnt=0;
int si_low_hover=0;
int si_hi_hover=0;

// Points used for drawing signal indicator.

int sip[21][4] =	{//from: x,  y, To: x,  y
							{18, 20,  6, 14},
//							{18, 20,  7, 13},
							{18, 20,  8, 12},
							{18, 20,  9, 11},
							{18, 20, 10, 10},
							{18, 20, 11,  9},
							{18, 20, 12,  9},
							{18, 20, 13,  8},
							{18, 20, 15,  8},
							{18, 20, 16,  7},
							{18, 20, 17,  7},
							{18, 20, 19,  7},
							{18, 20, 20,  7},
							{18, 20, 22,  8},
							{18, 20, 23,  8},
							{18, 20, 24,  9},
							{18, 20, 25,  9},
							{18, 20, 26, 10},
							{18, 20, 27, 11},
							{18, 20, 28, 12},
							{18, 20, 29, 13},
							{18, 20, 30, 14},
					};


// Get signal indicator bitmap resource
BOOL LoadSigInd(HINSTANCE hThisInstance)
{
	if (!(got_sigind))
	{
		if (hbm_sigind = LoadBitmap(hThisInstance,MAKEINTRESOURCE((WORD)IDS_SIGIND)))
		{
			if (GetObject(hbm_sigind, sizeof(bms), &bms)) got_sigind=TRUE;
			else DeleteObject(hbm_sigind);
		}
	}
	return(got_sigind);
}

// Free bitmap object
void FreeSigInd(void)
{
	if (got_sigind) DeleteObject(hbm_sigind);
}

// Draw signal indicator on toolbar
void DrawSigInd(HWND hwnd)
{
	HDC hdc;
	RECT r;
	int x=5,y=4;
	si_index=0;  // this is used by UpdateSigInd().
	extern double dRX_Quality;

	if (got_sigind)
	{
		hdc = GetDC(hwnd);
		SelectObject(hdc,null_pen);

		if (old_rect_flg)
		{  // erase last display of bitmap
			SelectObject(hdc,lgray_brush);
			Rectangle(hdc,old_rect.right-(bms.bmWidth+x),y,old_rect.right-(x-1),bms.bmHeight+y+1);
		}

		GetClientRect(hwnd, &r);
		GetClientRect(hwnd, &old_rect);
		old_rect_flg=TRUE;

		// need black background for bitmap
		SelectObject(hdc,black_brush);
		Rectangle(hdc,r.right-(bms.bmWidth+x),y,r.right-(x-1),bms.bmHeight+y+1);

		// Keep record of bitmaps current location
		sig_rect.left	= r.right-(bms.bmWidth+x);
		sig_rect.top	= y;
		sig_rect.bottom	= bms.bmHeight + y;
		sig_rect.right	= r.right-(x-1);

		// draw bitmap
		if (hdcMemory = CreateCompatibleDC(hdc))
		{
			SelectObject(hdcMemory, hbm_sigind);
			BitBlt(hdc,sig_rect.left,sig_rect.top, bms.bmWidth, bms.bmHeight, hdcMemory, 0, 0, SRCPAINT);
			DeleteDC(hdcMemory);
		}
		ReleaseDC(hwnd,hdc);
		show_sigind(0, 0);	// show sigind needle
	}
}

// Update signal indicator on toolbar.
// Draws a new line on signal indicator,
// removing old line first.
void UpdateSigInd(int direction_flg)
{
	si_old_index = si_index;

	if (old_rect_flg)
	{
		if (direction_flg)	// Move indictor right 1
		{
			si_low_hover=0;
			si_index ? si_index+=2 : si_index++;

			if (si_index > MAX_SI_POS)
			{
				si_hi_hover++;
				si_index=MAX_SI_POS;
				return;
			}
	 	}
		else
		{							// Move indictor left 1
			if (si_low_hover)
			{
				if (si_low_hover==1)
				{
					show_sigind(1, 0);
				}
				if (si_low_hover > 1)
				{
					si_low_hover=0;
					show_sigind(0, 1);
				}
			}
			si_hi_hover=0;
			si_index--;

			if (si_index < 0)
			{
				si_low_hover++;
				si_index=0;
				return;
			}
		}
		show_sigind(si_index, si_old_index);
	}
}


// Draw signal indicator needle.
// Draw needle at new_pos.
// old_pos is used to erase previous line.
void show_sigind(int new_pos,int old_pos)
{
	HDC hdc;
	int x,y;

	hdc = GetDC(ghWnd);

	// erase old line.
	SelectObject(hdc,SysPEN[WHITE]);
	x = sig_rect.left+sip[old_pos][0];
	y = sig_rect.top+sip[old_pos][1];
	MoveToEx(hdc,x,y,NULL);

	x = sig_rect.left+sip[old_pos][2];
	y = sig_rect.top+sip[old_pos][3];
	LineTo(hdc,x,y);

	// Draw new line.
	SelectObject(hdc,SysPEN[RED]);
	x = sig_rect.left+sip[new_pos][0];
	y = sig_rect.top+sip[new_pos][1];
	MoveToEx(hdc,x,y,NULL);

	x = sig_rect.left+sip[new_pos][2];
	y = sig_rect.top+sip[new_pos][3];
	LineTo(hdc,x,y);

	ReleaseDC(ghWnd,hdc);
}

