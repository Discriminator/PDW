// Language.cpp
//
// Contains language remapping routines.
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
#include <time.h>

#include "headers\resource.h"
#include "headers\PDW.h"
#include "headers\slicer.h"
#include "headers\toolbar.h"
#include "headers\gfx.h"
#include "headers\initapp.h"
#include "headers\sigind.h"
#include "headers\decode.h"
#include "headers\sound_in.h"
#include "headers\printer.h"
#include "headers\misc.h"
#include "headers\menu.h"
#include "headers\acars.h"
#include "headers\language.h"

char lang_name[10][40];
int lang_rev[10];
int lang_cnt = 0;
struct lang_db_info *lang_table[10];
FILE *lang_fp=NULL;


// This function does character remapping for the currently
// selected language. see misc.cpp
char remap_ch(char c)
{
	struct lang_db_info *p_info,*last_p;

	p_info = lang_table[Profile.lang_tbl_index];

	while (p_info != NULL)
	{
		last_p = p_info->next;
		if (p_info->label == c)
		{
			return(p_info->desc);
		}
		p_info = last_p;
	}
 	return(c);
}

// Tests if character is in current language table.
BOOL in_lang_tbl(char c)
{
	struct lang_db_info *p_info,*last_p;

	p_info = lang_table[Profile.lang_tbl_index];

	while (p_info != NULL)
	{
		last_p = p_info->next;
		if (p_info->label == c)
		{
			return(TRUE);
		}
		p_info = last_p;
	}
	return(FALSE);
}


// Open the language database file.
// Load language tables.
void read_language_tables(void)
{
	char lbuf[40];
	int ok = 1;

	// Set correct file path.
	// We use the ACARS path string as it is no longer in use. 
	wsprintf(szACARSPath, TEXT("%s\\%s"), szPath, "language.df");

	if ((lang_fp = fopen(szACARSPath,"r+")) == NULL) return;


	while((lang_cnt < 10) && (ok))
	{	// Get Language Name.
		lang_name[lang_cnt][0] = 0;
		if (!(find_read_string(lang_name[lang_cnt],"LANGUAGE",8))) break;
         
//		misc_debug_msg(lang_name[lang_cnt]);
//		misc_debug_msg("  ");

		// Get reverse flag.
		lang_rev[lang_cnt] = 0;
		if (find_read_string(lbuf,"REVERSED",8)) 
		{
			if (lbuf[0] == '1') lang_rev[lang_cnt] = 1; // Text is displayed right to left.

//			misc_debug_msg(lbuf);
//			misc_debug_msg("  <endof rev> ");
		}
		else break;

		// Get/Build table.
		lang_table[lang_cnt] = read_lang_tbl(&ok);
		lang_cnt++;
	}
	fclose(lang_fp);
}

// Free memory used by language tables.
void free_lang_tables(void)
{
	int i;
	for (i=0;i<lang_cnt;i++)
	{
		free_lang_db_info(lang_table[i]);
	}
	lang_cnt = 0;
}


