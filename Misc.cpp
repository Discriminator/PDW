//
// Misc.cpp
//

#ifndef STRICT
#define STRICT 1
#endif

#include <windows.h>
#include "headers\pdw.h"
#include "headers\initapp.h"
#include "headers\gfx.h"
#include "headers\decode.h"
#include "headers\misc.h"
#include "headers\language.h"
#include "headers\sound_in.h"
#include "headers\helper_funcs.h"
#include "utils\binary.h"
#include "utils\smtp.h"

#define FILTER_PARAM_LEN	500
#define MAXIMUM_GROUPSIZE	1000
#define CAPCODES_INDEX		0

#define MAX_SEPFILES		32

#define MONITOR				0
#define FILTER				1
#define SEPARATE			2

#define BLOCK_ADDRESS		0
#define BLOCK_TIME			1
#define BLOCK_CHECKSUM		2
#define BLOCK_MSGADDRESS	0x01	// == 1
#define BLOCK_ONLYMSG		0x02	// == 2
#define BLOCK_TIMER			0x03	// == 3
#define BLOCK_OPTION		0x03	// == First 2 bits
#define BLOCK_LOGFILE		0x04

#define OPEN_FILE			0
#define CLOSE_FILES			1

#define CURRENT				0
#define PREVIOUS			1

#define BUILDSHOWLINE_LINEFEED 1
#define BUILDSHOWLINE_LASTCHAR 2

unsigned int bch[1025], ecs[25];     // error correction sequence

int iMatch=-1, iAddrMatch=-1, iTextMatch=-1, iTextLength=0;
int iTextPositions[10], iTextLengths[10];

int iCurrentCycle, iCurrentFrame;	// Current flex cycle / frame

int aGroupCodes[17][MAXIMUM_GROUPSIZE];
int GroupFrame[17] = { -1, -1, -1, -1, -1, -1, -1, -1,
					   -1, -1, -1, -1, -1, -1, -1, -1, -1 };

int iPanePos=0;								// PH: Current line position, used by Wordwrap
int iLabelspace_Logfile[2];					// PH: # of spaces before label ([MONITOR] / [FILTER])

int  iConvertingGroupcall=0;				// PH: Set with groupbit if converting a groupcall
bool bMode_IDLE=true;						// PH: Set if not receiving data

bool bMobitexReplace;						// PH: Set if a mobitex message needs to be replaced (replace.txt)
char szMobitexReplace[256];					// PH: Contains mobitex replacement string (replace.txt)

char aNumeric[17]={"0123456789*U -]["};		// contains numeric paging data format

bool bShown[2]  = { false, false };
bool bLogged[3] = { false, false, false };

char Current_MSG[9][MAX_STR_LEN];			// PH: Buffer for all message items
											// 1 = MSG_CAPCODE
											// 2 = MSG_TIME
											// 3 = MSG_DATE
											// 4 = MSG_MODE
											// 5 = MSG_TYPE
											// 6 = MSG_BITRATE
											// 7 = MSG_MESSAGE
											// 8 = MSG_MOBITEX


char Previous_MSG[2][9][MAX_STR_LEN];		// PH: Buffer for previous message items
											// PH: [8]=last filtered messagetext

unsigned long int iSecondsElapsed=0;
unsigned long int aMessages[1000][3] = {0};	// PH: Array used for blocking messages

char szLogFileLine[MAX_STR_LEN+64];			// PH: Current Logfile line
char szSepfilenames[MAX_SEPFILES][MAX_PATH];// PH: Buffer for current separate filename and the sepfiles in current groupcall
FILE* pSepFilterFiles[MAX_SEPFILES];
extern char szWindowText[6][1000];
extern char szFilenameDate[16];				// PH: Global buffer for date as filename

BYTE message_color[MAX_STR_LEN+1];			// buffer for filter colors
BYTE messageitems_colors[7];				// buffer for message items colors
unsigned char message_buffer[MAX_STR_LEN+1];// buffer for message characters
unsigned char mobitex_buffer[MAX_STR_LEN+1];// buffer for mobitex characters
unsigned char rev_msg_buffer[MAX_STR_LEN+1];// required for logfile output

int iMessageIndex=0;

char *dsc_pchar;
BYTE *dsc_pcolor;

FILE *pBlocked = NULL;						// PH: Used for blocked messages


void ResetBools();
bool BlockChecker(char *address, int fnu, char *message, bool reject);

// Displays debug messages in the filter pane.
void misc_debug_msg(char *msg)
{
	display_color(&Pane2, COLOR_MESSAGE);
	display_show_strV2(&Pane2, msg);
}

// Reverse words in buffer which contain characters from the current character set.
// Note: Used by Hebrew language which displays messages right to left.
void reverse_msg_words(void)
{
	unsigned char *s = message_buffer; 
	int r;
	int start_x, end_x;

	int v = strlen((char *)s);

	for (int loop=0; loop < v; loop++)
	{
		if (in_lang_tbl(message_buffer[loop]) || isdigit(message_buffer[loop]))
		{
			start_x = loop;
			while (loop < v)
			{
				// If not a digit and not in language table then endof word/number.
				// Allow control characters.
				if (!in_lang_tbl(message_buffer[loop+1]))
				{
					if (message_buffer[loop+1] > 65) break;
				}
				if (message_buffer[loop+1] == 0) break;
				loop++;
			}
			end_x = loop;

			if (end_x > start_x)
			{
				r = 0;
				while (end_x >= start_x) rev_msg_buffer[r++] = message_buffer[end_x--];
				rev_msg_buffer[r] = 0;
				r=0;

				while (rev_msg_buffer[r] != 0)
				{
					message_buffer[start_x++] = rev_msg_buffer[r];
					r++;
				}
			}
		}
	}
}

// Passes string to filter buffer
void display_show_str(PaneStruct *pane, char strin[])
{
	for (int x=0; ((strin[x] != 0) && (x < 256)); x++)
	{
		display_show_char(pane, strin[x]);
	}
}

// Passes string to "pane's" buffer.
void display_show_strV2(PaneStruct *pane, char strin[])
{
	for (int x=0; ((strin[x] != 0) && (x < 256)); x++)
	{
		build_show_line(pane, strin[x], 0);
	}
}

// Builds the filter string to be shown in the upper/lower panes.
void display_show_char(PaneStruct *pane, char cin)
{
	if (Profile.monitor_mobitex)
	{
		if (cin == 0)
		{
			message_buffer[iMessageIndex] = '·';
			mobitex_buffer[iMessageIndex] = '·';
		}
		else if ((cin > 0) && (cin < 32))
		{
			message_buffer[iMessageIndex] = cin+32;
			mobitex_buffer[iMessageIndex] = cin;		// Keep original characters
		}
		else if ((cin > 126) && (cin != '»'))
		{
			message_buffer[iMessageIndex] = ' ';
			mobitex_buffer[iMessageIndex] = cin;		// Keep original characters
		}
		else
		{
			message_buffer[iMessageIndex] = cin;
			mobitex_buffer[iMessageIndex] = cin;		// Keep original characters
		}
	}
	else
	{
		if (cin == '\n')
		{
			cin = '»';	// PH: Convert 'new line' to '»'
		}
		else if (cin > 127)
		{
			cin = '?';	// PH: Display a questionmark instead of 'unknown' characters
		}
		else if ((cin >  0  && cin < 32 && cin != 10) &&
				 (cin != 23 && cin != 4 && !Profile.monitor_acars && !Profile.monitor_mobitex))
		{
			cin = '?';	// PH: Display a questionmark instead of 'unknown' characters
		}
		message_buffer[iMessageIndex] = cin;
	}
	message_color[iMessageIndex] = pane->currentColor;

	if (iMessageIndex < MAX_STR_LEN-1) iMessageIndex++;

} // end of display_show_char


//  - Output characters to the selected Pane's buffer. 
//  - Displays line of text when complete.
//  - After displaying line, start over on next line.
//void build_show_line(PaneStruct *pane, char cin, int linefeed)
void build_show_line(PaneStruct *pane, char cin, int option)
{
	int index = iItemPositions[7]; // PH: Used when moving to new line

	if (option == BUILDSHOWLINE_LINEFEED)
	{
		display_line(pane); // terminate/display line/start new line.

		// Get buffer pointers & set correct postion in buffers.
		dsc_pchar  = pane->buff_char  + pane->Bottom*(LINE_SIZE+1);
		dsc_pcolor = pane->buff_color + pane->Bottom*(LINE_SIZE+1);

		// Add spacing to continue message on next line in correct field.
		for (int i=0; i<index; i++)
		{
			*dsc_pchar++ = ' ';
			*dsc_pcolor++ = COLOR_MESSAGE;
			pane->currentPos++;
		}
		iPanePos = pane->currentPos;	// Get current pane position
		return;
	}
	
	// Get buffer pointers.
	dsc_pchar  = pane->buff_char;
	dsc_pcolor = pane->buff_color;

	// Get character/color.
	dsc_pchar [pane->Bottom*(LINE_SIZE+1) + pane->currentPos] = cin;
	dsc_pcolor[pane->Bottom*(LINE_SIZE+1) + pane->currentPos] = pane->currentColor;

	// Increment line position.
	pane->currentPos++;

	// Start new line?
	if (option != BUILDSHOWLINE_LASTCHAR)
	{
		if ((pane->currentPos > NewLinePoint) || (pane->currentPos == LINE_SIZE))
		{
			display_line(pane); // terminate/display line/start new line.

			// Get buffer pointers & set correct postion in buffers.
			dsc_pchar  = pane->buff_char  + pane->Bottom*(LINE_SIZE+1);
			dsc_pcolor = pane->buff_color + pane->Bottom*(LINE_SIZE+1);

			// PH: If monitoring ACARS and no linefeed, ensure a correct spacing
			if (Profile.monitor_acars) index += 16;

			// Add spacing to continue message on next line in correct field.
			for (int i=0; i<index; i++)
			{
				*dsc_pchar++ = ' ';
				*dsc_pcolor++ = COLOR_MESSAGE;
				pane->currentPos++;
			}
		}
	}
	iPanePos = pane->currentPos;	// Get current pane position
} // end of build_show_line()


// Display the current line.
void display_line(PaneStruct *pane)
{
	RECT	rect;
	unsigned int xx;
	int	scroll_amt, iVscrollInc;
	char *pchar  = pane->buff_char;
	BYTE *pcolor = pane->buff_color;

	extern unsigned int iSelectionStartRow, iSelectionEndRow;

	pchar [pane->Bottom*(LINE_SIZE+1) + pane->currentPos] = 0;
	pcolor[pane->Bottom*(LINE_SIZE+1) + pane->currentPos] = COLOR_UNUSED;

	pane->currentPos = 0;
	pane->Bottom++;

	// No space left in pane's buffer so remove first line to allow
	// a new line to be added on the end!
	if (pane->Bottom == pane->buff_lines)
	{
		pane->Bottom--;

		// scroll the buffer lines up 1 line
		for (xx=0; xx<pane->Bottom; xx++)
		{
			memcpy(&pchar [xx*(LINE_SIZE+1)], &pchar [(xx+1)*(LINE_SIZE+1)], LINE_SIZE+1);
			memcpy(&pcolor[xx*(LINE_SIZE+1)], &pcolor[(xx+1)*(LINE_SIZE+1)], LINE_SIZE+1);
		}

		pchar[xx*(LINE_SIZE+1) + 0]  = 0;
		pcolor[xx*(LINE_SIZE+1) + 0] = COLOR_UNUSED;
		pane->iVscrollPos--;
	}
	pane->iVscrollMax = max(0,pane->Bottom - pane->cyLines);

	// check if we need to scroll the display (i.e. are we at bottom?)
	if (pane->Bottom == (pane->iVscrollPos + (pane->cyLines) + 1))
	{
		iVscrollInc = 1;
		iVscrollInc = max(-(pane->iVscrollPos), min(iVscrollInc, pane->iVscrollMax - pane->iVscrollPos));

		pane->iVscrollPos += iVscrollInc;

		scroll_amt = cyChar * iVscrollInc;
		ScrollWindow(pane->hWnd, 0, -scroll_amt, NULL, NULL);

		rect.top	= (pane->cyLines - 1) * cyChar;
		rect.bottom	= pane->cyClient;
		rect.left	= 0;
		rect.right	= pane->cxClient;

		InvalidateRect(pane->hWnd, &rect, TRUE);

		SetScrollRange(pane->hWnd, SB_VERT, 0, pane->iVscrollMax, FALSE);
		SetScrollPos  (pane->hWnd, SB_VERT,    pane->iVscrollPos, TRUE);

		if (select_on && selected && (pane == select_pane))
		{
			iSelectionStartRow -= iVscrollInc;	// also scroll selection
			iSelectionEndRow   -= iVscrollInc;

			InvertSelection();
			selected = 1;
		}
	}
	else	// don't need to scroll display so just update
	{
		rect.top	= 0; // (pane->Bottom-1) * cyChar;  *Changed J.P*
		rect.bottom	= pane->Bottom * cyChar;
		rect.left	= 0;
		rect.right	= pane->cxClient;

		InvalidateRect(pane->hWnd, &rect, TRUE);
	}
	UpdateWindow(pane->hWnd);

	iPanePos = 0;
}


