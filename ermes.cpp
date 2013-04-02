//
// ERMES.CPP
//
//
#ifndef STRICT
#define STRICT 1
#endif

#include <windows.h>
#include <commctrl.h>
#include <mmsystem.h>
#include <stdio.h>
#include <commdlg.h>
#include <string.h>
#include <ctype.h>
#include <time.h>

#include "headers\pdw.h"
#include "headers\initapp.h"
#include "headers\gfx.h"
#include "headers\decode.h"
#include "headers\misc.h"
#include "headers\acars.h"
#include "headers\mobitex.h"
#include "headers\ermes.h"
#include "headers\helper_funcs.h"

#define min(a,b) (((a) < (b)) ? (a) : (b))
#define max(a,b) (((a) > (b)) ? (a) : (b))

#define	APT		0x09C461C9l	// address field terminating word.
#define MDEL	0x3777E7ABl	// message delimiter.
#define SYNC32	0x2288282a	// 32 bit sync (30bit sync word + 2 LSB preamble bits)

// display raw none interleaved or interleaved codewords?
//#define DEBUG_NONE_INTERLEAVED 1
//#define DEBUG_INTERLEAVED 1

//#define EM_DEBUG 1
//#define DB_MESSAGE_HEADER  1

//#define DB_LOG_RAWDATA 1

ERMES em;

// error detection / correction matrix
int gmat[18]={	0xF08, 0x784, 0x3C2, 0x1E1, 0xD96, 0x6CB, 0xE03, 0xA67, 0x855,
				0x94C, 0x4A6, 0x253, 0xC4F, 0xB41, 0x8C6, 0x463, 0xF57, 0xACD };

char numformat[17]={"0123456789/ U-.%"};					// contains numeric paging data format
char alpformat[33]={"@£$•ËÈ˘ÏÚ« ÿ¯ ≈Â            ∆Êﬂ…"};	// contains alphanumeric paging data format


// Default ERMES settings
ERMES::ERMES(void)
{
	// timer runs as long as we receive data.
	timer = 0;

	// Paging category - default, 0=tone,1=numeric,2=Alpha,3=transparent data
	pcat = 0;

	// Additional information indicator.
	// 0 = no additional information
	// 1 = additional information has been sent.
	aii = 0;

	// Number of codewords per batch.
	batch_len = 17;

	// Endof message indicator.
	done = 0;

	// setup error correction code.
	setupecc();

	// Local address.
	local_addr = 0; 

	// Bit position in message header.
	header_pos = 0;
}

ERMES::~ERMES(void)
{
}

// Return codeword with "nbits" reversed. (starting from LSB).
// Bits not reversed are set to 0.
long int ERMES::reverse_cw(long int cw, int nbits)
{
	long int rwr, rcw = 0L;
 
	for (int i=0; i<nbits; i++) 
	{
		if (cw & 0x01) rcw |= 1;

		rwr = rcw;
		rcw <<= 1;
		cw >>= 1;
	}
	return(rwr);
}
/*
int ERMES::nOnes(int k)
{
	for (int i=0, kt=0; i<=15; i++)
	{
		if ((k & 0x0001) != 0) kt++;
		k = k >> 1;
	}
	return(kt);
}
*/
int ERMES::lOnes(long int k)
{
	for (int i=0, kt=0; i<32; i++)
	{
		if ((k & 0x0001l) != 0l) kt++;
		k = k >> 1;
	}
	return(kt);
}


