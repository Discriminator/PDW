//
//  Mobitex.cpp
//
//
#ifndef STRICT
#define STRICT 1
#endif

#include <windows.h>

#include "headers\pdw.h"
#include "headers\initapp.h"
#include "headers\sound_in.h"
#include "headers\decode.h"
#include "headers\misc.h"
#include "headers\mobitex.h"
#include "headers\helper_funcs.h"

#define MB_MSG_SIZE			5000  // Max length of mobitex message
#define MAX_ERR				10    // was 15 - J.P

#define MPAK_CLASS_PSUBCOM	0
#define MPAK_CLASS_1		1	// ??
#define MPAK_CLASS_2		2	// ??
#define MPAK_CLASS_DTESERV	3

#define FRAMETYPE_MRM		0x01
#define FRAMETYPE_ACK		0x02
#define FRAMETYPE_NACK		0x03
#define FRAMETYPE_REB		0x04
#define FRAMETYPE_RES		0x05
#define FRAMETYPE_ABD		0x06
#define FRAMETYPE_ABL		0x07
#define FRAMETYPE_ATD		0x08
#define FRAMETYPE_ATL		0x09
#define FRAMETYPE_BKD		0x0C
#define FRAMETYPE_BKT		0x0D
#define FRAMETYPE_FRI		0x0E
#define FRAMETYPE_SVP		0x0F
#define FRAMETYPE_TST		0x10
#define FRAMETYPE_AKT		0x11
#define FRAMETYPE_NAT		0x12
#define FRAMETYPE_BBT		0x13

#define MPAKTYPE_TEXT		0x01
#define MPAKTYPE_DATA		0x02
#define MPAKTYPE_STATUS		0x03
#define MPAKTYPE_HPDATA		0x04

#define RX	0
#define TX	1


MOBITEX mb;

char obm[256];

int block[20];
//int blocks;

int frsyncs[18] =	{ 0xC4D7, 0xB433, 0xA2F7, 0xEB90, 0xEE16, 0x4BCC, 0x3B48, 0x5D08, 0x146F,
					  0x11E9, 0xEB23, 0xEF45, 0x09D7, 0x6877, 0x14DC, 0x10BA, 0xF628, 0x9788 };
int frevsyncs[18] = { 0x3B28, 0x4BCC, 0x5D08, 0x146F, 0x11E9, 0xB433, 0xC4B7, 0xA2F7, 0xEB90,
					  0xEE16, 0x14DC, 0x10BA, 0xF628, 0x9788, 0xEB23, 0xEF45, 0x09D7, 0x6877 };

// Variables for displaying message data.
char mb_msg_buf[MB_MSG_SIZE];
BYTE mb_msg_col[MB_MSG_SIZE];

int iBlockNumber, nCharacters=0;
unsigned int ESN_manufacturerscode, ESN_modelnumber, ESN_IDnumber;
bool bBadDestination=false, bBadSender=false, bBadHeader;
bool bFirstMPAK=true, bPrimaryBlock;
char szDestination[10], szSender[10], szType[10], szMPAK[32];
char szNeighbourList[MAX_STR_LEN], szBaseIdName[128]="";
char szLastBaseID[8], szTmpIdName[128];

extern char szWindowText[6][1000];

int aPreviousMAN[2][10];	// This array holds the previous RX & TX MAN's

char *MpakDteserv[32] = {	"UNKNOWN MPAK",			// 00
							"LOGIN REQUEST",		// 01
							"LOGIN GRANTED",		// 02
							"LOGIN REFUSED",		// 03
							"LOGOUT",				// 04
							"LOGOUT ORDER",			// 05
							"BORN",					// 06
							"ACTIVE",				// 07
							"INACTIVE",				// 08
							"DIE",					// 09
							"LIVE",					// 10
							"ROAMING ORDER",		// 11
							"ROAMING MESSAGE",		// 12
							"UNKNOWN MPAK",			// 13
							"UNKNOWN MPAK",			// 14
							"GROUPLIST",			// 15
							"FLEXREQ",				// 16
							"FLEXLIST",				// 17
							"INFOREQ",				// 18
							"INFO",					// 19
							"TIME",					// 20
							"AREALIST",				// 21
							"ESN REQUEST",			// 22
							"ESN INFO",				// 23
							"MODE",					// 24
							"UNKNOWN MPAK",			// 25
							"UNKNOWN MPAK",			// 26
							"UNKNOWN MPAK",			// 27
							"UNKNOWN MPAK",			// 28
							"UNKNOWN MPAK",			// 29
							"APPLICATION OPTIONS",	// 30
							"LOW POWER" };			// 31

char *FrameTypes[16] = {	"<???> ",	// 00
							"<MRM> ",	// 01
							"<ACK> ",	// 02
							"<NACK>",	// 03
							"<REB> ",	// 04
							"<RES> ",	// 05
							"<ABD> ",	// 06
							"<ABL> ",	// 07
							"<ATD> ",	// 08
							"<ATL> ",	// 09
							"<???> ",	// 10
							"<???> ",	// 11
							"<BKD> ",	// 12
							"<BKT> ",	// 13
							"<FRI> ",	// 14
							"<SVP> " };	// 15


//FILE *pTest=NULL;

MOBITEX::MOBITEX()	// Default Mobitex settings
{
	cfs = 1;				// system specific frame sync checking status
	frsync      = 0xEB90;	// system specific frame sync
	frsync_rev  = ~frsync;	// In case bit stream wrong way up.   
	bitsync     = 0xCCCC;	// bit sync: CCCC = Base, 3333 = Mobile.
	bitsync_rev = ~bitsync;	// In case bit stream wrong way up.

	bitrate = 8000;			// for getting baud rate from INI file.
	bitscr = 1;				// bit scrambling in use ?
	ramnet = 1;				// ramnet flag
	timer = 0;				// For restoring title bar msg/signal indicator.
	h3 = 0xEC;				// matrix variables for block encoding
	h2 = 0xD3;
	h1 = 0xBA;
	h0 = 0x75;
	min_msg_len = 5;		// Don't display message unless it's at least this length.
	show = 0x0F;
	nBitCount=0;
}

 
MOBITEX::~MOBITEX()
{
}

// Called if changing data mode or if switching between soundcard & serial port input
void MOBITEX::reset(void)
{
	nBitCount = 0;
	timer = 0;
}

// returns the number of ones in the byte passed to routine
int MOBITEX::mb_nOnes(int h)
{
	int nb=0;

	for (int i=0; i<8; i++)
	{
		if ((h & 0x01) == 1) nb++;
		h = h >> 1;
	}
	return(nb);
}

// returns number of ones in the integer passed to routine
int MOBITEX::mb_nOnes_int(int h)
{
	return(mb_nOnes(h) + mb_nOnes(h >> 8));
}


//***********************************************************************
//          RUN THROUGH BLOCK FORWARD ERROR CORRECTION CODE             *
//***********************************************************************
// PURPOSE:
//       does FEC on the 8 data bits and 4 FEC bits passed to it in goi
//       get_fec tries to check and correct errors... If the errors are
//       uncorrectable get_fec returns a 1; otherwise 0
//       but don't read too much into this - two thirds of the time
//       random crap going into this routine will not generate the
//       uncorrectable error signal... Please Rely on the CRC check
//
int MOBITEX::get_fec(int *goi)
{
	int dab=0, nb=0, bb=0;

	int uce=0;		// flag that indicates an uncorrectable error

	iUnfixedError = 0;		// flag that indicates an unfixed error.

	// calculate ecc bits from our current info bits
	dab = (*goi >> 4);
	nb  = (mb_nOnes(dab & h3) & 0x01) << 3;
	nb += (mb_nOnes(dab & h2) & 0x01) << 2;
	nb += (mb_nOnes(dab & h1) & 0x01) << 1;
	nb += (mb_nOnes(dab & h0) & 0x01);

	// get syndrome
	nb = nb ^ (*goi & 0x0F);

	if (mb_nOnes(nb) > 0) uce = 1;
	else uce = 0;

	// if syndrome is not equal to zero try to correct the bad bit
	if (mb_nOnes(nb) > 1)
	{
		if ((nb & 0x08) > 0) bb  = h3; else bb  = h3 ^ 0xff;
		if ((nb & 0x04) > 0) bb &= h2; else bb &= h2 ^ 0xff;
		if ((nb & 0x02) > 0) bb &= h1; else bb &= h1 ^ 0xff;
		if ((nb & 0x01) > 0) bb &= h0; else bb &= h0 ^ 0xff;

		// are we pointing to a single bit? if so we found it
		if (mb_nOnes(bb) == 1) *goi = *goi ^ (bb<<4);
		else
		{ 
			uce++;
			iUnfixedError = 1; // unfixed errors (used for COLOR_MB_BITERRORS)
		}
	}
	else *goi = *goi ^ nb;

	// single wrong bit in syndrome => error occured in FEC bits

	return(uce);
}

