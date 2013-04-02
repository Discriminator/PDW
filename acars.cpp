//
// Acars.cpp
//
//

#ifndef STRICT
#define STRICT 1
#endif

#include <windows.h>
#include <mmsystem.h>
#include <stdio.h>
#include <commdlg.h>
#include <string.h>
#include <ctype.h>

#include "headers\pdw.h"
#include "headers\initapp.h"
#include "headers\gfx.h"
#include "headers\misc.h"
#include "headers\acars.h"
#include "headers\helper_funcs.h"


//#define AC_OUT_DEBUG 1

ACARS acars;

extern unsigned long hourly_stat[NUM_STAT][2];
extern unsigned long hourly_char[NUM_STAT][2];
extern unsigned long daily_stat[NUM_STAT][2];
extern unsigned long daily_char[NUM_STAT][2];

unsigned int line_pos;

TCHAR szACARSPath[MAX_PATH];  // Used for finding paths to ACARS tables.

// These help decide if Uplink/Downlink or Ground to Ground message.
char *uplink[17]    = {"51","A1","A2","A3","A4","A5","A6","A7","A8","A9","A0","C0","C1","C2","C3","RA",":;"};
char *up_or_down[5] = {"_*","_","_J","54","H1"};
char *gnd_to_gnd[6] = {"CB","CC","CD","CE","CF","Q5"};

unsigned int sup[3] = { 0xFFFF,		// Last 2 bytes of (max) 16 bytes pre-key
						0x2AAB,		// Bit Sync
						0x1616 };	// Char Sync
unsigned int slr[3] = { 0, 0, 0 };


ACARS::ACARS()
{
	ac_sr0 = 0;
	ac_sr1 = 0;
	ac_alive = false;
	ac_blk = 0;
	ac_synced = false;
	ac_bits = 0;

	index = 0;   // Indicates where we are in the acars signal.
	nch = 0;     // Number of bytes to get for new position.
	mode[0] = 0;
	address[0] = 0;
	ack = 0;
	label[0] = 0;
	dbi = 0;
	stx = 0;
	msg_nr[0] = 0;
	flight_nr[0] = 0;
	text[0] = 0;
//	etx_etb = 0;

	// structures for managing database information.
	db_label = NULL;				// Label info
	db_aircraft_type = NULL;		// Aircraft type
	db_origin = NULL;				// Origin of aircraft
	db_airline = NULL;				// Airline
	db_gnd_station = NULL;			// Ground station
	db_routes = NULL;				// Routes
}

ACARS::~ACARS()
{
}

// Reset routine. Required for switching data mode and also
// for switching between  Soundcard/Serial port input.
void ACARS::reset(void)
{
	ac_sr0 = 0;
	ac_sr1 = 0;
	ac_alive = false;
	ac_blk = 0;
	ac_synced = false;
	ac_bits = 0;

	index = 0;   // Indicates where we are in the acars signal.
	nch = 0;     // Number of bytes to get for new position.
}


