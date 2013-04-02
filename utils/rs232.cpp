#include <windows.h>
#include <winbase.h>
#include "..\headers\slicer.h"
#include "..\utils\debug.h"
#include "..\utils\ostype.h"
#include "..\headers\pdw.h"
#include "rs232.h"

#define SLICER_BUFSIZE 10000

volatile HANDLE m_hRxThread = INVALID_HANDLE_VALUE;
ULONG WINAPI RxThread(LPVOID pCl);
BOOL	m_bConnectedToComport = FALSE;
HANDLE	m_ComPortHandle = INVALID_HANDLE_VALUE;
DWORD	m_dwThreadId = 0;
BOOL	bKeepThreadAlive;
double  nTiming ;	// was int
BOOL    bOrgcomPortRS232 ;
BOOL    bSlicerDriver = FALSE;

WORD  rs232_freqdata[SLICER_BUFSIZE] ;
BYTE  rs232_linedata[SLICER_BUFSIZE] ;
DWORD rs232_cpstn;

BYTE  byRS232Data[SLICER_BUFSIZE * (sizeof(WORD) + sizeof(BYTE))] ;

#define assert(a)		if(!(a))  { OUTPUTDEBUGMSG(("SIMULATE ASSERT in file %s at %d\n", __FILE__, __LINE__ )); }

int rs232_connect(SLICER_IN_STR *pInSlicer, SLICER_OUT_STR *pOutSlicer)
{
	extern double ct1600;
	int rc = RS232_NO_DUT;
	char pcComPort[] = "COM1:";
	DCB m_comDCB = { 0 } ;
	COMMPROP ComProp = { 0 } ;
	COMMTIMEOUTS ComTimeOuts = { 0 } ;

	// This as user can switch to slicer/rs232 without ports are close/opened (yet)
	bOrgcomPortRS232 = Profile.comPortRS232 ;
	if(pInSlicer->com_port > 9) {
		sprintf(pcComPort, "\\\\.\\COM%d", pInSlicer->com_port) ;
	}
	else {
		sprintf(pcComPort, "COM%d", pInSlicer->com_port) ;
	}

	switch (Profile.comPortRS232)
	{
		case 1:
			nTiming = 500;
			break ;
		case 2:
		default:
			nTiming = ct1600;
			break ;
		case 3:
			nTiming = 1.0/((float) 8000 * 839.22e-9);
			break ;
	}

	OUTPUTDEBUGMSG((("calling: rs232_connect(%s)\n"), pcComPort));

	pOutSlicer->freqdata = rs232_freqdata ;
	pOutSlicer->linedata = rs232_linedata ;
	pOutSlicer->cpstn	 = &rs232_cpstn ;
	pOutSlicer->bufsize  = SLICER_BUFSIZE ;

	if (m_bConnectedToComport)
	{
		rc = rs232_disconnect();
		assert (rc >= 0);
		if (rc < 0) {
			return rc;
		}
	}
	/********************************************************************************************
	* Seek contact with the serial.sys driver. Configure it for overlapped operation, this is   *
	* done so the receiving thread (later on in this code) can be terminated by the main thread *
	********************************************************************************************/	
	m_ComPortHandle = CreateFile(pcComPort,GENERIC_READ, 0, 0, OPEN_EXISTING, 0, 0);
	if(m_ComPortHandle == INVALID_HANDLE_VALUE)
	{
	    OUTPUTDEBUGMSG((("ERROR: CreateFile() %08lX!\n"), GetLastError()));
		CloseHandle(m_ComPortHandle);
		return RS232_NO_DUT;
	}


	//	We must check if this is a Slicer Driver
	if(!GetCommProperties(m_ComPortHandle, &ComProp)) {
	    OUTPUTDEBUGMSG((("ERROR: GetCommProperties() %08lX!\n"), GetLastError()));
		CloseHandle(m_ComPortHandle);
		return RS232_NO_DUT;
	}

	if(ComProp.dwProvSpec1 == 0x48576877 && ComProp.dwProvSpec2 == 0x68774857) 
	{
		bSlicerDriver = TRUE ;
	}
	else 
	{
		bSlicerDriver = FALSE ;
	}
	if(!bOrgcomPortRS232)
	{
		if(!bSlicerDriver)
		{
	        OUTPUTDEBUGMSG((("ERROR:ComProp.dwProvSpec1 != 0x48576877 || ComProp.dwProvSpec2 != 0x68774857!\n")));
		    CloseHandle(m_ComPortHandle);
		    MessageBox(NULL, "Please install the Slicer driver from the install package!", "Slicer Driver Not Installed", MB_OK | MB_ICONEXCLAMATION) ;
		    return RS232_NO_DUT;
		}
	}
	//	We got a connection with the serial.sys driver
	if(!GetCommState(m_ComPortHandle,&m_comDCB))
	{
	    OUTPUTDEBUGMSG((("ERROR: GetCommState() %08lX!\n"), GetLastError()));
		CloseHandle(m_ComPortHandle);
		return RS232_NO_DUT;
	}

	// Our specific part of the connection 
	// All the = Zero and by that default
	m_comDCB.BaudRate			= bOrgcomPortRS232 ? CBR_19200 : (nOSType == OS_WIN2000) ? CBR_SLICER_2K : CBR_SLICER_XP;	// This means SlicerMode for the COM driver
	m_comDCB.ByteSize			= 8 ;
	m_comDCB.Parity				= NOPARITY ;
	m_comDCB.StopBits			= ONESTOPBIT;
	m_comDCB.fBinary			= TRUE;
	m_comDCB.fParity			= FALSE;
	m_comDCB.fDtrControl        = DTR_CONTROL_DISABLE ;
	m_comDCB.fRtsControl		= bOrgcomPortRS232 ? RTS_CONTROL_DISABLE : RTS_CONTROL_ENABLE ;

	if(!SetCommState(m_ComPortHandle,&m_comDCB)) {
	    OUTPUTDEBUGMSG((("ERROR: GetCommState() %08lX!\n"), GetLastError()));
		CloseHandle(m_ComPortHandle);
		return RS232_NO_DUT;
	}
	if(!SetCommMask(m_ComPortHandle, bOrgcomPortRS232 ? 0 : EV_CTS | EV_DSR | EV_RLSD)) {
	    OUTPUTDEBUGMSG((("ERROR: SetCommMask() %08lX!\n"), GetLastError()));
		CloseHandle(m_ComPortHandle);
		return RS232_NO_DUT;
	}
	/* Purge buffers:*/
	if(!PurgeComm(m_ComPortHandle,PURGE_TXABORT|PURGE_RXABORT|PURGE_TXCLEAR|PURGE_RXCLEAR)) {
	    OUTPUTDEBUGMSG((("ERROR: PurgeComm() %08lX!\n"), GetLastError()));
		CloseHandle(m_ComPortHandle);
		return RS232_NO_DUT;
	}
	if(bOrgcomPortRS232) {
		ComTimeOuts.ReadIntervalTimeout = ComTimeOuts.ReadTotalTimeoutMultiplier = MAXDWORD ;
		ComTimeOuts.ReadTotalTimeoutConstant = 100 ; // 100 ms timeout
 		if(!SetCommTimeouts(m_ComPortHandle,&ComTimeOuts)) {
			OUTPUTDEBUGMSG((("ERROR: SetCommTimeouts() %08lX!\n"), GetLastError()));
			CloseHandle(m_ComPortHandle);
			return RS232_NO_DUT;
		}
	}
	m_bConnectedToComport= TRUE;

	/************************************************************************************
	* Next step : fire up a thread that takes care of placing received data in a buffer *
	************************************************************************************/

	bKeepThreadAlive = TRUE;
	m_hRxThread = CreateThread(NULL, 0, RxThread, (LPVOID) NULL, CREATE_SUSPENDED, &m_dwThreadId) ;
	ResumeThread(m_hRxThread);

	return(RS232_SUCCESS) ;
}

