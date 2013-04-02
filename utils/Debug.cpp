//
// Debug.cpp
//
//

#include <windows.h>

#include "debug.h"

int	nDebugOutput ;
int	nDebugNum ;


#ifdef _DEBUG


#define MAX_DEBUG	2048

char szDebug[MAX_DEBUG] ;

int DebugPrintf(char *fmt,...)
{
	int	len ;
    va_list marker;
    va_start( marker, fmt );

	len = wvsprintf(szDebug, fmt, marker) ;

	OutputDebugString(szDebug) ;
    va_end( marker );
    return(len) ;
 }
#endif