void AddAssignment(int assignedframe, int groupbit, int capcode)
{
	if ((GroupFrame[groupbit] != assignedframe) &&
		(GroupFrame[groupbit] != -1) &&
		(aGroupCodes[groupbit][CAPCODES_INDEX]))
	{
		if (groupbit < 16) Remove_MissedGroupcall(groupbit);
	}

	if (aGroupCodes[groupbit][CAPCODES_INDEX] < MAXIMUM_GROUPSIZE)
	{
		aGroupCodes[groupbit][CAPCODES_INDEX]++;
		aGroupCodes[groupbit][aGroupCodes[groupbit][CAPCODES_INDEX]] = capcode;

		GroupFrame[groupbit] = assignedframe;
	}
	if (iMessageIndex)
	{	
		message_buffer[iMessageIndex] = 0;		// terminate the buffer string
		iMessageIndex = 0;
	}
}

void ConvertGroupcall(int groupbit, char *vtype, int capcode)
{
	char szFile[MAX_PATH];

	extern int nCount_Blocked;	// PH: To keep track of the number of blocked messages
	extern int nCount_Missed[2];

	int addresses=0;
	char address[16]="";

	message_buffer[iMessageIndex] = 0;		// terminate the buffer string

	if (capcode >= 2029568 && capcode <= 2029583)	// PH: If current capcode is a groupcode
	{
		if (GroupFrame[groupbit] == iCurrentFrame)
		{
			SortGroupCall(groupbit);	// PH: Sort current groupcall in ascending order

			if (Profile.BlockDuplicate)
			{
				for (int nCapcode=1; nCapcode <= aGroupCodes[groupbit][CAPCODES_INDEX]; nCapcode++)
				{
					addresses += aGroupCodes[groupbit][nCapcode];			// Make a sum of all capcodes
				}
				addresses += aGroupCodes[groupbit][CAPCODES_INDEX] << 25;	// MSB bits represent the number of capcodes

				sprintf(address, "%i", addresses);

				if (BlockChecker(address, 0, (char*)message_buffer, false))
				{
					if (Profile.show_rejectblocked)
					{
						sprintf(szWindowText[5], "Blocked Duplicate GroupMessage : %i %s", 2029568+groupbit, message_buffer);
					}

					if (Profile.BlockDuplicate & BLOCK_LOGFILE)
					{
						sprintf(szFile, "%s\\blocked.txt", szPath);

						if ((pBlocked = fopen(szFile, "a")) != NULL)
						{
							Get_Date_Time();
							fprintf(pBlocked, "%i %s %s  %s\n", 2029568+groupbit, szCurrentTime, szCurrentDate, message_buffer);
							fclose(pBlocked);
							pBlocked = NULL;
						}
					}
					nCount_Blocked++;

					GroupFrame[groupbit] = -1;

					memset(aGroupCodes[groupbit], 0, sizeof(aGroupCodes[groupbit]));

					iMessageIndex = 0;
					return;
				}
			}
			iConvertingGroupcall=groupbit+1;

			strcpy(Current_MSG[MSG_TYPE], " GROUP ");

			CreateDateFilename("", NULL);		// Get current date to use as filename

			for (int nCapcode=1; nCapcode <= aGroupCodes[groupbit][CAPCODES_INDEX]; nCapcode++)
			{
				if (aGroupCodes[groupbit][nCapcode] == 9999999) strcpy(Current_MSG[MSG_CAPCODE], "???????");
				else sprintf(Current_MSG[MSG_CAPCODE], "%07i", aGroupCodes[groupbit][nCapcode]);

				ShowMessage();	// PH: Display current short instruction
			}
			sprintf(Current_MSG[MSG_CAPCODE], "%07i", capcode);
			strcpy(Current_MSG[MSG_TYPE], vtype);

			ShowMessage();		// PH: Display current groupcode & original vector type

			memset(aGroupCodes[groupbit], 0, sizeof(int) * MAXIMUM_GROUPSIZE);

			GroupFrame[groupbit] = -1;

			iConvertingGroupcall=0;		// PH: Reset for next groupmessage
		}
		else
		{
			if (GroupFrame[groupbit] != -1) Remove_MissedGroupcall(groupbit);
			if (Current_MSG[MSG_BITRATE][3] == '0') nCount_Missed[1]++;
			CountBiterrors(5);
			ShowMessage();		// PH: Display only groupcode
			return;
		}
	}
	iMessageIndex = 0;
}


void SortGroupCall(int groupbit)	// PH: Sort aGroupCodes[groupbit]
{
	for (int nCapcode=1; nCapcode <= aGroupCodes[groupbit][CAPCODES_INDEX]; nCapcode++)
	{
		int min, j;
		for (min=nCapcode, j=nCapcode+1; aGroupCodes[groupbit][j] > 0; j++)
		{
			if (aGroupCodes[groupbit][j] < aGroupCodes[groupbit][min]) min = j;
		}

		int tmp=aGroupCodes[groupbit][nCapcode];

		aGroupCodes[groupbit][nCapcode] = aGroupCodes[groupbit][min];
		aGroupCodes[groupbit][min] = tmp;  // swap them
	}
}


void Check4_MissedGroupcalls()
{
	int difference, assignedframe;
	int currentframe=iCurrentFrame;

	for (int groupbit=0; groupbit<16; groupbit++)
	{
		if (GroupFrame[groupbit] != -1)		// Check if assignment in buffer for this groupcode
		{
			assignedframe = GroupFrame[groupbit];	// Get assigned frame

			if ((assignedframe > 120) && (currentframe < 8)) currentframe  += 128;	// Assigned frame was in previous cycle
			if ((assignedframe < 8) && (currentframe > 120)) assignedframe += 128;	// Assigned frame is in next cycle

			difference = assignedframe-currentframe;	// Calculate difference

			if (difference <= 0)	// If difference is 0 or lower, a groupcall has been missed
			{
				Remove_MissedGroupcall(groupbit);
			}
		}
	}
}


void Remove_MissedGroupcall(int groupbit)
{
	extern int nCount_Missed[2], nCount_Messages, nCount_Groupcalls;
	char szFile[MAX_PATH];
	
	FILE *pFLEX_missed = NULL;			// PH: file: "missed-groupcalls.txt"

	sprintf(szFile, "%s\\missed-groupcalls.txt", szPath);

	if (!FileExists(szFile))
	{
		if ((pFLEX_missed = fopen(szFile, "a")) != NULL)
		{
			fprintf(pFLEX_missed, " Missed Groupcalls:\n\n");
		}
	}
	else if (pFLEX_missed == NULL) pFLEX_missed = fopen(szFile, "a");

	if (pFLEX_missed)
	{
		Get_Date_Time();
		fprintf(pFLEX_missed, " %s %s (%i-%03i) : ", szCurrentDate, szCurrentTime, groupbit+2029568, GroupFrame[groupbit]);

		SortGroupCall(groupbit);	// PH: Sort current groupcall in ascending order

		for (int nCapcode=1; aGroupCodes[groupbit][nCapcode] > 0; nCapcode++)
		{
			if (aGroupCodes[groupbit][nCapcode] == 9999999) fprintf(pFLEX_missed, "??????? ");
			else fprintf(pFLEX_missed, "%07i ", aGroupCodes[groupbit][nCapcode]);
		}
		memset(aGroupCodes[groupbit], 0, sizeof(int) * MAXIMUM_GROUPSIZE);

		fwrite("\n", 1, 1, pFLEX_missed);
		fclose(pFLEX_missed);
		pFLEX_missed = NULL;
	}

	strcpy(szWindowText[5], "MISSED GROUPCALL!");

	GroupFrame[groupbit] = -1;

	nCount_Missed[0]++;

	CountBiterrors(10);
}