// this routine takes a raw codeword passed to it as parameter
// col; does error correction & detection; and returns the bit
// reversed data portion of the codeword as a long int.
// The first 18 bits are the actual data; the second MSB is set
// to indicate that the codeword contains uncorrectable errors.
long int ERMES::checkecc(long int col)
{
	int i, ecc=0, nbb, tabe, bb, ts, ts2, synd=0;
	long int acw = 0x00000l;

	for (i=0; i<18; i++)
	{
		acw = acw << 1;
		if ((col & 0x01l) == 1)
		{
			ecc ^= gmat[i];
			acw = acw + 0x0001l;
		}
		col = col >> 1;
	}

	// build up syndrome... 
	int nb = 0;

	for (i=18; i<30; i++)
	{
		if ((ecc & 0x800) != 0) ts  = 1; else ts  = 0;
		if ((col & 0x01l) == 1) ts2 = 1; else ts2 = 0;

		ecc  = ecc  << 1;
		col  = col  >> 1;
		synd = synd << 1;

		if (ts != ts2)
		{
			nb++;
			synd += 0x0001;
		}
	}
	synd = synd & 0x3fff;

	CountBiterrors(0);

	// quick check - if no errors then we can return at once  
	if (synd == 0x0000)
	{
		return (acw);
	}

	// try to identify, correct errors 
	tabe = syndlup[synd];
	nbb = (tabe >> 14) & 0x03;

	if (nbb == 3)   // uncorrectable errors are marked, kissed goodbye
	{
		acw += 0x40000000l;
		return(acw);
	}
	CountBiterrors(nbb);

	// at this point we have correctable errors somewhere in code word
	nbb = (tabe >> 12) & 0x03; // get number of correctable errors in data

	// check if nothing to correct (all errors in syndrome)
	if (nbb == 0) return (acw);

	// at this point - we have at least one error in data, so correct it
	bb = tabe & 0x003f;
	if (bb < 18)
	{
		acw = acw ^ (0x01l << bb);
	}
	else
	{
		// we should never ever reach this point unless there is a
		// serious problem somewhere...
	}

	// we're finished if only one error in data needed to be corrected
	if (nbb == 1) return (acw);

	// Allright - so we have a second error in the data to correct
	bb = (tabe >> 6) & 0x3f;
	if (bb < 18)
	{
		acw = acw ^ (0x01l << bb);
	}
	else
	{ 
		// once again - should never reach this point
	}

	// two data errors corrected, no more processing to be done 
	return(acw);
}