//************************************************************************
//               BIT SCRAMBLING SEQUENCE GENERATOR                       *
//************************************************************************
// following is the pseudo-random bit scrambling generator.
// It's the output of a 9 stage linear-feedback shift register with taps
// at position 5 and 9 XORed and fed back into the first stage.
// An input of less than zero resets the scrambler.
// Otherwise it returns one bit at a time each time it is called
//
int MOBITEX::mb_bs(int st)
{
	static int rs, ud;

	// leave if system isn't supposed to use bit scrambling 
	if (bitscr == 0) return(0);

	if (st < 0) rs = 0x1E;  // inputs <0 reset scrambler 
	else
	{
		if ((rs & 0x01) > 0) ud = 1;
		else ud = 0;

		if ((rs & 0x10) > 0) ud = ud ^ 0x01;
		rs = rs >> 1;

		if (ud > 0) rs ^= 0x100;
	}
	return(ud);
}


//***********************************************************************
//                       CRC GENERATING ROUTINE                         *
//***********************************************************************
// CRC generator - passing a -1 to this routine resets it, otherwise
// pass all 144 data bits in the mobitex data block to it (starting
// with byte 0, LSB bit). The returned value will then be the
// CRC value. Passing any other negative value just returns CRC.

unsigned int MOBITEX::mb_crc(signed int bit)
{
	short unsigned int sr = 0x00, cr;

	if (bit >= 0)
	{
		if (bit == 1) cr ^= sr;

		if (sr & 0x8000) sr = (sr << 1) ^ 0x0811;
		else sr = sr << 1;
	}
	else if (bit == -1)		// -1 resets the crc state
	{
		sr = 0xF8E7;
		cr = 0x2A5D;
	}
	CountBiterrors((cr == 0xCCCC) ? 0 : 1);

	return(cr);
}

/*
unsigned int MOBITEX::mb_crc_1200(unsigned char *input, unsigned int syndrome)
{
	unsigned int r=0;

	for (int i=0; i<6; i++)
	{
		unsigned char c = input[i];

		for (int nBit=0; nBit<8; nBit++)
		{
			r = r << 1;
			if (c & 0x80) r |= 0x1;
			if (r & 0x200000) r = (r&0x1FFFFF) ^ 0x3B9FB5;
			c = c << 1;
		}
	}
	// flush register
	for (int nBit=0; nBit<21; nBit++)
	{
		r = r << 1;
		if (r & 0x200000) r = (r & 0x1FFFFF) ^ 0x3B9FB5;
	}
	return (r & 0x1FFFFF) ^ syndrome;
}
*/

//*********************************************************************
//          PROCESS RECEIVED 240 BIT MOBITEX DATA BLOCK               *
//*********************************************************************
// process MOBITEX data block 
//
void MOBITEX::barfrog()
{
	int i,j,k,b,cb, nErrors=0;
	char c, blocks2resend[10];

	unsigned int crcc=0x0000;

	// process data block into 20 byte chunk stored in array 'block'
	mb_crc(-1);				// reset crc routine 

	for (i=0; i<20; i++)	// uninterleave the data into b (holds 8 data bits + 4 FEC bits) 
	{
		for (j=0, b=0; j<12; j++)
		{
			b = b << 1;
			k = (j*20) + i;
			if (obm[k] == 1) b ^= 0x01;
		}

		if (!((iBlockNumber+1 >= LINK_CONTROL_INFO.BlockLength) &&	// Check if we don't exceed the
			 (i >= LINK_CONTROL_INFO.BytesLastBlock)))				// number of bytes in this frame
		{
			if (get_fec(&b))	// run through error correction routine 
			{
				nErrors++;
				CountBiterrors(1);
			}
			else CountBiterrors(0);
		}

		// spit out data bits to CRC routine - LSB data bit first... 
		cb = b >> 4;

		if (i < 18)	// Mark bad bits, so they can be highlighted later.
		{
			blk_err[i] = iUnfixedError ? 1 : 0;

			for (j=0; j<8; j++)
			{
				if ((cb & 0x01) == 1) mb_crc(1);
				else                  mb_crc(0);

				cb = cb >> 1;
			}
		}
		else crcc = (crcc << 8) ^ (cb & 0xff);

		b = b >> 4;
		block[i] = b;
	}
	iBlockNumber++;

	bPrimaryBlock = (iBlockNumber == 1) ? true : false;	// Block #1 is the primary block

	// at this point array block holds the data; nErrors gives the number of errors detected
	// by the FEC code ('corrected' errors count as one, uncorrectable count as 2);
	// and crcc gives the received CRC code. We use this info to decide if we got a good block
	// if CRC is correct or nErrors < MAX_ERR we'll say it's a good block

	i = mb_crc(-2);
  
	if ((i == crcc) || (nErrors < MAX_ERR))
	{
		if (bPrimaryBlock)
		{
			GetLinkControlInfo();	// Get link control information

			if (LINK_CONTROL_INFO.FrameID == FRAMETYPE_MRM)	// MRM DATA
			{
				bBadHeader = GetMpakHeader();	// Get MPAK header, will return true if we
			}									// don't want to see this type of MPAK
			else if ((LINK_CONTROL_INFO.FrameID != FRAMETYPE_MRM) && (LINK_CONTROL_INFO.FrameID != FRAMETYPE_RES))
			{
				sprintf(szDestination, "%i", LINK_CONTROL_INFO.Destination);	// If no MRM or RES frame,
				strcpy(szSender , "NETWORK");							// get destination from frame header

				if (LINK_CONTROL_INFO.FrameID == FRAMETYPE_SVP)	// SWEEP frames
				{
					SWEEP.type = block[6];	// Get type of sweep frame

					if (LINK_CONTROL_INFO.BlockLength > 1)
					{
						SWEEP.fbi = block[9];	// Get Frequency Band Information
					}
					SWEEP.channellist = (LINK_CONTROL_INFO.BlockLength > 1) ? true : false;
				}
				memset(mb_msg_col, COLOR_MISC, sizeof(mb_msg_col));
			}
		}

		switch (LINK_CONTROL_INFO.FrameID)	// Handle the different frame types
		{
			case FRAMETYPE_RES :	// Handle <RES> frames

			if (!(mb.show & MOBITEX_VERBOSE)) return; // Don't show <RES> frames

			if (bPrimaryBlock)
			{
				strcpy(mb_msg_buf, "<RES> ");
				nCharacters=strlen(mb_msg_buf);
				memset(mb_msg_col, COLOR_MISC, nCharacters);
				sprintf(szDestination, "%07i", LINK_CONTROL_INFO.Destination);	// Get RX-MAN

				for (int i=0; i<10; i++)
				{
					if (aPreviousMAN[RX][i] = LINK_CONTROL_INFO.Destination)
					{
						sprintf(szSender, "%07i", aPreviousMAN[TX][i]);	// Get TX-MAN
						break;
					}
					strcpy(szSender, "UNKNOWN");	// If Sender not found, show "UNKNOWN"
				}
				break;	// Skip first, data starts in second block
			}

			// fall trough to display message

			case FRAMETYPE_MRM :	// Handle <MRM> frames

			for (i=0; i<18; i++)       // store each character in buffer.
			{
				if (nCharacters > (MB_MSG_SIZE-2)) break; // enough room in buffer?

				if (bPrimaryBlock)
				{
					i=17;	// Actual data starts at last byte of the primary block
				}

				if (!((iBlockNumber == LINK_CONTROL_INFO.BlockLength) &&
					  (i >= LINK_CONTROL_INFO.BytesLastBlock)))
				{
					c = block[i] & 0x7F;
					mb_msg_col[nCharacters] = blk_err[i] ? COLOR_BITERRORS : COLOR_MESSAGE;
					mb_msg_buf[nCharacters] = c;
					nCharacters++;
				}
			}
			break;

			case FRAMETYPE_REB :	// Handle <REB> frames

			if (!(mb.show & MOBITEX_VERBOSE)) return; // Don't show <REB> frames

			if (block[7])	// If the number of blocks != 0
			{
				strcpy(mb_msg_buf, "[Request to repeat block ");

				for (i=1, j=1; i<=block[7]; i*=2, j++)	// Loop until we reach the value
				{										// which we have found in byte-7
					if (block[7] & i)	// Check if current block (i) is in byte-7
					{
						sprintf(blocks2resend, "%s%02i", isdigit(mb_msg_buf[strlen(mb_msg_buf)-1]) ? "/" : "", j);
						strcat(mb_msg_buf, blocks2resend);
					}
				}
				strcat(mb_msg_buf, "]");
			}
			else strcpy(mb_msg_buf, "[Request to repeat block]");

			strcpy(szType, "<REB>");
			nCharacters=strlen(mb_msg_buf);

			break;

			case FRAMETYPE_SVP :	// Handle <SVP> frames

			if (SWEEP.type == 1)				// When receiving a SVP-1 frame, let's call
			{									// display_showmo() to update the WindowText
				display_showmo(MODE_MOBITEX);	// to clear rejected/blocked messages
			}

			if (!(mb.show & MOBITEX_SHOW_SWEEP)) return; // Don't show SWEEP FRAMES

			if (SWEEP.channellist && !bPrimaryBlock)
			{
				if ((iBlockNumber == 2) && !block[1])	// If we are in the second block in
				{										// which the second byte should be 0
					SWEEP.channels = block[0];			// The first byte tells the number of neighours
				}

				if (GetNeighbourChannels())	// true if the channellist has been received correctly
				{
					strcpy(mb_msg_buf, szNeighbourList);
					nCharacters=strlen(mb_msg_buf);
				}
			}
			else if (block[3] == 15)
			{
				if ((block[4] & 0x1F) == 24)
				{
					if (block[6] == 1)
					{
						sprintf(mb_msg_buf, "TxPower=%idB RssiProcedure=%s RssiPeriod=%ims TimeToNext=%i MaxRepetitionFactor=%i»BaseStatus=%i ScanTime=%.1fsec BadBase=%idBµV GoodBase=%idBµV BetterBase=%idBµV",
											block[7],		// TX-POWER
											block[8] ? "ContinousMode" : "FrameMode",	// RSSI_PROC
											block[9]*20,	// RSSI_PERIOD
											block[10],		// TIME_TO_NEXT
											block[11],		// MAX_REP
											block[12],		// BASE_STATUS
									(double)block[13]/10,	// SCAN_TIME
											block[14],		// BAD_BASE
											block[15],		// GOOD_BASE
											block[16]);		// BETTER_BASE
					}
					else if (block[6] == 6)
					{
						sprintf(mb_msg_buf, "CycleTime=%isec TimeToNext=%isec RssiPeriod=%i ?=%i EvaluateOthers=%iRSSIperiods»BaseStatus=%i Cycle=%i",
											block[7]/4,		// Cycle Time
											block[8]/4,		// TimeToNext
											block[9],		// RSSI_PERIOD
											block[10],		// ?
											block[11],		// Evaluate Others
											block[12],		// BASE_STATUS
											block[13]);		// SCAN TIME
					}
				}
				else if ((block[4] & 0x1F) == 23)
				{
					if (GetSlaveChannels())	// Will return true if the channellist
					{						// has been received correctly
						strcpy(mb_msg_buf, szNeighbourList);	// Put the neighbourlist
					}											// in the messagebuffer
				}
				nCharacters=strlen(mb_msg_buf);
			}

			if (bPrimaryBlock)	// Is this the first (primary) block?
			{
				sprintf(szType, "<SVP%i>", SWEEP.type);
				sprintf(szDestination, "       ");
				sprintf(szSender,     " %s  ", LINK_CONTROL_INFO.BaseID);
			}
			break;

			case FRAMETYPE_ACK	:
			case FRAMETYPE_NACK	:
			case FRAMETYPE_ABD	:
			case FRAMETYPE_ABL	:
			case FRAMETYPE_ATD	:
			case FRAMETYPE_ATL	:
			case FRAMETYPE_FRI	:

			if (!(mb.show & MOBITEX_VERBOSE)) return; // Don't show control messages

			strcpy(szType, FrameTypes[LINK_CONTROL_INFO.FrameID]);

			break;

			case FRAMETYPE_TST :

			if (!(mb.show & MOBITEX_VERBOSE)) return; // Don't show control messages

			strcpy(szType, "<TST> ");

			break;

			case FRAMETYPE_AKT :

			if (!(mb.show & MOBITEX_VERBOSE)) return; // Don't show control messages

			strcpy(szType, "<AKT> ");

			break;

			default :

			if (!(mb.show & MOBITEX_VERBOSE)) return; // Don't show unknown frames

			sprintf(mb_msg_buf, "<UNKNOWN FrameID=%i>", LINK_CONTROL_INFO.FrameID);
			sprintf(szType, "      ");
			nCharacters=strlen(mb_msg_buf);

			break;
		}
	}
	return;
}