void ShowMessage()
{
	int pos, i, j, k, tmp_pos, iSepfile, pane;
	int panepos;							// PH: Temp line positions, used by Wordwrap
	int iMOBITEX=0;							// PH: Set if converting a Mobitex message
	int BlockOnlyMsg =((Profile.BlockDuplicate & BLOCK_OPTION) == BLOCK_ONLYMSG);

	bool bMONITOR=true;		// PH: To indicate if current message has to be written to Pane1

	bool bBlock=false, bSkip_character=false;
	bool bMATCH=false, bMONITOR_ONLY=false, bFILTERED=false;
	bool bShowMessage=true, bFragment=false, bGroupcode;
	bool bNumeric=false;
	bool bNewFile, bNewLine;					// PH: To indicate if the logfile is new / already exists
	bool bSeparator[2] = { true, true };		// PH: Set if a separator is needed
	bool bCombine = false;						// PH: Used for grouping not-group messages

	static bool bPlayWaveFile, bPreviousDoubleDisplay=false;
	static bool bPreviousNumeric[2]= { false, false };	// PH: Was prev.MSG numeric?
														// ([MONITOR] / [FILTER])
	char szFilename[MAX_PATH];					// Buffer for filenames
	char szFragment[50]="";						// Buffer for fragment text
	char temp[MAX_STR_LEN];						// Temp buffer
	char ch;									// Buffer for current character (message_buffer[pos])
	char szLabelspacing[100];					// PH: Spaces before label
	char szCurrentLabel[2][FILTER_LABEL_LEN+50] = { "", "" };
	char aGroupnumbers[16][8]={"-1", "-2", "-3", "-4", "-5", "-6", "-7", "-8", "-9", "10", "11", "12", "13", "14", "15", "16"};

	extern bool bTrayed, bDoubleDisplay;
	extern int  nCount_Messages, nCount_Rejected, nCount_Blocked, nCount_Groupcalls;

	DWORD dwColor;

	PaneStruct *pPane;

	if (!iConvertingGroupcall)
	{
		message_buffer[iMessageIndex]=0;		// terminate the buffer string
		message_color[iMessageIndex] = COLOR_UNUSED;
		iMessageIndex = 0;
	}
	
	if (Profile.lang_mi_index)
	{
		if (Profile.reverse_msg)
		{
			reverse_msg_words();	// Reverse message words if language requires it.
		}

		for (pos=0; message_buffer[pos] != 0; pos++)
		{
			message_buffer[pos] = remap_ch(message_buffer[pos]);
		}
	}

	memcpy(Current_MSG[MSG_MESSAGE], message_buffer, MAX_STR_LEN);

	bMobitexReplace=false;

	iMatch=Check_4_Filtermatch();	// PH: Check if current message matches a filter

	bGroupcode = memcmp(Current_MSG[MSG_CAPCODE], "20295", 5) ? false : true;

	if (!iConvertingGroupcall)
	{
		nCount_Messages++;
	}
	else if (bGroupcode)
	{
		nCount_Groupcalls++;
		nCount_Messages++;

		if (Profile.FlexGroupMode & FLEXGROUPMODE_HIDEGROUPCODES)
		{
			bShowMessage=false;

			if (!bShown[MONITOR] && (Profile.BlockDuplicate & BLOCK_OPTION) == BLOCK_TIMER)	// Nothing shown + using timer
			{
				BlockChecker(0, 0, Current_MSG[MSG_MESSAGE], true);	// Current message might me in block-buffer
			}
		}
	}

	if (bShowMessage)
	{
		if ((strstr(Current_MSG[MSG_TYPE], "NUM")) ||
			(strstr(Current_MSG[MSG_TYPE], "TONE")))
		{
			bNumeric = true;	// This is a numeric / tone-only message
		}

		if (iMatch != -1)
		{
			Profile.filters[iMatch].hitcounter++;								   // Update hitcounter

			memcpy(Profile.filters[iMatch].lasthit_time, Current_MSG[MSG_TIME], 8);// Update last time
			memcpy(Profile.filters[iMatch].lasthit_date, Current_MSG[MSG_DATE], 8);// Update last date

			bUpdateFilters = true;										// Update filters.ini in UpdateFilters()

			bMATCH=true;

			if (Profile.filters[iMatch].monitor_only || bMobitexReplace)
			{
				bMONITOR_ONLY=true;
			}
			else
			{
				bFILTERED=true;
				if (Profile.filterwindowonly) bMONITOR=false;	// Don't display filtered messages in monitor pane
			}

			if (Profile.filters[iMatch].reject)
			{
				if (Profile.show_rejectblocked)
				{
					sprintf(szWindowText[5], "Rejected Message : %s %s", Current_MSG[MSG_CAPCODE], Current_MSG[MSG_MESSAGE]);
				}

				if (bGroupcode)
				{
					LogFileHandling(NULL, NULL, CLOSE_FILES);

					if (bPlayWaveFile)
					{
						PlayWaveFile(NULL, NULL, true);
						bPlayWaveFile = false;
					}
					if (iConvertingGroupcall)
					{
						ResetBools();
					}
				}
				nCount_Rejected++;
				return;
			}
		}
		else if ((strstr(Current_MSG[MSG_MODE], "POCSAG")) &&
				(memcmp(Current_MSG[MSG_MESSAGE], "WHC+", 4) == 0))
		{
			return;
		}

		if (Profile.BlockDuplicate && !iConvertingGroupcall)	// Do we want to block duplicate messages?
		{
			bBlock = BlockChecker(Current_MSG[MSG_CAPCODE], strstr(Current_MSG[MSG_MODE], "POCSAG") ? atoi(&Current_MSG[MSG_MODE][7]) : 0, Current_MSG[MSG_MESSAGE], false);

			if (bBlock)
			{
				if (Profile.show_rejectblocked)			// Show in title bar?
				{
					sprintf(szWindowText[5], "Blocked Duplicate Message : %s %s", Current_MSG[MSG_CAPCODE], (Profile.monitor_mobitex && !Current_MSG[MSG_MESSAGE][0]) ? Current_MSG[MSG_TYPE] : Current_MSG[MSG_MESSAGE]);
				}

				if (Profile.BlockDuplicate & BLOCK_LOGFILE)
				{
					sprintf(szFilename, "%s\\blocked.txt", szPath);	// Add to blocked.txt

					if ((pBlocked = fopen(szFilename, "a")) != NULL)
					{
						fprintf(pBlocked, "%s %s %s  %s\n",	Current_MSG[MSG_CAPCODE],
															Current_MSG[MSG_TIME],
															Current_MSG[MSG_DATE],
															Current_MSG[MSG_MESSAGE]);
						fclose(pBlocked);
						pBlocked = NULL;
					}
				}
				nCount_Blocked++;

				return;
			}
			else if (bFILTERED && !(Profile.BlockDuplicate >> 4))	// Block duplicate FILTERED messages
			{
				if (CompareMessage(MSG_CAPCODE, FILTER) ||		// Compare capcodes/addresses
					(BlockOnlyMsg))								// or ignore if only comparing message
				{
					if (CompareMessage(MSG_MESSAGE, FILTER) &&	// Compare messages
						CompareMessage(MSG_TYPE, FILTER))		// Compare types (mostly for Mobitex)
					{
						bFILTERED=false;
						bMONITOR_ONLY=true;
					}
				}
			}
		}
		if (bTrayed && Profile.SystemTrayRestore)
		{
			switch (Profile.SystemTrayRestore)
			{
				default:
				case 1:
						SystemTrayWindow(false);
				break;

				case 2:
						if (bMONITOR_ONLY || bFILTERED) SystemTrayWindow(false);
				break;

				case 3:
						if (bFILTERED) SystemTrayWindow(false);
				break;
			}
		}

		if (Profile.FlexGroupMode)
		{
			if (!iConvertingGroupcall && (Profile.FlexGroupMode & FLEXGROUPMODE_COMBINE))
			{
				if (CompareMessage(MSG_MESSAGE, MONITOR) &&	!CompareMessage(MSG_CAPCODE, MONITOR))
				{
					if (memcmp(Current_MSG[MSG_CAPCODE], Previous_MSG[MONITOR][MSG_CAPCODE], 2) == 0)
					{
						bCombine=true;
					}
				}
			}
		}
		if (!bShown[MONITOR] && !bCombine) memset(szSepfilenames, 0, sizeof(szSepfilenames));

		for (pane=0; pane<2; pane++)	// Loop trough messageitems, first show MONITOR,
		{								// then show FILTER

			if (pane == MONITOR)
			{
				if (!bMONITOR || bCombine) continue;
				else
				{
					pPane = &Pane1;		// Show message in Pane1
				}
			}
			else if (pane == FILTER)
			{
				if (!bFILTERED || bCombine) break;
				else
				{
					pPane = &Pane2;		// Show message in Pane2
				}
			}

			if (Profile.Separator || Profile.FlexGroupMode)
			{
				if (bShown[pane])	// Already shown? (groupcall)
				{
					bSeparator[pane] = false;
				}
				else if (bDoubleDisplay)
				{
					if (strcmp(Current_MSG[MSG_TYPE], "NUMERIC") == 0)
					{
						bSeparator[pane] = false;
						bPreviousNumeric[pane] = false;
					}
				}
				else if (bNumeric && bPreviousNumeric[pane] && !Profile.FlexGroupMode)	// If prev+current messages are numeric
				{
					bSeparator[pane] = false;
				}
				else if (CompareMessage(MSG_MESSAGE, MONITOR) && !Profile.FlexGroupMode)	// Compare messages
				{
					bSeparator[pane] = false;
				}
			}
			else bSeparator[pane] = false;

			if (!bDoubleDisplay) bPreviousNumeric[pane] = bNumeric;

			if (Profile.FlexGroupMode && bShown[pane])
			{
				bSeparator[pane] = false;
				continue;
			}

			if (strstr(Current_MSG[MSG_MODE], "FLEX") && (Current_MSG[MSG_BITRATE][3] != '0'))
			{
				if (Profile.FlexGroupMode)
				{
					 sprintf(szFragment, "  [Fragment #%c]", char(Current_MSG[MSG_BITRATE][3])+1);
				}
				else sprintf(szFragment, "[Continued message - Fragment #%c]", char(Current_MSG[MSG_BITRATE][3])+1);

				bFragment=true;
			}

			if (bSeparator[pane] && pPane->Bottom) // If pane is not empty, add an empty line
			{
				display_show_strV2(pPane, " ");
				display_line(pPane);	// Ensure last line is displayed.
			}

			memcpy(Previous_MSG[pane], Current_MSG, (MAX_STR_LEN * 9));
			display_show_strV2(pPane, " ");

			// Display&log message
			for (i=0; i<7; i++)
			{
				if (Profile.ScreenColumns[i] == 0) break;

				for (j=iPanePos; j<iItemPositions[Profile.ScreenColumns[i]]; j++)
				{
					display_show_strV2(pPane, " ");
				}

				if (Profile.ScreenColumns[i] == MSG_MESSAGE)
				{
					if (!Profile.FlexGroupMode && bFragment)
					{
						display_color(pPane, COLOR_INSTRUCTIONS);
						display_show_strV2(pPane, szFragment);
						display_line(pPane);

						for (j=iPanePos; j<iItemPositions[MSG_MESSAGE]; j++)
						{
							display_show_strV2(pPane, " ");
						}
					}

					for (pos=0; message_buffer[pos] != 0; pos++, bSkip_character=false)
					{
						ch = message_buffer[pos];
						display_color(pPane, message_color[pos]);

						if (iTextMatch != -1)	// Do we (also) have a text iMatch?
						{
							if (iTextLengths[0] && !Profile.FlexGroupMode)
							{
								for (k=0; k<10 && iTextPositions[k]; k++)
								{
									if (pos >= iTextPositions[k] && pos < (iTextPositions[k] + iTextLengths[k]))
									{
										display_color(pPane, COLOR_FILTERMATCH);
									}
								}
							}
							else if (pos >= iTextMatch && pos < (iTextMatch+iTextLength))
							{
								display_color(pPane, COLOR_FILTERMATCH);
							}
						}

						if (Profile.monitor_acars)				// ACARS.CPP adds ATB-character
						{										// to mark the end of a line.
							if (ch == char(23))
							{
								build_show_line(pPane, ' ', BUILDSHOWLINE_LINEFEED);
								bSkip_character=true;
							}
						}
						else if (ch == ' ')				// PH: If character is a space
						{
							if (iPanePos == iItemPositions[MSG_MESSAGE])
							{
								bSkip_character=true;	// PH: If first char is a space, skip it
							}
							else if (!Profile.monitor_mobitex)	// PH: Check for wordwrap
							{
								tmp_pos = pos;
								panepos=iPanePos;

								while (message_buffer[tmp_pos+1] != ' ' && message_buffer[tmp_pos] != 0)
								{
									tmp_pos++;
									panepos++;
								}

								if (panepos > NewLinePoint)		// PH: Move to new line
								{
									build_show_line(pPane, ' ', BUILDSHOWLINE_LINEFEED);
									bSkip_character=true;
								}
							}
						}
						else if (ch == '»')		// Check for linefeed character '»'
						{
							if (Profile.Linefeed)
							{
								if (iPanePos == iItemPositions[MSG_MESSAGE]) bSkip_character=true;
								else
								{
									build_show_line(pPane, ' ', BUILDSHOWLINE_LINEFEED);
									bSkip_character=true;
								}
							}
						}

						if (!bSkip_character)		// Show current character on screen
						{
							build_show_line(pPane, ch, (message_buffer[pos+1] == 0) ? BUILDSHOWLINE_LASTCHAR : 0);
						}
					}

					if (Profile.FlexGroupMode)
					{
						display_line(pPane);	// Display first group line (time/message)
					}
				}
				else
				{
					display_color(pPane, messageitems_colors[Profile.ScreenColumns[i]]);

					if (Profile.ScreenColumns[i] == MSG_CAPCODE)
					{
						if (Profile.FlexGroupMode)
						{
							if (iConvertingGroupcall)
							{
								sprintf(temp, "GROUP%s", aGroupnumbers[iConvertingGroupcall-1]);
							}
							display_color(pPane, messageitems_colors[MSG_TYPE]);
							display_show_strV2(pPane, iConvertingGroupcall? temp : Current_MSG[MSG_TYPE]);

							if (Profile.FlexGroupMode & FLEXGROUPMODE_LOGGING)
							{
								sprintf(szLogFileLine, "%s %s  %s  %s\n",
															Current_MSG[MSG_TIME],
															Current_MSG[MSG_DATE],
															iConvertingGroupcall? temp : Current_MSG[MSG_TYPE],
															Current_MSG[MSG_MESSAGE]);
							}
							continue;
						}
						else
						{
							if (strlen(Current_MSG[MSG_CAPCODE]) == 7)
							{
								display_show_strV2(pPane, " ");	// Add extra space if 7 digit code
							}
							if (bFILTERED && (iAddrMatch != -1))
							{
								// did capcode iMatch (with text iMatch)?
								// change color of temp_str capcode to filter iMatch color...
								display_color(&Pane2, COLOR_FILTERMATCH);
							}
						}
					}
					if (Profile.ScreenColumns[i] == MSG_BITRATE)
					{
						display_show_strV2(pPane, " ");
					}
					display_show_strV2(pPane, Current_MSG[Profile.ScreenColumns[i]]);
				}
			}

			if (bDoubleDisplay && (strcmp(Current_MSG[MSG_TYPE], "NUMERIC") == 0))	// testen of dit nog werkt
			{
				display_show_strV2(pPane, " ");
				display_line(pPane);		// Separate grouped num+alpha message
			}
			bShown[pane]=true;
		}
		iMessageIndex = 0;					// reset to beginning of filter buffer

		if (Profile.FlexGroupMode)
		{
			for (pane=0; pane<2; pane++)	// Loop trough messageitems, first show MONITOR,
			{								// then show FILTER
				if (pane == MONITOR)
				{
					if (!bMONITOR) continue;
					else
					{
						pPane = &Pane1;		// Show message in Pane1
					}
				}
				else if (pane == FILTER)
				{
					if (!bFILTERED) break;
					else
					{
						pPane = &Pane2;		// Show message in Pane2
					}
				}
				memcpy(Previous_MSG[pane], Current_MSG, (MAX_STR_LEN * 9));

				for (i=0, iPanePos=0; i<3; i++)
				{
					if (Profile.ScreenColumns[i] == MSG_CAPCODE)
					{
						if (!iConvertingGroupcall && bFragment)
						{
							display_color(pPane, COLOR_INSTRUCTIONS);
							display_show_strV2(pPane, " ");
							display_show_strV2(pPane, szFragment);
						}

						for (int j=iPanePos; j<iItemPositions[Profile.ScreenColumns[i]]; j++)
						{
							display_show_strV2(pPane, " ");
						}
						display_color(pPane, messageitems_colors[MSG_CAPCODE]);
						display_show_strV2(pPane, Current_MSG[MSG_CAPCODE]);
					}
				}
			}
		}

		if (bMATCH && Profile.filters[iMatch].label_enabled && Profile.filters[iMatch].label[0])
		{
			// Display description (LABEL) text
			if (strstr(Profile.filters[iMatch].label, "%"))
			{
				MakeFilterLabel(Profile.filters[iMatch].label, Current_MSG[MSG_CAPCODE], szCurrentLabel[0]);
			}
			else strcpy(szCurrentLabel[0], Profile.filters[iMatch].label);

			dwColor = (COLOR_FILTERLABEL+Profile.filters[iMatch].label_color);

			if (bMONITOR && (bMONITOR_ONLY || Profile.LabelLog))
			{
				if (Pane1.currentPos > iItemPositions[MSG_MESSAGE]) display_line(&Pane1);
			}
			if (bFILTERED)
			{
				if (Pane2.currentPos > iItemPositions[MSG_MESSAGE]) display_line(&Pane2);
			}

			memset(szLabelspacing, 0, sizeof(szLabelspacing));
			memset(szLabelspacing, ' ', iItemPositions[MSG_MESSAGE]-iPanePos);

			sprintf(szCurrentLabel[1], "- %s -", szCurrentLabel[0]);	// Create "- label -" for logfiles

			if (bMONITOR_ONLY)
			{
				// PH: Get/show label in monitor pane
				display_show_strV2(&Pane1, szLabelspacing);		// First, show the correct # of spaces
				display_color(&Pane1, dwColor);
				display_show_strV2(&Pane1, szCurrentLabel[0]);	// Show monitorlabel in Pane1
			}
			else if (bFILTERED)
			{
				// PH: Get/show filterlabel in filter pane
				display_show_strV2(&Pane2, szLabelspacing);		// First, show the correct # of spaces
				display_color(&Pane2, dwColor);
				display_show_strV2(&Pane2, szCurrentLabel[0]);	// Show filterlabel in Pane2
			
				if (Profile.LabelLog && bMONITOR)
				{
					// PH: Show filterlabel also in monitor pane
					display_show_strV2(&Pane1, szLabelspacing);		// First, show the correct # of spaces
					display_color(&Pane1, dwColor);
					display_show_strV2(&Pane1, szCurrentLabel[0]);	// Show filterlabel in Pane1
				}
			}
		}	// end of filter label

		if (iPanePos)
		{
			if (bMONITOR)  display_line(&Pane1);		// Ensure last line is displayed.
			if (bFILTERED) display_line(&Pane2);		// Ensure last line is displayed.
		}

		if (bMATCH && Profile.filter_cmd_file_enabled) // Activate commandfile, if enabled
		{
			if (Profile.filters[iMatch].cmd_enabled && Profile.filter_cmd[0])
			{
				ActivateCommandFile();
			}
		}

		if (!iConvertingGroupcall)
		{
			CreateDateFilename("", NULL);	// Get current date to use as filename
		}

		if (Profile.logfile_enabled && bMONITOR && !pLogFile)
		{
			LogFileHandling(MONITOR, szFilename, OPEN_FILE);

			bNewFile = (!FileExists(szFilename)) ? true : false;
			pLogFile = fopen(szFilename, "a");
		}

		if (pLogFile)
		{
			bNewLine = (bSeparator[MONITOR] && !bNewFile) ? true : false;

			if (Profile.FlexGroupMode & FLEXGROUPMODE_LOGGING)
			{
				if (isdigit(szLogFileLine[0]) && !bLogged[MONITOR] && !bCombine)
				{
					fprintf(pLogFile, "%s%s", bNewLine ? "\n" : "", szLogFileLine);
				}
				fprintf(pLogFile, "%s    %s  %s\n", bFragment ? szFragment : "               ", Current_MSG[MSG_CAPCODE], szCurrentLabel[0]);

				if (!bLogged[MONITOR])
				{
					bLogged[MONITOR] = true;
				}
			}
			else
			{
				CollectLogfileLine(Profile.ColLogfile, false);

				if (szCurrentLabel[1][0] && Profile.LabelLog) // PH: Add labels also in logfile
				{
					if (Profile.LabelNewline)
					{
						strcat(szLogFileLine, "\n");
						memset(szLabelspacing,  0, sizeof(szLabelspacing));
						memset(szLabelspacing, 32, iLabelspace_Logfile[MONITOR]+1);
						strcat(szLogFileLine, szLabelspacing);
					}
					else strcat(szLogFileLine, " ");
					strcat(szLogFileLine, szCurrentLabel[1]);
				}
				strcat(szLogFileLine, "\n");

				fprintf(pLogFile, "%s%s", bNewLine ? "\n" : "", szLogFileLine);
			}
		}

		if (bMATCH)
		{
			if (!(Profile.FlexGroupMode & FLEXGROUPMODE_LOGGING))
			{
				CollectLogfileLine(Profile.ColFilterfile, true);

				if (szCurrentLabel[1][0])
				{
					if (Profile.LabelNewline)
					{
						strcat(szLogFileLine, "\n");
						memset(szLabelspacing, 0,  sizeof(szLabelspacing));
						memset(szLabelspacing, 32, iLabelspace_Logfile[FILTER]+1);
						strcat(szLogFileLine, szLabelspacing);
					}
					else strcat(szLogFileLine, " ");
					strcat(szLogFileLine, szCurrentLabel[1]);
				}
				strcat(szLogFileLine, "\n");
			}

			if (bFILTERED)
			{
				if (Profile.filterfile_enabled && !pFilterFile)
				{
					LogFileHandling(FILTER, szFilename, OPEN_FILE);

					bNewFile = (!FileExists(szFilename)) ? true : false;
					pFilterFile = fopen(szFilename, "a");
				}
				if (pFilterFile)		// PH: Write current message to filterfile
				{
					bNewLine = (bSeparator[FILTER] && !bNewFile) ? true : false;

					if (Profile.FlexGroupMode & FLEXGROUPMODE_LOGGING)
					{
						if (isdigit(szLogFileLine[0]) && !bLogged[FILTER] && !bCombine)
						{
							fprintf(pFilterFile, "%s%s", bNewLine ? "\n" : "", szLogFileLine);
							bLogged[FILTER]=true;
						}
						fprintf(pFilterFile, "%s    %s  %s\n", bFragment ? szFragment : "               ", Current_MSG[MSG_CAPCODE], szCurrentLabel[0]);
					}
					else fprintf(pFilterFile, "%s%s", bNewLine ? "\n" : "", szLogFileLine);
				}
			}

			if (Profile.filters[iMatch].sep_filterfile_en && Profile.filters[iMatch].sep_filterfiles)
			{
				for (int k=0; k<Profile.filters[iMatch].sep_filterfiles; k++)
				{
					LogFileHandling(SEPARATE+k, szSepfilenames[CURRENT], OPEN_FILE);

					for (iSepfile=1; iSepfile<MAX_SEPFILES; iSepfile++)
					{
						if (stricmp(szSepfilenames[CURRENT], szSepfilenames[iSepfile]) == 0)
						{
							bNewLine = false;
							bLogged[SEPARATE] = true;
							break;	// Exit if filename already exists
						}
						else if (!szSepfilenames[iSepfile][0])
						{
							bNewLine = (Profile.Separator && FileExists(szSepfilenames[CURRENT])) ? true : false;
							strcpy(szSepfilenames[iSepfile], szSepfilenames[CURRENT]);
							bLogged[SEPARATE] = false;
							break;
						}
					}
					if (!pSepFilterFiles[iSepfile]) pSepFilterFiles[iSepfile] = fopen(szSepfilenames[CURRENT], "a");

					if (pSepFilterFiles[iSepfile])		// PH: Write current message to separate filterfile
					{
						if (Profile.FlexGroupMode & FLEXGROUPMODE_LOGGING)
						{
							if (!bLogged[SEPARATE] && isdigit(szLogFileLine[0]))
							{
								fprintf(pSepFilterFiles[iSepfile], "%s%s", bNewLine ? "\n" : "", szLogFileLine);
							}
							fprintf(pSepFilterFiles[iSepfile], "%s    %s  %s\n", bFragment ? szFragment : "               ", Current_MSG[MSG_CAPCODE], szCurrentLabel[0]);
						}
						else fprintf(pSepFilterFiles[iSepfile], "%s%s", bNewLine ? "\n" : "", szLogFileLine);
					}
				}
			}

			if (Profile.filterbeep)	// Playsound to indicate that a filtered item was received
			{
				if (PlayWaveFile(bMONITOR_ONLY, bFILTERED, false)) bPlayWaveFile=true;
			}
		}
	} // if (bShowMessage)

	if (!iConvertingGroupcall || bGroupcode)
	{
		LogFileHandling(NULL, NULL, CLOSE_FILES);

		if (!(Profile.FlexGroupMode & FLEXGROUPMODE_COMBINE)) memset(szSepfilenames, 0, sizeof(szSepfilenames));

		if (bPlayWaveFile)
		{
			PlayWaveFile(NULL, NULL, true);
			bPlayWaveFile = false;
		}
		
		if (bFragment) bFragment=false;

		ResetBools();
/*
		bShown[MONITOR] = false;
		bShown[FILTER]  = false;

		bLogged[MONITOR]  = false;
		bLogged[FILTER]   = false;
		bLogged[SEPARATE] = false;
*/
		memset(message_buffer, 0, sizeof(message_buffer));
	}

	if (Profile.SMTP)
	{
		SendMail(0, bMATCH, bMONITOR_ONLY,
					bMATCH ? Profile.filters[iMatch].smtp : 0,	// if no iMatch, smtp=0
					Current_MSG[MSG_CAPCODE],
					Current_MSG[MSG_TIME],
					Current_MSG[MSG_DATE],
					Current_MSG[MSG_MODE],
					Current_MSG[MSG_TYPE],
					Current_MSG[MSG_BITRATE],
					iMOBITEX ? Current_MSG[MSG_MOBITEX] : Current_MSG[MSG_MESSAGE],
					szCurrentLabel[0]);
	}

	if (Current_MSG[MSG_MOBITEX][0]) Current_MSG[MSG_MOBITEX][0] = '\0';

} // end of ShowMessage()


