/***************************************************
 * Filename : ostype.h
 *			  All rights reserved.
 * Started  : 07-Dec-1998 by Herman Withaar
 *			: $Date: 2004/07/13 21:50:01 $	$Revision: 1.1.1.1 $
 * Purpose  :
 * Revision :
 *
 * To do    :
 *
 *********************************************************************
 * COPYRIGHT (c) 1994, 1995 by AT&T.			   All Rights Reserved.
 * COPYRIGHT (c) 1996, 2000 by Lucent Technologies.All Rights Reserved.
 *********************************************************************/
/*********************************************************************
 Log:   V:/dev3dev/wvutil/code/ostype.c_v
*********************************************************************/

#include <windows.h>
#include <tchar.h>

#include "debug.h"
#include "ostype.h"

OSVERSIONINFO	OSVersionInfo  = { sizeof(OSVERSIONINFO), 0 } ;
TCHAR			szOSType[128] ;
int 			nOSType ;

// Windows 95 retail, OEM     4.00.950	                         Build 950
// Windows 95 retail, SP1     4.00.950A
// Windows 95 retail, SR2     4.00.950B (4.00.1111)              Build 1111
// Windows 95 retail, SR2.1   4.00.950B (4.03.1212 - 4.03.1214)
// Windows 95 retail, SR2.5   4.00.950C (4.03.1214)
// Windows 98 retail, OEM     4.10.1998
// Windows 98 SE              4.10.2222A
// Windows Millenium          4.90.3000

int GetOSType(TCHAR *szOS)
{ 
	nOSType = OS_UNKNOWN ;

	OSVersionInfo.dwOSVersionInfoSize = sizeof(OSVERSIONINFO) ;

	if(GetVersionEx(&OSVersionInfo)) {

#ifdef _WIN32_WCE
		wsprintf(szOSType, TEXT("WINCE Versie %ld.%02ld"), OSVersionInfo.dwMajorVersion, OSVersionInfo.dwMinorVersion) ;
		nOSType = OS_WINCE	;
	}
#else
		switch(OSVersionInfo.dwPlatformId) {
			case VER_PLATFORM_WIN32_WINDOWS :
				if( ( (OSVersionInfo.dwMajorVersion > 4) || ( (OSVersionInfo.dwMajorVersion == 4) && (OSVersionInfo.dwMinorVersion >= 10) ) )) {
					nOSType = OS_WIN98 ;
					wsprintf(szOSType, TEXT("WIN98 Version %ld.%02ld %s Build %u"), OSVersionInfo.dwMajorVersion, OSVersionInfo.dwMinorVersion,
								OSVersionInfo.szCSDVersion, OSVersionInfo.dwBuildNumber & 0xFFFF) ;
				}
				else  {
					nOSType = OS_WIN95 ;
					wsprintf(szOSType, TEXT("WIN95 Version %ld.%02ld %s Build %u"), OSVersionInfo.dwMajorVersion, OSVersionInfo.dwMinorVersion,
								OSVersionInfo.szCSDVersion, OSVersionInfo.dwBuildNumber & 0xFFFF) ;
				}
				break ;
			case VER_PLATFORM_WIN32_NT :
				if(OSVersionInfo.dwMajorVersion > 4) {
					if(OSVersionInfo.dwMinorVersion > 0) {
						nOSType = OS_WIN2001 ;
						wsprintf(szOSType, TEXT("WIN2001 Version %ld.%02ld %s Build %u"), OSVersionInfo.dwMajorVersion, OSVersionInfo.dwMinorVersion,
								OSVersionInfo.szCSDVersion, OSVersionInfo.dwBuildNumber & 0xFFFF) ;
					}
					else {				
						nOSType = OS_WIN2000 ;
						wsprintf(szOSType, TEXT("WIN2000 Version %ld.%02ld %s Build %u"), OSVersionInfo.dwMajorVersion, OSVersionInfo.dwMinorVersion,
								OSVersionInfo.szCSDVersion, OSVersionInfo.dwBuildNumber & 0xFFFF) ;
					}
				}
				else {
					nOSType = OS_WINNT ;
					wsprintf(szOSType, TEXT("WINNT Version %ld.%02ld %s Build %u"), OSVersionInfo.dwMajorVersion, OSVersionInfo.dwMinorVersion,
								OSVersionInfo.szCSDVersion, OSVersionInfo.dwBuildNumber & 0xFFFF) ;
				}
				break ;
		}
	}
#endif
	else {
		wsprintf(szOSType, TEXT("Unknown OS!")) ;
		nOSType = OS_UNKNOWN ;
	}
	if(szOS) {
		_tcscpy(szOS, szOSType) ;
	}
	OUTPUTDEBUGMSG((TEXT("OS Type %d String =  >%s<"), nOSType, szOSType)) ;
	return(nOSType) ;
}