//*******************************************************************
//              FRAME SYNCHRONIZATION OF RAW BIT STREAM             *
//*******************************************************************
// this routine tries to achieve frame sync up in the raw bit stream
//
void MOBITEX::frame_sync(int bit)
{
	static int s1=0x0000,s2=0x0000, bc=0, nu;
	int fec, og=0, fsb, fsbr;
//	int ControlByte1, ControlByte2, BaseID, AreaID, CFlags, Parity;
	int ControlByte1, ControlByte2;

	extern unsigned long hourly_stat[NUM_STAT][2];
	extern unsigned long hourly_char[NUM_STAT][2];
	extern unsigned long daily_stat[NUM_STAT][2];
	extern unsigned long daily_char[NUM_STAT][2];

	// is a bit counter for # of bits left to form into a data block
	if (nBitCount == 0)
	{
		// nBitCount = 0 so we aren't trying to process a data block;
		// instead try to sync up with incoming bit stream

		// keep sliding buffers up to date 
		s1 = s1 << 1;
		if ((s2 & 0x8000)) s1++;
		s2 = s2 << 1;
		if (bit) s2++;
/*
		// sliding window
		for (int i=0; i<blocks; i++)
		{
			block[i] <<= 1;
			block[i] += (block[i+1] & 0x80) ? 1 : 0;
		}
		// add new bit
		block[blocks] <<= 1;
		block[blocks] += bit;

		if (mb.bitrate == 8000)
		{
			fsb  = mb_nOnes_int((block[0] << 8 | block[1]) ^ bitsync);
			fsbr = mb_nOnes_int((block[0] << 8 | block[1]) ^ bitsync_rev);

			if (cfs)	// Checking frame sync?
			{
				fsb  += mb_nOnes_int((block[2] << 8 | block[3]) ^ frsync);
				fsbr += mb_nOnes_int((block[2] << 8 | block[3]) ^ frsync_rev);
				be = 2;	// was 4
			}

			if (fsb <= cfs)
			{
				if (!cfs)	// Not checking frame sync
				{
					for (int i=0; i<18; i++)
					{
						if (mb_nOnes_int((block[2] << 8 | block[3]) ^ frsyncs[i]) == 0) break;
					}
					if (i == 18) return;
				}
				live_frsync = ((block[2] << 8 | block[3]) & 0xFFFF);
				bc = 25;
				GetFrameHeader();
			}
			else if (fsbr == 0) // ....is signal reversed?
			{
				if (!cfs)	// Not checking frame sync
				{
					for (int i=0; i<18; i++)
					{
						if (mb_nOnes_int((block[2] << 8 | block[3]) ^ frevsyncs[i]) == 0) break;
					}
					if (i == 18) return;
				}
				InvertData();	// Invert receive polarity

				return;
			}
		}
		else
		{
			if (block[0] == 0xAA && block[1] == 0xCE && (block[2] >> 6) == 0)
			{
				GetFrameHeader();
				bc = 25;
			}
			else if (block[0] == 0x55 && block[1] == 0x31 && (block[2] >> 6) == 0x3)
			{
				InvertData();	// Invert receive polarity
				return;
			}
		}
*/
		if (bc == 0)	// Not synced, looking for ROSI header
		{
			fsb  = mb_nOnes_int(s1^bitsync);
			fsbr = mb_nOnes_int(s1^bitsync_rev);

			if (cfs)	// Checking frame sync?
			{
				fsb  += mb_nOnes_int(s2^frsync);
				fsbr += mb_nOnes_int(s2^frsync_rev);
			}

			// if first two integers match up within a bit or two then we've gotten frame sync
			if (fsb <= cfs)
			{
				if (!cfs)	// Not checking frame sync
				{
					for (int i=0; i<18; i++)
					{
						if (mb_nOnes_int(s2^frsyncs[i]) == 0) break;
						else if (i == 17) return;
					}
				}
				live_frsync = (s2 & 0xFFFF);
				bc = 25;
			}
			else if (fsbr == 0) // ....is signal reversed?
			{
				if (!cfs)	// Not checking frame sync
				{
					for (int i=0; i<18; i++)
					{
						if (mb_nOnes_int(s2^frevsyncs[i]) == 0) break;
						else if (i == 17) return;
					}
				}
				InvertData();	// Invert receive polarity

				live_frsync = (~s2 & 0xFFFF);
				bc = 25;
			}
		}

		// bc is a bit counter used to pick off the two status bytes and the FEC byte in the header.                                    
		if (bc > 0)
		{
			bc--;

			if (bc == 0)
			{
				// strip off control bytes, run them through FEC routine
				ControlByte1 =  (s1 & 0xFF) << 4;
				ControlByte1 += (s2 >> 4) & 0xF;
				ControlByte2 =  (s2 >> 4) & 0xFF0;
				ControlByte2 += (s2 & 0xF);

				fec = get_fec(&ControlByte1) + get_fec(&ControlByte2);

				CountBiterrors(fec);

				if (!timer)	// In case we lose Mobitex-sync, clear BaseID and Name to prevent
				{			// the wrong ID from being displayed when switching channels
					strcpy(LINK_CONTROL_INFO.BaseID, "?");
					strcpy(szBaseIdName, "");
				}

				if (!fec)	// Only if ZERO errors
				{
					FRAME_HEADER.BaseID = (s2 & 0xFC0000) >> 18;	// Get Base Identification
					FRAME_HEADER.AreaID = (s2 & 0x3F000)  >> 12;	// Get Area Identification
					FRAME_HEADER.CFlags = (s2 & 0xF00)    >> 8;		// Get Control Flags
					FRAME_HEADER.Parity = (s2 & 0xFF);				// Get Parity

					sprintf(LINK_CONTROL_INFO.BaseID, "%02X%02X", FRAME_HEADER.AreaID, FRAME_HEADER.BaseID);
				}

				if ((strcmp(LINK_CONTROL_INFO.BaseID, szLastBaseID) != 0) ||
					(!szBaseIdName[0]))	// Only if we detect an other BaseID
				{
					GetBaseID(LINK_CONTROL_INFO.BaseID, szBaseIdName);	// Get Base Name
					strcpy(szLastBaseID, LINK_CONTROL_INFO.BaseID);

					if (szBaseIdName[0]) sprintf(szTmpIdName, " (%s)", szBaseIdName);

					if (bitsync == 0xCCCC)
					{
						sprintf(szWindowText[3], "%d (base) - NetworkID: %04X - BaseID: %s%s", mb.bitrate, (cfs ? frsync : live_frsync), LINK_CONTROL_INFO.BaseID, szBaseIdName[0] ? szTmpIdName : "");
					}
					if (bitsync == 0x3333) sprintf(szWindowText[3], "%d (mobile)", mb.bitrate);

					display_showmo(MODE_MOBITEX);
				}
				timer = 10; // Lets stay in Mobitex mode for another second

				// if monitoring a RAM network and CFlags == 0 data block(s) follow
				// otherwise just be adventurous and see if we get a valid block
				if (ramnet)
				{
					if (!FRAME_HEADER.CFlags)
					{
						nBitCount = 1440;
					}
					else return;	// If CFlags != 0 , this frame should be empty
				}
				else nBitCount = 1440;

				MPAK_HEADER.Destination=0;
				MPAK_HEADER.Sender=0;
				bBadDestination=false;
				bBadSender=false;
				iBlockNumber=0;
				nCharacters=0;
				nu = 0;
				mb_bs(-1);
			}   // endof "if bc == 0"
		}   // endof "if bc > 0"
	}
	else
	{
		if (mb_bs(1) == 1) og = bit ^ 0x01;
		else og = bit;

		obm[nu++] = og;

		if (nu == 240)
		{
			nu = 0;

			barfrog();

			if (iBlockNumber >= LINK_CONTROL_INFO.BlockLength) nBitCount = 1;
			else nBitCount = 241;

//			if (barfrog() < MAX_ERR)
//			{
//				nBitCount = 241;
//			}
//			else nBitCount = 1;
		}

		// If end of signal display msg if it's at least 'n' characters long.
		nBitCount--;

		if (nBitCount == 0)
		{
			for (nu=nCharacters-1; nu>=0; nu--)
			{
				if ((mb_msg_buf[nu] < 32) || (mb_msg_buf[nu] > 126)) nCharacters--;
				else break;
			}

			if (!szMPAK[0] && (nCharacters < min_msg_len))
			{
				if (!(mb.show & MOBITEX_VERBOSE)) return;	// Message is too short
			}

			if (bBadHeader) return;							// See GetHeaderInfo()

			if ((MPAK_HEADER.Sender > 16777216) ||
				(MPAK_HEADER.Destination > 16777216))
			{
				return;	// If MAN is too high (errors)
			}

			if (strlen(szDestination) != strlen(szSender))
			{
				return;		// If MAN lengths are different	(errors)
			}

			// ********* Destination MAN ***********
			strcpy(Current_MSG[MSG_CAPCODE], szDestination);

			if (bBadDestination)	// Destination byte contains biterrors
			{
				messageitems_colors[MSG_CAPCODE] = COLOR_BITERRORS;
			}
			else messageitems_colors[MSG_CAPCODE] = COLOR_ADDRESS;

			// ************* Time/Date **************
			Get_Date_Time();
			strcpy(Current_MSG[MSG_TIME], szCurrentTime);
			strcpy(Current_MSG[MSG_DATE], szCurrentDate);
			messageitems_colors[MSG_TIME] = COLOR_TIMESTAMP;
			messageitems_colors[MSG_DATE] = COLOR_TIMESTAMP;

			// ********* Sender MAN *****************
			strcpy(Current_MSG[MSG_MODE], szSender);

			if (bBadSender)	// Sender byte contains biterrors
			{
				messageitems_colors[MSG_MODE] = COLOR_BITERRORS;
			}
			else messageitems_colors[MSG_MODE] = COLOR_MB_SENDER;

			// ********* Message TYPE ***************
			strcpy(Current_MSG[MSG_TYPE], szType);
			messageitems_colors[MSG_TYPE] = COLOR_MODETYPEBIT;

			// ********* Bitrate ********************
			sprintf(Current_MSG[MSG_BITRATE], "%d",bitrate);
			messageitems_colors[MSG_BITRATE] = COLOR_MODETYPEBIT;

			// ********* Message / MPAK *************

			if (szMPAK[0])	// Show MPAK
			{
				display_color(&Pane1, COLOR_MISC);
				display_show_str(&Pane1, szMPAK);
				strcpy(szMPAK, "");
			}
			else			// Show Message
			{
				hourly_stat[STAT_MOBITEX][STAT_ALPHA]++;
				daily_stat[STAT_MOBITEX][STAT_ALPHA]++;

				if ((nCharacters    == 18)  &&	// Coordinates
					(mb_msg_buf[0]  == '+') &&
					(mb_msg_buf[1]  == ' ') &&
					(mb_msg_buf[15] == ' ') &&
					(mb_msg_buf[16] ==  0 ) &&
					(mb_msg_buf[17] == '"'))
				{
					for (nu=2; nu<15; nu+=2)
					{
						if (mb_msg_buf[nu] < 32) mb_msg_buf[nu] += 90;
					}
				}

				for (nu=0; nu<nCharacters; nu++)
				{
					display_color(&Pane1, mb_msg_col[nu]);
					display_show_char(&Pane1, mb_msg_buf[nu]);
					hourly_char[STAT_MOBITEX][STAT_ALPHA]++;
					daily_char[STAT_MOBITEX][STAT_ALPHA]++;
				}
			}
			ShowMessage();   // Display Message.

			if (szWindowText[5][0]) display_showmo(MODE_MOBITEX);
		}
	}
}

