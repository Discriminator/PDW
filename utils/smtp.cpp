// 
/*
**	SMTP routines for mailsend - a simple mail sender via SMTP
**
*/

#include <windows.h>
#include <stdio.h>
#include "..\headers\pdw.h"
#include "smtp_int.h"
#include "smtp.h"
#include "..\utils\debug.h"

static SOCKET smtp_socket = INVALID_SOCKET;
static char buf[BUFSIZ];

static HANDLE MailThread ;
static THEMAIL mail ;
static int nMaxLen ;
static BOOL keepbusy = TRUE ;
static BOOL bWsaStartup ;

#define MAX_MAIL		100
#define MAX_MAIL_LEN	512

static char szMailBuffer[MAX_MAIL][MAX_MAIL_LEN] ;
static int  nBufferdMailStart ;
static int  nBufferdMailEnd ;

static byte dtable[256];

extern int nSMTPerrors;
extern int iSMTPlastError;


char *szSmtpCharSets[] = {
	"us-ascii     (Standard)",
	"iso-8859-1   (West European)",
	"iso-8859-2   (East European)",
	"iso-8859-3   (South European)", 
	"iso-8859-4   (North European)",
	"iso-8859-5   (Cyrillic)",
	"iso-8859-6   (Arabic)", 
	"iso-8859-7   (Greek)", 
	"iso-8859-8   (Hebrew)",
	"iso-8859-9   (Turkish)",
	"iso-8859-10  (Nordic)",
	"iso-2022-kr  (Korean)",
	"KOI8-R       (Russian)",
	"EUC-KR       (Korean)",
	"Shift_JIS    (Japanese)",
	"ISO-2022-JP  (Japanese)",
	"EUC-JP       (Japanese)",
	"GB2312       (Chinese)",
	"Big5         (Traditional Chinese)",
	"windows-1250 (Central Europ Windows)",
	"windows-1251 (Cyrillic Windows)",
	"windows-1252 (Western Europ Windows)",
	"windows-1253 (Greek Windows)",
	"windows-1254 (Turkish (Windows)",
	"windows-1255 (Hebrew Windows)",
	"windows-1256 (Arabic Windows)",
	"windows-1257 (Baltic Windows)",
	"windows-1258 (Vietnamese Windows)"
} ;

	
char *EncodeBase64(char *szIn, char *szOut)
{
	char *pIn = szIn, *pOut = szOut ;
	int i,hiteof= FALSE;

	for(i= 0;i<9;i++){
		dtable[i]= 'A'+i;
		dtable[i+9]= 'J'+i;
		dtable[26+i]= 'a'+i;
		dtable[26+i+9]= 'j'+i;
	}
	for(i= 0;i<8;i++){
		dtable[i+18]= 'S'+i;
		dtable[26+i+18]= 's'+i;
	}
	for(i= 0;i<10;i++){
		dtable[52+i]= '0'+i;
	}
	dtable[62]= '+';
	dtable[63]= '/';


	while(!hiteof){
		byte igroup[3],ogroup[4];
		int c,n;
	
		igroup[0]= igroup[1]= igroup[2]= 0;
		for(n= 0;n<3;n++){
			c = *pIn++;
			if(!c){
				hiteof= TRUE;
				break;
			}
			igroup[n]= (byte)c;
		}
		if(n> 0){
			ogroup[0]= dtable[igroup[0]>>2];
			ogroup[1]= dtable[((igroup[0]&3)<<4)|(igroup[1]>>4)];
			ogroup[2]= dtable[((igroup[1]&0xF)<<2)|(igroup[2]>>6)];
			ogroup[3]= dtable[igroup[2]&0x3F];

			if(n<3){
				ogroup[3]= '=';
				if(n<2){
					ogroup[2]= '=';
				}
			}
			for(i= 0;i<4;i++){
				*pOut++ = ogroup[i];
			}
		}
	}
	*pOut = '\0' ;

	OUTPUTDEBUGMSG((("EncodeBase64(): In >%s< out >%s< \n"),szIn, szOut));
	return(szOut) ;
}

