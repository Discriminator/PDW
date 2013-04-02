#include <windows.h>

#if 0
#define SMIF_SUCCESS 0 /* Okay */
#define SMIF_NO_DATA 1 /* No data available when polling; try again later */
#define SMIF_UNKNOWN -1 /* Unknown failure; no specific information. */
#define SMIF_NO_CONNECTION -2 /* Before doing a read, write or handling a frame receive, a connection should be made3.*/
#define SMIF_CONNECTED -3 /* Configuration functions cannot be called when a connection is up.*/
#define SMIF_TIME_OUT -4 /* Time-out on communication with hardware. */
#define SMIF_NO_DUT -5 /* No DUT detected. Reconfigure connection and try to initiate a new connection4. */
#define SMIF_INVALID_LENGTH -6 /* A byte string of length zero is not supported. */
#define SMIF_NOT_SUPPORTED -7 /* This function is not supported with the current driver. */
#define SMIF_TOO_SHORT -8 /* The length of the frame is too short */
#endif

#define smif_8  BYTE
#define smif_16 WORD
#define smif_32 DWORD

#ifndef BIT0
#define BIT0  0x0001
#define BIT1  0x0002
#define BIT2  0x0004
#define BIT3  0x0008
#define BIT4  0x0010
#define BIT5  0x0020
#define BIT6  0x0040
#define BIT7  0x0080
#define BIT8  0x0100
#define BIT9  0x0200
#define BIT10 0x0400
#define BIT11 0x0800
#define BIT12 0x1000
#define BIT13 0x2000
#define BIT14 0x4000
#define BIT15 0x8000
#define BIT16 0x00010000
#define BIT17 0x00020000
#define BIT18 0x00040000
#define BIT19 0x00080000
#define BIT20 0x00100000
#define BIT21 0x00200000
#define BIT22 0x00400000
#define BIT23 0x00800000
#define BIT24 0x01000000
#define BIT25 0x02000000
#define BIT26 0x04000000
#define BIT27 0x08000000
#define BIT28 0x10000000
#define BIT29 0x20000000
#define BIT30 0x40000000
#define BIT31 0x80000000
#endif

#if !defined EXTERN_C
#ifdef __cplusplus
#define EXTERN_C extern "C"
#else
#define EXTERN_C
#endif //__cplusplus
#endif //EXTERN_C

extern void DebugString(TCHAR *fmt,...) ;

#if defined _DEBUG || defined DBG

#define OUTPUTDEBUGMSG(dprintf_exp)			((void) (DebugString dprintf_exp))
#define ASSERTDEBUGMSG(cond, dprintf_exp)	((void)((cond) ? 0:(DebugString dprintf_exp)))
#define DEBUGASSERT(dprintf_exp)			((void) (DebugString dprintf_exp)); 

#else

#define OUTPUTDEBUGMSG(dprintf_exp)
#define ASSERTDEBUGMSG(cond, dprintf_exp)
#define DEBUGASSERT(dprintf_exp)

#endif // DEBUG

#define RS232_BAUDRATE /*9600*/ 115200 /*19200*/
#define RS232_BYTESIZE 8
#define RS232_PARITY   NOPARITY /*EVENPARITY*/
#define RS232_STOPBITS ONESTOPBIT
// the time it takes to transfer one byte over the rs232 line, including start/stop/inter-byte time
#define RS232_BYTE_TIME_US (1000000 * 12 / RS232_BAUDRATE)