void ACARS::frame(int bit)
{   
	// update register
	ac_sr0 = ac_sr0 >> 1;
	if ((ac_sr1 & 0x01) != 0) ac_sr0 = ac_sr0 ^ 0x80;
	ac_sr1 = ac_sr1 >> 1;
	if (bit) ac_sr1 = ac_sr1 ^ 0x80;

	// update bit buffer
	for (int i=0; i<2; i++)
	{
		slr[i] = slr[i] >> 1;
		if ((slr[i+1] & 0x01) != 0x00) slr[i] = slr[i] ^ 0x8000;
	}
	slr[2] = slr[2] >> 1;

	if (bit) slr[2] = slr[2] | 0x8000;

	if (ac_blk == 0)
	{
		if (!ac_synced)
		{
			if ((nOnes(slr[0] ^ 0) + nOnes(slr[1] ^ 0) + nOnes(slr[2] ^ 0)) == 0)
			{
				InvertData();	// Found 48 inverted prekey bits (all zero), so invert data
			}
			else if ((nOnes(slr[0] ^ sup[0]) < 2) &&	// Must be 0xFFFF (last 16 bits of prekey)
					 (nOnes(slr[1] ^ sup[1]) < 2) &&	// Must be 0x2AAB (Bit Sync)
					 (nOnes(slr[2] ^ sup[2]) < 2))		// Must be 0x1616 (Char Sync)
			{
				ac_synced = true;
				ac_bits = 0;
			}
		}
		else	// Synced, next byte must be <SOH>
		{
			if (ac_bits++ == 7)
			{
				if (short_nOnes(ac_sr1 ^ 0x01) == 0)	// Find <SOH>
				{
					ac_blk = 26;
					text[0] = 0;
					index = 4;
					parity_errors=0;
					ac_alive = true; // for signal indicator.
					display_showmo(MODE_ACARS);
				}
				ac_synced = false;
				ac_bits = 0;
			}
		}
	}
	else	// Decode acars frame.
	{
		if (ac_bits++ == 7)
		{
			ac_blk--;
			ac_bits = 0;

			if (Profile.acars_parity_check)
			{
				if (parity_check(ac_sr1)) parity_errors++; // keep count of total parity errors
			}
            
			switch (index)
			{
				case 4:                // Get mode - 1 byte.

				mode[0] = ac_sr1;
				mode[1] = '\0';
				nch = 0;
				index++;

				break;

				case 5:                // Get address info - 7 bytes.

				address[nch] = ac_sr1;
				nch++;

				if (nch == 7)
				{
					address[nch] = 0;
					nch = 0;
					index++;
				}
				break;

				case 6:                // Get ack/nak - 1 byte.

				ack = ac_sr1;
				nch = 0;
				index++;
			
				break;

				case 7:                // Get label info - 2 bytes.

				label[nch] = ac_sr1;
				nch++;
				if (nch == 2)
				{
					label[nch] = 0;
					nch = 0;
					index++;
				}
				break;

				case 8:                // Get DBI - 1 byte.

				dbi = ac_sr1;
				nch = 0;
				index++;
				
				break;

				case 9:                // Get STX - 1 byte.

				stx = ac_sr1;

				if ((stx & 0x7F) == 0x02) ac_blk = 235; // Extend frame length if there's txt.

				nch = 0;
				index++;
				
				break;

				case 10:                // Get message number - 4 bytes.

				msg_nr[nch] = ac_sr1;
				nch++;

				if (nch == 4)
				{
					msg_nr[nch] = 0;
					nch = 0;
					index++;
				}
				break;

				case 11:                // Get flight number - 6 bytes.

				flight_nr[nch] = ac_sr1;
				nch++;

				if (nch == 6)
				{
					flight_nr[nch] = 0;
					nch = 0;
					index++;
				}
				break;

				case 12:                // Get message - upto 220 bytes.
	
				if ((stx & 0x7F) == 0x02)
				{
					if (nch == 0)	// If only etx or etb, skip msg field.
					{
						if (((ac_sr1 & 0x7F) == 0x03) || ((ac_sr1 & 0x7F) == 0x17))
						{
//							etx_etb = ac_sr1;
							index++;
							break;
						}
					}
					text[nch] = ac_sr1;
					nch++;

					if (parity_errors > 15) // is msg garbage?
					{
						text[nch] = 0;
						nch = 0;
						index++;   // move on.
						break;
					} 

					// endof text if > 219 or found <ETX> or <ETB>
					if ((nch == 220) || ((ac_sr1 & 0x7F) == 0x03) || ((ac_sr1 & 0x7F) == 0x17))
					{
						text[nch] = 0;
						nch = 0;
						index++;
					}
				}
				else	 // no <stx> ? ...get etx/etb and move on.
				{
					nch = 0;
					index++;
//					etx_etb = ac_sr1;
				}
				break;

				case 13:           // Display the message

				if (!parity_errors) ac_out_msg();

				nch = 0;
				index++;
				ac_blk = 1; // skip BCS & BCS-suffix for now!
				ac_alive = false;
				display_showmo(MODE_IDLE);

				break;

				default:
				break;
			}
		}
		hourly_char[STAT_ACARS2400][STAT_ALPHA]++;
		daily_char[STAT_ACARS2400][STAT_ALPHA]++;
	}
}