// This routine displays the message information. Each message has a 36 bit header
// giving pager type (alpha / numeric / tone only), message type (message fragment number,
// address reassignment, etcetera), external operator identity, et cetera.
void ERMES::showme(long int l, int c)
{
	extern unsigned long hourly_stat[NUM_STAT][2];
	extern unsigned long hourly_char[NUM_STAT][2];
	extern unsigned long daily_stat[NUM_STAT][2];
	extern unsigned long daily_char[NUM_STAT][2];

	static long int nb=43, sl=0, ii;

	if (c == -1)
	{
		nb = 43;	// initial holdoff 36 bits plus one 7 bit word
	}
	else
	{
//		#ifdef EM_DEBUG	// debug - mark each codeword we use
//		char temp[10];
//		sprintf(temp, "<CW%i>", nb);
//		display_color(&Pane2, COLOR_NUMERIC);
//		display_show_strV2(&Pane2, temp);
//		#endif
  
		// run through each data bit in the codeword
		for (int i=0; i<18; i++)
		{
			// update shift register sl
			sl = sl << 1;

			if ((l & 0x20000l) != 0l)
			{
				sl += 1;
			}
			l = l << 1;

			// hex dump 36 bit header
			if ((nb > 7) && (nb < 42))
			{ 
				ii = nb >> 2;
				ii = ii << 2;

				if (ii == nb) 
				{
					sl = sl & 0x0f;

					#ifdef DB_MESSAGE_HEADER 
					sprintf(db_buf,"%x", sl);
					display_color(&Pane2, COLOR_BITERRORS);
					display_show_strV2(&Pane2, db_buf);
					#endif

					local_addr |= sl;
					sl = 0;
					header_pos++;
                      
					// Get local address & paging category.
					// Paging categories:
					// 00 (0) - tone
					// 01 (1) - numeric
					// 10 (2) - alphanumeric
					// 11 (3) - transparent data
					if (header_pos == 8) 
					{
						shown++;
						aii = (local_addr >> 3) & 0x01;

						// if additional info sent then variable information field
						// has changed so switch to alpha message as default for now.
						if (aii == 0) pcat = (local_addr & 0x03); // get paging category
						else		  pcat = 2;

						local_addr >>= 10; // get local address
						sprintf(Current_MSG[MSG_CAPCODE],"%07d", (local_addr & 0x3FFFFF));

						// Show numeric/tone only messages?
						if ((pcat < 2) && (!Profile.shownumeric)) shown=0;

						local_addr = 0;   // reset

						/* Show Capcode */

						messageitems_colors[1] = COLOR_ADDRESS;

						/* Show Time/Date */

						Get_Date_Time();
						strcpy(Current_MSG[MSG_TIME], szCurrentTime);
						strcpy(Current_MSG[MSG_DATE], szCurrentDate);
						messageitems_colors[2] = COLOR_TIMESTAMP;
						messageitems_colors[3] = COLOR_TIMESTAMP;

						sprintf(Current_MSG[MSG_MODE], "ERMES-%d", pcat);
						messageitems_colors[4] = COLOR_MODETYPEBIT;

						if (pcat < 2)
						{
							strcpy(Current_MSG[MSG_TYPE], pcat ? "NUMERIC" : " TONE  ");

							hourly_stat[STAT_ERMES][STAT_NUMERIC]++;
							daily_stat[STAT_ERMES][STAT_NUMERIC]++;
						}
						else
						{
							strcpy(Current_MSG[MSG_TYPE], (pcat==2) ? " ALPHA " : "TRANSP ");

							hourly_stat[STAT_ERMES][STAT_ALPHA]++;
							daily_stat[STAT_ERMES][STAT_ALPHA]++;
						}
						messageitems_colors[5] = COLOR_MODETYPEBIT;

						strcpy(Current_MSG[MSG_BITRATE], "6250");	// Add Bit Rate.
						messageitems_colors[6] = COLOR_MODETYPEBIT;

						if (pcat == 0)	 // tone only?
						{
							display_color(&Pane1, COLOR_NUMERIC);
							display_show_str(&Pane1, "TONE ONLY");
							done = 1;  // stop message incase garbage follows
						}
					}
					local_addr <<= 4;
				}
			}

			#ifdef DB_MESSAGE_HEADER 
			if (nb == 8) display_show_strV2(&Pane2, " ");
			#endif

			nb--;

			// Display message based on paging category. 
			if (nb == 0)
			{   
				if (pcat == 1)  // numeric data?
				{
					hourly_char[STAT_ERMES][STAT_NUMERIC]++;
					daily_char[STAT_ERMES][STAT_NUMERIC]++;

					nb = 4;
					sl = sl & 0x0F;
					display_color(&Pane1, COLOR_NUMERIC);
					display_show_char(&Pane1, (char)numformat[sl]);
				}
				else	// if here - alpha or transparent data.
				{
					hourly_char[STAT_ERMES][STAT_ALPHA]++;
					daily_char[STAT_ERMES][STAT_ALPHA]++;

					nb = 7;
					sl = sl & 0x7f;

					if (pcat == 2)	// alpha?
					{
						display_color(&Pane1, err_cw ? COLOR_BITERRORS : COLOR_MESSAGE);
					}
					else				// or transparent data.
					{
						display_color(&Pane1, COLOR_MISC);
					}
					if (sl > 31) display_show_char(&Pane1, (char)sl);
					else         display_show_char(&Pane1, (char)alpformat[sl]);
				}
			}
		} // endof main for loop.
	}
}


