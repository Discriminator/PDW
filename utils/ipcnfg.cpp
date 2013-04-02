// ipcnfg.cpp
//
// ip related routines
//
// char *GetIPAddress() ;
//
//

#include <windows.h>
#include <string.h>

#include "ipcnfg.h"
#include "debug.h"

static char szIPAddress[32] ;

char *GetIPAddress(void)
{
	PHOSTENT pH ;
	char local_host_name[128];
	WORD wVersionRequested;
	WSADATA wsaData;
	int err;

	// Request at least version 2.2
	wVersionRequested = 0x0202 ;

	err = WSAStartup( wVersionRequested, &wsaData );
	if ( err != 0 ) {
		OUTPUTDEBUGMSG((("Error: WinSock DLL on this system has wrong version\n")));
		strcpy(szIPAddress, "error") ;
		return(szIPAddress) ;
	}

	/* Confirm that the WinSock DLL supports 2.2.*/
	/* Note that if the DLL supports versions greater    */
	/* than 2.2 in addition to 2.2, it will still return */
	/* 2.2 in wVersion since that is the version we      */
	/* requested.                                        */

	if ( LOBYTE( wsaData.wVersion ) != 2 || HIBYTE( wsaData.wVersion ) != 2 ) {
		OUTPUTDEBUGMSG((("Error: WinSock DLL on this system has wrong version\n")));
		WSACleanup( );
		strcpy(szIPAddress, "error") ;
		return(szIPAddress);
	}

	gethostname(local_host_name, sizeof(local_host_name));
	OUTPUTDEBUGMSG((("Local host name : %s\n"), local_host_name));
	pH = gethostbyname(local_host_name);
	wsprintf(szIPAddress, "%d.%d.%d.%d", (UCHAR)pH->h_addr_list[0][0], (UCHAR)pH->h_addr_list[0][1], (UCHAR)pH->h_addr_list[0][2], (UCHAR)pH->h_addr_list[0][3]);
	OUTPUTDEBUGMSG(((szIPAddress)));
	return(szIPAddress) ;
}