char *DecodeBase64(char *szIn, char *szOut)
{
	int i, j;
	char *pIn = szIn, *pOut = szOut ;

	for(i= 0;i<255;i++){
		dtable[i]= 0x80;
	}
	for(i= 'A';i<='I';i++){
		dtable[i]= 0+(i-'A');
	}
	for(i= 'J';i<='R';i++){
		dtable[i]= 9+(i-'J');
	}
	for(i= 'S';i<='Z';i++){
		dtable[i]= 18+(i-'S');
	}
	for(i= 'a';i<='i';i++){
		dtable[i]= 26+(i-'a');
	}
	for(i= 'j';i<='r';i++){
		dtable[i]= 35+(i-'j');
	}
	for(i= 's';i<='z';i++){
		dtable[i]= 44+(i-'s');
	}
	for(i= '0';i<='9';i++){
		dtable[i]= 52+(i-'0');
	}
	dtable['+']= 62;
	dtable['/']= 63;
	dtable['=']= 0;

	while(TRUE){
		byte a[4],b[4],o[3];
		
		for(i = 0; i < 4; i++){
			int c = *pIn++;		
			if(!c){
				if(i> 0){
					OUTPUTDEBUGMSG((("DecodeBase64(): Input line incomplete.\n")));
				}
				*pOut = '\0'  ;
				OUTPUTDEBUGMSG((("DecodeBase64(): In >%s< out >%s< \n"),szIn, szOut));
				return(szOut);
			}
			if(dtable[c]&0x80){
				OUTPUTDEBUGMSG((("DecodeBase64(): Illegal character '%c' in input line.\n"),c));
				i--;
				continue;
			}
			a[i]= (byte)c;
			b[i]= (byte)dtable[c];
		}
		o[0]= (b[0]<<2)|(b[1]>>4);
		o[1]= (b[1]<<4)|(b[2]>>2);
		o[2]= (b[2]<<6)|b[3];
		i = a[2]=='='?1:(a[3]=='='?2:3);

		for(j = 0; j < i; j++) {
			*pOut++ = o[j] ;
		}
		if(i < 3){
			*pOut = '\0'  ;
			OUTPUTDEBUGMSG((("DecodeBase64(): In >%s< out >%s< \n"),szIn, szOut));
			return(szOut);
		}
	}	
}

void AddResponse(char *buf) 
{
// #ifdef _DEBUG
	HDC		hDC ;
	SIZE	Size ;

	if(mail.hResponse) {
		hDC = GetDC(mail.hResponse) ;
		GetTextExtentPoint32(hDC, buf, strlen(buf), &Size);
		if(Size.cx > nMaxLen) {
			nMaxLen = Size.cx ;
			SendMessage(mail.hResponse, LB_SETHORIZONTALEXTENT, Size.cx, 0L) ;
		}
		SendMessage(mail.hResponse, LB_ADDSTRING, 0, (LPARAM) (LPSTR) buf) ;
		OUTPUTDEBUGMSG((("AddResponse() : >>> %s"),buf));

//		FILE *pResponse = NULL;
//		if ((pResponse = fopen("SMTP-response.txt", "a")) != NULL)
//		{
//			fprintf(pResponse, "%s\n", buf);
//			fclose(pResponse);
//			pResponse = NULL;
//		}
	}
// #endif
}


struct in_addr *atoAddr(char *address)
{
	static struct in_addr saddr;
	struct hostent *host;
	
	saddr.s_addr=inet_addr(address);
	if(saddr.s_addr != -1) return (&saddr);
	host = gethostbyname(address);
	if(host != (struct hostent *) NULL) {
		return((struct in_addr *) *host->h_addr_list);
	}
	return((struct in_addr *) NULL);
}

int initWinSock(void)
{
	WORD	version_requested;
	WSADATA wsa_data;
	int		err;
	
	if(!bWsaStartup) {
		version_requested=MAKEWORD(2,0);
		err = WSAStartup(version_requested,&wsa_data);
		if(err != 0) {
			OUTPUTDEBUGMSG(((" Unable to initialize winsock (%d)\n"),err));
			AddResponse("Unable to initialize winsock\n");
			bWsaStartup = FALSE ;
			return(-1);
		}
		bWsaStartup = TRUE ;
	}
	return(0);
}

