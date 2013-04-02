//
// Printer.cpp
//

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
#include "headers\decode.h"
#include "headers\sound_in.h"
#include "headers\printer.h"

PRINTDLG printdlg; 
DOCINFO docinfo;
int printOK = 1;
HWND PrtCancel_hDlg = NULL;


// Print out buffer, 
// Seperate each line if '\n' is detected.
//
void PrintBuffer(LPSTR lpBuffer)
{
	TEXTMETRIC tm;
	LPSTR s;
	int i, copies;
	int prtX = 0;       // current X output location
	int prtY = 0;       // current Y output location

	/* initialize PRINTDLG struct */
	PrintInit(&printdlg, ghWnd);

	if (!PrintDlg(&printdlg)) return;

	docinfo.cbSize = sizeof(DOCINFO);
	docinfo.lpszDocName = "Print...";
	docinfo.lpszOutput = NULL;
	docinfo.lpszDatatype = NULL;
	docinfo.fwType = 0;

	StartDoc(printdlg.hDC, &docinfo);

	/* get text metrics for printer */
	GetTextMetrics(printdlg.hDC, &tm);
	printOK = 1;
	SetAbortProc(printdlg.hDC, (ABORTPROC) AbortFunc);
	PrtCancel_hDlg = CreateDialog(ghInstance, "PrCancel", ghWnd, (DLGPROC) KillPrint);

	for (copies=0; copies<printdlg.nCopies; copies++)
	{
		StartPage(printdlg.hDC);
		prtX = 0;
		prtY = 0;
		i=0;
		s = lpBuffer;

		while (((*s) && (i < 15000)))
		{
			if (*s == '\n')
			{
				prtX = 0;
				prtY += tm.tmHeight + tm.tmExternalLeading;
				s++;
				continue;
			}
			if (((*s != ' ') && ((*s < 0x21) || (*s > 0x7E))))
			{
				s++;
				continue;
			}

			TextOut(printdlg.hDC, prtX, prtY,(LPSTR)s, 1);

			prtX += tm.tmAveCharWidth + tm.tmExternalLeading;
			s++;
			i++;
		}
		EndPage(printdlg.hDC);
	}

   if (printOK)
	{
		if (PrtCancel_hDlg) DestroyWindow(PrtCancel_hDlg);
		EndDoc(printdlg.hDC);
	}
	DeleteDC(printdlg.hDC);
}


/* Initialize PRINTDLG structure. */
void PrintInit(PRINTDLG *printdlg, HWND hwnd)
{
	printdlg->lStructSize = sizeof(PRINTDLG);
	printdlg->hwndOwner = hwnd;
	printdlg->hDevMode = NULL;
	printdlg->hDevNames = NULL;
	printdlg->hDC = NULL;
	printdlg->Flags = PD_RETURNDC | PD_NOSELECTION | PD_NOPAGENUMS | PD_HIDEPRINTTOFILE;
	printdlg->nFromPage = 0;
	printdlg->nToPage = 0;
	printdlg->nMinPage = 0;
	printdlg->nMaxPage = 0;
	printdlg->nCopies = 1;
	printdlg->hInstance = NULL;
	printdlg->lCustData = 0;
	printdlg->lpfnPrintHook = NULL;
	printdlg->lpfnSetupHook = NULL;
	printdlg->lpPrintTemplateName = NULL;
	printdlg->lpSetupTemplateName = NULL;
	printdlg->hPrintTemplate = NULL;
	printdlg->hSetupTemplate = NULL;
}
 

/* Printer abort function. */
BOOL CALLBACK AbortFunc(HDC hdc, int err)
{
	MSG message;

	while (PeekMessage(&message, NULL, 0, 0, PM_REMOVE))
	{
		if (!IsDialogMessage(PrtCancel_hDlg, &message))
		{
			TranslateMessage(&message);
			DispatchMessage(&message);
		}
	}
	return printOK;
}

/* Let user kill print process. */
LRESULT CALLBACK KillPrint(HWND hdwnd, UINT message,
                           WPARAM wParam, LPARAM lParam)
{
	switch (message)
	{
		case WM_INITDIALOG:
			CenterWindow(hdwnd);
		break;

		case WM_COMMAND:
	
		switch (LOWORD(wParam))
		{
			case IDCANCEL:
				printOK = 0;
				DestroyWindow(PrtCancel_hDlg);
				PrtCancel_hDlg = NULL;
			return 1;
		}
		break;
	}
	return 0;
}
