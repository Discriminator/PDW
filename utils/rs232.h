
#define CBR_SLICER_2K			CBR_110  // 100
#define CBR_SLICER_XP			CBR_300  // 200

#define RS232_SUCCESS			0
#define RS232_NO_DUT			1
#define RS232_NO_CONNECTION		2
#define RS232_UNKNOWN			3

#define DRIVER_TYPE_NOT_LOADED	0
#define DRIVER_TYPE_SLICER		1
#define DRIVER_TYPE_RS232		2



EXTERN_C int rs232_connect(SLICER_IN_STR *pInSlicer, SLICER_OUT_STR *pOutSlicer);
EXTERN_C int rs232_transmit_data(unsigned char buffer[], int nBytes);
EXTERN_C int rs232_get_rx_data(unsigned char buffer[], int nBytes);
EXTERN_C int rs232_disconnect(void);
EXTERN_C int rs232_read(void) ;
EXTERN_C int slicer_read(void) ;


EXTERN_C int OpenComPort(void) ;
EXTERN_C int WriteComPort(char *szLine) ;
EXTERN_C int CloseComPort(void) ;

EXTERN_C int *FindComPorts(void) ;

EXTERN_C int GetRs232DriverType(void) ;
