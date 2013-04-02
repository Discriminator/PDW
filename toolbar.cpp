//
// Toolbar.cpp
//
// Functions to allocate the toolbar/buttons and button bitmaps
// Function to set toolbar tooltips
//

#include <windows.h>
#include <commctrl.h>
#include <mmsystem.h>
#include <stdio.h>
#include <commdlg.h>
#include <string.h>
#include <time.h>

#include "headers\resource.h"
#include "headers\toolbar.h"


#define NUM_TB_BUTTONS 18   // 14 buttons and 5 seperators
#define NUM_TB_IMAGES  13   // 14 button images
// #define TOOLBAR_DEBUG  1

TBBUTTON toolbar_btns[NUM_TB_BUTTONS];  // Toolbar buttons
UINT tb_btn_image[NUM_TB_IMAGES];       // Bitmap Handles to Toolbar images


// Allocate toolbar and return its window handle
HWND ShowMakeToolBar(HWND parent_hwnd,HINSTANCE hThisInstance)
{
	HWND tbhwnd=NULL;

#ifdef TOOLBAR_DEBUG
 HDC hdc;
#endif

	for(int i=0; i<NUM_TB_IMAGES; i++) tb_btn_image[i]=0; // Ensure only images allocated are freed.

	InitCommonControls();               // Ensure dll is loaded for CreateToolbarEx.

	if(GetToolBarImages(hThisInstance)) // Load bitmap button images
	{
		SetToolBarButtons();            // Set type/state for each button

		// Create Toolbar without buttons. Add these later
		tbhwnd = CreateToolbarEx(parent_hwnd,
							     WS_CHILD|WS_VISIBLE|WS_DLGFRAME|TBSTYLE_TOOLTIPS,
								 IDW_TOOL_BAR,
								 0,
								 NULL,
								 0,
								 0,
								 0,
								 18,
								 18,
								 18,
								 18,
								 sizeof(TBBUTTON));

		if(!(tbhwnd))
		{                         // Got tool bar?
			FreeToolBarImages(hThisInstance);  // error? - free any allocated objects
			return(NULL);
		}
		Add_TB_ButtonsBitmaps(tbhwnd,hThisInstance); // Add rest of bitmaps/buttons.
	}

#ifdef TOOLBAR_DEBUG
	hdc = GetDC(parent_hwnd);

	if(tbhwnd == NULL) TextOut(hdc, 0, 70, "NOT Allocated!", 14);
	else TextOut(hdc, 0, 70, "OK!!", 4);// UpdateWindow(hToolbar);

	ReleaseDC(parent_hwnd, hdc);

#endif

	return(tbhwnd);
}


// Load toolbar bitmap images
// Start from TOOL_BAR_BTN0
//
BOOL GetToolBarImages(HINSTANCE hThisInstance)
{
	WORD btn_index = (WORD)IDT_TOOLBAR_BTN0;

	for(int i=0; i<NUM_TB_IMAGES; i++)
	{
		if(!(tb_btn_image[i] =(UINT)LoadBitmap(hThisInstance,MAKEINTRESOURCE((WORD)btn_index))))
		{
			FreeToolBarImages(hThisInstance);
			return(FALSE);
		}
		btn_index++;
	}
	return(TRUE);
}

// Free Toolbar Bitmap images
void FreeToolBarImages(HINSTANCE hThisInstance)
{
	for(int i=0;i < NUM_TB_IMAGES;i++) if (tb_btn_image[i]) DeleteObject((HGDIOBJ)tb_btn_image[i]);
}


