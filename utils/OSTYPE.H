/***************************************************
 * Filename : ostype.c
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
 Log:   V:/dev3dev/wvutil/code/ostype.h_v
*********************************************************************/

#ifndef __OSTYPE_H__
#define __OSTYPE_H__


#define OS_UNKNOWN  0x0000
#define OS_WIN95    0x0001
#define OS_WIN98    0x0002
#define OS_WINNT    0x0004
#define OS_WIN2000  0x0008
#define OS_WINCE    0x0010
#define OS_WINXP    0x0020
//#define OS_WINME    0x0040	


#define IS_OSWIN9X(x)			(x & (OS_WIN95 | OS_WIN98))		
#define IS_OSWINNT(x)			(x & (OS_WINNT | OS_WIN2000 | OS_WINCE | OS_WINXP))		


#ifndef NDIS_MINIPORT_DRIVER

#define IS_OSWINME				((OSVersionInfo.dwMajorVersion == 4) && (OSVersionInfo.dwMinorVersion >= 90))

#ifdef __cplusplus
extern "C" {
#endif


extern OSVERSIONINFO OSVersionInfo ;
extern TCHAR		 szOSType[128] ;
extern int			 nOSType ;

int GetOSType(TCHAR *szOS) ;

#ifdef __cplusplus
}
#endif

#endif // !NDIS_MINIPORT_DRIVER

#endif // _OSTYPE_H__