// returns SOCKET on success INVALID_SOCKET on failure
SOCKET clientSocket(char *address,int port)
{
	SOCKET				s;
	struct sockaddr_in	sa;
	struct in_addr		*addr;
	int 				rc;
	
	rc = initWinSock();
	if(rc != 0) {
		OUTPUTDEBUGMSG((("clientSocket() : Error in initWinSock() = 0x%04x\n"), rc));
		AddResponse("clientSocket() : Error in initWinSock()\n");
		return(INVALID_SOCKET);
	}	
	addr = atoAddr(address);
	if(addr == NULL) {
		OUTPUTDEBUGMSG((("clientSocket() : Invalid address: %s\n"),address));
		AddResponse("clientSocket() : Invalid address\n");
		return(INVALID_SOCKET);
	}
	
	memset((char *) &sa,0,sizeof(sa));
	sa.sin_family = AF_INET;
	sa.sin_port = htons((unsigned short) port);
	sa.sin_addr.s_addr = addr->s_addr;
	
	// open the socket
	s = socket(AF_INET,SOCK_STREAM,PF_UNSPEC);
	if(s == INVALID_SOCKET) {
		OUTPUTDEBUGMSG((("clientSocket() : Could not create socket\n")));
		AddResponse("clientSocket() : Could not create socket\n");
		return(INVALID_SOCKET);
	}
	// connect
	connect(s,(struct sockaddr *) &sa,sizeof(sa));
	return(s);
}




// this function writes a character string out to a socket.
// it returns -1 if the connection is closed while it is trying to write
static int sockWrite(SOCKET sock,char *str,size_t count)
{
	size_t	bytesSent=0 ; 
	int		thisWrite ;
	
	while (bytesSent < count)  {
		thisWrite=send(sock,str,count-bytesSent,0) ;
		if(thisWrite <= 0) {
			return (thisWrite) ;
		}
		bytesSent += thisWrite ; 
		str += thisWrite ;
	}
	return(count) ;
}

int sockPuts(SOCKET sock,char *str)
{
// 	OUTPUTDEBUGMSG((("sockPuts() : %s\n"), str));
	AddResponse(str) ;
	return(sockWrite(sock,str,strlen(str)));
}

int sockGets(SOCKET sockfd,char *str,size_t count)
{
	int bytesRead;
	size_t totalCount = 0;
	char buf[1], *currentPosition;
	char lastRead = 0;

	currentPosition = str;
	while(lastRead != 10) {
		bytesRead=recv(sockfd,buf,1,0);
		if(bytesRead <= 0) {
			// the other side may have closed unexpectedly
			return (-1);
		}
		lastRead=buf[0];
		if((totalCount < count) && (lastRead != 10) && (lastRead != 13)) {
			*currentPosition=lastRead;
			currentPosition++;
			totalCount++;
		}
	}
	if (count > 0) {
		*currentPosition=0;
	}
	return (totalCount);
}


// disconnect to SMTP server and returns the socket fd
static void smtpDisconnect(SOCKET sfd)
{
	closesocket(sfd) ;
}

// connect to SMTP server and returns the socket fd
static SOCKET smtpConnect(char *smtp_server,int port)
{
	SOCKET sfd;
	
	sfd = clientSocket(smtp_server,port);
	if(sfd == INVALID_SOCKET) {
		OUTPUTDEBUGMSG((("smtpConnect() : Could not connect to SMTP server \"%s\" at port %d\n"), smtp_server,port));
		AddResponse("smtpConnect() : Could not connect to SMTP server\n");

		nSMTPerrors++;	// PH: Counts # Errors

		return (INVALID_SOCKET);
	}
	// save it. we'll need it to clean up
	smtp_socket = sfd;
	return(sfd);
}