int rs232_disconnect()
{
	int rc;
	COMMTIMEOUTS ComTimeOuts = { 0 } ;

	OUTPUTDEBUGMSG(("calling: rs232_disconnect()\n"));
	if (!m_bConnectedToComport) {
		// return when already connected
		return RS232_NO_CONNECTION;		
	}
	/**************************
	* Terminate the Rx thread *
	**************************/
	OUTPUTDEBUGMSG(("main thread : set Rx Thread terminate event\n"));
	bKeepThreadAlive = FALSE;
	Sleep(250);
	TerminateThread(m_hRxThread, -1) ;
	Sleep(100);
	OUTPUTDEBUGMSG(("main thread : closing handles\n"));

	if(!SetCommTimeouts(m_ComPortHandle,&ComTimeOuts)) {
		OUTPUTDEBUGMSG((("ERROR: SetCommTimeouts() %08lX!\n"), GetLastError()));
	}

	if(!SetCommMask(m_ComPortHandle, 0)) {
		OUTPUTDEBUGMSG((("ERROR: SetCommMask() %08lX!\n"), GetLastError()));
	}

	assert(m_ComPortHandle != INVALID_HANDLE_VALUE);
	rc = CloseHandle(m_ComPortHandle);
	if (!rc) {
		OUTPUTDEBUGMSG(("main thread : error closing handle!\n"));
		rc = RS232_UNKNOWN;
	}
	else {
		OUTPUTDEBUGMSG(("main thread : handle closed.\n"));
		m_ComPortHandle = INVALID_HANDLE_VALUE;
		m_bConnectedToComport = FALSE;
	}
	return(RS232_SUCCESS) ;
}

