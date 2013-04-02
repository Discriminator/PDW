// Decode.cpp
//
// Serial port decoding routines.
//
// pdw_decode():
//
//   - Decides which functions to call for decoding:
//     POCSAG/FLEX/ACARS/MOBITEX or ERMES signals.
//   - Decides when to log/messages statistics info.
//   - Calls the signal indicator.
//
#ifndef STRICT
#define STRICT 1
#endif

#include <windows.h>

#include "headers\PDW.h"
#include "headers\initapp.h"
#include "headers\sigind.h"
#include "headers\decode.h"
#include "headers\acars.h"
#include "headers\mobitex.h"
#include "headers\ermes.h"
#include "headers\helper_funcs.h"
#include "utils\debug.h"

FILE *pd_raw_fp = NULL;		// Debug - Raw data file for logging POCSAG/FLEX symbol data.

char szCurrentDate[40]="";	// PH: Buffer for current date
char szCurrentTime[40]="";	// PH: Buffer for current time

// Filter globals
FILTER filter;

// Statistics - globals
unsigned long hourly_stat[NUM_STAT][2];
unsigned long hourly_char[NUM_STAT][2];
unsigned long daily_stat[NUM_STAT][2];
unsigned long daily_char[NUM_STAT][2];

SYSTEMTIME statTime;
SYSTEMTIME prev_statTime;
int stat_timer = 0;

char ob[32];
//unsigned int bch[1025];
//unsigned int ecs[25];            // error correction sequence

int    ircver=0;
double ct1600, ct3200, ct_bit;
double rcver[64];                // rcv clock error ring buf
double exc=0.0;

// receive symbol clock tightness - lower numbers mean slower loop response
double rcv_clkt = 0.065;         // rcv loop clock tightness
double rcv_clkt_hi = 0.065;      // coarse rcv clock (sync ups only)
double rcv_clkt_fl = 0.015;      // fine rcv clock (data) - FLEX
double rcv_clkt_po = 0.025;      // fine rcv clock (data) - POCSAG/ACARS
double rcv_clkt_mb = 0.080;      // sync ups & data - Mobitex
double rcv_clkt_em = 0.030;      // sync ups & data - Ermes

// Flex globals
FLEX phase_A, phase_B, phase_C, phase_D;	// FLEX phases
int  flex_timer = 0;						// FLEX active timer
bool bReflex = false;						// Reflex mode flag
bool bFlexActive = false;					// FLEX mode flag

// Pocsag globals
POCSAG pocsag;
int pocsag_baud_rate;						// Pocsag baudrate
int pocbit = 0;								// Pocsag mode flag

// this table translates received modem status line combinations into
// the received symbol; it gives the number of modem status lines that
// are high (other than RI) for a given line status nybble
int rcv[16]={ 0, 1, 1, 2, 0, 1, 1, 2, 1, 2, 2, 3, 1, 2, 2, 3 };

// These hold or track VxD input data from comport
unsigned long  bufsize;
unsigned long  *cpstn = NULL;
unsigned short int *freqdata = NULL;
unsigned char  *linedata = NULL;

unsigned int pd_i=0, pd_cw1=49, pd_cw0=48, pd_ldsr=0;
unsigned int pd_lcw=0, pd_dsr=0;
signed int pd_ct5=0, pd_ct12=0, pd_ct16=0, pd_ct24=0, pd_dinc=0;
double pd_clk=0.0, pd_rer=0.0;
int sig_flg=0;

// HWi Add variables to hold the original values when in playback mode
unsigned long  old_bufsize;
unsigned long  *old_cpstn;
unsigned short int *old_freqdata;
unsigned char  *old_linedata;