// this routine processes the raw stream of codewords, and it tries to
// pick out address & message information
void ERMES::docw(long int col, int blkc)
{
	long int ecol;
	short int cycle, frame, batch;
	static short int nadd=0, mess=0, lwd=0, ncbc=0;

	static int last_shown=0;
	extern bool bEmpty_Frame;		// Set if FLEX-Frame=EMTPY / ERMES-Batch=0

	extern char szWindowText[6][1000];	// PH: Additional info in titlebar

	// if blkc is zero we must have gone on to a new batch 
	if (blkc == 0)
	{
		#ifdef EM_DEBUG	// debug - mark start/end of block
		display_color(&Pane2, COLOR_NUMERIC);
		display_show_strV2(&Pane2, "<B0>");
		#endif
 
		nadd = 0;
		mess = 0;
		done = 0;
		lwd  = 0;
		ncbc = 0;
	}
	ecol = checkecc(col);

	// code words 0 to 2 have misc information 
	if (blkc < 3)
	{
//		if (blkc == 0) // PH: unused????
//		{
//			if (ecol < 0x40000000l)
//			{   
//				opco = (int) ((ecol >> 8)  & 0x07l);
//				coco = (int) ((ecol >> 11) & 0x7fl);
//			}
//		}
//		else if (blkc == 1)
		if (blkc == 1)
		{
			if (ecol > 0x3fffffffl)
			{   
				cycle = 666;
				frame = 666;
				batch = 666;
				batch_len = 17;
			}
			else
			{
				cycle = (int) ((ecol >> 7) & 0x3fl);
				frame = (int) ((ecol >> 4) & 0x07l);
				batch = (int)  (ecol & 0x0fl);

				if (Profile.show_cfs)
				{
					sprintf(szWindowText[3], "Cycle:%02i Frame:%i Batch:%c", cycle, frame, char('A'+batch));
				}
				display_showmo(MODE_ERMES);

				if (batch == 0x0F)
				{
					batch_len = 21;	// set correct batch length!
					bEmpty_Frame=true;
				}
				else
				{
					batch_len = 17;
					bEmpty_Frame=false;
				}
			}
		}
	}
	else	// address and message fields code words come here
	{
		if (mess)
		{
			if (ecol > 0x3fffffffl)
			{
				ncbc++;     // keep count of consequitive bad codewords
				err_cw = 1; // indicate current codeword has errors.
			}
			else
			{
				err_cw = 0;
				ncbc = 0;
			}

//******* not sure this is required? J.P

			// if this exceeds 7 assume we've lost sync
			if (ncbc > 7)
			{
				if(shown)
				{
					ShowMessage();   // Display Message.
					shown=0;
				}
				done = 1;
			}
//******************/

			if (!done && (blkc > 7))	// process message fields 
			{       
				if (lOnes(col ^ MDEL) < 3)
				{
					#ifdef EM_DEBUG	// debug - mark start/end of message
					display_color(&Pane2, COLOR_NUMERIC);
					display_show_strV2(&Pane2, "<MD>");
					#endif

					// found message delimeter 
					if (lwd)
					{
						done = 1;
					}
					else if (shown && (shown != last_shown))	// PH : Fix ???
					{
						shown=0;
						header_pos=0;
						last_shown=shown;
						ShowMessage();	// Display Message.
					}
					lwd = 1;
					showme(ecol,-1);
				}
				else
				{
					lwd = 0;
					showme(ecol,0);
				}

				if (shown && done)
				{
					ShowMessage();	// Display Message.
					shown=0;
				}
			}
		} // endof "if mess == 1"
		else
		{
			if (lOnes(col ^ APT) < 4)
			{
				#ifdef EM_DEBUG	// debug - mark start/end of message
				display_color(&Pane2, COLOR_NUMERIC);
				display_show_strV2(&Pane2, "<APT>");
				#endif
				// we found address field delimeter 
				mess = 1;

// This code was removed as it stopped many messages from being shown.
				// check for no addresses 
				if (!nadd || blkc == 3)
				{
					done = 1;
				}

				if (nadd == 0)
				{
					done = 1;
				}
				ncbc = 0; 
			}
			else
			{
				// we're in address field; we haven't found apt yet 
				if (ecol > 0x3fffffffl)
				{
					ncbc++;
				}
				else // show first inititial address.
				{
//					shown=0; // don't show initial addr anymore-just reset
					header_pos=0;

					ncbc = 0;

					if (ecol != 0x00l) nadd++;
				}
			}

			// if two consequitive bad address blocks... give up on this frame 
			if (ncbc >= 2)
			{
				mess = 0;
			}
		}
	}
}