bool BlockChecker(char *address, int fnu, char *message, bool reject)
{
	extern int nCount_BlockBuffer[2];

	bool bBlock=false;

	int i, j, sum=0;
	int BlockTimer   = (Profile.BlockDuplicate >> 4) * 60;
	int BlockOnlyMsg =((Profile.BlockDuplicate & BLOCK_OPTION) == BLOCK_ONLYMSG);

	char temp[10];

	unsigned long int lChecksum=0, lAddress=0;

	if (!iConvertingGroupcall && (Profile.BlockDuplicate & BLOCK_OPTION) != BLOCK_TIMER)	// Not using the timer, only check last message
	{
		if (CompareMessage(MSG_CAPCODE, MONITOR) ||		// Compare capcodes/addresses
			(BlockOnlyMsg))								// or ignore if only comparing message
		{
			if (CompareMessage(MSG_MESSAGE, MONITOR) &&	// Compare messages
				CompareMessage(MSG_TYPE, MONITOR))		// Compare types (mostly for Mobitex)
			{
				bBlock=true;
			}
		}
	}
	else if (BlockTimer)
	{
		for (i=0, j=0; i<strlen(message); i++)
		{
			if ((i < 10) && isdigit(message[i]))	// Treat strings of numbers as one big number
			{
				j=i;
				while (isdigit(message[i]) && (i <= 10)) i++;
				strncpy(temp, &message[j], i-j);
				temp[i-j] = '\0';
				sum += atoi(temp);
				i--;
				continue;
			}
			sum += (message[i] - 31);		// Sum up ASCII-values
		}
		lChecksum = MAKELONG(sum, strlen(message));	// Checksum is combination of ASCII-sum and number of characters

		if (reject)							// At this point, we don't want to show a FLEX-groupcode,
		{									// but we didn't show any capcodes either,
			for (i=0; i<1000; i++)			// so let's assume that all capcodes are rejected
			{								// and current groupcall can be removed from array
				if (aMessages[i+1][BLOCK_ADDRESS] == 0)
				{
					if (aMessages[i][BLOCK_CHECKSUM] == lChecksum)	// Message the same?
					{
						memset(aMessages[i], 0, sizeof(aMessages[i]));
						break;
					}
				}
			}
		}
		else
		{
			sprintf(temp, "%i%s", fnu, address);

			lAddress = atoi(temp);

			for (i=0; i<1000; i++)
			{
				if (aMessages[i][BLOCK_ADDRESS])
				{
					if (aMessages[i][BLOCK_ADDRESS] == lAddress)		// Address the same?
					{
						if ((aMessages[i][BLOCK_TIME]+BlockTimer) > iSecondsElapsed)	// Less then blocktimer?
						{
							if (aMessages[i][BLOCK_CHECKSUM] == lChecksum)	// Message the same?
							{
								bBlock=true;						// Block this message
								break;
							}
						}
					}
				}
			}
			if (!bBlock)	// If block-timer && not blocking current message && no groupcall, put current message in array aMessages
			{
				if (aMessages[999][BLOCK_ADDRESS])	// Array full?
				{
					memmove(aMessages[0], aMessages[1], sizeof(aMessages));
				}
				for (i=0; i<1000; i++)
				{
					if (aMessages[i][BLOCK_ADDRESS] == 0)
					{
						memset(aMessages[i], 0, sizeof(aMessages));

						aMessages[i][BLOCK_ADDRESS]  = lAddress;
						aMessages[i][BLOCK_TIME]     = iSecondsElapsed;
						aMessages[i][BLOCK_CHECKSUM] = lChecksum;

						break;
					}
				}
			}
		}
	}

	for (i=0; aMessages[i][BLOCK_ADDRESS]; i++);

	nCount_BlockBuffer[0] = i;

	if (nCount_BlockBuffer[0] > nCount_BlockBuffer[1]) nCount_BlockBuffer[1]=nCount_BlockBuffer[0];

	return (bBlock);
}