// Reset all main variables, when switching between
// serial port and soundcard.
void pd_reset_all(void)
{
	pd_i=0, pd_cw1=49, pd_cw0=48, pd_ldsr=0;
	pd_dsr=0, pd_lcw=0;
	pd_ct5=0, pd_ct12=0, pd_ct16=0, pd_ct24=0, pd_dinc=0;
	pd_clk=0.0, pd_rer=0.0;

	mb.reset();				// reset mobitex routines
	acars.reset();			// reset acars routines
	pocsag.frame(-1);		// reset pocsag routines
	flex_reset();			// reset flex routines

	if (Profile.monitor_acars)				// Decoding ACARS?
	{
		exc = 0.0;
		rcv_clkt = rcv_clkt_po;
		ct_bit = 496.4;
	}
	else if (Profile.monitor_mobitex)		// Decoding Mobitex?
	{
		exc = 0.0;
		rcv_clkt = rcv_clkt_mb;				// sync-ups & data.
		ct_bit = 1.0/((float) mb.bitrate * 839.22e-9);
//		ct_bit = 993.0;
	}
	else if (Profile.monitor_ermes)			// Decoding Ermes?
	{
		em.frame(-1);						// Reset ermes routines! 
		exc = 0.0;
		rcv_clkt = rcv_clkt_em;				// sync-ups & data
		ct_bit = 1.0/(3125.0*838.8e-9);
	}
	else if (Profile.monitor_paging)
	{
		ct_bit = ct1600;					// reset rcv clock to 1600 baud
		rcv_clkt = rcv_clkt_hi;				// widen up rcv clk loop again
	}
}


// RAH: Record and playback

FILE *pRecording = NULL;		// Pointer to recording file

bool bPlayback  = false;		// Playback in progress
bool bRecording = false;		// Recording in progress
bool bAutoRecording = false;	// AutoRecording in progress

struct
{
	unsigned short int freqdata;
	unsigned char linedata;
} recdata;

int recsize = 3;				// Ignore word alignment, 3 bytes exact
unsigned long nextcpstn = 1;	// Ahead of cpstn for playback
unsigned long rcpstn = 0;		// Index used by cpstn for playback
unsigned short int ltick;		// Last tick for pocflex rec compatibility

#define RECBUFSIZE  4096
#define FILEBUFSIZE 2048

// Playback and file buffers
unsigned short int rfreqdata[RECBUFSIZE];
unsigned char rlinedata[RECBUFSIZE];
char rbuffer[FILEBUFSIZE];

// RAH: Start playback of recording
bool Start_Playback(LPTSTR lpstrFile)
{
	pd_reset_all();

	// HWi Save values to restore it after the playback
	old_cpstn    = cpstn;
	old_freqdata = freqdata;
	old_linedata = linedata;
	old_bufsize  = bufsize;

	cpstn    = &rcpstn;			// Set pointers to fixed locations
	freqdata = &rfreqdata[0];
	linedata = &rlinedata[0];
	bufsize  = RECBUFSIZE;
	ltick    = 0;

	if ((pRecording = fopen(lpstrFile,"rb")) == NULL)	// read
	{
		MessageBox(NULL,"Error opening playback file.","PDW",MB_ICONWARNING);
		bPlayback = false;
	}
	else
	{
		setbuf(pRecording,rbuffer);
		bPlayback = true;
			}
	return(bPlayback);
}

// RAH: Stop playback of recording
void Stop_Playback(void)
{
	bPlayback = false;

	if (pRecording)
	{
		fclose(pRecording);
		pRecording = NULL;
		MessageBeep(MB_OK);
	}

	// HWi Restore values after the playback
	cpstn    = old_cpstn;
	freqdata = old_freqdata;
	linedata = old_linedata;
	bufsize  = old_bufsize;
}

// RAH: Handle playback of recording
void pdw_playback(void)
{
	BOOL bMore = TRUE;
	BOOL bAnything = FALSE;

	while (bMore)
	{
		bMore = fread(&recdata,recsize,1,pRecording);

		if (bMore)
		{	// Data from playback file
			freqdata[rcpstn] = ltick - recdata.freqdata; // Calculate time since last
			linedata[rcpstn] = recdata.linedata;
			ltick  = recdata.freqdata;
			rcpstn = nextcpstn++;
			bAnything = TRUE;
			if (nextcpstn >= RECBUFSIZE) nextcpstn = 0;
			if (nextcpstn == pd_i)  break;	// Interpret now
		}
	}
	if (bAnything)
	{
		pdw_decode();
	}
	else // Done
	{
		Stop_Playback();
	}
}