// this routine processes the raw bit stream. Its output is a
// steady stream of error corrected code words that are sent on for
// further processing to routine docw. It is synchronized by passing
// -1 to the routine upon sync detection.
//
// Processing: first 240 bits (8 codewords) are not interleaved...
//             Afterwards codewords are interleaved in blocks of 9
//             J.P. Now know that this is not correct as the initial
//             address field can upto 140 codewords in length.
//             (will fix later)
void ERMES::rawbit(int bit)
{
	static short int rbc=0, blkc=0, iblk=0, ifin=0;
	static long  int col=0;
    
	if (bit == -1)	// reset routine
	{
		ifin = 0;
		rbc  = 0;
		iblk = 0;
		blkc = 0;
		#ifdef EM_DEBUG
		display_line(&Pane2);			// Ensure last line is displayed.
		#endif
	}
	else
	{
		if (blkc >= 8)
		{
			// collect, process interleaved code words
			if (rbc < batch_len)  // last batch is 190 codewords -
			{                     // all other batches are 154 codewords.
				fin[ifin++] = bit;

				if (ifin >= 270) // process 9-codeword blocks
				{
					for (int ii=0; ii<9; ii++)
					{
						for (int ij=0, il=0; ij<30; ij++)
						{
							il = (ij*9) + ii;
							col = col >> 1;

							if (fin[il] == 1) col += 0x20000000l;
						}

						#ifdef DEBUG_INTERLEAVED

						// display interleaved codewords in hex format.

						sprintf(db_buf,"%x ", reverse_cw(col, 30));
						misc_debug_msg(db_buf);

						if (db_index++ == 7)
						{
							display_line(&Pane2);
							db_index = 0;
						}
						#endif

						docw(col,blkc);
						blkc++;
					}
					ifin = 0;
					rbc++;
				}
			}
		} // endof "if (blkc >= 8)"
		else
		{
			// process first eight non-interleaved code words 

			col = col >> 1;
			if (bit == 1) col += 0x20000000l;
			iblk++;

			if (iblk >= 30)
			{   
				// process the completed code word 
				docw(col,blkc);
				iblk = 0;
				blkc++;

				#ifdef DEBUG_NONE_INTERLEAVED
				// display none interleaved codewords in hex format.
				display_color(&Pane2, COLOR_EM_CAPCODE);
				sprintf(db_buf,"%x ", reverse_cw(col, 30));
				display_show_strV2(&Pane2, db_buf);
				if (blkc == 8)	display_line(&Pane2);
				#endif
			}
		}
	}
}


// Set up syndrome lookup table for error detection/correction
//
// each entry in syndlup[] is structured as follows:
//     bits 15,14 give number of errors (3 = uncorrectable)
//     bits 13,12 give number of data bits to correct
//     bits 6-11 gives location of second error in data
//     bits 0-5  gives location of first error in data
//     if there are less than 2 bad bits in the data then 31
//       is placed in bits 6-11 (and 0-5 if needed)
//
// Note: bit numbers are reversed because the codeword is bit
//       reversed in the checkecc routine (don't ask).
void ERMES::setupecc(void)
{
	short int i,j,k,l;

	// first mark every possible syndrome as an uncorrectable error 
	for (i=0; i<4096; i++)  syndlup[i] = (short int)0xCFFF;

	// now mark first syndrome (0x000) as having no errors
	syndlup[0] = 0x0fff;

	// mark single bit errors in data
	for (i=0; i<18; i++)
	{
		k = 0x5FC0 + (17-i);  // indicates 1 error in data at bit i
		l = gmat[i];
		syndlup[l] = k;
	}

	// mark single bit errors in syndrome
	for (i=0; i<12; i++)
	{
		k = 0x4FFF;      // indicates 1 error, none in data 
		l = (0x01 << i);
		syndlup[l] = k;
	}

	// mark double bit errors in data
	for (i=0; i<18; i++)
	{
		for (j=0; j<18; j++)
		{
			if (i != j)
			{
				// two errors total; two correctable in data
				k = 0xA000 + (17-i) + ((17-j) << 6);
				l = gmat[i] ^ gmat[j];
				syndlup[l] = k;
			}
		}
	}

	// mark double errors in syndrome 
	for (i=0; i<12; i++)
	{
		for (j=0; j<12; j++)
		{
			if (i != j)
			{
				k = (short int)0x8FFF;  // two errors total; none in data 
				l = (0x01 << i) ^ (0x01 << j);
				syndlup[l] = k;
			}
		}
	}

	// Finally: mark one error in syndrome; one error in data 
	for (i=0; i<18; i++)
	{
		for (j=0; j<12; j++)
		{
			k = 0x9FC0 + (17-i);   // two errors total; one in data 
			l = gmat[i] ^ (0x01 << j);
			syndlup[l] = k;
		}
	}

}