void ResetBools()
{
	bShown[MONITOR] = false;
	bShown[FILTER]  = false;

	bLogged[MONITOR]  = false;
	bLogged[FILTER]   = false;
	bLogged[SEPARATE] = false;
}


char LogFileHandling(int file, char *szFileName, int action)
{
	int  i=1, UseDate=0;
	char filename[MAX_PATH];		// Temp buffer for setting filenames
	char ext[5];					// Temp buffer for file extension
	
	CreateDateFilename("", NULL);	// TEST/TEMP

	if (action == OPEN_FILE)
	{
		switch (file)
		{
			case MONITOR:
			strcpy(filename, Profile.logfile);
			strcpy(ext, ".log");
			UseDate = Profile.logfile_use_date;
			break;

			case FILTER:
			strcpy(filename, Profile.filterfile);
			strcpy(ext, ".flt");
			UseDate = Profile.filterfile_use_date;
			break;

			case SEPARATE:
			strcpy(filename, Profile.filters[iMatch].sep_filterfile[0]);
			break;
			case SEPARATE+1:
			strcpy(filename, Profile.filters[iMatch].sep_filterfile[1]);
			break;
			case SEPARATE+2:
			strcpy(filename, Profile.filters[iMatch].sep_filterfile[2]);
			break;

			default:
			break;
		}
		szFileName[0] = '\0';

		if (!FileExists(Profile.LogfilePath)) CreateDirectory(Profile.LogfilePath, NULL);

		if (strstr(filename, "\\") == 0)	// if "\" is in filename
		{
			sprintf(szFileName, "%s\\", Profile.LogfilePath);
		}
		if (UseDate)
		{
			strcat(szFileName, szFilenameDate);
			strcat(szFileName, ext);
		}
		else strcat(szFileName, filename);

		return(*szFileName);
	}
	else // if (action == CLOSE_FILES)
	{
		if (pLogFile)						// PH: If active, close logfile
		{
			fclose(pLogFile);
			pLogFile = NULL;
		}
		if (pFilterFile)					// PH: If active, close filterfile
		{
			fclose(pFilterFile);
			pFilterFile = NULL;
		}

		for (int i=0; i<MAX_SEPFILES; i++)
		{
			if (pSepFilterFiles[i])
			{
				fclose(pSepFilterFiles[i]);
				pSepFilterFiles[i] = NULL;
			}
		}
		return(FALSE);
	}
}


int CompareMessage(int item, int mon_or_filt)
{
	if ((item == MSG_MESSAGE) && !Current_MSG[item][0] && !Previous_MSG[mon_or_filt][item][0]) return true;

	if (strcmp(Current_MSG[item], Previous_MSG[mon_or_filt][item]) != 0) return false;

	return true;
}


char *MakeFilterLabel(char *szLabel, char *szCapcode, char *szNewLabel)
{
	unsigned max = strlen(szCapcode), pos=0;
	bool bFound=false;

	unsigned tmp;

	while (*szLabel)
	{
		if (*szLabel == '%')
		{
			szLabel++;
			while ((tmp = *szLabel - '1') < max)
			{
				*szNewLabel++ = szCapcode[tmp];
				szLabel++;
			}
			*szNewLabel++ = *szLabel++;
		}
		else *szNewLabel++ = *szLabel++;
	}
	*szNewLabel = '\0';
	return (szNewLabel);
}


bool PlayWaveFile(bool bMONITOR_ONLY, bool bFILTERED, bool bPlay)
{
	char *p={0};
	char szText[FILTER_TEXT_LEN+2]="";
	char szCapcode[FILTER_CAPCODE_LEN+1];
	static int Prio[2] = { 19, 19 };		// PH: Holds CURRENT/PREVIOUS priority
	static int PrioTMP[2] = { 19, 19 };		// PH: Holds TEMP priorities
	static char szWavefile[2][MAX_PATH];	// PH: Buffer for CURRENT/PREVIOUS wavefilename
	static char szWavefileTMP[2][MAX_PATH];	// PH: Buffer TEMP wavefilenames
	static DWORD dwFileSize[2];				// PH: Holds CURRENT/PREVIOUS filesize
	static DWORD dwFileSizeTMP[2];			// PH: Holds TEMP filesizes

	if (!bPlay)
	{
		PrioTMP[CURRENT] = bFILTERED ? 0 : 10;

		// Prio's :
		//
		//  0 : Filtered  / 1234567-text.wav / textfilter
		//  1 : Filtered  / 1234567.wav
		//  2 : Filtered  / 123456?.wav
		//  3 : Filtered  / 12345??.wav
		//  4 : Filtered  / 1234???.wav
		//  5 : Filtered  / 123????.wav
		//  6 : Filtered  / 12?????.wav
		//  7 : Filtered  / 1??????.wav
		//  8 : Filtered  / ???????.wav
		//  9 : Filtered  / SoundX.wav
		// 10 : Monitored / 1234567-text.wav / textfilter
		// 11 : Monitored / 1234567.wav
		// 12 : Monitored / 123456?.wav
		// 13 : Monitored / 12345??.wav
		// 14 : Monitored / 1234???.wav
		// 15 : Monitored / 123????.wav
		// 16 : Monitored / 12?????.wav
		// 17 : Monitored / 1??????.wav
		// 18 : Monitored / ???????.wav
		// 19 : Monitored / monitor-only.wav

		if ((Profile.filterbeep == 2) && bMONITOR_ONLY)
		{
			return (false);
		}

		if (Profile.filters[iMatch].capcode[0])
		{
			if (Profile.filters[iMatch].text[0])
			{
				sprintf(szText, "-%s", Profile.filters[iMatch].text);
			}
			sprintf(szWavefileTMP[CURRENT], "%s\\%s%s.wav", szWavePathName, Current_MSG[MSG_CAPCODE], szText);

			if ((!FileExists(szWavefileTMP[CURRENT])) && (strstr(Profile.filters[iMatch].capcode, "?")))
			{
				strcpy(szCapcode, Profile.filters[iMatch].capcode+(strlen(Profile.filters[iMatch].capcode)-strlen(Current_MSG[MSG_CAPCODE])));

				while (p=strchr(szCapcode, '?'))
				{
					*p='X';
					PrioTMP[CURRENT]++;
				}
				sprintf(szWavefileTMP[CURRENT], "%s\\%s%s.wav", szWavePathName, szCapcode, szText);
			}
			if (Profile.filters[iMatch].text[0]) PrioTMP[CURRENT]--;
		}
		else if (Profile.filters[iMatch].text[0])
		{
			sprintf(szWavefileTMP[CURRENT], "%s\\%s.wav", szWavePathName, Profile.filters[iMatch].text);
		}

		if (!FileExists(szWavefileTMP[CURRENT]))		// PH: If code.wav is not found
		{
			if (Profile.filters[iMatch].wave_number > 0)
			{
				if (bMONITOR_ONLY)
				{
					sprintf(szWavefileTMP[CURRENT], "%s\\monitor_only.wav", szWavePathName);
					PrioTMP[CURRENT]=19;
				}
				else if (bFILTERED)		// PH: else play sound*.wav
				{
					sprintf(szWavefileTMP[CURRENT], "%s\\Sound%i.wav", szWavePathName, Profile.filters[iMatch].wave_number-1);
					PrioTMP[CURRENT]=9;
				}
			}
		}
		if (FileExists(szWavefileTMP[CURRENT]))		// PH: Check if code.wav exists
		{
			HANDLE hFile =  CreateFile(szWavefileTMP[CURRENT],
								GENERIC_READ,
								FILE_SHARE_READ,
								NULL,
								OPEN_EXISTING,
								FILE_ATTRIBUTE_NORMAL,
								NULL);

			dwFileSizeTMP[CURRENT] = GetFileSize(hFile, NULL);	// Get the current file size

			CloseHandle(hFile);
			hFile = NULL;

			if (((PrioTMP[CURRENT]  < PrioTMP[PREVIOUS]) && strcmp(szWavefileTMP[PREVIOUS], szWavefileTMP[CURRENT]) != 0) ||
				((PrioTMP[CURRENT] == PrioTMP[PREVIOUS]) && (dwFileSizeTMP[PREVIOUS] < dwFileSizeTMP[CURRENT])) ||
				  szWavefile[CURRENT][0] == 0)
			
			{
				strcpy(szWavefile[CURRENT], szWavefileTMP[CURRENT]);
				Prio[CURRENT] = PrioTMP[CURRENT];
				dwFileSize[CURRENT]=dwFileSizeTMP[CURRENT];
			}
			else return (true);

			strcpy(szWavefileTMP[PREVIOUS], szWavefileTMP[CURRENT]);
			PrioTMP[PREVIOUS] = PrioTMP[CURRENT];
			dwFileSizeTMP[PREVIOUS]=dwFileSizeTMP[CURRENT];

			return (true);
		}
	}
	else
	{
		if (!sndPlaySound(szWavefile[CURRENT], SND_FILENAME|SND_ASYNC|SND_NOSTOP))
		{
			if (((Prio[CURRENT]  < Prio[PREVIOUS]) && strcmp(szWavefile[PREVIOUS], szWavefile[CURRENT]) != 0) ||
				((Prio[CURRENT] == Prio[PREVIOUS]) && (dwFileSize[PREVIOUS] < dwFileSize[CURRENT])))
			{
				sndPlaySound(szWavefile[CURRENT], SND_FILENAME|SND_ASYNC);
			}
		}
		strcpy(szWavefile[PREVIOUS], szWavefile[CURRENT]);
		Prio[PREVIOUS] = Prio[CURRENT];
		dwFileSize[PREVIOUS]=dwFileSize[CURRENT];

		strcpy(szWavefile[CURRENT], "");
		Prio[CURRENT] = 19;
		dwFileSize[CURRENT] = 0;
	}

	return (false);
}