/***********************************************************
* This worker thread will place received data in a mailbox *
***********************************************************/
DWORD WINAPI RxThread(LPVOID pCl)
{
	do
	{
		if(bOrgcomPortRS232) {
			rs232_read() ;
		}
		else {
			slicer_read() ;
		}
		Sleep(50) ;
	}
	while (bKeepThreadAlive);
	
	OUTPUTDEBUGMSG(("RxThread : terminating...\n"));
	m_hRxThread = INVALID_HANDLE_VALUE;
	ExitThread(0L) ;
	return 0;
}


int rs232_read(void) 
{
	DWORD	dwRead ; // , dwEvent, dwSetEvent;
	int     bit ;
	BYTE    byData[256] ;

	//	OUTPUTDEBUGMSG((("rs232_read() \n")));
	if(m_ComPortHandle == INVALID_HANDLE_VALUE)
	{
		OUTPUTDEBUGMSG((("rs232_read : COMport not open!\n")));
		return(0) ;
	}

	if(!ReadFile(m_ComPortHandle, byData, sizeof(byData), &dwRead, 0))
	{
		OUTPUTDEBUGMSG((("rs232_read : Error in reading 0x%0x!\n"), GetLastError()));
		PurgeComm(m_ComPortHandle, PURGE_RXCLEAR) ;
	}
	else {
		for(int i=0; i<dwRead; i++) {
			for (int j=7; j>=0; j--)
			{
				if(Profile.fourlevel) {
					j-- ;
					bit = (byData[i] >> j) & 3;
				}
				else {
					bit = (byData[i] >> j) & 1;
				}	
				rs232_linedata[rs232_cpstn] = bit << 4 ;
				rs232_freqdata[rs232_cpstn++] = nTiming ;	
				if(rs232_cpstn >= SLICER_BUFSIZE) {
					rs232_cpstn = 0 ;
				}
			}
		}
	}
	return(0) ;
}


int slicer_read(void) 
{
	DWORD dwRead, i, num ;
	WORD	*freq ;
	BYTE	*line ;

	if(m_ComPortHandle == INVALID_HANDLE_VALUE) 
	{
		OUTPUTDEBUGMSG((("rs232_read : COMport not open!\n")));
	}
	if(!ReadFile(m_ComPortHandle, byRS232Data, sizeof(byRS232Data), &dwRead, 0)) 
	{
		OUTPUTDEBUGMSG((("rs232_read : Error in reading 0x%0x!\n"), GetLastError()));
		PurgeComm(m_ComPortHandle, PURGE_RXCLEAR) ;
	}

	num = dwRead / (sizeof(WORD) + sizeof(BYTE)) ;
//	OUTPUTDEBUGMSG((("rs232_read : Reading %d bytes --> %d slices\n"), dwRead, num));
	line = byRS232Data ;
	freq = (WORD *) (byRS232Data + num * sizeof(BYTE));
	for(i = 0; i < num; i++) {
		rs232_linedata[rs232_cpstn] = *line++ ;
		rs232_freqdata[rs232_cpstn++] = *freq++ ;
		if(rs232_cpstn >= SLICER_BUFSIZE) {
			rs232_cpstn = 0 ;
		}
	}
	return(i) ;
}


#define _COMPORT_1		0
#define _COMPORT_2		1
#define _COMPORT_3		2
#define _COMPORT_4		3

// Variables to put into pdw.ini
int nComPort2 =  _COMPORT_1;
HANDLE m_ComPortHandle2 = INVALID_HANDLE_VALUE;
BOOL m_bConnectedToComport2 = FALSE;