// frame_ermes looks for 15 bit ermes sync; then sends the raw bit
// stream to routine rawbit for further processing.
void ERMES::frame(int gin)
{
	static short int ts=0, st=0, inverted=0;
	static int sync=0;
	short int nh;
/*
	// sync sequence is 15 bits long
	ts = ts << 1;
	if ((gin & 0x02) != 0) ts += 1;
	ts = ts & 0x7fff;
*/
	sync = sync << 1;
	if (gin > 1) sync++;
	sync = sync << 1;
	if ((gin == 1) || (gin == 2)) sync++;

	sync = sync & 0xFFFFFFFF;

	if (gin == -1)	// Reset - used when changing mode!
	{
		st = 0;
		return;
	}

	if (st == 0)
	{
		// find number of mismatched bits
//		nh = nOnes(ts ^ SYNC);
		nh = lOnes(sync ^ SYNC32);

		// PH: We allow max 1 error on syncup
		if (nh < 2)
		{
			if (timer < 10) timer = 20; // Display title bar msg for 3.0 seconds

			// reset raw bit processing routine that
			rawbit(-1);

			// found sync, so now we'll try to process a raw batch
			// each batch is 3/4 of a second long or a little over 2300 symbols long
			st = 2300;
			inverted = 0;

			display_color(&Pane2, COLOR_NUMERIC);
			display_show_strV2(&Pane2, "<SYNC>");
		}
		else if (nh == 15)
		{
			// if we find inverted sync 5 times in a row then flip receive polarity
			if (inverted++ > 4)
			{
				inverted = 0;
				InvertData();	// Invert receive polarity
			}
		}
	}
	else
	{
		// dump raw receive data - gin holds number of lines that are high;
		// this can be converted into two data bits (using a gray code).

		// gin == 0 => 00
		// gin == 1 => 01
		// gin == 2 => 11
		// gin == 3 => 10

		if (gin > 1) gin = gin ^ 0x01;

		// gin == 0 => 00
		// gin == 1 => 01
		// gin == 2 => 10
		// gin == 3 => 11

		rawbit((gin & 0x02) ? 1 : 0);
		rawbit((gin & 0x01) ? 1 : 0);

//		if (gin > 1) rawbit(1);
//		else rawbit(0);
//		
//		if ((gin == 1) || (gin == 2)) rawbit(1);
//		else rawbit(0);

		st--;
	}
}

/*
the Preamble word (PRE):
00 10 00 10 00 10 00 10 00 10 00 10 00 10 00
the Synchronisation Word (SW):
10 00 10 10 00 10 00 00 10 10 00 00 10 10 10
100010100010000010100000101010
the Address Partition Terminator (APT):
10 01 00 11 10 00 01 10 00 10 00 11 10 01 00
and the Message Delimiter (MD):
11 01 01 01 11 10 01 11 11 10 11 10 11 10 11

010101010101010
101101001100111

*/