int Check_4_Filtermatch()
{
	int msg_len, code_len, txt_len, nFilters;
	int mode = 0;
	int i=0, j=0, k=0, l=0, m=0, n=0, pos=0;

	bool bCompare=false, bCorrectHeader=false, bKeyword=false;

	char szTextTMP[FILTER_TEXT_LEN+1]="";
	char* pSearch={0};

	iMatch = -1;			// PH: Set to filter-# if current message matches a filter

	if ((Profile.monitor_mobitex && strstr(Current_MSG[MSG_TYPE], "<SVP"))) return (-1);

	if (!Current_MSG[MSG_CAPCODE][0]) return (-1);

	if (Profile.monitor_paging)
	{
		if (strstr(Current_MSG[MSG_MODE], "FLEX"))
		{
			mode=FLEX_FILTER;
		}
		else if (strstr(Current_MSG[MSG_MODE], "POCSAG"))
		{
			mode=POCSAG_FILTER;
		}
	}
	else if (Profile.monitor_ermes)
	{
		mode=ERMES_FILTER;
	}
	else if (Profile.monitor_acars)
	{
		mode=ACARS_FILTER;
	}
	else if (Profile.monitor_mobitex)
	{
		mode=MOBITEX_FILTER;
	}

	code_len = strlen(Current_MSG[MSG_CAPCODE]);
	msg_len  = strlen(Current_MSG[MSG_MESSAGE]);
	nFilters = Profile.filters.size();

	// check if address matched a filter (iAddrMatch is filter index)
	// we may need to iMatch text strings for this address as well...
	for (int iFilter=0; iFilter<nFilters; iFilter++, i=j=k=l=0)
	{
		iAddrMatch  = -1;	// Set to filter-# if current message matches a capcode
		iTextMatch  = -1;	// If text-iMatch, set to position where filtertext starts
		iTextLength =  0;	// If single text-iMatch, set to text length

		if (iTextLengths[0])
		{
			for (i=0; i<10 && iTextLengths[i]; i++)
			{
				iTextPositions[i]=0;
				iTextLengths[i]=0;
			}
		}

		if (Profile.filters[iFilter].type == mode)
		{
			if (mode == FLEX_FILTER)
			{
				if (strlen(Profile.filters[iFilter].capcode) != code_len)
				{
					continue;	// Skip if short address & long filter, skip if filter starts with 00
				}
			}

			if (mode == MOBITEX_FILTER)
			{
				switch (Profile.filters[iFilter].capcode[strlen(Profile.filters[iFilter].capcode)-2])
				{
					case 'R' :

					if (filter_addr(Current_MSG[MSG_CAPCODE], Profile.filters[iFilter].capcode))
					{
						iAddrMatch = iFilter;
					}
					break;

					case 'T' :

					if (filter_addr(Current_MSG[MSG_MODE], Profile.filters[iFilter].capcode))
					{
						iAddrMatch = iFilter;
					}
					break;

					default :

					if ((filter_addr(Current_MSG[MSG_CAPCODE], Profile.filters[iFilter].capcode)) ||
						(filter_addr(Current_MSG[MSG_MODE],    Profile.filters[iFilter].capcode)))
					{
						iAddrMatch = iFilter;
					}
					break;
				}
			}
			else
			{
				if (filter_addr(Current_MSG[MSG_CAPCODE], Profile.filters[iFilter].capcode))
				{
					if ((mode == POCSAG_FILTER) && Profile.filters[iFilter].capcode[7] == '-')
					{
						if (Current_MSG[MSG_MODE][7] == Profile.filters[iFilter].capcode[8])
						{
							iAddrMatch = iFilter;
						}
						else continue;
					}
					else iAddrMatch = iFilter;
				}
			}
		}

		// Is there (also) text that must be matched?
		if (Profile.filters[iFilter].type == TEXT_FILTER || (iAddrMatch == iFilter) && Profile.filters[iFilter].text[0])
		{
			txt_len = strlen(Profile.filters[iFilter].text);

			if (Profile.filters[iFilter].match_exact_msg)
			{
				if (stricmp(Current_MSG[MSG_MESSAGE], Profile.filters[iFilter].text) == 0)
				{
					iTextMatch = 0;
					iTextLength = txt_len;
				}
			}
			else if (txt_len <= msg_len)	// Text string has to be shorter than message
			{
				// now scan the temp_str for the temp_filter string...
				if (Profile.filters[iFilter].text[0] == '^')
				{
					if (strnicmp(Current_MSG[MSG_MESSAGE], &Profile.filters[iFilter].text[1], txt_len-1) == 0)
					{
						iTextMatch = 0;
						iTextLength = txt_len-1;
					}
				}
				else if (strstr(Profile.filters[iFilter].text, "&") != 0)
				{
					while (Profile.filters[iFilter].text[i] != 0 && l < 10)
					{
						while (Profile.filters[iFilter].text[i] != '&' && Profile.filters[iFilter].text[i] != 0)
						{
							szTextTMP[j++] = Profile.filters[iFilter].text[i++];
						}
						szTextTMP[j]='\0';

						pSearch = strstr(&Current_MSG[MSG_MESSAGE][k], szTextTMP);

						if (!pSearch)
						{
							if (iTextLengths[0])
							{
								for (i=0; i<10; i++)
								{
									iTextPositions[i]=0;
									iTextLengths[i]=0;
								}
							}
							break;
						}

						if (Profile.filters[iFilter].text[i] == '&')
						{
							i++;
							j=0;
						}
						k = (pSearch - Current_MSG[MSG_MESSAGE]);

						iTextPositions[l]=k;
						iTextLengths[l++]=strlen(szTextTMP);

						k += strlen(szTextTMP);
					}
					if (pSearch)
					{
						iTextMatch = iFilter;
					}
				}
				else
				{
					for (pos=0; pos <= (msg_len-txt_len); pos++)
					{
						if (strnicmp(&Current_MSG[MSG_MESSAGE][pos], Profile.filters[iFilter].text, txt_len) == 0)
						{
							iTextLength = txt_len;
							iTextMatch = pos;
							break;
						}
					}
				}
			}
			if (Profile.filters[iFilter].type == TEXT_FILTER || (iAddrMatch == iFilter))
			{
				if (iTextMatch != -1) iMatch = iFilter;
			}
			else continue;
		}
		else iMatch = iAddrMatch;

		if (iMatch != -1) break;
	}
	return (iMatch);
}


void ActivateCommandFile()
{
//	int  arg_pos=0, arg, tmp_index;		// Command file / argument stuff
	int  arg_pos=0, arg;
	int  pos, i=0;
//	char param_str[MAX_STR_LEN], tmp_fname[MAX_PATH], tmp_pagername[100], tmp[10];
	char param_str[MAX_STR_LEN], tmp_pagername[100], tmp[10];
	char szCommandFile[MAX_STR_LEN];	// was MAX_PATH
	char szLabel[FILTER_LABEL_LEN+50];
//	char ch;							// Buffer for current character

	tmp_pagername[0] = 0;

	while (Profile.filter_cmd_args[i] != 0)
	{
		if ((i > 254) || (arg_pos > FILTER_PARAM_LEN)) break;

		if (Profile.filter_cmd_args[i] == '%')
		{
			arg=atoi(&Profile.filter_cmd_args[i+1]);

			if (arg>0 && arg<8)
			{
				for (pos=0; Current_MSG[arg][pos] != 0; pos++, arg_pos++)
				{
					if (Profile.monitor_mobitex && (arg==7) && (Current_MSG[7][pos] == '"' || Current_MSG[7][pos] == '\''))
					{
						param_str[arg_pos] = ' ';
					}
					else param_str[arg_pos] = Current_MSG[arg][pos];
				}
				i+=2;
			}
			else if (Profile.filter_cmd_args[i+1] == '8')
			{
				MakeFilterLabel(Profile.filters[iMatch].label, Current_MSG[MSG_CAPCODE], szLabel);
				pos = 0;
				while (szLabel[pos] != 0)
				{
					param_str[arg_pos++] = szLabel[pos++];
				}
				i+=2;
			}
			else if ((Profile.filter_cmd_args[i+1] == 'c') ||
					 (Profile.filter_cmd_args[i+1] == 'C'))
			{
				sprintf(tmp, "%02i", iCurrentCycle);
				pos = 0;
				while (tmp[pos] != 0)
				{
					param_str[arg_pos++] = tmp[pos++];
				}
				i+=2;
			}
			else if ((Profile.filter_cmd_args[i+1] == 'r') ||
					 (Profile.filter_cmd_args[i+1] == 'R'))
			{
				sprintf(tmp, "%03i", iCurrentFrame);
				pos = 0;
				while (tmp[pos] != 0)
				{
					param_str[arg_pos++] = tmp[pos++];
				}
				i+=2;
			}
/*			else if ((Profile.filter_cmd_args[i+1] == 'f') ||
					 (Profile.filter_cmd_args[i+1] == 'F'))
			{
				i+=3;
				tmp_index=0;
				tmp_fname[0] = 0;

				// get temp filename
				while ((Profile.filter_cmd_args[i] != '>') &&
					   (Profile.filter_cmd_args[i] != 0))
				{
					tmp_fname[tmp_index++] = Profile.filter_cmd_args[i++]; 
				}

				if (Profile.filter_cmd_args[i] == '>') i++;

				tmp_fname[tmp_index] = 0;

				if (tmp_fname[0])
				{
					FILE *tmp_fp = NULL;

					if ((tmp_fp = fopen(tmp_fname,"w+")) != NULL)
					{
						// save message to temp file.
						if (tmp_pagername[0])	// Need to add name of
						{						// person to be paged?
							pos = 0;

							while (tmp_pagername[pos] != 0) 
							{
								ch = tmp_pagername[pos++];
								fwrite((char *)&ch, 1, 1, tmp_fp);
							}
						}
						pos = 0;
						while (message_buffer[pos] != 0)
						{
							ch = message_buffer[pos++];
							fwrite((char *)&ch, 1, 1, tmp_fp);
						}
						fclose(tmp_fp);
						tmp_fp = NULL;
					}
				}
			}
*/
			else
			{
				param_str[arg_pos++] = Profile.filter_cmd_args[i];
				i++;
			}
		}
		else
		{
			param_str[arg_pos++] = Profile.filter_cmd_args[i];
			i++;
		}
	}

	param_str[arg_pos] = 0;

	PROCESS_INFORMATION pif;	//Gives info on the thread and..
								//..process for the new process
	STARTUPINFO si;				//Defines how to start the program

	ZeroMemory(&si,sizeof(si));	//Zero the STARTUPINFO struct
	si.cb = sizeof(si);			//Must set size of structure

	strcpy(szCommandFile, Profile.filter_cmd);

	if (param_str[0])
	{
		strcat(szCommandFile, " ");
		strcat(szCommandFile, param_str);
	}
	if (strlen(szCommandFile) > MAX_STR_LEN) szCommandFile[MAX_STR_LEN] = 0;

	CreateProcess(NULL, szCommandFile, NULL, NULL, FALSE, NULL, 0, NULL, &si, &pif);

	// Close process and thread handles.
	CloseHandle(pif.hProcess);
	CloseHandle(pif.hThread);

//	MessageBox(ghWnd, szCommandFile, "PDW Commandfile", MB_ICONINFORMATION);
}


