// filename:		helper_funcs.cpp
// Description:		Implementation  of commonly used functions in helper_funcs.h
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

#include "headers\helper_funcs.h"
#include <windows.h>
#include <stdio.h>
#include "utils\debug.h"


void GetFileName(const char* filepath, char* buffer, int buffersize)
{
	// pre:  filepath and buffer are not 0, buffersize is the maximum size of chars copied
	// post: the filename in filepath is stored in buffer, this means that from filepath
	//		 c:\blabla\yada.png buffer is filled with yada.png

	if (!filepath || !buffer) return;

	int i=strlen(filepath)-1;
	while (i>=0 && buffersize-- >0 && filepath[i]!='\\' && filepath[i]!='/') i--;
	strcpy(buffer,filepath+i+1);
}
//--------------------------------------------------------------------------------------------

void GetFilePath(const char* filepath, char* buffer, int buffersize)
{
	if (!filepath || !buffer) return;

	if ((filepath[1] == ':') || (filepath[1] == '\\'))
	{
		int i=strrchr(filepath, '\\') - filepath;

		strncpy(buffer,filepath,i);
		buffer[i]='\0';
	}
	else strcpy(buffer,"");
}
//--------------------------------------------------------------------------------------------

void GetFileNameWithoutExtension(char* filepath)
{
	int i,j;

	if (!filepath) return;

	i=strrchr(filepath, '\\') - filepath+1;
	j=strrchr(filepath, '.') - filepath;

	for (int k=0; i<j; i++, k++)
	{
		filepath[k]=filepath[i];
	}
	filepath[k]='\0';
}

bool FileExists(const char* file)
{
	// pre:  file is not 0 and file should be an absolute path
	// post: true has been returned if the file named by file does exist, otherwise false
	//			has been returned

	if(!file)return false;

	// get the filename of the file and store it in tmp
	char tmp[MAX_PATH];
	GetFileName(file,tmp,sizeof(tmp));

	// get WIN32_FIND_DATA for the file pointed to file
	WIN32_FIND_DATA d;
	ZeroMemory(&d,sizeof(d));
	HANDLE h=FindFirstFile(file,&d);

	// don't forget to close the handle, otherwise we have a mem-leak
	FindClose(h);

	// if the filename retrieved from file and the filename retrieved
	// by getting the WIN32_FIND_DATA are the same when compared (not case-sensitive)
	// then this file DOES exist, otherwise it DOESN'T exist
	return strcmp(lowercase(tmp),lowercase(d.cFileName))==0;
}
//--------------------------------------------------------------------------------------------

char* lowercase(char* str)
{
	// pre:  str is not 0
	// post: str has been converted to an all-lowercase-characters string and str
	//			has been returned again

	if(!str)return 0;

	for(unsigned int i=0;i<strlen(str);i++)
		str[i] = tolower(str[i]);

	return str;
}


void ChangeFileExtension(char* filename, char* extension)
{
	char* temp = &filename[0];

	if (temp=strrchr(filename, '.')) *temp=0;
	strcat(filename, extension);
}

// Look in str for '.'.
// Return TRUE if found else FALSE.
BOOL Need_Ext(char *filename)
{
	unsigned int i=0;

	if (strstr(filename, ".") == 0) return (TRUE);

	if (strstr(filename, "\\") != 0)
	{
		i=strrchr(filename, '\\') - filename+1;

		while (i < strlen(filename))
		{
			if (filename[i] == '.') return(FALSE);
			i++;
		}
	}
	else if (strstr(filename, ".") != 0) return (FALSE);

	return(TRUE);
}

char *GetRightMostChars(char* string, char* right_string, int num)
{
	int i=0;
	int length=strlen(string);

	for (i=0; i<length; i++)
	{
		right_string[i] = string[length-num+i];
	}

	return right_string;
}