// Set info for each toolbar button.
void SetToolBarButtons(void)
{
	WORD btn_index =(WORD)IDT_TOOLBAR_BTN0;

	for (int i=0, x=0; i<NUM_TB_BUTTONS; i++)
	{
		toolbar_btns[i].fsState=(BYTE)(TBSTATE_ENABLED);
		toolbar_btns[i].dwData=0;
		toolbar_btns[i].iString=0;

		switch(i)
		{   // workout where to put seperators.
			case 0:
			// 1 = open/close log file
			case 2:
			// 3,4,5,6,7 = copy/upper/lower/save/print window panes
			case 8:
			// 9,10 = options/filter
			case 11:
			// 12,13 = statistics/pause
			case 14:
			// 15,16 = help/clear screen

			toolbar_btns[i].iBitmap = 0;
			toolbar_btns[i].fsStyle=(BYTE)TBSTYLE_SEP;
			toolbar_btns[i].idCommand = 0;
			break;

			default:  // Actual button
			toolbar_btns[i].fsStyle=(BYTE)TBSTYLE_BUTTON;
			toolbar_btns[i].iBitmap = x++;
			toolbar_btns[i].idCommand = btn_index++;
		}
	}
}

// Add all toolbar bitmaps then add all toolbar buttons
void Add_TB_ButtonsBitmaps(HWND tbar_hwnd,HINSTANCE hThisInstance)
{
	WORD btn_index = (WORD)IDT_TOOLBAR_BTN0;
	TBADDBITMAP tb_add_bmp;

	for(int i=0; i<NUM_TB_BUTTONS; i++)      // 1st add bitmaps.
	{
		tb_add_bmp.hInst = hThisInstance;  // Location of bitmap
		tb_add_bmp.nID = btn_index++;      // Bitmap resource identifier
		SendMessage(tbar_hwnd,(UINT)TB_ADDBITMAP,1,(LPARAM)&tb_add_bmp);
	}
	// Then add buttons
	SendMessage(tbar_hwnd,(UINT)TB_ADDBUTTONS,(WPARAM)NUM_TB_BUTTONS,(LPARAM)toolbar_btns);
}

// used to ensure toolbar stays correct size.
void TB_AutoSize(HWND hTbar)
{
	SendMessage(hTbar, (UINT)TB_AUTOSIZE, 0, 0);
}


// Called when parent window receives a WM_NOTIFTY message.
// Sets the toolbar Tool Tip text if it's a TTN_NEEDTEXT message.
void SetToolTXT(HINSTANCE hThisInstance, LPARAM lParam)
{
	LPTOOLTIPTEXT new_txt;		// Used for setting tool-tip text for toolbar.

	new_txt = (LPTOOLTIPTEXT)lParam;
	new_txt->hinst = hThisInstance;

	switch(new_txt->hdr.idFrom)	// Set text, based on which control the cursor is currently on
	{
		case IDT_TOOLBAR_BTN0:
		new_txt->lpszText = "Logfile";
		break;

		case IDT_TOOLBAR_BTN1:
		new_txt->lpszText = "Copy Selection";
		break;

		case IDT_TOOLBAR_BTN2:
		new_txt->lpszText = "Copy Monitor Window";
		break;

		case IDT_TOOLBAR_BTN3:
		new_txt->lpszText = "Copy Filter Window";
		break;

		case IDT_TOOLBAR_BTN4:
		new_txt->lpszText = "Save Clipboard";
		break;

		case IDT_TOOLBAR_BTN5:
		new_txt->lpszText = "Print Clipboard";
		break;

		case IDT_TOOLBAR_BTN6:
		new_txt->lpszText = "Options";
		break;

		case IDT_TOOLBAR_BTN7:
		new_txt->lpszText = "Filters";
		break;

		case IDT_TOOLBAR_BTN8:
		new_txt->lpszText = "Statistics";
		break;

		case IDT_TOOLBAR_BTN9:
		new_txt->lpszText = "Pause";
		break;

		case IDT_TOOLBAR_BTN10:
		new_txt->lpszText = "Help";
		break;

		case IDT_TOOLBAR_BTN11:
		new_txt->lpszText = "Clear Screen";
		break;

		case IDT_TOOLBAR_BTN12:
		new_txt->lpszText = "Change Mode";
		break;

//		default:
//		new_txt->lpszText = "*";
//		break;
	}
}