// read SMTP response. returns 0 on success, -1 on failure 
static int smtpResponse(int sfd)
{
	int n, err ;
	char buf[BUFSIZ], tmp[BUFSIZ] ;

	memset(buf,0,sizeof(buf));
	n = sockGets(sfd, buf, sizeof(buf)-1);
//	OUTPUTDEBUGMSG((("smtpResponse() : %s\n"),buf));
	AddResponse(buf) ;
	err = atoi(buf) ;
	OUTPUTDEBUGMSG((("smtpResponse(): Err: %d!\n"), err));
	if(err == 334) {
		DecodeBase64(buf+4, tmp) ;
		strcpy(buf+4, tmp) ;
	}

	if(buf[0] == '1' || buf[0] == '2' || buf[0] == '3' && buf[3] == A_SPACE) {
		return (0);
	}
	OUTPUTDEBUGMSG((("smtpResponse(): ERROR!\n")));
	nSMTPerrors++;			// PH: Counts # Errors
	iSMTPlastError = err;	// PH: Last Error

	return (-1);
}

// SMTP: HELO
static int smtpHelo(int sfd)
{
	// read off the greeting 
	smtpResponse(sfd);
	_snprintf(buf,sizeof(buf)-1,"HELO %s\r\n", mail.helo_domain);
//	_snprintf(buf,sizeof(buf)-1,"EHLO %s\r\n", mail.helo_domain);
	sockPuts(sfd,buf);
	return (smtpResponse(sfd));
}


// SMTP: Authentication
static int smtpLogin(int sfd)
{
	char szTmp[128] ;


	if(mail.options & MAIL_OPTION_AUTH) {
		_snprintf(buf,sizeof(buf)-1,"AUTH LOGIN\r\n");
		sockPuts(sfd,buf);
		if(smtpResponse(sfd)) return(TRUE) ;

		_snprintf(buf,sizeof(buf)-1, "%s\r\n", EncodeBase64(mail.user, szTmp));
		sockPuts(sfd,buf);
		if(smtpResponse(sfd)) return(TRUE) ;

		_snprintf(buf,sizeof(buf)-1, "%s\r\n", EncodeBase64(mail.password, szTmp));
		sockPuts(sfd,buf);
		return(smtpResponse(sfd));
	}
	return(FALSE) ;
}


// SMTP: MAIL FROM 
static int smtpMailFrom(int sfd)
{
	_snprintf(buf,sizeof(buf)-1,"MAIL FROM: <%s>\r\n",mail.from);
//	OUTPUTDEBUGMSG((("smtpMailFrom() : >>> %s"),buf));
	sockPuts(sfd,buf);
	return (smtpResponse(sfd));
}

// SMTP: quit
static int smtpQuit(int sfd)
{
	sockPuts(sfd,"QUIT\r\n");
	return (smtpResponse(sfd));
}

// SMTP: RSET
// aborts current mail transaction and cause both ends to reset
static int smtpRset(int sfd)
{
	sockPuts(sfd,"RSET\r\n");
	return (smtpResponse(sfd));
}


char *StripSpecial(char *szStr)
{
	int len = strlen(szStr) ;

	while(len--) {
		switch(szStr[len]) {
			case ',' :
			case ';' :
			case ' ' :
				szStr[len] = '\0' ;
				break ;
			default:
				return(szStr) ;
		}
	}
	return(szStr) ;
}

// SMTP: RCPT TO
static int smtpRcptTo(int sfd)
{
	static char szTemp[MAX_MAIL * 5] ;
	char *pTmp1 = szTemp, *pTmp2 ;

	strncpy(szTemp, mail.to, (MAX_MAIL * 5) - 1) ;
	StripSpecial(szTemp) ;

	while(1) {
		pTmp2 = strchr(pTmp1, ';') ;
		if(NULL == pTmp2) {
			pTmp2 = strchr(pTmp1, ',') ;
		}
		if(pTmp2) {
			*pTmp2 = '\0' ;
		}
		_snprintf(buf, sizeof(buf)-1, "RCPT TO: <%s>\r\n", pTmp1);
		sockPuts(sfd,buf);
		if (smtpResponse(sfd) != 0) {
			smtpRset(sfd);
			return (-1);
		}
		if(pTmp2) {
			pTmp1 = pTmp2 ;
			pTmp1++ ;
		}
		else {
			break ;
		}
	} 
	return (0);
	
}