// RAH: Start recording
// HWi Inserted recording again
void Start_Recording(LPTSTR lpstrFile)
{
	// HWi
	char szBuffer[80];
	sprintf(szBuffer, "Recording %s", lpstrFile);
	SetWindowText(ghWnd, (LPSTR) szBuffer);
	bRecording = true;

	if ((pRecording = fopen(lpstrFile,"ab")) == NULL)	// append
    {
		MessageBox(NULL,"Error opening output file for recording.","PDW",MB_ICONWARNING);
		bRecording = false;
        // HWi 
		SetWindowText(ghWnd, (LPSTR) pdw_version);
    }
    else
    {
		setbuf(pRecording,rbuffer);
		fflush(pRecording);
    }
}

// RAH: Stop recording
void Stop_Recording(void)
{
	bRecording = false;

	if (pRecording)
	{
		fclose(pRecording);
		pRecording = NULL;
	}
	SetWindowText(ghWnd, (LPSTR) pdw_version);
}

// RAH: Select a file for recording
BOOL Open_Recording(OPENFILENAME *pofn, LPTSTR lpstrFile, bool bOpennotsave)
{
	static char szRec[] = {"Recordings (*.rec)\0*.rec\0All Files (*.*)\0*.*\0\0"};
	char szTitle[256];
	char szExt[] = ".rec";
	char szInitialDir[MAX_PATH];

	sprintf(szInitialDir, "%s\\Recordings", szPath);
	strcpy(szTitle, bOpennotsave ? "Playback Recording" : "Start Recording");

	pofn->lStructSize		= sizeof(OPENFILENAME);
	pofn->hwndOwner			= ghWnd;
	pofn->lpstrFilter		= szRec;
	pofn->nFilterIndex		= 1;
	pofn->lpstrFile			= lpstrFile;
	pofn->nMaxFile			= MAX_PATH;
    pofn->lpstrInitialDir	= bOpennotsave ? szInitialDir : szPath;
	pofn->Flags				= OFN_HIDEREADONLY;
	pofn->lpstrTitle		= szTitle;
	pofn->lpstrDefExt		= szExt;

	return(GetOpenFileName(pofn));
}

void pdw_decode_acars(void)
{
	if (pd_i != *cpstn)
	{
		UpdateSigInd(0);		// Move signal indicator left 1
	}

	while (pd_i != *cpstn)
	{
		pd_clk += freqdata[pd_i];
		pd_cw0  = linedata[pd_i] >> 4;
		pd_cw1  = rcv[pd_cw0];

		// if two level interface: force symbol to be either 0 or 3 and
		// process as if 4 level interface were used. Phases B & D then
		// always get a long stream of 0's and don't produce any output.

		if (!Profile.fourlevel && (pd_cw1 > 0)) pd_cw1 = 3;

		if (Profile.invert) pd_cw1 ^= 0x03;	// if in invert mode - invert both bits

		while (pd_clk >= (exc + 0.5 * ct_bit))
		{
			acars.frame(pd_cw1 > 1);		// Put into ACARS decoding routine
			pd_clk = pd_clk - ct_bit;

			if (acars.ac_alive)
			{
				if (sig_flg < 3)			// Move signal indicator right 1
				{
					UpdateSigInd(1);
					sig_flg++;
				}
			}
		}

		// clk now holds new boundary position. update exc slowly...
		// for now only use center transitions to adjust rcv clock

		if (pd_cw1 > 1) pd_dsr = 0;
		else			pd_dsr = 1;

		// was 0.02 ....
		pd_rer = pd_clk - exc;

		if (pd_dsr != pd_ldsr) exc = exc + rcv_clkt * (pd_rer);

		pd_ldsr = pd_dsr;
		pd_i++;

		if (pd_i == bufsize) pd_i = 0;
	}
}