/*
void MOBITEX::GetFrameHeader(void)
{
//	int ControlByte1, ControlByte2;
	static char szLastBaseID[8], szTmpIdName[128];

	if (mb.bitrate == 8000)
	{
		int fec = get_fec(&block[4]) + get_fec(&block[5]);

		CountBiterrors(fec);

		if (!timer)	// In case we lose Mobitex-sync, clear BaseID and Name to prevent
		{			// the wrong ID from being displayed when switching channels
			strcpy(LINK_CONTROL_INFO.BaseID, "?");
			strcpy(szBaseIdName, "");
		}

		if (!fec)	// Only if ZERO errors
		{
//			FRAME_HEADER.BaseID = (s2 & 0xFC0000) >> 18;	// Get Base Identification
//			FRAME_HEADER.AreaID = (s2 & 0x3F000)  >> 12;	// Get Area Identification
//			FRAME_HEADER.CFlags = (s2 & 0xF00)    >> 8;		// Get Control Flags
//			FRAME_HEADER.Parity = (s2 & 0xFF);				// Get Parity

			sprintf(LINK_CONTROL_INFO.BaseID, "%02X%02X", FRAME_HEADER.AreaID, FRAME_HEADER.BaseID);
		}
	}
	else
	{
		FRAME_HEADER.BaseID = (block[2] << 4) + (block[3] >> 4);
		FRAME_HEADER.CFlags = (block[3] & 0x3);

		sprintf(LINK_CONTROL_INFO.BaseID, "%03X", FRAME_HEADER.BaseID);
	}
	
	if ((strcmp(LINK_CONTROL_INFO.BaseID, szLastBaseID) != 0) ||
		(!szBaseIdName[0]))	// Only if we detect an other BaseID
	{
		GetBaseID(LINK_CONTROL_INFO.BaseID, szBaseIdName);	// Get Base Name
		strcpy(szLastBaseID, LINK_CONTROL_INFO.BaseID);

		if (szBaseIdName[0]) sprintf(szTmpIdName, " (%s)", szBaseIdName);

		if (bitsync == 0xCCCC)
		{
			sprintf(szWindowText[3], "%d (base) - NetworkID: %04X - BaseID: %s%s", mb.bitrate, (cfs ? frsync : live_frsync), LINK_CONTROL_INFO.BaseID, szBaseIdName[0] ? szTmpIdName : "");
		}
		if (bitsync == 0x3333) sprintf(szWindowText[3], "%d (mobile)", mb.bitrate);

		display_showmo(MODE_MOBITEX);
	}
	timer = 10; // Lets stay in Mobitex mode for another second

//	Update_RX_Quality();

	// if monitoring a RAM network and CFlags == 0 data block(s) follow
	// otherwise just be adventurous and see if we get a valid block
	if (ramnet)
	{
		if (!FRAME_HEADER.CFlags)
		{
			nBitCount = 1440;
		}
		else return;	// If CFlags != 0 , this frame should be empty
	}
	else nBitCount = 1440;

	MPAK_HEADER.Destination=0;
	MPAK_HEADER.Sender=0;
	bBadDestination=false;
	bBadSender=false;
	iBlockNumber=0;
	nCharacters=0;
//	nu = 0;
	mb_bs(-1);
}
*/