// Display ACARS packets.
// Address,label,msg_nr and some other parts of msg are given a
// parity check. Failed characters are highlighted in grey.
void ACARS::ac_out_msg(void)
{
	unsigned int i, error=0;

	//******* Display Aircraft Reg ******

	Current_MSG[MSG_CAPCODE][0] = 0;

	for (i=0; i<7; i++)
	{
		Current_MSG[MSG_CAPCODE][i] = (char)(address[i+1] & 0x7F);
		if (parity_check(address[i])) error=1;
	}
	Current_MSG[MSG_CAPCODE][i] = 0;

	messageitems_colors[1] = error ? COLOR_BITERRORS : COLOR_ADDRESS;

	// ************* Time/Date **************

	Get_Date_Time();
	strcpy(Current_MSG[MSG_TIME], szCurrentTime);
	strcpy(Current_MSG[MSG_DATE], szCurrentDate);
	messageitems_colors[2] = COLOR_TIMESTAMP;
	messageitems_colors[3] = COLOR_TIMESTAMP;

	//******* Display message number ******

	for (i=0, error=0; i<4; i++)
	{
		Current_MSG[MSG_MODE][i] = (char)(msg_nr[i] & 0x7F);

		if (parity_check(msg_nr[i])) error=1;
	}
	Current_MSG[MSG_MODE][i] = 0;

	messageitems_colors[4] = error ? COLOR_BITERRORS : COLOR_AC_MESSAGE_NR;

	//********* DBI ********************

	sprintf(Current_MSG[MSG_TYPE], "%c", (char)(dbi & 0x7F));
	messageitems_colors[5] = parity_check(dbi) ? COLOR_BITERRORS : COLOR_AC_DBI;

	//********* MODE ********************

	mode_parity_error = parity_check(mode[0]);

	messageitems_colors[6] = mode_parity_error ? COLOR_BITERRORS : COLOR_MODETYPEBIT;
	Current_MSG[MSG_BITRATE][0] = (char)(mode[0] & 0x7F);

	//************* Label *****************

	out_label();
           
	//************* Address ***************

	out_address();

	//************* Flight No. ***************

	out_flight_nr();

	//************* Ground Station. ***************

	if (!mode_parity_error) out_ground_station();

	//************* Message text ***************

	out_message_txt();           

	//************* Uplink/Downlink Ack/Nak ***************

	out_dir_ack_nak();
}


// Display Label field.
// This consists of:
//
// Msg Label: B1...REQUEST OCEANIC CLEARANCE
//
// Note: Description after label is only displayed if label passed parity check.
// 
void ACARS::out_label(void)
{
	struct dbase_info *p_info;
	unsigned int i, v;

	label_parity_errors=0;

	display_color(&Pane1, COLOR_MISC);
	display_show_str(&Pane1, "Message Label : ");

	line_pos = 16;

	for (i=0; i<2; i++, line_pos++)
	{                          
		if (!parity_check(label[i])) display_color(&Pane1, COLOR_MESSAGE);
		else
		{           
			display_color(&Pane1, COLOR_BITERRORS);
			label_parity_errors++;
		}

		label[i] &= 0x7F;

		if (label[i] < 0x20) label[i] = ' ';

		display_show_char(&Pane1, label[i]);
	}

	// If label descriptions loaded from file and label field is ok -
	// display description.

	if ((!label_parity_errors) && (db_label != NULL))
	{
		p_info = db_label;
	
		while (p_info)
		{    
			if (stricmp(label,p_info->label) == 0)
			{
				display_color(&Pane1, COLOR_MESSAGE);
				display_show_str(&Pane1, "...");
				line_pos += 3; 
				v = strlen(p_info->desc);

				for (i=0; i<v; i++, line_pos++)	// Display description.
				{
					display_show_char(&Pane1, p_info->desc[i]);

					if (line_pos > NewLinePoint) line_pos = 0;
				}
				break; 
			}
			p_info = p_info->next;
		}
	}
	display_show_char(&Pane1, char(23));	// PH: Add ETB-char to mark end of line
}