void pdw_decode_mobitex(void)
{
	if (mb.timer)				// Check if dropped out of mobitex mode.
	{
		mb.timer--;
		if (mb.timer == 0) display_showmo(MODE_IDLE);
		
		UpdateSigInd(1);		// Move signal indicator right 1
	}
	else if (pd_i != *cpstn)
	{
		UpdateSigInd(0);		// Move signal indicator left 1
	}

	while (pd_i != *cpstn)
	{
		pd_clk += freqdata[pd_i];
		pd_cw0  = linedata[pd_i] >> 4;
		pd_cw1  = rcv[pd_cw0];

		// if two level interface: force symbol to be either 0 or 3 and
		// process as if 4 level interface were used. Phases B & D then
		// always get a long stream of 0's and don't produce any output.

		if (!Profile.fourlevel && (pd_cw1 > 0)) pd_cw1 = 3;

		if (Profile.invert) pd_cw1 ^= 0x03;     // if in invert mode - invert both bits

		while (pd_clk >= (exc + 0.5 * ct_bit))
		{
			mb.frame_sync(pd_cw1 > 1);			// Put into MOBITEX decoding routine
			pd_clk = pd_clk - ct_bit;
		}

		// clk now holds new boundary position. update exc slowly...
		// for now only use center transitions to adjust rcv clock

		if (pd_cw1 > 1) pd_dsr = 0;
		else			pd_dsr = 1;

		// was 0.02 ....
		pd_rer = pd_clk - exc;

		if (pd_dsr != pd_ldsr) exc = exc + rcv_clkt * (pd_rer);

		pd_ldsr = pd_dsr;
		pd_i++;

		if (pd_i == bufsize) pd_i = 0;
	}
}


void pdw_decode_ermes(void)
{ 
	if (em.timer)				// Check if dropped out of ermes mode.
	{
		em.timer--;
		if (em.timer == 0) display_showmo(MODE_IDLE);
		
		UpdateSigInd(1);		// Move signal indicator right 1
	}
	else if (pd_i != *cpstn)
	{
		UpdateSigInd(0);		// Move signal indicator left 1
	}

	while (pd_i != *cpstn)
	{
		pd_clk += freqdata[pd_i];
		pd_cw0  = linedata[pd_i] >> 4;
		pd_cw1  = rcv[pd_cw0];

		// if two level interface: force symbol to be either 0 or 3 and
		// process as if 4 level interface were used. Phases B & D then
		// always get a long stream of 0's and don't produce any output.

		if (!Profile.fourlevel && (pd_cw1 > 0)) pd_cw1 = 3;

		if (Profile.invert) pd_cw1 ^= 0x03;     // if in invert mode - invert both bits

		while (pd_clk >= (exc + 0.5 * ct_bit))
		{
			em.frame(pd_cw1);					// Put into ERMES decoding routine
			pd_clk = pd_clk - ct_bit;
		}
        
		// clk now holds new boundary position. update exc slowly...
		// for now only use center transitions to adjust rcv clock

		if (pd_cw1 > 1) pd_dsr = 0;
		else			pd_dsr = 1;

		// was 0.02 ....
		pd_rer = pd_clk - exc;

		if (pd_dsr != pd_ldsr) exc = exc + rcv_clkt * (pd_rer);

		pd_ldsr = pd_dsr;
		pd_i++;

		if (pd_i == bufsize) pd_i = 0;
	}
}


