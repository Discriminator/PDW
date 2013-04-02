#ifndef PDW_PRINTER_H
#define PDW_PRINTER_H

// printer.h

void PrintInit(PRINTDLG *printdlg, HWND hwnd);
void PrintBuffer(LPSTR lpBuffer);
BOOL CALLBACK AbortFunc(HDC hdc, int err);
LRESULT CALLBACK KillPrint(HWND, UINT, WPARAM, LPARAM);

#endif