void CollectLogfileLine(char *string, bool bFilter)
{
	extern int FLEX_9;
	int spacing=0;

	szLogFileLine[0] = '\0';
	
	for (int col=1; col<8; col++)
	{
		if (col == 7)
		{
			iLabelspace_Logfile[bFilter ? FILTER : MONITOR] = strlen(szLogFileLine);
			spacing = strlen(szLogFileLine);
			strcat(szLogFileLine, " ");
		}

		if (strchr(string, '0'+col))
		{
			if (col == 7)
			{
				if (Profile.monitor_acars)
				{
					for (int pos=0; Current_MSG[MSG_MESSAGE][pos]!=0; pos++)
					{
						if (Current_MSG[MSG_MESSAGE][pos] == char(23))
						{
							strcat(szLogFileLine, "\n");
							for (int i=0; i<spacing+1; i++) strcat(szLogFileLine, " ");
						}
						else strncat(szLogFileLine, (char*)&Current_MSG[MSG_MESSAGE][pos], 1);
					}
				}
				else if (Profile.monitor_mobitex)
				{
					if (Current_MSG[MSG_MOBITEX][0] && bFilter)
					{
						for (int pos=0; Current_MSG[MSG_MOBITEX][pos]!=0; pos++)
						{
							if (Current_MSG[MSG_MOBITEX][pos] == '»')
							{
								strcat(szLogFileLine, "\n");
								for (int i=0; i<spacing+1; i++) strcat(szLogFileLine, " ");
							}
							else strncat(szLogFileLine, (char*)&Current_MSG[MSG_MOBITEX][pos], 1);
						}
					}
					else
					{
						strcat(szLogFileLine, Current_MSG[MSG_MESSAGE]);
					}
				}
				else if ((strstr(Current_MSG[MSG_MESSAGE], "»") != 0) && Profile.Linefeed)
				{
					for (int pos=0; Current_MSG[MSG_MESSAGE][pos]!=0; pos++)
					{
						if (Current_MSG[MSG_MESSAGE][pos] == '»')
						{
							strcat(szLogFileLine, "\n");
							for (int i=0; i<spacing+1; i++) strcat(szLogFileLine, " ");
						}
						else
						{
							strncat(szLogFileLine, (char*)&Current_MSG[MSG_MESSAGE][pos], 1);
						}
					}
				}
				else strcat(szLogFileLine, Current_MSG[MSG_MESSAGE]);
			}
			else strcat(szLogFileLine, Current_MSG[col]);
			
			if (col < 7) strcat(szLogFileLine, " ");

			if (col == 1 && Profile.monitor_paging && FLEX_9 > 25)
			{
				if (strlen(Current_MSG[MSG_CAPCODE]) == 7)
				{
					strcat(szLogFileLine, "  ");
				}
			}
		}
	}
}

void display_color(PaneStruct *pane, BYTE ct)
{
	pane->currentColor = ct;
	return;
} // end of display_color


void display_showmo(int mode)
{
	bMode_IDLE = mode ? false : true;

	if (bMode_IDLE)
	{
		strcpy(szWindowText[2], "");
		strcpy(szWindowText[3], "");
		strcpy(szWindowText[4], "");
	}
	else if (mode == MODE_ERMES)
	{
		strcpy(szWindowText[2], "ERMES 6250");
	}
	else if (mode == MODE_MOBITEX)
	{
		strcpy(szWindowText[2], "MOBITEX");
	}
	else if (mode == MODE_ACARS)
	{
		strcpy(szWindowText[2], "ACARS 2400");
	}
	else if ((mode & MODE_POCSAG) == MODE_POCSAG)
	{
		strcpy(szWindowText[2], "POCSAG-");

		if      ((mode & MODE_P512)  == MODE_P512)  strcat(szWindowText[2], "512");
		else if ((mode & MODE_P1200) == MODE_P1200) strcat(szWindowText[2], "1200");
		else if ((mode & MODE_P2400) == MODE_P2400) strcat(szWindowText[2], "2400");
	}
	else if ((mode & MODE_FLEX_A) == MODE_FLEX_A)		// must be FLEX or ReFLEX...
	{
		if ((mode & MODE_REFLEX) == MODE_REFLEX)		// PH: ReFLEX ?
		{
			strcpy(szWindowText[2], "REFLEX ");
		}
		else											// PH: FLEX
		{
			strcpy(szWindowText[2], "FLEX ");
		}
		if ((mode & MODE_FLEX_D) == MODE_FLEX_D)
		strcat(szWindowText[2], "4 level 6400 (Phases ABCD)");	// PH: added "4 level 6400"

		else if ((mode & MODE_FLEX_B) == MODE_FLEX_B)
		strcat(szWindowText[2], "4 level 3200 (Phases AB)");	// PH: added "4 level 3200"

		else if ((mode & MODE_FLEX_C) == MODE_FLEX_C)
		strcat(szWindowText[2], "2 level 3200 (Phases AC)");	// PH: added "2 level 3200"

		else if ((mode & MODE_FLEX_A) == MODE_FLEX_A)
		strcat(szWindowText[2], "2 level 1600 (Phase A)");		// PH: added "2 level 1600"

		if (Profile.show_cfs)	// PH: Collect cycle/frame status
		{
			sprintf(szWindowText[3], "Cycle:%02i Frame:%03i", iCurrentCycle, iCurrentFrame);
		}
	}
	SetNewWindowText("");

} // end of display_showmo()


void CreateDateFilename(char *ext, SYSTEMTIME *yesterday)
{
	SYSTEMTIME now;
	char *months[12] = {"JAN", "FEB", "MAR", "APR", "MAY", "JUN",
						"JUL", "AUG", "SEP", "OCT", "NOV", "DEC"};

	if (yesterday) now=*yesterday;
	else GetLocalTime(&now);

	if (Profile.MonthNumber)
	{
		 sprintf(szFilenameDate, "%02d%02d%02d%s", now.wYear % 100, now.wMonth, now.wDay, ext);
	}
	else sprintf(szFilenameDate, "%02d%s%02d%s", now.wYear % 100, months[now.wMonth-1], now.wDay, ext);

	return;
}


void display_show_hex21(PaneStruct *pane, long int l)
{
	char tt[10];

	sprintf(tt, "%06lX", l);
	if (l > 0x3fffffl) display_color(pane, COLOR_BITERRORS);
	display_show_str(pane, tt);
}


void display_show_hex16(PaneStruct *pane, int l)
{
	char tt[10];

	sprintf(tt, "%04X", l);
	display_show_str(pane, tt);
}


//int filter_addr(char addr_str[], char filter_str[], int size)
int filter_addr(char addr_str[], char filter_str[])
{
	int i=0;

	int size=strlen(addr_str);

	while (i < size)
	{
		if ((filter_str[i] == '?') || (addr_str[i] == filter_str[i]))
		{
			i++;
		}
		else return 0;
	}
	return 1;
}


int short_nOnes(char k)
{
	int kt=0;

	if (k == 0) return(0);

	for (int i=0; i<=7; i++)
	{
		if ((k & 0x01) != 0) kt++;
		k = k >> 1;
	}
	return(kt);
}


int nOnes(int k)
{
	int kt=0;

	if (k == 0) return(0);

	for (int i=0; i<=15; i++)
	{
		if ((k & 0x0001) != 0) kt++;
		k = k >> 1;
	}
	return(kt);
}


int bit10(int gin)
{
	int k=0;

	for (int i=0; i<10; i++)
	{
		if ((gin & 0x01) != 0) k++;
		gin = gin >> 1;
	}
	return(k);
}


int ecd()
{
	int synd, b1, b2, i;
	int errors=0, parity=0;

	int ecc = 0x000;
	int acc = 0;

	// run through error detection and correction routine

	for (i=0; i<=20; i++)
	{
		if (ob[i] == 1)
		{
			ecc = ecc ^ ecs[i];
			parity = parity ^ 0x01;
		}
	}

	for (i=21; i<=30; i++)
	{
		acc = acc << 1;
		if (ob[i] == 1) acc = acc ^ 0x01;
	}

	synd = ecc ^ acc;

	if (synd != 0) // if nonzero syndrome we have error
	{
		if (bch[synd] != 0) // check for correctable error
		{
			b1 = bch[synd] & 0x1f;
			b2 = bch[synd] >> 5;
			b2 = b2 & 0x1f;

			if (b2 != 0x1f)
			{
				ob[b2] = ob[b2] ^ 0x01;
				ecc = ecc ^ ecs[b2];
			}

			if (b1 != 0x1f)
			{
				ob[b1] = ob[b1] ^ 0x01;
				ecc = ecc ^ ecs[b1];
			}
			errors = bch[synd] >> 12;
		}
		else errors = 3;

		if (errors == 1) parity = parity ^ 0x01;
	}

	// check parity ....
	parity = (parity + bit10(ecc)) & 0x01;

	if (parity != ob[31]) errors++;

	if (errors > 3) errors = 3;

	CountBiterrors(errors);

	return(errors);
}


void setupecc()
{
	unsigned int srr, j, k;
	int i, n;

	// calculate all information needed to implement error correction
	srr = 0x3B4;

	for (i=0; i<=20; i++)
	{
		ecs[i] = srr;
		if ((srr & 0x01) != 0) srr = (srr >> 1) ^ 0x3B4;
		else                   srr = srr >> 1;
	}

	// bch holds a syndrome look-up table telling which bits to correct
	// first 5 bits hold location of first error; next 5 bits hold location
	// of second error; bits 12 & 13 tell how many bits are bad
	for (i=0; i<1024; i++) bch[i] = 0;

	for (n=0; n<=20; n++)	// two errors in data
	{
		for (i=0; i<=20; i++)
		{
			j = (i << 5) + n;
			k = ecs[n] ^ ecs[i];
			bch[k] = j + 0x2000;
		}
	}

	// one error in data
	for (n=0; n<=20; n++)
	{
		k = ecs[n];
		j = n + (0x1f << 5);
		bch[k] = j + 0x1000;
	}

	// one error in data and one error in ecc portion
	for (n=0; n<=20; n++)
	{
		for (i=0; i<10; i++)  // ecc screwed up bit
		{
			k = ecs[n] ^ (1 << i);
			j = n + (0x1f << 5);
			bch[k] = j + 0x2000;
		}
	}

	// one error in ecc
	for (n=0; n<10; n++)
	{
		k = 1 << n;
		bch[k] = 0x3ff + 0x1000;
	}

	// two errors in ecc
	for (n=0; n<10; n++)
	{
		for (i=0; i<10; i++)
		{
			if (i != n)
			{
				k = (1 << n) ^ (1 << i);
				bch[k] = 0x3ff + 0x2000;
			}
		}
	}
}


