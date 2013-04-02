// filename:		helper_funcs.h
// Description:		Prototypes of commonly used functions
// Author:			Andreas Verhoeven
// Last modified:	19 January 2004, 20:00
// Version:			1
// Comments:
//					<none>
// Notes:
//					<none>
// 
//	(c) copyright Andreas Verhoeven 2004
//	Do not modify, reproduce or use this file without my permission!
//
//


#ifndef HELPER_FUNCS_H_303001_
#define HELPER_FUNCS_H_303001_

#include <windows.h>

extern char szCurrentDate[40];
extern char szCurrentTime[40];

void GetFileName(const char* filepath, char* buffer, int buffersize);
void GetFilePath(const char* filepath, char* buffer, int buffersize);
void GetFileNameWithoutExtension(char* filepath);
char* lowercase(char* s);
bool FileExists(const char* file);
void Get_Date_Time(void);
void ChangeFileExtension(char* filename, char* extension);
char* GetRightMostChars(char* string, char* right_string, int num);

#endif
//HELPER_FUNCS_H_303001_