char *MOBITEX::GetBaseID(char *szBaseID, char *szBaseIdName)
{
	char *pSearch=NULL;
	char *pBase = szBaseIdName;
 	char szFilename[MAX_PATH], szLine[128];
	bool bFound=false;
	FILE *pFile;

	sprintf(szFilename, "%s\\base-ids.txt", szPath);

	pFile = fopen(szFilename ,"r");	// Check if filename exists

	if (pFile)
	{
		while (fgets(szLine, sizeof(szLine), pFile) != NULL)
		{
			szLine[strlen(szLine)-1] = '\0';

			if (szLine[0] == '#' || !szLine[3])	// If comment or line is too short
			{
				continue;	// Skip comment
			}
			if (strncmp((char*)szBaseID, &szLine[0], 4) == 0)	// Find BaseID
			{
				for (int j=5; szLine[j]!=0; j++)		// Found BaseID
				{
					*pBase++ = szLine[j];				// Add Base Name
 				}
				break;
			}
		}
		fclose(pFile);		// Close file
		pFile=NULL;
	}
	*pBase = '\0';
	return (szBaseIdName);	// Return Base Name
}


void MOBITEX::GetLinkControlInfo(void)
{
	LINK_CONTROL_INFO.Destination    =((block[0] << 16) | (block[1] << 8) | (block[2]));
	LINK_CONTROL_INFO.FrameID        = (block[3] & 0x1F);
	LINK_CONTROL_INFO.SeqNumber      = (block[4] & 0x0F);
	LINK_CONTROL_INFO.BlockLength    = (block[5] & 0xFF);
//	LINK_CONTROL_INFO.BlockLength    = block[5];

	if (blk_err[4])		// If the Bytes in last Block byte contains errors
	{					// set BytesLastBlock to 18 to read all bytes
		 LINK_CONTROL_INFO.BytesLastBlock = 18;
	}
	else LINK_CONTROL_INFO.BytesLastBlock = ((block[4] & 0xF0) >> 4) | ((block[3] & 0x20) >> 1);
}


bool MOBITEX::GetMpakHeader()
{
	int mpak_address1 = ((block[6] << 16) | (block[7]  << 8) | (block[8]));
	int mpak_address2 = ((block[9] << 16) | (block[10] << 8) | (block[11]));

	// Not really sure if I got this right, address1 should be the sender and address2
	// the addressee, but sometimes this seems to be the other way round,
	// so let's compare the Frame.destination with the MPAK addresses

	MPAK_HEADER.Destination = LINK_CONTROL_INFO.Destination;
	MPAK_HEADER.Sender      = (mpak_address1 == LINK_CONTROL_INFO.Destination) ? mpak_address2 : mpak_address1;

	for (int i=1, j=0; i<10; i++, j++)
	{
		aPreviousMAN[RX][i] = aPreviousMAN[RX][j];
		aPreviousMAN[TX][i] = aPreviousMAN[TX][j];
	}
	aPreviousMAN[RX][0] = MPAK_HEADER.Destination;
	aPreviousMAN[TX][0] = MPAK_HEADER.Sender;

	switch (MPAK_HEADER.Destination)
	{
		case 1 :	// This is the Network-MAN

		strcpy(szDestination, "NETWORK");

		break;

		case 7 :	// This is the All Users MAN

		strcpy(szDestination, "<-ALL->");

		break;

		default :	// Just display the MAN-number

		sprintf(szDestination, "%07i", MPAK_HEADER.Destination);

		break;
	}

	switch (MPAK_HEADER.Sender)
	{
		case 1 :	// This is the Network-MAN

		strcpy(szSender, "NETWORK");

		break;

		case 0 :	// ?????

		strcpy(szSender, "NETWORK");

		break;

		default :	// Just display the MAN-number

		sprintf(szSender, "%07i", MPAK_HEADER.Sender);

		break;
	}
	
	MPAK_HEADER.MpakType		= (block[13] & 0x1F);		// Get MPAK-Type
	MPAK_HEADER.MpakClass		= (block[13] & 0xC0) >> 6;	// Get MPAK-Class
	MPAK_HEADER.AddressList		= (block[12] & 0x04) >> 3;	// Get Address List
	MPAK_HEADER.HPID			=  block[17];				// Get Higher Protocol ID

	if (blk_err[6] || blk_err[7] || blk_err[8])
	{
		bBadDestination=true;	// Destination byte contains biterrors
	}
	if (blk_err[9] || blk_err[10] || blk_err[11])
	{
		bBadSender=true;		// Sender byte contains biterrors
	}

	if (MPAK_HEADER.MpakClass == MPAK_CLASS_PSUBCOM)
	{
		switch (MPAK_HEADER.MpakType)
		{
			case MPAKTYPE_TEXT :
			
			if (!(mb.show & MOBITEX_SHOW_TEXT)) return(true); // Don't show TEXT
			strcpy(szType," TEXT ");
			
			break;

			case MPAKTYPE_DATA :

			if (!(mb.show & MOBITEX_SHOW_DATA)) return(true); // Don't show DATA
			strcpy(szType," DATA ");

			break;

			case MPAKTYPE_STATUS :

//			if (!(mb.show & MOBITEX_SHOW_STATUS)) return(true); // Don't show STATUS
			strcpy(szType,"STATUS");

			break;

			case MPAKTYPE_HPDATA :

			if (!(mb.show & MOBITEX_SHOW_HPDATA)) return(true); // Don't show HPDATA
			strcpy(szType,"HPDATA");

			if ((mb.show & MOBITEX_SHOW_HPID))	// Show HPID:xxx before message
			{
				sprintf(mb_msg_buf, "[HPID:%i] ", MPAK_HEADER.HPID);
				nCharacters=strlen(mb_msg_buf);
				memset(mb_msg_col, COLOR_MISC, nCharacters);
			}
			break;

			default:
				strcpy(szType," ???? ");
			break;
		}
	}
	else if (MPAK_HEADER.MpakClass == MPAK_CLASS_DTESERV)
	{
		if (!(mb.show & MOBITEX_SHOW_MPAK)) return(true); // Don't show MPAK messages

		if (MPAK_HEADER.MpakType == 20) // TIME
		{
			if (blk_err[14] || blk_err[15] || blk_err[16])
			{
				return(true); // If errors in any of the time-bytes, don't show this MPAK
			}
			else
			{
				MPAK_HEADER.lTime	= (((unsigned char) block[14] << 16) |
									   ((unsigned char) block[15] << 8)  |
									   ((unsigned char) block[16]));
				MOBITEX_TIME.Hour	= (MPAK_HEADER.lTime % 1440) / 60;
				MOBITEX_TIME.Minute	= (MPAK_HEADER.lTime % 1440) % 60;
				MOBITEX_TIME.MD		=  MPAK_HEADER.lTime / 1440;
				MOBITEX_TIME.MT		= (4291+(10*(MOBITEX_TIME.MD-(36525*(((100*MOBITEX_TIME.MD)+30690)/36525))/100)))/10;
				MOBITEX_TIME.Year   = 1984+(100*MOBITEX_TIME.MD+30690) / 36525+MOBITEX_TIME.MT / 429;
				MOBITEX_TIME.Month  = (100*MOBITEX_TIME.MT)/3061-1-(12*(MOBITEX_TIME.MT/429));
				MOBITEX_TIME.Day	= MOBITEX_TIME.MT-(((100*MOBITEX_TIME.MT)/3061)*3061)/100;

				sprintf(szMPAK, "[TIME: %02i:%02i %02i-%02i-%i]", MOBITEX_TIME.Hour, MOBITEX_TIME.Minute, MOBITEX_TIME.Day, MOBITEX_TIME.Month, MOBITEX_TIME.Year);
			}
		}
		else if (MPAK_HEADER.MpakType == 6 ||	// BORN
				 MPAK_HEADER.MpakType == 7 ||	// ACTIVE
				 MPAK_HEADER.MpakType == 12 ||	// ROAM
				 MPAK_HEADER.MpakType == 23 ||	// ESN INFO
				 MPAK_HEADER.MpakType == 31)	// LOW POWER
		{
			ESN_manufacturerscode	= block[14];
				
			ESN_modelnumber			= (block[15] & 0xFC) >> 2;
				
			ESN_IDnumber			= (((block[15] & 0x03) << 16) |
									    (block[16] << 8) |
									    (block[17]));

			if (MPAK_HEADER.MpakType == 31)	// LOW POWER
			{
				int subtype = (block[12] & 0x0F);
				sprintf(szMPAK, "[%s (%s) - ESN: %i.%i.%i]", MpakDteserv[MPAK_HEADER.MpakType], MpakDteserv[subtype], ESN_manufacturerscode, ESN_modelnumber, ESN_IDnumber);
			}
			else sprintf(szMPAK, "[%s - ESN: %i.%i.%i]", MpakDteserv[MPAK_HEADER.MpakType], ESN_manufacturerscode, ESN_modelnumber, ESN_IDnumber);
		}
		else sprintf(szMPAK, "[%s]", MpakDteserv[MPAK_HEADER.MpakType]);

		strcpy(szType," MPAK ");
	}
	else strcpy(szType," ???? ");	// Default / unknown type

	return(false);
}