// SMTP: DATA
static int smtpData(int sfd)
{
	sockPuts(sfd,"DATA\r\n");
	return(smtpResponse(sfd));
}

// SMTP: EOM
static int smtpEom(int sfd)
{
	sockPuts(sfd,"\r\n.\r\n");
	return (smtpResponse(sfd));
}

// SMTP: mail
static int smtpMail(int sfd, char *data)
{	
	char szBuffer[128], *pTmp ;
	char szSubject[1024]="";
	char szBody[1024]="";
	extern int nSMTPemails;

	for (int i=0; data[i]!=0; i++)
	{
		if (data[i] == '»')
		{
			strcat(szSubject, " - ");
			strcat(szBody, "\n");
		}
		else
		{
			szSubject[strlen(szSubject)] = data[i];
			szBody[strlen(szBody)] = data[i];
		}
	}

	if (mail.options & MAIL_OPTION_SUBJECT)
	{
		if (szSubject[0])
		{
			memset(buf,0,sizeof(buf));
			(void) _snprintf(buf,sizeof(buf)-1,"Subject: %s\r\n", szSubject);
			sockPuts(sfd,buf);

			memset(buf,0,sizeof(buf));
			strcpy(szBuffer, szSmtpCharSets[((Profile.nMailOptions & 0x1F0000) >> 16) -1]) ;
			pTmp = strchr(szBuffer, ' ') ;
			if(pTmp != NULL) {
				*pTmp = '\0' ;
			}
			(void) _snprintf(buf,sizeof(buf)-1,"Content-type: text/plain; charset=\"%s\"\r\n", szBuffer);
			sockPuts(sfd,buf);
		}
	}
	
	// headers
	if(mail.from)
	{
		memset(buf,0,sizeof(buf));
		(void) _snprintf(buf,sizeof(buf)-1,"From: %s\r\n",mail.from);
		sockPuts(sfd,buf);
	}
	if(mail.to)
	{
		memset(buf,0,sizeof(buf));
		(void) _snprintf(buf,sizeof(buf)-1,"To: %s\r\n",mail.to);
		sockPuts(sfd,buf);	
	}
	
	if(mail.cc)
	{
		memset(buf,0,sizeof(buf));
		(void) _snprintf(buf,sizeof(buf)-1,"Cc: %s\r\n",mail.cc);
		sockPuts(sfd,buf);
	}
	if(mail.bcc)
	{
		memset(buf,0,sizeof(buf));
		(void) _snprintf(buf,sizeof(buf)-1,"Bcc: %s\r\n",mail.bcc);
		sockPuts(sfd,buf);
	}
	memset(buf,0,sizeof(buf));
	_snprintf(buf,sizeof(buf)-1,"X-Mailer: %s\r\n",MAILSEND_VERSION);
	sockPuts(sfd,buf);
	
	
	sockPuts(sfd,"\r\n");
	
	if ((mail.options & MAIL_OPTION_MSG) && szBody[0])
	{
		sockPuts(sfd, szBody);
		sockPuts(sfd,"\r\n");
	}
	nSMTPemails++;
	return (0);
}