int OpenComPort(void)
{
	int rc = RS232_NO_DUT;
	char pcComPort[] = "COM1:";
	DCB m_comDCB = { 0 } ;
	COMMPROP ComProp = { 0 } ;

	OUTPUTDEBUGMSG((("calling: OpenComPort()\n")));

	pcComPort[3] = '1' + nComPort2 ;

	if (m_bConnectedToComport2) {
		rc = CloseComPort();
		assert (rc >= 0);
		if (rc < 0) return rc;
	}

	assert (m_ComPortHandle2 == INVALID_HANDLE_VALUE);
	m_ComPortHandle2 = CreateFile(pcComPort,GENERIC_READ | GENERIC_WRITE, 0, 0, OPEN_EXISTING, 0, 0);
	if(m_ComPortHandle2 == INVALID_HANDLE_VALUE)
	{
	    OUTPUTDEBUGMSG((("ERROR: CreateFile() %08lX!\n"), GetLastError()));
		CloseHandle(m_ComPortHandle2);
		return RS232_NO_DUT;
	}

	//	We got a connection with the serial.sys driver
	if(!GetCommState(m_ComPortHandle2,&m_comDCB))
	{
	    OUTPUTDEBUGMSG((("ERROR: GetCommState() %08lX!\n"), GetLastError()));
		CloseHandle(m_ComPortHandle2);
		return RS232_NO_DUT;
	}

	// Our specific part of the connection 
	// All the = Zero and by that default
	m_comDCB.BaudRate			= CBR_19200 ;
	m_comDCB.ByteSize			= 8 ;
	m_comDCB.Parity				= NOPARITY ;
	m_comDCB.StopBits			= ONESTOPBIT;
	m_comDCB.fBinary			= TRUE;
	m_comDCB.fParity			= FALSE;
	m_comDCB.fDtrControl        = DTR_CONTROL_ENABLE ;
	m_comDCB.fRtsControl        = RTS_CONTROL_ENABLE ;

	if(!SetCommState(m_ComPortHandle2,&m_comDCB)) {
	    OUTPUTDEBUGMSG((("ERROR: GetCommState() %08lX!\n"), GetLastError()));
		CloseHandle(m_ComPortHandle2);
		return RS232_NO_DUT;
	}

	/* Purge buffers */
	if(!PurgeComm(m_ComPortHandle2,PURGE_TXABORT|PURGE_RXABORT|PURGE_TXCLEAR|PURGE_RXCLEAR)) {
	    OUTPUTDEBUGMSG((("ERROR: PurgeComm() %08lX!\n"), GetLastError()));
		CloseHandle(m_ComPortHandle2);
		return RS232_NO_DUT;
	}

	m_bConnectedToComport2 = TRUE;

	return(rc) ;
}

char chStartChar = 10 ;
char chEndChar = 4 ;

int WriteComPort(char *szLine)
{
	char szTemp[1024] ;
	int len = 0 ;
	DWORD dwWrite  ;

	OUTPUTDEBUGMSG(((">>> WriteComPort()\n")));

	if(chStartChar) {
		szTemp[len++] = chStartChar ;
	}
	len += wsprintf(szTemp + len, "%s", szLine) ;
	if(chEndChar) {
		szTemp[len++] = chEndChar ;
		szTemp[len] = 0;
	}

	if(!WriteFile(m_ComPortHandle2, szTemp, len, &dwWrite, 0)) {
		OUTPUTDEBUGMSG((("WriteComPort : Error in Writing 0x%0x!\n"), GetLastError()));
	}
	OUTPUTDEBUGMSG((("<<< WriteComPort()\n")));
	return (0) ;
}


int CloseComPort(void)
{
	int rc; 

	OUTPUTDEBUGMSG((("CloseComPort()\n")));
	rc = CloseHandle(m_ComPortHandle2);
	if (!rc) {
		OUTPUTDEBUGMSG(("CloseComPort: Error closing handle!\n"));
		rc = RS232_UNKNOWN;
	}
	else {
		OUTPUTDEBUGMSG(("CloseComPort: comport closed.\n"));
		m_ComPortHandle2 = INVALID_HANDLE_VALUE;
		m_bConnectedToComport2 = FALSE;
	}
	return(0) ;
}

int nComPortsArr[11] ;

int *FindComPorts(void)
{
	DWORD error ;
	int nNumFound = 0 ;
	char szPort[32] ;
	HANDLE hCom ;
	for(int i=1; i<50; i++) {
		if(i > 9) {
			wsprintf(szPort, "\\\\.\\COM%d", i) ;
		}
		else {
			wsprintf(szPort, "COM%d", i) ;
		}

		error = ERROR_SUCCESS ;
		hCom = CreateFile(szPort, GENERIC_READ, 0, 0, OPEN_EXISTING, 0, 0);
		if(hCom == INVALID_HANDLE_VALUE) {
			error = GetLastError() ;
		}
		if(error == ERROR_FILE_NOT_FOUND) {
			OUTPUTDEBUGMSG((("COM%d: Not Found\n"), i));
			CloseHandle(hCom);
		}
		else {
			OUTPUTDEBUGMSG((("COM%d: Found\n"), i));
			CloseHandle(hCom);
			nComPortsArr[nNumFound++] = i ;
		}
	}
	nComPortsArr[nNumFound] = 0 ;
	return(nComPortsArr) ;
}


int GetRs232DriverType(void)
{
	return(m_bConnectedToComport ? (bSlicerDriver ?  DRIVER_TYPE_SLICER : DRIVER_TYPE_RS232) : DRIVER_TYPE_NOT_LOADED) ;
}