// Display Address field.
// This consists of:
//
// Aircraft Reg : N18257U  Country: United States
// Aircraft Type: Boing 747
//
// Note: Country and Aircraft type only displayed if address passed parity check.
// 
void ACARS::out_address(void)
{
	struct dbase_info *p_info;
	unsigned int i, v, errors=0;
	char *s;
  
	display_color(&Pane1, COLOR_MISC);
	display_show_str(&Pane1, "Aircraft Reg  : ");

	line_pos = 16;

	for (i=0; i<7; i++, line_pos++)
	{
		if (!parity_check(address[i])) display_color(&Pane1, COLOR_MESSAGE);
		else
		{
			display_color(&Pane1, COLOR_BITERRORS);
			errors++;
		}
		address[i] &= 0x7F;
		if (address[i] < 0x20) address[i] = ' ';

		if (!(i == 0 && address[i] == '.')) display_show_char(&Pane1, address[i]);
	}

	// If COUNTRY descriptions loaded from file and address field is ok -> display country.

	if (!errors && (db_origin != NULL))
	{
		char ta[10];
		strcpy(ta,address);
		s = ta;

		while ((*s == '.') && (*s != '\0')) s++; // Remove any prefixed '.'

		i = strlen(s);

		if (i > 3)	// REG-CODE PREFIX upto 4 characters
		{
			ta[4] = '\0';    //  after '.'
			i = 4;
		}
		else ta[i] = '\0';

		while (i)   // Run loop upto 4 times to find match
		{
			p_info = db_origin;

			while (p_info)
			{
				if (stricmp(s,p_info->label) == 0)
				{
					display_color(&Pane1, COLOR_MISC);
					display_show_str(&Pane1, "  Country: ");
					display_color(&Pane1, COLOR_MESSAGE);
					line_pos += 11;
					v = strlen(p_info->desc);

					for (int z=0; z<v; z++, line_pos++)	// Display description.
					{
						display_show_char(&Pane1, p_info->desc[z]);
						if (line_pos > NewLinePoint) line_pos = 0;
					}
					i = 1; // stop outer loop
					break;
				}
				p_info = p_info->next;
			}
			i--;
			s[i] = '\0';  // make match-str 1 char shorter per loop.
		}
	}

	// If Aircraft descriptions loaded from file and address field is ok -> display description.

	if (!errors && (db_aircraft_type != NULL))
	{
		p_info = db_aircraft_type;
		s = address;
		while ((*s == '.') && (*s != '\0')) s++;

		if (*s != '\0')
		{
			while (p_info)
			{
				if (stricmp(s,p_info->label) == 0) 
				{
					display_show_char(&Pane1, char(23));	// PH: Add ETB-char to mark end of line
					line_pos = 0;
					display_color(&Pane1, COLOR_MISC);
					display_show_str(&Pane1, "Aircraft Type : ");
					display_color(&Pane1, COLOR_MESSAGE);
					line_pos += 16; 
					v = strlen(p_info->desc);

					for (i=0; i<v; i++, line_pos++)	// Display description.
					{
						display_show_char(&Pane1, p_info->desc[i]);
						if (line_pos > NewLinePoint) line_pos = 0;
					}
					break;
				}
				p_info = p_info->next;
			}
		}
	}
	display_show_char(&Pane1, char(23));	// PH: Add ETB-char to mark end of line
}


// Display Flight Number field.
// This consists of:
//
// Flight No: ACF095  Airline: Air Canada
//
// Note: Airline is only displayed if flight number passed parity check.
//
void ACARS::out_flight_nr(void)
{
	struct dbase_info *p_info;
	unsigned int i, v, errors=0;

	display_color(&Pane1, COLOR_MISC);
	display_show_str(&Pane1, "Flight Number : ");

	line_pos = 16;

	for (i=0; i<6; i++, line_pos++)
	{
		if (!parity_check(flight_nr[i])) display_color(&Pane1, COLOR_MESSAGE);
		else
		{
			display_color(&Pane1, COLOR_BITERRORS);
			errors++;
		}

		flight_nr[i] &= 0x7F;
		if(flight_nr[i] < 0x20) flight_nr[i] = ' ';
		display_show_char(&Pane1, flight_nr[i]);
	}

	if (!errors && (db_routes != NULL))
	{
		p_info = db_routes;
	
		while (p_info)
		{    
			if (stricmp(flight_nr,p_info->label) == 0)
			{
				display_color(&Pane1, COLOR_MESSAGE);
				display_show_str(&Pane1, " [");
				line_pos += 2; 
				v = strlen(p_info->desc);

				for (i=0; i<v; i++, line_pos++)	// Display description.
				{
					display_show_char(&Pane1, p_info->desc[i]);

					if (line_pos > NewLinePoint) line_pos = 0;
				}
				display_show_str(&Pane1, "]");
				line_pos += 1; 
				break; 
			}
			p_info = p_info->next;
		}
	}
	display_show_char(&Pane1, char(23));	// PH: Add ETB-char to mark end of line

	line_pos = 0;
	
	// If Airline descriptions loaded from file and flight-no field is ok -> display description.
	if (!errors && (db_airline != NULL))
	{
		p_info = db_airline;
		flight_nr[2] = '\0'; // only match first two characters.

		while (p_info)
		{
			if (stricmp(flight_nr,p_info->label) == 0) 
			{
				display_color(&Pane1, COLOR_MISC);
				display_show_str(&Pane1, "Airline       : ");
				display_color(&Pane1, COLOR_MESSAGE);
				line_pos += 16;
				v = strlen(p_info->desc);

				for (i=0; i<v; i++, line_pos++)	// Display description.
				{
					display_show_char(&Pane1, p_info->desc[i]);
					if (line_pos > NewLinePoint) line_pos = 0;
				}
				display_show_char(&Pane1, char(23));	// PH: Add ETB-char to mark end of line
				break;
			}
			p_info = p_info->next;
		}
	}
}

