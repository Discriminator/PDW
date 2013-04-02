#ifndef TOOL_BAR_H
#define TOOL_BAR_H

HWND ShowMakeToolBar(HWND parent_hwnd,HINSTANCE hThisInstance);
BOOL GetToolBarImages(HINSTANCE hThisInstance);
void FreeToolBarImages(HINSTANCE hThisInstance);
void SetToolBarButtons(void);
void Add_TB_ButtonsBitmaps(HWND tbar_hwnd,HINSTANCE hThisInstance);
void TB_AutoSize(HWND hTbar);
void SetToolTXT(HINSTANCE hThisInstance, LPARAM lParam);

#endif