// Decode POCSAG/FLEX/ACARS/MOBITEX/ERMES.....
void pdw_decode(void)
{
	if (bRecording)	// Hwi Added for recording function
	{
		static unsigned int last_write;
		static unsigned int tmp;

		while (tmp != *cpstn)
		{
			last_write -= freqdata[tmp];
			fwrite(&last_write,    2, 1, pRecording);
			fwrite(&linedata[tmp], 1, 1, pRecording);
			tmp++;

			if (tmp == bufsize) tmp = 0;
		}
	}

	sig_flg=0;
	check_save_data();				// Log messages/status info.

	if (Profile.monitor_mobitex)	// Decoding Mobitex..
	{
		pdw_decode_mobitex();
		return;
	}
	if (Profile.monitor_ermes)		// Decoding Ermes..
	{
		pdw_decode_ermes();
		return;   
	}    
	if (Profile.monitor_acars)		// Decoding ACARS..
	{
		pdw_decode_acars();
		return;
	}

	if (flex_timer)		// Check if dropped out of flex mode.
	{
		flex_timer--;

		if (flex_timer == 0)
		{
			display_showmo(MODE_IDLE);
		}
		UpdateSigInd(1);		// Move signal indicator right 1
		sig_flg++;
	}
	else if (pd_i != *cpstn)
	{
		UpdateSigInd(0);		// Move signal indicator left 1
	}

	while (pd_i != *cpstn)
	{
//		pd_clk += freqdata[pd_i];
		if (Profile.comPortRS232 > 1) pd_clk = freqdata[pd_i];
		else pd_clk += freqdata[pd_i];
		pd_cw0  = linedata[pd_i] >> 4;
		pd_cw1  = rcv[pd_cw0];

		// if two level interface: force symbol to be either 0 or 3 and
		// process as if 4 level interface were used. Phases B & D then
		// always get a long stream of 0's and don't produce any output.

		if (!Profile.fourlevel && (pd_cw1 > 0)) pd_cw1 = 3;

		if (Profile.invert) pd_cw1 ^= 0x03;	// if in invert mode - invert both bits

		if (Profile.decodepocsag && !bFlexActive && ((pd_cw1 >> 1) != pd_lcw))
		{
			// look for POCSAG preamble sequence...

			if ((pd_dinc > 421)  && (pd_dinc < 571))  pd_ct24++;
			else if (pd_ct24 > 5) pd_ct24 -= 3;

			if ((pd_dinc > 842)  && (pd_dinc < 1142)) pd_ct12++;
			else if (pd_ct12 > 5) pd_ct12 -= 3;

			if ((pd_dinc > 1976) && (pd_dinc < 2674)) pd_ct5++;
			else if (pd_ct5 > 5) pd_ct5 -= 3;

			if (pd_ct24 > 180)
			{
				pd_ct24 = 0;

				if (Profile.pocsag_2400)
				{
					display_showmo(MODE_POCSAG+MODE_P2400);
					ct_bit = Profile.comPortRS232 ? 500 : 496.4;
					pocsag_baud_rate = STAT_POCSAG2400;

					if (!pocbit)
					{
						exc = 0.0;
						pd_clk = 0.0;
					}
					rcv_clkt = rcv_clkt_po;
					pocbit = 600;
				}
			}
			else if (pd_ct12 > 180)
			{
				pd_ct12 = 0;
				
				if (Profile.pocsag_1200)
				{
					display_showmo(MODE_POCSAG+MODE_P1200);
					ct_bit = Profile.comPortRS232 ? 1000 : 993.0;
					pocsag_baud_rate = STAT_POCSAG1200;

					if (!pocbit)
					{
						exc = 0.0;
						pd_clk = 0.0;
					}
					rcv_clkt = rcv_clkt_po;
					pocbit = 600;
				}
			}
			else if (pd_ct5 > 180)
			{
				pd_ct5 = 0;
				
				if (Profile.pocsag_512)
				{
					display_showmo(MODE_POCSAG+MODE_P512);
					ct_bit = Profile.comPortRS232 ? 2000 : 2327.4;
					pocsag_baud_rate = STAT_POCSAG512;

					if (!pocbit)
					{
						exc = 0.0;
						pd_clk = 0.0;
					}
					rcv_clkt = rcv_clkt_po;
					pocbit = 600;
				}
			}
			pd_dinc = freqdata[pd_i];
			pd_lcw  = pd_cw1 >> 1;
		}
		else pd_dinc += freqdata[pd_i];		// didn't have transition across center

		while (pd_clk >= (exc + 0.5 * ct_bit))
		{
//			if (Profile.comPortRS232 == 2) pd_clk=0;
//			else pd_clk = pd_clk - ct_bit;
			pd_clk = pd_clk - ct_bit;

			if (Profile.decodeflex && !pocbit)
			{
				frame_flex(pd_cw1);			// Put into FLEX decoding routine
				rcver[ircver++] = pd_rer;
				if (ircver > 63) ircver = 0;
			}
			else if (Profile.decodepocsag && pocbit)
			{
				if (sig_flg < 3)
				{
					UpdateSigInd(1);		// Move signal indicator right 1
					sig_flg++;
				}
				pocsag.frame(pd_cw1 > 1);	// Put into POCSAG decoding routine

				// decrement pocsag bit counter; if zero we're back to flex
				// note : pocbit is reset by pocsag processing routines whenever
				// a good word has been received

				pocbit--;
				if (pocbit == 0)				// we have just dropped out of POCSAG mode
				{
					display_showmo(MODE_IDLE);
					pocsag.frame(-1);			// reset pocsag routine
					ct_bit = ct1600;			// reset rcv clock to 1600 baud
					rcv_clkt = rcv_clkt_hi;		// widen up rcv clk again
				}
			}
			if (Profile.comPortRS232 > 1) break;
		}

		if (Profile.comPortRS232 < 2)
		{
			// clk now holds new boundary position. update exc slowly...
			// for now only use center transitions to adjust rcv clock

			if (pd_cw1 > 1) pd_dsr = 0;
			else			pd_dsr = 1;

			// was 0.02 ....
			pd_rer = pd_clk - exc;

			if (pd_dsr != pd_ldsr) exc = exc + rcv_clkt * (pd_rer);

			pd_ldsr = pd_dsr;
		}
		pd_i++;
		
		if (pd_i == bufsize) pd_i = 0;
	}

	if (pocbit)
	{
		pocbit--;
		if (pocbit == 0)				// we have just dropped out of POCSAG mode
		{
			display_showmo(MODE_IDLE);
			pocsag.frame(-1);			// reset pocsag routine
			ct_bit = ct1600;			// reset rcv clock to 1600 baud
			rcv_clkt = rcv_clkt_hi;		// widen up rcv clk again
		}
	}
}