// Display Ground Station.
// 
// Only displayed if Mode passed parity check.
//
void ACARS::out_ground_station(void)	
{
	struct dbase_info *p_info;
 
	// If Ground Station descriptions loaded from file and Mode field is ok -> display description.
	if (db_gnd_station != NULL)
	{
		p_info = db_gnd_station;

		while (p_info)
		{
			if (stricmp(mode,p_info->label) == 0) 
			{
				display_color(&Pane1, COLOR_MISC);
				display_show_str(&Pane1, "Ground Station: ");
				display_color(&Pane1, COLOR_MESSAGE);
				line_pos += 22; 
				int v = strlen(p_info->desc);

				for (int i=0; i<v; i++, line_pos++)	// Display description.
				{
					display_show_char(&Pane1, p_info->desc[i]);
					if (line_pos > NewLinePoint) line_pos = 0;
				}
				display_show_char(&Pane1, char(23));	// PH: Add ETB-char to mark end of line
				break;
			}
			p_info = p_info->next;
		}
	}
}



// Display Message.
void ACARS::out_message_txt(void)
{
	char c;

	if (text[0] == '\0')	// No message?
	{
		hourly_stat[STAT_ACARS2400][STAT_NUMERIC]++;
		daily_stat[STAT_ACARS2400][STAT_NUMERIC]++;
		return;
	}

	int l = strlen(text);

	// get number of alpha messages. 
	hourly_stat[STAT_ACARS2400][STAT_ALPHA]++;
	daily_stat[STAT_ACARS2400][STAT_ALPHA]++;

	display_color(&Pane1, COLOR_MISC);
	display_show_str(&Pane1, "Message       : ");

	line_pos = 16;

	for (int i=0; (i<l)&&(i<220); i++, line_pos++)
	{
		c = (text[i] & 0x7F);
		if (c < 0x20) c = ' ';

		display_color(&Pane1, !parity_check(text[i]) ? COLOR_MESSAGE : COLOR_BITERRORS);
		display_show_char(&Pane1, c);
	}
	display_show_char(&Pane1, char(23));	// PH: Add ETB-char to mark end of line
}


// Display Uplink/Downlink & Ack/Nack.
//
void ACARS::out_dir_ack_nak(void)
{
	unsigned int i, f=0;

	display_color(&Pane1, COLOR_TIMESTAMP);

	if (!label_parity_errors)
	{
		for (i=0; i<5; i++) // Check if could be up or down link msg.
		{
			if (stricmp(label,up_or_down[i]) == 0)
			{
				f=1;
			}
		}

		if(!f) // If sure not 2-way message, work out UP/DOWN/GND-TO-GND.
		{
			for(i=0; i<6; i++) // is it a ground to ground link ?
			{
				if (stricmp(label,gnd_to_gnd[i]) == 0)
				{
					display_show_str(&Pane1, "GROUND TO GROUND ");
					f=1;
				}
			}

			if(!f) // If not GND to GND then look for UP/DOWNLINK.
			{
				for(i=0; i<17; i++) // is it an uplink ?
				{
					if (stricmp(label,uplink[i]) == 0)
					{
						display_show_str(&Pane1, "UPLINK ");
						f=1;
						break;
					}
				}

				if(!f)	// Must be downlink.
				{
					display_show_str(&Pane1, "DOWNLINK ");
				}
			}
		}
	}
	display_show_str(&Pane1, ((ack & 0x7F) == 0x15) ? "NAK" : "ACK");
	ShowMessage();   // Display Message.
}


int ACARS::parity_check(char c)
{
	unsigned int eo=1;

	for (int i=0; i<8; i++)
	{
		if (c & 0x01) eo ^= 0x01;
		c >>= 1;
	}
	CountBiterrors(eo);

	return(eo);
}