void WriteStatFileHourly(FILE *fp)
{
	long num_msg, num_char, alpha_msg, alpha_char;

	fprintf(fp, "              Numeric   Numeric      Alpha     Alpha\n");
	fprintf(fp, "                 Msgs     Chars       Msgs     Chars\n");

	fprintf(fp, "POCSAG-512   %8ld %9ld   %8ld %9ld\n",
				hourly_stat[STAT_POCSAG512][STAT_NUMERIC],
				hourly_char[STAT_POCSAG512][STAT_NUMERIC],
				hourly_stat[STAT_POCSAG512][STAT_ALPHA],
				hourly_char[STAT_POCSAG512][STAT_ALPHA]);

	fprintf(fp ,"POCSAG-1200  %8ld %9ld   %8ld %9ld\n",
				hourly_stat[STAT_POCSAG1200][STAT_NUMERIC],
				hourly_char[STAT_POCSAG1200][STAT_NUMERIC],
				hourly_stat[STAT_POCSAG1200][STAT_ALPHA],
				hourly_char[STAT_POCSAG1200][STAT_ALPHA]);

	fprintf(fp ,"POCSAG-2400  %8ld %9ld   %8ld %9ld\n",
				hourly_stat[STAT_POCSAG2400][STAT_NUMERIC],
				hourly_char[STAT_POCSAG2400][STAT_NUMERIC],
				hourly_stat[STAT_POCSAG2400][STAT_ALPHA],
				hourly_char[STAT_POCSAG2400][STAT_ALPHA]);

	fprintf(fp, "FLEX-1600    %8ld %9ld   %8ld %9ld\n",
				hourly_stat[STAT_FLEX1600][STAT_NUMERIC],
				hourly_char[STAT_FLEX1600][STAT_NUMERIC],
				hourly_stat[STAT_FLEX1600][STAT_ALPHA],
				hourly_char[STAT_FLEX1600][STAT_ALPHA]);

	fprintf(fp, "FLEX-3200    %8ld %9ld   %8ld %9ld\n",
				hourly_stat[STAT_FLEX3200][STAT_NUMERIC],
				hourly_char[STAT_FLEX3200][STAT_NUMERIC],
				hourly_stat[STAT_FLEX3200][STAT_ALPHA],
				hourly_char[STAT_FLEX3200][STAT_ALPHA]);

	fprintf(fp, "FLEX-6400    %8ld %9ld   %8ld %9ld\n",
				hourly_stat[STAT_FLEX6400][STAT_NUMERIC],
				hourly_char[STAT_FLEX6400][STAT_NUMERIC],
				hourly_stat[STAT_FLEX6400][STAT_ALPHA],
				hourly_char[STAT_FLEX6400][STAT_ALPHA]);

	fprintf(fp, "ACARS-2400   %8ld %9ld   %8ld %9ld\n",
				hourly_stat[STAT_ACARS2400][STAT_NUMERIC],
				hourly_char[STAT_ACARS2400][STAT_NUMERIC],
				hourly_stat[STAT_ACARS2400][STAT_ALPHA],
				hourly_char[STAT_ACARS2400][STAT_ALPHA]);

	fprintf(fp, "MOBITEX      %8ld %9ld   %8ld %9ld\n",
				hourly_stat[STAT_MOBITEX][STAT_NUMERIC],
				hourly_char[STAT_MOBITEX][STAT_NUMERIC],
				hourly_stat[STAT_MOBITEX][STAT_ALPHA],
				hourly_char[STAT_MOBITEX][STAT_ALPHA]);

	fprintf(fp, "ERMES        %8ld %9ld   %8ld %9ld\n",
				hourly_stat[STAT_ERMES][STAT_NUMERIC],
				hourly_char[STAT_ERMES][STAT_NUMERIC],
				hourly_stat[STAT_ERMES][STAT_ALPHA],
				hourly_char[STAT_ERMES][STAT_ALPHA]);

	fprintf(fp, "             -------- ---------   -------- ---------\n");

	num_msg	=	hourly_stat[STAT_FLEX6400][STAT_NUMERIC] +
				hourly_stat[STAT_FLEX3200][STAT_NUMERIC] +
				hourly_stat[STAT_FLEX1600][STAT_NUMERIC] +
				hourly_stat[STAT_POCSAG2400][STAT_NUMERIC] +
				hourly_stat[STAT_POCSAG1200][STAT_NUMERIC] +
				hourly_stat[STAT_POCSAG512][STAT_NUMERIC] +
				hourly_stat[STAT_ACARS2400][STAT_NUMERIC] +
				hourly_stat[STAT_MOBITEX][STAT_NUMERIC] +
				hourly_stat[STAT_ERMES][STAT_NUMERIC];
	num_char =	hourly_char[STAT_FLEX6400][STAT_NUMERIC] +
				hourly_char[STAT_FLEX3200][STAT_NUMERIC] +
				hourly_char[STAT_FLEX1600][STAT_NUMERIC] +
				hourly_char[STAT_POCSAG2400][STAT_NUMERIC] +
				hourly_char[STAT_POCSAG1200][STAT_NUMERIC] +
				hourly_char[STAT_POCSAG512][STAT_NUMERIC] +
				hourly_char[STAT_ERMES][STAT_NUMERIC];
	alpha_msg = hourly_stat[STAT_FLEX6400][STAT_ALPHA] +
				hourly_stat[STAT_FLEX3200][STAT_ALPHA] +
				hourly_stat[STAT_FLEX1600][STAT_ALPHA] +
				hourly_stat[STAT_POCSAG2400][STAT_ALPHA] +
				hourly_stat[STAT_POCSAG1200][STAT_ALPHA] +
				hourly_stat[STAT_POCSAG512][STAT_ALPHA] +
				hourly_stat[STAT_ACARS2400][STAT_ALPHA] +
				hourly_stat[STAT_MOBITEX][STAT_ALPHA] +
				hourly_stat[STAT_ERMES][STAT_ALPHA];
	alpha_char =hourly_char[STAT_FLEX6400][STAT_ALPHA] +
				hourly_char[STAT_FLEX3200][STAT_ALPHA] +
				hourly_char[STAT_FLEX1600][STAT_ALPHA] +
				hourly_char[STAT_POCSAG2400][STAT_ALPHA] +
				hourly_char[STAT_POCSAG1200][STAT_ALPHA] +
				hourly_char[STAT_POCSAG512][STAT_ALPHA] +
				hourly_char[STAT_ERMES][STAT_ALPHA];

	fprintf(fp, " Totals      %8ld %9ld   %8ld %9ld\n\n", num_msg, num_char, alpha_msg, alpha_char);

	return;
}


void WriteStatFileDaily(FILE *fp)
{
	long num_msg, num_char, alpha_msg, alpha_char;

	fprintf(fp, "              Numeric   Numeric      Alpha     Alpha\n");
	fprintf(fp, "                 Msgs     Chars       Msgs     Chars\n");

	fprintf(fp, "POCSAG-512   %8ld %9ld   %8ld %9ld\n",
				daily_stat[STAT_POCSAG512][STAT_NUMERIC],
				daily_char[STAT_POCSAG512][STAT_NUMERIC],
				daily_stat[STAT_POCSAG512][STAT_ALPHA],
				daily_char[STAT_POCSAG512][STAT_ALPHA]);

	fprintf(fp, "POCSAG-1200  %8ld %9ld   %8ld %9ld\n",
				daily_stat[STAT_POCSAG1200][STAT_NUMERIC],
				daily_char[STAT_POCSAG1200][STAT_NUMERIC],
				daily_stat[STAT_POCSAG1200][STAT_ALPHA],
				daily_char[STAT_POCSAG1200][STAT_ALPHA]);

	fprintf(fp, "POCSAG-2400  %8ld %9ld   %8ld %9ld\n",
				daily_stat[STAT_POCSAG2400][STAT_NUMERIC],
				daily_char[STAT_POCSAG2400][STAT_NUMERIC],
				daily_stat[STAT_POCSAG2400][STAT_ALPHA],
				daily_char[STAT_POCSAG2400][STAT_ALPHA]);

	fprintf(fp, "FLEX-1600    %8ld %9ld   %8ld %9ld\n",
				daily_stat[STAT_FLEX1600][STAT_NUMERIC],
				daily_char[STAT_FLEX1600][STAT_NUMERIC],
				daily_stat[STAT_FLEX1600][STAT_ALPHA],
				daily_char[STAT_FLEX1600][STAT_ALPHA]);

	fprintf(fp, "FLEX-3200    %8ld %9ld   %8ld %9ld\n",
				daily_stat[STAT_FLEX3200][STAT_NUMERIC],
				daily_char[STAT_FLEX3200][STAT_NUMERIC],
				daily_stat[STAT_FLEX3200][STAT_ALPHA],
				daily_char[STAT_FLEX3200][STAT_ALPHA]);

	fprintf(fp, "FLEX-6400    %8ld %9ld   %8ld %9ld\n",
				daily_stat[STAT_FLEX6400][STAT_NUMERIC],
				daily_char[STAT_FLEX6400][STAT_NUMERIC],
				daily_stat[STAT_FLEX6400][STAT_ALPHA],
				daily_char[STAT_FLEX6400][STAT_ALPHA]);

	fprintf(fp, "ACARS-2400   %8ld %9ld   %8ld %9ld\n",
				daily_stat[STAT_ACARS2400][STAT_NUMERIC],
				daily_char[STAT_ACARS2400][STAT_NUMERIC],
				daily_stat[STAT_ACARS2400][STAT_ALPHA],
				daily_char[STAT_ACARS2400][STAT_ALPHA]);

	fprintf(fp, "MOBITEX      %8ld %9ld   %8ld %9ld\n",
				daily_stat[STAT_MOBITEX][STAT_NUMERIC],
				daily_char[STAT_MOBITEX][STAT_NUMERIC],
				daily_stat[STAT_MOBITEX][STAT_ALPHA],
				daily_char[STAT_MOBITEX][STAT_ALPHA]);

	fprintf(fp, "ERMES        %8ld %9ld   %8ld %9ld\n",
				daily_stat[STAT_ERMES][STAT_NUMERIC],
				daily_char[STAT_ERMES][STAT_NUMERIC],
				daily_stat[STAT_ERMES][STAT_ALPHA],
				daily_char[STAT_ERMES][STAT_ALPHA]);

//--Endof ERMES

	fprintf(fp, "             -------- ---------   -------- ---------\n");

	num_msg =	daily_stat[STAT_FLEX6400][STAT_NUMERIC] +
				daily_stat[STAT_FLEX3200][STAT_NUMERIC] +
				daily_stat[STAT_FLEX1600][STAT_NUMERIC] +
				daily_stat[STAT_POCSAG2400][STAT_NUMERIC] +
				daily_stat[STAT_POCSAG1200][STAT_NUMERIC] +
				daily_stat[STAT_POCSAG512][STAT_NUMERIC] +
				daily_stat[STAT_ACARS2400][STAT_NUMERIC] +
				daily_stat[STAT_MOBITEX][STAT_NUMERIC] +
				daily_stat[STAT_ERMES][STAT_NUMERIC];
	num_char =	daily_char[STAT_FLEX6400][STAT_NUMERIC] +
				daily_char[STAT_FLEX3200][STAT_NUMERIC] +
				daily_char[STAT_FLEX1600][STAT_NUMERIC] +
				daily_char[STAT_POCSAG2400][STAT_NUMERIC] +
				daily_char[STAT_POCSAG1200][STAT_NUMERIC] +
				daily_char[STAT_POCSAG512][STAT_NUMERIC] +
				daily_char[STAT_ERMES][STAT_NUMERIC];
	alpha_msg = daily_stat[STAT_FLEX6400][STAT_ALPHA] +
				daily_stat[STAT_FLEX3200][STAT_ALPHA] +
				daily_stat[STAT_FLEX1600][STAT_ALPHA] +
				daily_stat[STAT_POCSAG2400][STAT_ALPHA] +
				daily_stat[STAT_POCSAG1200][STAT_ALPHA] +
				daily_stat[STAT_POCSAG512][STAT_ALPHA] +
				daily_stat[STAT_ACARS2400][STAT_ALPHA] +
				daily_stat[STAT_MOBITEX][STAT_ALPHA] +
				daily_stat[STAT_ERMES][STAT_ALPHA];
	alpha_char= daily_char[STAT_FLEX6400][STAT_ALPHA] +
				daily_char[STAT_FLEX3200][STAT_ALPHA] +
				daily_char[STAT_FLEX1600][STAT_ALPHA] +
				daily_char[STAT_POCSAG2400][STAT_ALPHA] +
				daily_char[STAT_POCSAG1200][STAT_ALPHA] +
				daily_char[STAT_POCSAG512][STAT_ALPHA] +
				daily_char[STAT_ERMES][STAT_ALPHA];

	fprintf(fp, "  Totals     %8ld %9ld   %8ld %9ld\n", num_msg, num_char, alpha_msg, alpha_char);

	return;
}


void CountBiterrors(int errors)
{
	extern double dRX_Quality;
	static int nErrors=5, nErrorChecks=100, noerrors=0, count=0;

	if (errors)
	{
		nErrorChecks+=abs(errors);
		noerrors=0;
		count=1;
	}
	else
	{
		nErrorChecks++;
		noerrors++;
	}
	nErrors+=errors;

	if (nErrorChecks > 10000)
	{
		nErrorChecks/=1.999;
		nErrors/=2;
	}
	if (noerrors > 50)
	{
		count++;
		nErrors-=count;
		if (nErrors < 0) nErrors=0;
		noerrors=0;
	}
	dRX_Quality = 100 - ((double)(nErrors*100)/nErrorChecks);
}


void Get_Date_Time(void)
{
	switch (Profile.DateFormat)
	{
		default:
		case 0:
			GetDateFormat(LOCALE_USER_DEFAULT, 0, NULL, "dd'-'MM'-'yy", szCurrentDate, 40);
		break;

		case 1:
			GetDateFormat(LOCALE_USER_DEFAULT, 0, NULL, "MM'-'dd'-'yy", szCurrentDate, 40);
		break;

		case 2:
			GetDateFormat(LOCALE_USER_DEFAULT, 0, NULL, "yy'-'MM'-'dd", szCurrentDate, 40);
		break;
	}

	GetTimeFormat(LOCALE_USER_DEFAULT, TIME_FORCE24HOURFORMAT,
												  NULL, "HH':'mm':'ss", szCurrentTime, 40);
}


void InvertData(void)
{
	Profile.invert ^= 0x01; // Flip receive polarity

	low_audio  = Profile.invert ? DEFAULT_HI_AUDIO : DEFAULT_LO_AUDIO;
	high_audio = Profile.invert ? DEFAULT_LO_AUDIO : DEFAULT_HI_AUDIO;
}