bool MOBITEX::GetNeighbourChannels()
{
	int pos, j;
	static int count, neighbours[32];
	char szChannel[64];

	if (iBlockNumber == 2)
	{
		memset(neighbours, 0, sizeof(neighbours));
		count=0;
	}

	for (int i = !block[1] ? 2 : 0; i<18; i+=2, count++)
	{
		neighbours[count] = ((block[i] << 8) | block[i+1]);

		if (blk_err[i] || blk_err[i+1])
		{
			neighbours[count] += 0x8000;	// Biterror(s), mark as bad
		}

		if (count == (SWEEP.channels*2))
		{
			break;	// We have reached the maximum neighbours for this base
		}
	}

	if (count == (SWEEP.channels*2))
	{
		if (szBaseIdName[0]) sprintf(szNeighbourList, "[LIST OF NEIGHBOUR CHANNELS BaseID:%s - %s]»", LINK_CONTROL_INFO.BaseID, szBaseIdName);
		else sprintf(szNeighbourList, "[LIST OF NEIGHBOUR CHANNELS BaseID:%s]»", LINK_CONTROL_INFO.BaseID);

		for (count=0; count/2 < SWEEP.channels; count+=2)
		{
			switch (SWEEP.fbi)	// Show frequency depending on the Frequency Band Idenfication
			{
				case 0 :	// 800 MHz
					
				sprintf(szChannel, "Neighbour-%02i : %.5f MHz / %.5f MHz»", (count+2)/2, (double)819+((neighbours[count+1] & 0x7FFF)*0.00625), (double)819+((neighbours[count] & 0x7FFF)*0.00625));

				break;

//				case 3 :	// ? MHz
//					
//				sprintf(szChannel, "Neighbour-%02i : %.4f MHz / %.4f MHz»", (count+2)/2, (double)890+((neighbours[count+1] & 0x7FFF)*0.0125), (double)890+((neighbours[count] & 0x7FFF)*0.0125));
//
//				break;

				case 4 :	// 900 MHz
					
				sprintf(szChannel, "Neighbour-%02i : %.4f MHz / %.4f MHz»", (count+2)/2, (double)890+((neighbours[count+1] & 0x7FFF)*0.0125), (double)890+((neighbours[count] & 0x7FFF)*0.0125));

				break;

				case 3 :	// ? MHz
				case 5 :	// 400 MHz
					
				sprintf(szChannel, "Neighbour-%02i : %.4f MHz / %.4f MHz»", (count+2)/2, (double)380+((neighbours[count+1] & 0x7FFF)*0.0125), (double)380+((neighbours[count] & 0x7FFF)*0.0125));

				break;

				default :	// Unknown FBI

				sprintf(szNeighbourList, "[UKNOWN FBI(%i)]", SWEEP.fbi);
				return(true);

				break;
			}
			strcat(szNeighbourList, szChannel);

			if (neighbours[count+1] & 0x8000)
			{
				for (j=strlen(szNeighbourList)-1; j>strlen(szNeighbourList)-15; j--)
				{
					mb_msg_col[j] = COLOR_BITERRORS;
				}
			}
			if (neighbours[count] & 0x8000)
			{
				pos = strrchr(szNeighbourList, '/') - szNeighbourList;
				for (j=pos-2; j>pos-15; j--)
				{
					mb_msg_col[j] = COLOR_BITERRORS;
				}
			}
		}
		return(true);
	}
	return(false);
}


bool MOBITEX::GetSlaveChannels()
{
	int pos, j, fbi;
	static int count, neighbours[32];
	char szChannel[64];

	memset(neighbours, 0, sizeof(neighbours));
	count=0;

	for (int i=8; i<18; i+=2, count++)
	{
		neighbours[count] = (((block[i] & 0x1F) << 8) | block[i+1]);

		if (blk_err[i] || blk_err[i+1])
		{
			neighbours[count] += 0x8000;	// Biterror(s), mark as bad
		}

		if (count == (block[7] * 2))
		{
			break;	// We have reached the maximum neighbours for this base
		}
		fbi = (block[i] & 0xE0) >> 5;	// Get Frequency Band Information
	}

	if (count == (block[7] * 2))
	{
		if (szBaseIdName[0]) sprintf(szNeighbourList, "[LIST OF SLAVE CHANNELS for BaseID:%s - %s]»", LINK_CONTROL_INFO.BaseID, szBaseIdName);
		else sprintf(szNeighbourList, "[LIST OF SLAVE CHANNELS for BaseID:%s]»", LINK_CONTROL_INFO.BaseID);

		for (count=0; count/2 < block[7]; count+=2)
		{
			switch (fbi)	// Show frequency depending on the Frequency Band Idenfication
			{
				case 0 :	// 800 MHz
					
				sprintf(szChannel, "Slave-%02i : %.5f MHz / %.5f MHz»", (count+2)/2, (double)819+((neighbours[count+1] & 0x7FFF)*0.00625), (double)819+((neighbours[count] & 0x7FFF)*0.00625));

				break;

				case 4 :	// 900 MHz
					
				sprintf(szChannel, "Slave-%02i : %.4f MHz / %.4f MHz»", (count+2)/2, (double)890+((neighbours[count+1] & 0x7FFF)*0.0125), (double)890+((neighbours[count] & 0x7FFF)*0.0125));

				break;

				case 5 :	// 400 MHz
					
				sprintf(szChannel, "Slave-%02i : %.4f MHz / %.4f MHz»", (count+2)/2, (double)380+((neighbours[count+1] & 0x7FFF)*0.0125), (double)380+((neighbours[count] & 0x7FFF)*0.0125));

				break;

				default :	// Unknown FBI

				sprintf(szNeighbourList, "[UKNOWN FBI(%i)]", fbi);
				return(true);

				break;
			}
			strcat(szNeighbourList, szChannel);

			if (neighbours[count+1] & 0x8000)
			{
				for (j=strlen(szNeighbourList)-1; j>strlen(szNeighbourList)-15; j--)
				{
					mb_msg_col[j] = COLOR_BITERRORS;
				}
			}
			if (neighbours[count] & 0x8000)
			{
				pos = strrchr(szNeighbourList, '/') - szNeighbourList;
				for (j=pos-2; j>pos-15; j--)
				{
					mb_msg_col[j] = COLOR_BITERRORS;
				}
			}
		}
		return(true);
	}
	return(false);
}

#define ASCII(x) (((x)<0x20)?'.':(x))

