#ifndef SMTP_INT_H
#define SMTP_INT_H

#include <stdio.h>
#include <string.h>
#include <stdarg.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>
#include <malloc.h>
#include <time.h>
#include <io.h>
#include <fcntl.h>
#include <memory.h>
#include <share.h>
#include <sys/types.h>
#include <sys/stat.h>

#include <windows.h>


SOCKET clientSocket(char *,int);
int	sockGets(SOCKET,char *,size_t);
int	sockPuts(SOCKET sock,char *str);

/*
**	header for mailsend - a simple mail sender via SMTP
*/

#define MFL __FILE__,__LINE__

#define MAILSEND_VERSION	"pdwmail 1.0 http://www.hwithaar.com/"
#define MAILSEND_PROG		"pdwmail"
#define MAILSEND_AUTHOR 	"withaarh@hwithaar.com"
#define MAILSEND_URL		"http://www.hwithaar.com/"

#define MAILSEND_SMTP_PORT	25
#define MAILSEND_DEF_SUB	"hello!"

#define A_SPACE ' '

#define EMPTY_OK		0x01
#define EMPTY_NOT_OK	0x02

/* the mail sturct */
typedef struct _THEMAIL
{
	char *from ;
	char *to ;
	char *cc ;
	char *bcc ;
	char *subject ;
	char *user ;
	char *password ;
	char *smtp_server ;
	char *helo_domain ;
	int  smtp_port ;
	int	 options ;
	HWND hResponse ;
} THEMAIL;


void		errorMsg(char *format,...);


#endif /* ! SMTP_INT_H */