// returns 0 on success, -1 on failure
int xSendMail(THEMAIL *pMail)
{
	SOCKET	sfd;
	int 	rc = (-1);
	char *pTmp ;
	extern int nSMTPsessions;
	
	if(nBufferdMailStart == nBufferdMailEnd)
	{
		return(0) ;
	}

	pTmp = szMailBuffer[nBufferdMailEnd] ;
	nBufferdMailEnd++ ;
	if(nBufferdMailEnd >= MAX_MAIL) {
		nBufferdMailEnd = 0 ;
	}

	if (pMail->from == (char *) NULL) {
		OUTPUTDEBUGMSG((("No From address specified")));
		AddResponse("xSendMail(): No From address specified\n");
		return (0);
	}
	if (pMail->smtp_server == (char *) NULL) {
		pMail->smtp_server= "127.0.0.1" ;
		OUTPUTDEBUGMSG((("No smtp_server specified using default : %s"), pMail->smtp_server)) ;
	}
	if (pMail->smtp_port == -1) {
		pMail->smtp_port=MAILSEND_SMTP_PORT;
		OUTPUTDEBUGMSG((("No smtp_port specified using default port %d"), pMail->smtp_port));
	}
	if (pTmp == (char *) NULL) {
		pTmp = MAILSEND_DEF_SUB;
		OUTPUTDEBUGMSG((("No subject specified using default subject %s"), pTmp));
	}
	if (pMail->helo_domain == (char *) NULL) {
		pMail->helo_domain= "127.0.0.1" ;
		OUTPUTDEBUGMSG((("No domain specified using default %s"), pMail->helo_domain));
	}
	
	// open the network connection
	sfd = smtpConnect(pMail->smtp_server, pMail->smtp_port);
	if(sfd == INVALID_SOCKET)
	{
		nSMTPerrors++;			// PH: Counts # of Errors
		return(0) ;
	}
	else nSMTPsessions++;		// PH: Counts # of sessions

	if(!(rc = smtpHelo(sfd))) {
		if(!(rc = smtpLogin(sfd))) {
			if(!(rc = smtpMailFrom(sfd))) {
				if(!(rc = smtpRcptTo(sfd))) {
					if(!(rc = smtpData(sfd))) {
						if(!(rc = smtpMail(sfd, pTmp))) {
							if(!(rc = smtpEom(sfd))) {
								rc = smtpQuit(sfd) ;
							}
						}
					}
				}
			}
		}
	}

	// close the network connection
	smtpDisconnect(sfd);
	return(1);
}


DWORD WINAPI MailThreadFunc(LPVOID lpData)
{
	OUTPUTDEBUGMSG((("MailThreadFunc()")));

	while(keepbusy) {
		if(!xSendMail((THEMAIL *) lpData)) {
			Sleep(1000) ;
		}
	}
	OUTPUTDEBUGMSG((("MailThreadFunc() 	ExitThread(0)\n")));
	ExitThread(0);
	return 0;
}

void StartMail(int nOptions)
{
	DWORD dummy = 0;

//	OUTPUTDEBUGMSG((("StartMail()")));
	if(nOptions & MAIL_OPTION_ENABLE)
	{
		if(MailThread != 0)
		{
			OUTPUTDEBUGMSG((("StartMail() MailThread != 0  Mail is already Started!")));
			return;
		}
		keepbusy = TRUE ;
		MailThread = CreateThread(0,0,MailThreadFunc,(LPVOID) &mail,0, &dummy);
		OUTPUTDEBUGMSG((("StartMail() CreateThread\n")));
	}
	else
	{
		if(MailThread == 0)
		{
			OUTPUTDEBUGMSG((("StartMail() MailThread == 0  Mail is already Stopped!")));
			return;
		}
		keepbusy = FALSE ;
		Sleep(500) ;
		CloseHandle(MailThread);
		MailThread = 0;
		OUTPUTDEBUGMSG((("StartMail() CloseHandle(MailThread)\n")));
	}
}