/*

0                1-2                 2-3             3
bitsync 8 bits | framesync 10 bits | basID 10 bits | flags 4 bits |

4-6           7              7                  8            9
MAN 24 bits | flags 3 bits | frametype 5 bits | seq 8 bits | len 8 bits

//

void MOBITEX::frame1200(int bit)
{
	int i;

    // sliding window
	for (i = 0; i < 12; ++i)
	{
		block1200[i] <<= 1;
		block1200[i] += (block1200[i+1] & 0x80)?1:0;
	}
	// add new bit
    block1200[12] <<= 1;
	block1200[12] += bit;

	// check for bitsync/framesync sequence (inverted for mob)
	// bitsync == 10101010 && framesync == 1100111000 
	if (cnt==0 && (block1200[0]==0xAA && block1200[1]==0xCE && (block1200[2]>>6)==0))
	{
		unsigned int dstMAN = (block1200[4]<<16) + (block1200[5]<<8) + block1200[6];
//		baseID = (block1200[2]<<4) + (block1200[3]>>4);
		sprintf(LINK_CONTROL_INFO.BaseID, "%04X", (block1200[2]<<4) + (block1200[3]>>4));
		crcBias = (block1200[0]==0xAA) ? baseID : (dstMAN>>3);  

		live_frsync = (block1200[1] + (block1200[2]>>6));

		if ((strcmp(LINK_CONTROL_INFO.BaseID, szLastBaseID) != 0) ||
			(!szBaseIdName[0]))	// Only if we detect an other BaseID
		{
			GetBaseID(LINK_CONTROL_INFO.BaseID, szBaseIdName);	// Get Base Name
			strcpy(szLastBaseID, LINK_CONTROL_INFO.BaseID);

			if (szBaseIdName[0]) sprintf(szTmpIdName, " (%s)", szBaseIdName);

			if (bitsync == 0xCCCC)
			{
				sprintf(szWindowText[3], "%d (base) - NetworkID: %04X - BaseID: %s%s", mb.bitrate, live_frsync, LINK_CONTROL_INFO.BaseID, szBaseIdName[0] ? szTmpIdName : "");
			}
			if (bitsync == 0x3333) sprintf(szWindowText[3], "%d (mobile)", mb.bitrate);

			display_showmo(MODE_MOBITEX);
		}
		// check flags on ROSI header, bits 1 ad 2 indicate no following frame
		if ((block1200[3]&0x3) != 0 && (block1200[0]==0x55 || block1200[0]==0xAA))
		{
			//fprintf(stdout, "%s:%0.4u f=%.1X", (baseID == crcBias)?"BAS":"MOB", baseID, block1200[3]&0xf);
			//fprintf(stdout, "%0.2X%0.2X %0.2X%0.2X %0.2X%0.2X %0.2X%0.2X", block1200[4], block1200[5], block1200[6], block1200[7], block1200[8], block1200[9], block1200[10], block1200[11]);
		}
		else // this is a ROSI frame
		{
			frametag = 0;

			switch (block1200[7]&0x1f)
			{
				case 0x01:	// MRM  Paketdata
					if (!frametag)
						frametag = "MRM";
				case 0x02:	// ACK  Bekräftelse
					if (!frametag)
						frametag = "ACK";
				case 0x03:	// NACK Negativ Bekräftelse
					if (!frametag)
						frametag = "NACK";
				case 0x04:	// REB  Begärand omsändning
					if (!frametag)
						frametag = "REB";
				case 0x05:	// RES  Omsändning
					if (!frametag)
						frametag = "RES";
				case 0x06:	// ABD  Accessbegäran Data
					if (!frametag)
						frametag = "ABD";
				case 0x07:	// ABT  Accessbegäran Tal
					if (!frametag)
						frametag = "ABT";
				case 0x08:	// ABL  Accessbegäran Larm
					if (!frametag)
						frametag = "ABL";
				case 0x09:	// ATD  Accesstillstånd Data
					if (!frametag)
						frametag = "ATD";
				case 0x0a:	// ATT  Accesstillstånd Tal
					if (!frametag)
						frametag = "ATT";
				case 0x0b:	// ATL  Accesstillstånd Larm  
					if (!frametag)
						frametag = "ATL";
				case 0x0c:	// BKD  Byt Kanal Data
					if (!frametag)
						frametag = "BKD";
				case 0x0d:	// BKT  Byt Kanal Tal
					if (!frametag)
						frametag = "BKT";
				case 0x0e:	// FRI  Fri
					if (!frametag)
						frametag = "FRI";
				case 0x0f:	// SVP  Svep
					if (!frametag)
						frametag = "SVP";
				case 0x10:	// TST  Tyst
					if (!frametag)
						frametag = "TST";
				case 0x11:	// AKT  Aktivitetsfråga
					if (!frametag)
						frametag = "AKT";
				case 0x12:	// NAT  Negativt Accesstillstånd Tal
					if (!frametag)
						frametag = "NAT";
				case 0x13:	// BBT  Byt Bas Tal
					if (!frametag)
						frametag = "BBT";
			};

			if (frametag != "FRI" && frametag != "SVP" && frametag != "ACK" && frametag != "NACK")
			{
			// print frameheader

			if ((strcmp(LINK_CONTROL_INFO.BaseID, szLastBaseID) != 0) ||
				(!szBaseIdName[0]))	// Only if we detect an other BaseID
			{
				GetBaseID(LINK_CONTROL_INFO.BaseID, szBaseIdName);	// Get Base Name
				strcpy(szLastBaseID, LINK_CONTROL_INFO.BaseID);

				if (szBaseIdName[0]) sprintf(szTmpIdName, " (%s)", szBaseIdName);

				if (bitsync == 0xCCCC)
				{
					sprintf(szWindowText[3], "%d (base) - NetworkID: %04X - BaseID: %s%s", mb.bitrate, (cfs ? frsync : live_frsync), LINK_CONTROL_INFO.BaseID, szBaseIdName[0] ? szTmpIdName : "");
				}
				if (bitsync == 0x3333) sprintf(szWindowText[3], "%d (mobile)", mb.bitrate);

				display_showmo(MODE_MOBITEX);
			}
			if (frametag)
				fprintf(stdout, "%s:%0.4u f=%.1X (%0.6X) %0.1X/<%s> seq=%0.1X", (baseID == crcBias)?"BAS":"MOB", baseID,  block1200[3]&0xf, dstMAN, (block1200[7]>>5)&0x07, frametag, block1200[8]&0xf);
			else
				fprintf(stdout, "%s:%0.4u f=%.1X (%0.6X) %0.1X/<%0.2X> seq=%0.1X l=%.3u (%u)", (baseID == crcBias)?"BAS":"MOB", baseID,  block1200[3]&0xf, dstMAN, (block1200[7]>>5)&0x07, block1200[7]&0x1f, block1200[8]&0xf, block1200[9], block1200[8]>>4);

			bytesInLastBlock = block1200[8]>>4;
			if (0 != mb_crc_1200(block1200+4, (block1200[10]<<13) + (block1200[11]<<5) + (block1200[12]>>3)))
			{
				//fprintf(stdout, "\x1B[31m\tCRC failed!\x1B[0m");
				CONSOLE_SCREEN_BUFFER_INFO csbi;
				HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
				GetConsoleScreenBufferInfo(hConsole, &csbi);
				SetConsoleTextAttribute(hConsole, FOREGROUND_RED | FOREGROUND_INTENSITY);
				fprintf(stdout, "\tCRC failed!");
				SetConsoleTextAttribute(hConsole, csbi.wAttributes);
			}
			else
			{
				if (frametag == "RES")
					fprintf(stdout, "\tData blocks:\n\t\t\t\t\t\t");

				// advance to next block (if any)
				if (block1200[9] > 1)
				{
					blocksLeft = block1200[9] - 1;
					cnt = 80 + 21;
					return;
				}
			}
			}
		}
	}
	else if (cnt==0 && (block1200[0]==0x55 && block1200[1]==0x31 && (block1200[2]>>6)==0x3))
	{
		InvertData();	// Invert receive polarity
	}

	// if we have a block ready in the buffer
	if (blocksLeft > 0 && --cnt == 0)
	{
		bool crcOK = (crcBias == mb_crc_1200(block1200, (block1200[6]<<13) + (block1200[7]<<5) + (block1200[8]>>3)));
		CONSOLE_SCREEN_BUFFER_INFO csbi;
		HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
		GetConsoleScreenBufferInfo(hConsole, &csbi);
		if (!crcOK)
		{
			//fprintf(stdout, "\x1B[31m");
			SetConsoleTextAttribute(hConsole, FOREGROUND_RED | FOREGROUND_INTENSITY);
		}
	
		if (frametag == "MRM")
		{
			unsigned int srcMAN = (block1200[0]<<16) + (block1200[1]<<8) + block1200[2];
			unsigned int dstMAN = (block1200[3]<<16) + (block1200[4]<<8) + block1200[5];

			fprintf(stdout, "\tMPAK: From: %u To: %u", srcMAN, dstMAN);
			frametag = "MRMhdr";
		}
		else if (frametag == "MRMhdr")
		{
			frametag = 0;
			fprintf(stdout, "\n\t\t\t\t\t\t");

			// MPAK ident
			char* formatString = 0;
			switch (block1200[1])
			{
			// PSUBCOMS   TODO: Handle address lists
			case 0x01:	// TEXT
				fprintf(stdout, "%0.2X/[TEXT] t=%0.2X%0.2X%0.2X\n\t\t\t\t\t\t%c", block1200[0], block1200[2], block1200[3], block1200[4], ASCII(block1200[5]));
				frametag = "TEXT";
				break;
			case 0x02:	// DATA
				fprintf(stdout, "%0.2X/[DATA] t=%0.2X%0.2X%0.2X Data=%0.2X    %c", block1200[0], block1200[2], block1200[3], block1200[4], block1200[5], ASCII(block1200[5]));
				break;
			case 0x03:	// STATUS
				formatString = "%0.2X/[STATUS] t=%0.2X%0.2X%0.2X Code=%0.3u";
				break;
			case 0x04:	// HPDATA
				{
					// lookup HPID
					unsigned char hpid = block1200[3];
					char *protoName = 0;
					switch (hpid)
					{
					case 1:
						protoName = "Gateway";
						break;
					case 2:
						protoName = "OVLS";
						break;
					case 3:
						protoName = "MTP/1";
						break;
					case 4:
						protoName = "Two-Way Messaging";
						break;
					case 5:
						protoName = "TCP/IP Packet Driver";
						break;
					case 10:
						protoName = "Interactive Paging";
						break;
					case 11:
						protoName = "WAP/WDP";
						break;
					case 12:
						protoName = "WAP/WCMP";
						break;
					case 97:
						protoName = "X.25 Interface Transport";
						break;
					case 98:
						protoName = "Reliable Messaging";
						break;
					case 127:
						protoName = "RNMP";
						break;
					default:
						if (!protoName)
						{
							if (hpid < 128)
								protoName = "MOA unassigned";
							else if (hpid > 199 && hpid < 221)
								protoName = "operator specific";
							else
								protoName = "user specific";
						}
					}
					fprintf(stdout, "%0.2X/[HPDATA] t=%0.2X%0.2X%0.2X HPID=%0.3u (%s)", block1200[0], block1200[2], block1200[3], block1200[4], block1200[5], protoName);
					break;
				}
			case 0x21:	// EXTPAK
				formatString = "%0.2X/[EXTPAK] t=%0.2X%0.2X%0.2X %0.2X";
				break;

			// DTESERVS
			case 0xC1:	// LOGINREQ
				formatString = "%0.2X/[LOGINREQ] (%0.2X%0.2X%0.2X) Password=%c";
				break;
			case 0xC2:	// LOGINGRA
				formatString = "%0.2X/[LOGINGRA] (%0.2X%0.2X%0.2X) %0.2X";
				break;
			case 0xC3:	// LOGINREF
				formatString = "%0.2X/[LOGINREF] (%0.2X%0.2X%0.2X) %0.2X";
				break;
			case 0xC4:	// LOGOUT
				formatString = "%0.2X/[LOGINOUT] (%0.2X%0.2X%0.2X) %0.2X";
				break;
			case 0xC5:	// LOGOUTORD
				formatString = "%0.2X/[LOGINOUTORD] (%0.2X%0.2X%0.2X) %0.2X";
				break;
			case 0xC6:	// BORN
				formatString = "%0.2X/[BORN] ESN=%0.2X%0.2X%0.2X%0.2X";
				break;
			case 0xC7:	// ACTIVE
				formatString = "%0.2X/[ACTIVE] ESN=%0.2X%0.2X%0.2X%0.2X";
				break;
			case 0xC8:	// INACTIVE
				formatString = "%0.2X/[INACTIVE] %0.2X%0.2X%0.2X%0.2X";
				break;
			case 0xC9:	// DIE
				formatString = "%0.2X/[DIE] %0.2X%0.2X%0.2X%0.2X";
				break;
			case 0xCA:	// LIVE
				formatString = "%0.2X/[LIVE] %0.2X%0.2X%0.2X%0.2X";
				break;
			case 0xCB:	// ROAMORD
				formatString = "%0.2X/[ROAMORD] %0.2X%0.2X%0.2X%0.2X";
				break;
			case 0xCC:	// ROAM
				formatString = "%0.2X/[ROAM] ESN=%0.2X%0.2X%0.2X%0.2X";
				break;
			case 0xCF:	// GROUPLIST
				formatString = "%0.2X/[GROUPLIST] n=%0.3u (%0.2X%0.2X%0.2X)";
				break;
			case 0xD0:	// FLEXREQ
				formatString = "%0.2X/[FLEXREQ] %0.2X%0.2X%0.2X%0.2X";
				break;
			case 0xD1:	// FLEXLIST
				formatString = "%0.2X/[FLEXLIST] n=%0.3u (%0.2X%0.2X%0.2X)";
				break;
			case 0xD2:	// INFOREQ
				formatString = "%0.2X/[INFOREQ] %0.2X%0.2X%0.2X%0.2X";
				break;
			case 0xD3:	// INFO
				formatString = "%0.2X/[INFO] n=%0.3u (%0.2X%0.2X%0.2X)";
				break;
			case 0xD4:	// TIME
				formatString = "%0.2X/[TIME] t=%0.2X%0.2X%0.2X %0.2X";
				break;
			case 0xD5:	// AREALIST
				formatString = "%0.2X/[AREALIST] %0.2X%0.2X%0.2X%0.2X";
				break;
			case 0xD6:	// ESNREQ
				formatString = "%0.2X/[ESNREQ] %0.2X%0.2X%0.2X%0.2X";
				break;
			case 0xD7:	// ESNINFO
				formatString = "%0.2X/[ESNINFO] ESN=%0.2X%0.2X%0.2X%0.2X";
				break;
			case 0xD8:	// MODE
				formatString = "%0.2X/[MODE] ModeID=%0.3u %0.2X%0.2X%0.2X";
				break;

			default:
				fprintf(stdout, "%0.2X/[%0.2X] %0.2X %0.2X %0.2X %0.2X", block1200[0], block1200[1], block1200[2], block1200[3], block1200[4], block1200[5]);
				fprintf(stdout, "    %c%c%c%c", ASCII(block1200[2]), ASCII(block1200[3]), ASCII(block1200[4]), ASCII(block1200[5]));
			}
			if (formatString)
				fprintf(stdout, formatString, block1200[0], block1200[2], block1200[3], block1200[4], block1200[5]);
		}
		else if (frametag == "SVP" && block1200[0] == 0x02)
		{
			fprintf(stdout, "\t<SVP2> %0.2X System Channel: %.4f / %.4f", block1200[1], (float)(((block1200[2]<<8)+block1200[3])*0.0125)+68, (float)(((block1200[4]<<8)+block1200[5])*0.0125)+68);
			frametag = 0;
		}
		else if (frametag && !strncmp(frametag, "AT", 2))
		{
			fprintf(stdout, "\t%0.2X %0.2X Channel: %.4f / %.4f", block1200[0], block1200[1], (float)(((block1200[2]<<8)+block1200[3])*0.0125)+68, (float)(((block1200[4]<<8)+block1200[5])*0.0125)+68);
			frametag = 0;
		}
		else if (frametag && !strncmp(frametag, "BK", 2))
		{
			fprintf(stdout, "\t%0.2X %0.2X Channel: %.4f / %.4f", block1200[0], block1200[1], (float)(((block1200[2]<<8)+block1200[3])*0.0125)+68, (float)(((block1200[4]<<8)+block1200[5])*0.0125)+68);
			frametag = 0;
		}
		else if (frametag == "TEXT")
		{
			unsigned char lastChar = (blocksLeft == 1) ? bytesInLastBlock : 6;

			for (i = 0; i < lastChar; ++i)
			{
				if (block1200[i] == 0xd || block1200[i] == 0xa)
					fprintf(stdout, "%c\t\t\t\t\t\t", block1200[i]);
				else
					fprintf(stdout, "%c", block1200[i]);
			}
		}
		else
		{
			// dump block data
			if (frametag)
				fprintf(stdout, "\t%0.2X%0.2X%0.2X%0.2X%0.2X%0.2X\t%c%c%c%c%c%c", block1200[0], block1200[1], block1200[2], block1200[3], block1200[4], block1200[5], ASCII(block1200[0]), ASCII(block1200[1]), ASCII(block1200[2]), ASCII(block1200[3]), ASCII(block1200[4]), ASCII(block1200[5]));
			else
			{
				fprintf(stdout, "\n\t\t\t\t\t\t");
				if (blocksLeft == 1)
				{
					for (i = 0; i < bytesInLastBlock; ++i)
						fprintf(stdout, "%0.2X", block1200[i]);
					fprintf(stdout, "\t");
					if (bytesInLastBlock < 4)
						fprintf(stdout, "\t");
					for (i = 0; i < bytesInLastBlock; ++i)
						fprintf(stdout, "%c", ASCII(block1200[i]));
				}
				else
					fprintf(stdout, "%0.2X%0.2X%0.2X%0.2X%0.2X%0.2X\t%c%c%c%c%c%c", block1200[0], block1200[1], block1200[2], block1200[3], block1200[4], block1200[5], ASCII(block1200[0]), ASCII(block1200[1]), ASCII(block1200[2]), ASCII(block1200[3]), ASCII(block1200[4]), ASCII(block1200[5]));
			}
			frametag = 0;
		}

		if (!crcOK)
		{
			//fprintf(stdout, " CRC failed!\x1B[0m");
			if (frametag != "TEXT")
				fprintf(stdout, " CRC failed!");
			SetConsoleTextAttribute(hConsole, csbi.wAttributes);
		}

		// advance to next block
		if (--blocksLeft > 0)
			cnt = 48 + 21;
		else
			// flush stdout after last block
			fflush(stdout);
	}
}
*/