//***************** DATABASE FUNCTIONS FROM HERE ******************

// Read all database files
void ACARS::read_data(void)
{
	db_label = read_db_file(2,"label.df");				// Label info
	db_aircraft_type = read_db_file(7,"aircraft.df");	// Aircraft type
	db_origin = read_db_file(4,"country.df");			// Origin of aircraft
	db_airline = read_db_file(2,"airline.df");			// Airline
	db_gnd_station = read_db_file(1,"ground.df");		// Ground station
	db_routes = read_db_file(6,"routes.df");			// Routes
}

// Free all database memory.
void ACARS::free_data(void)
{
	if (db_label != NULL)			free_db_info(db_label);				// Label info
	if (db_aircraft_type != NULL)	free_db_info(db_aircraft_type);		// Aircraft type
	if (db_origin != NULL)			free_db_info(db_origin);			// Origin of aircraft
	if (db_airline != NULL)			free_db_info(db_airline);			// Airline
	if (db_gnd_station != NULL)		free_db_info(db_gnd_station);		// Ground station
	if (db_routes != NULL)			free_db_info(db_routes);			// Routes
}

// Reads ACARS database text files.
// Allocates a dbase_info structure for each valid label/desc field.
// Chains the dbase_info structures together.
//
// If it succeeds it returns a pointer to the first db_ase_info
// structure in the chain.
// If it fails it returns NULL.
//
struct dbase_info *ACARS::read_db_file(int max_label_size,char *fname)
{
	struct dbase_info *rp_info=NULL,*p_info=NULL, *last_p=NULL;
	FILE *fp=NULL;
	unsigned int cnt, db_label_cnt=0;
	char c, *s;

	// Set correct file path.
	wsprintf(szACARSPath, TEXT("%s\\%s"), szPath, fname);

	if ((fp = fopen(szACARSPath,"r+")) == NULL) return(NULL);
  
	p_info = new dbase_info;
	p_info->label[0] = 0;
	p_info->desc[0] = 0;
	p_info->next = NULL;

	rp_info = p_info;
           
	while (!feof(fp))
	{
		c = getc(fp);

		if (c == ' ' || c == '\n' || c == '\t' || c == EOF) continue;

		if (c == '!' || c == '#')    // Skip comment lines
		{
			while ((c = getc(fp)) != EOF)
			{
				if (c == '\n') break; 
			}
			continue;
		}

		// Allocate next db struct
		if (db_label_cnt != 0)
		{
			last_p = p_info;
			p_info = new dbase_info;
			if (!p_info) break; 
			p_info->label[0] = 0;
			p_info->desc[0] = 0;
			p_info->next = NULL;
			last_p->next = p_info;
		}

		cnt = 0;
		s = p_info->label;
		do
		{
			if (c == ' ' || c == '=') break;
			if (c != '\t')
			{
				*s++ = c;
				cnt++;
				if (cnt == max_label_size) break;
			}
		}
		while ((c = getc(fp)) != EOF);

		*s = '\0';
		if (c == EOF) break;

		// Skip ' ' & '='

		while ((c = getc(fp)) != EOF)
		{     
			if ((c != ' ') && (c != '=') && (c != '\t')) break;
		}

		// Get Description - max 59 characters + null.

		cnt = 0;
		s = p_info->desc;

		do
		{
			if (c == '\n') break;

			if (c != '\t')
			{
				*s++ = c;
				cnt++;
				if(cnt == 58) break;
			}
		}
		while((c = getc(fp)) != EOF);

		*s = '\0';
		db_label_cnt++; // Number of valid labels.

		if (c == EOF) break;
	}
	fclose(fp);

/** Debug - display last entry **
     misc_debug_msg(" Label: ");
 if(p_info->label[0] != 0)
     misc_debug_msg(p_info->label);

     misc_debug_msg(" Desc: ");
 if(p_info->desc[0] != 0)
     misc_debug_msg(p_info->desc);
****/

	return(rp_info); // return 1st dbase_info structure in chain. 
}


// Free memory for chain of dbase_info structures.
void ACARS::free_db_info(struct dbase_info *p_info)
{
	struct dbase_info *last_p;

	if (p_info != NULL)
	{   
		last_p = p_info->next;

		while (p_info != NULL)
		{
			delete p_info;  
			p_info = last_p;
			if (p_info) last_p = p_info->next;
		}
	} 
}