// Save statistics.
void check_save_data(void)
{
	unsigned int index;
	char filename[1024];
	extern char szFilenameDate[16];		// Global buffer for date as filename
	bool bDaily=false;

	/*** Statistics output and statfile updates ***/

	stat_timer--;

	if (stat_timer == 0)
	{
		GetLocalTime(&statTime);
		Get_Date_Time();

		if (statTime.wDay  != prev_statTime.wDay) bDaily=true;
		if (statTime.wHour != prev_statTime.wHour)
		{
			if (Profile.stat_file_enabled)
			{
				if (Profile.stat_file_use_date)
				{
					CreateDateFilename(".st", bDaily ? &prev_statTime : NULL);
					sprintf(filename, "%s\\%s", szLogPathName, szFilenameDate);
				}
				else sprintf(filename, "%s\\%s.st", szLogPathName, Profile.stat_file);

				if ((pStatFile = fopen(filename, "a")) != NULL)
				{
					fprintf(pStatFile, "*************** HOURLY (%02d:00-%02d:00) ***************\n\n", prev_statTime.wHour, statTime.wHour);
					WriteStatFileHourly(pStatFile);

					if (bDaily)
					{
						fprintf(pStatFile, "*********************** DAILY **********************\n\n");

						WriteStatFileDaily(pStatFile);
					}
					fclose(pStatFile);
					pStatFile=NULL;
				}
			}
			for (index=0; index<NUM_STAT; index++)
			{
				hourly_stat[index][STAT_NUMERIC] = 0L;
				hourly_char[index][STAT_NUMERIC] = 0L;
				hourly_stat[index][STAT_ALPHA] = 0L;
				hourly_char[index][STAT_ALPHA] = 0L;
			}
			if (bDaily)
			{
				for (index=0; index<NUM_STAT; index++)
				{
					daily_stat[index][STAT_NUMERIC] = 0L;
					daily_char[index][STAT_NUMERIC] = 0L;
					daily_stat[index][STAT_ALPHA] = 0L;
					daily_char[index][STAT_ALPHA] = 0L;
				}
			}
			prev_statTime = statTime;
		}
		stat_timer = 100 - ((statTime.wSecond % 10) * 10);
	}
}