// Free memory for chain of lang_dbase_info structures.
void free_lang_db_info(struct lang_db_info *p_info)
{
	struct lang_db_info *last_p;

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

// Find/Read string.
BOOL find_read_string(char *name,char *id,int id_len)
{
	char *s, c;
	char idbuf[40];
	int cnt;

	while (!feof(lang_fp))
	{
		c = getc(lang_fp);

		if (c==EOF) return (FALSE);
		if (c == ' ' || c == '\n' || c == '\t') continue;
		if (c == '!' || c == '#')    // Skip comment lines
		{
			while ((c = getc(lang_fp)) != EOF)
			{
				if (c == '\n') break;
			}
			continue;
		}

		cnt = 0;
		s = idbuf;
		do
		{
			if (c == ' ' || c == '=') break;
			if (c != '\t')
			{
				*s++ = c;
				cnt++;
				if (cnt == id_len || cnt > 38) break;
			}
		}
		while ((c = getc(lang_fp)) != EOF);

		*s = '\0';

		if (c == EOF) return(FALSE);
		if (!((stricmp(idbuf,id))== 0)) // Found id sting?
		continue;

		// Skip ' ' & '='

		while ((c = getc(lang_fp)) != EOF)
		{     
			if ((c != ' ') && (c != '=') && (c != '\t')) break;
		}

		// Get label - upto 39 characters + NULL.
		cnt = 0;
		s = name;
		do
		{
			if (c == '\n') break;
			if (c != '\t')
			{
				*s++ = c;
				cnt++;
				if(cnt == 38)
				break;
			}   
		}
		while ((c = getc(lang_fp)) != EOF);
		*s = '\0';

		return(TRUE);
	}
	return(FALSE);
}


// Read current language table.
// Allocates a lang_db_info structure for each table entry.
// Endof table if we see "[NEW TABLE]" or if it's EOF.
//
// Sets "ok" to TRUE if another table follows.
// Returns a pointer to the first lang_db_info structure in chain.
//
struct lang_db_info *read_lang_tbl(int *ok)
{
	struct lang_db_info *rp_info=NULL,*p_info=NULL, *last_p=NULL;
	char c, *s;
	char inbuf[40];
	int cnt,db_label_cnt=0;

	*ok = FALSE;

	p_info = new lang_db_info;         
	p_info->label = 0;
	p_info->desc = 0;
	p_info->next = NULL;

	rp_info = p_info;
     
	while (!feof(lang_fp))
	{
		c = getc(lang_fp);

		if (c == ' ' || c == '\n' || c == '\t' || c == EOF) continue;

		if (c == '!' || c == '#')    // Skip comment lines
		{
			while ((c = getc(lang_fp)) != EOF)
			{
				if (c == '\n') break; 
			}
			continue;
		}

		cnt = 0;
		s = inbuf;
		do
		{
			if (c == ' ' || c == '=') break;
			if (c != '\t')
			{
				*s++ = c;
				cnt++;
				if (cnt == 38) break;
			}  
		}
		while ((c = getc(lang_fp)) != EOF);

		*s = '\0';

		if((stricmp(inbuf,"[NEW"))== 0) // Found new table?
		{
			while ((c = getc(lang_fp)) != EOF)
			{
				if (c == '\n') break; 
			}                    
			if  (c == EOF)	*ok = FALSE;
			else				*ok = TRUE;

			return (rp_info);
		}
                 
		if (c == EOF) break;

		// Allocate next db struct
		if (db_label_cnt != 0)
		{
			last_p = p_info;
			p_info = new lang_db_info;

			if (!p_info) break; 
			p_info->label = 0;
			p_info->desc = 0;
			p_info->next = NULL;
			last_p->next = p_info;               
		}
		p_info->label = (char)atoi(inbuf);
                                                      
		// Skip ' ' & '='

		while ((c = getc(lang_fp)) != EOF)
		{     
			if ((c != ' ') && (c != '=') && (c != '\t')) break;
		}

		// Get Description - upto 38 characters + null.
		cnt = 0;
		s = inbuf;
		do
		{
			if (c == '\n') break;
			if (c != '\t')
			{                   
				*s++ = c;
				cnt++;
				if (cnt == 38) break;
			}   
		}
		while ((c = getc(lang_fp)) != EOF);

		*s = '\0';
		p_info->desc = (char)atoi(inbuf);
		db_label_cnt++; // Number of valid labels.

		if (c == EOF) break;
	}

/** Debug - display first entry **
     sprintf(inbuf," Label: %c",(char)rp_info->label);
     misc_debug_msg(inbuf);

     sprintf(inbuf," Desc: %c",(char)rp_info->desc);
     misc_debug_msg(inbuf);
**/

return(rp_info); // return 1st lang_db_info structure in chain. 
}
