// Menu.cpp
//
// Menu routines.
//
//            - Routines for adjusting the Monitor menu.
//            - Routines for adjusting the language menu.
//            - Routines which set the current language table.
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
#include "headers\language.h"
#include "headers\menu.h"


// Check/uncheck the specified menu item.
// submenu_no = menu (position)containing the item to check/uncheck.
// item_id = identifier of the menu item.
// item_checked = whether to check or uncheck the item.
void check_menu_item(int submenu_no,UINT item_id,BOOL item_checked)
{
	HMENU hmenu = NULL;
	HMENU hsubmenu = NULL;   
	MENUITEMINFO miInfo;

	hmenu    = GetMenu(ghWnd);
	hsubmenu = GetSubMenu(hmenu, submenu_no);         

	miInfo.cbSize = sizeof(MENUITEMINFO);  
	miInfo.fMask = MIIM_CHECKMARKS | MIIM_STATE;
	miInfo.fType = 0; 
	miInfo.fState = item_checked ? MFS_CHECKED : MFS_UNCHECKED; 
	miInfo.wID = 0; 
	miInfo.hSubMenu = NULL; 
	miInfo.hbmpChecked = NULL; 
	miInfo.hbmpUnchecked = NULL; 
	miInfo.dwItemData = 0; 
	miInfo.dwTypeData = 0; 
	miInfo.cch = 0; 

	SetMenuItemInfo(hsubmenu, item_id, FALSE, &miInfo);
}

// Checks/Unchecks menu items based on profile settings.
void set_menu_items(void)
{
	static int last_lang = -1;

	check_menu_item(6, IDM_POCSAGFLEX, false);
	check_menu_item(6, IDM_ACARS,      false);
	check_menu_item(6, IDM_MOBITEX,    false);
	check_menu_item(6, IDM_ERMES,      false);

	if (Profile.monitor_paging)			// Check POCSAG/FLEX... menu item
	{
		check_menu_item(6, IDM_POCSAGFLEX, true);
	}
	else if (Profile.monitor_acars)		// Check ACARS... menu item
	{
		check_menu_item(6, IDM_ACARS, true);
	}
	else if (Profile.monitor_mobitex)	// Check Mobitex... menu item
	{
		check_menu_item(6, IDM_MOBITEX, true);
	}
	else if (Profile.monitor_ermes)		// Check Ermes... menu item
	{
		check_menu_item(6, IDM_ERMES, true);
	}
	check_menu_item(4, IDM_FILTERFILE_EN,     Profile.filterfile_enabled);
	check_menu_item(4, IDM_FILTERCOMMANDFILE, Profile.filter_cmd_file_enabled);

	// Check correct Langauge item.
	// Also set the current language table.

	if (Profile.lang_mi_index > lang_cnt) Profile.lang_mi_index = 0;

	Profile.reverse_msg = FALSE;
	Profile.lang_tbl_index = 0;

	if (Profile.lang_mi_index)
	{
		Profile.lang_tbl_index = (Profile.lang_mi_index-1);
		Profile.reverse_msg = lang_rev[Profile.lang_tbl_index];
	}

	if (last_lang != Profile.lang_mi_index)
	{
		if(last_lang != -1)  // Uncheck last selection
		check_menu_item(7,(IDM_ENGLISH+last_lang),FALSE);

		// Check new item
		check_menu_item(7,(IDM_ENGLISH+Profile.lang_mi_index),TRUE);

		last_lang = Profile.lang_mi_index;
	}
}

// Add required language menu items.
void set_lang_menu(void)
{
   HMENU hmenu = NULL;
   HMENU hsubmenu = NULL;
   MENUITEMINFO miInfo;
   int count,i;
   int id=1;
      
  /* get handle of main menu */
   hmenu = GetMenu(ghWnd);

  /* get handle of 1st popup menu */
   hsubmenu = GetSubMenu(hmenu, 7); 

  /* get number of items in the popup */
          count = GetMenuItemCount(hsubmenu);
   
   for (i=0; i<lang_cnt; i++)
   {
		/* append new menu item */
		miInfo.cbSize = sizeof(MENUITEMINFO);
		miInfo.fMask = MIIM_TYPE | MIIM_ID;
		miInfo.fType = MFT_STRING;
		miInfo.fState = 0;
		miInfo.wID = IDM_ENGLISH + id;
		miInfo.hSubMenu = NULL;
		miInfo.hbmpChecked = NULL;
		miInfo.hbmpUnchecked = NULL;
		miInfo.dwItemData = 0;
		miInfo.dwTypeData = lang_name[i];
		InsertMenuItem(hsubmenu, count, 1, &miInfo);

		count++;
		id++;
	}   
}