int SendMail(HWND hResponse, bool bMatch, bool bMonitor_only, int iSeparateSMTP, char *sz1, char *sz2, char *sz3, char *sz4, char *sz5, char *sz6, char *sz7, char *szLabel)
{
	int	 len = 0 ;
	char szBuffer[1024] = { 0 } ;
//	char szSubject[1024]="";

//	OUTPUTDEBUGMSG((("SendMail()")));
	mail.hResponse = hResponse ;
	if(hResponse) 
	{
		SendMessage(hResponse, LB_RESETCONTENT, 0, 0L) ;
	}
	if(mail.options & MAIL_OPTION_ENABLE) 
	{
		switch(mail.options & MAIL_OPTION_MODES) 
		{
			case MAIL_OPTION_MODE_ALL :
				OUTPUTDEBUGMSG((("SendMail() Send : Mode All")));
			break ;

			case MAIL_OPTION_MODE_FILTER :	
			if(!bMatch || bMonitor_only)
			{
				OUTPUTDEBUGMSG((("SendMail() Not Send : !bMatch || bMonitor_only")));
				return(0) ;
			}
			OUTPUTDEBUGMSG((("SendMail() Send : bMatch(%d) || bMonitor_only(%d)"), bMatch, bMonitor_only));
			break ;

			case MAIL_OPTION_MODE_MONITOR :
			if(!bMatch)
			{
				OUTPUTDEBUGMSG((("SendMail() Not Send: !bMatch")));
				return(0) ;
			}
			OUTPUTDEBUGMSG((("SendMail() Send : bMatch(%d) || bMonitor_only(%d)"), bMatch, bMonitor_only));

			break ;
			case MAIL_OPTION_MODE_SELECTABLE :
			if(!bMatch || !iSeparateSMTP)
			{
				OUTPUTDEBUGMSG((("SendMail() Not Send: !bMatch || !iSeparateSMTP")));
				return(0) ;
			}
			OUTPUTDEBUGMSG((("SendMail() Send : iSeparateSMTP(%d)"), iSeparateSMTP));
			break ;
		}
	}
	else 
	{
		OUTPUTDEBUGMSG((("SendMail() Mail Disabled")));
		return(0) ;
	}

	if(mail.options & MAIL_OPTION_ADDRESS)
	{
		len += wsprintf(szBuffer + len, "%s ", sz1) ;
	}
	if(mail.options & MAIL_OPTION_TIME)
	{
		len += wsprintf(szBuffer + len, "%s ", sz2) ;
	}
	if(mail.options & MAIL_OPTION_DATE)
	{
		len += wsprintf(szBuffer + len, "%s ", sz3) ;
	}
	if(mail.options & MAIL_OPTION_MODE)
	{
		len += wsprintf(szBuffer + len, "%s ", sz4) ;
	}
	if(mail.options & MAIL_OPTION_TYPE)
	{
		len += wsprintf(szBuffer + len, "%s ", sz5) ;
	}
	if(mail.options & MAIL_OPTION_BITRATE)
	{
		len += wsprintf(szBuffer + len, "%s ", sz6) ;
	}
	if(mail.options & MAIL_OPTION_MESSAGE)
	{
		len += wsprintf(szBuffer + len, "%s ", sz7) ;
	}
	if(mail.options & MAIL_OPTION_LABEL)
	{
		len += wsprintf(szBuffer + len, "- %s ", szLabel) ;
	}

	if(!mail.smtp_port)
	{
		OUTPUTDEBUGMSG((("SendMail() Error: MailInit NOT called!")));
		nSMTPerrors++;		// PH: Counts # of Errors

		return(-1) ;
	}

	nMaxLen = 0 ;
	if(szBuffer[0])
	{
		OUTPUTDEBUGMSG((("SendMail() Send : >%s<\n"), szBuffer));
		strncpy(szMailBuffer[nBufferdMailStart], szBuffer, MAX_MAIL_LEN) ;
		nBufferdMailStart++ ;

		if(nBufferdMailStart >= MAX_MAIL)
		{
			nBufferdMailStart = 0 ;
		}
	}
//	OUTPUTDEBUGMSG((("SendMail() nBufferdMailStart %d nBufferdMailEnd %d\n"), nBufferdMailStart, nBufferdMailEnd));
	return(0) ;
}	


int MailInit(char *szMailHost, char *szMailHeloDomain, char *szMailFrom, char *szMailTo, char *szMailUser, char *szMailPassword, int iMailPort, int nOptions)
{
	memset(&mail, 0, sizeof(mail)) ;
	mail.from = szMailFrom ;
	mail.to = szMailTo ;
	mail.cc = NULL ;
	mail.bcc = NULL ;
	mail.smtp_server = szMailHost ;
	mail.helo_domain = szMailHeloDomain ;
	mail.user = szMailUser ;
	mail.password = szMailPassword ;
//	mail.smtp_port = -1 ;
	mail.smtp_port = iMailPort ;
	mail.helo_domain = NULL ;
	mail.options = nOptions ;
	StartMail(nOptions) ;
	return(0) ;
}
