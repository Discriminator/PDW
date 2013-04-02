#ifndef PDW_MISC_H
#define PDW_MISC_H

#define MSG_CAPCODE		1
#define MSG_TIME		2
#define MSG_DATE		3
#define MSG_MODE		4
#define MSG_TYPE		5
#define MSG_BITRATE		6
#define MSG_MESSAGE		7
#define MSG_MOBITEX		8

#define FLEXGROUPMODE_LOGGING			0x02
#define FLEXGROUPMODE_COMBINE			0x04
#define FLEXGROUPMODE_HIDEGROUPCODES	0x08

extern unsigned long int iSecondsElapsed;

extern char Current_MSG[9][MAX_STR_LEN];	// PH: Buffer for all message items
extern unsigned char message_buffer[MAX_STR_LEN+1];
extern unsigned char mobitex_buffer[MAX_STR_LEN+1];
extern BYTE messageitems_colors[7];			// buffer for message items colors

// Extra Globals used by display_show_char().
extern char *dsc_pchar;
extern BYTE *dsc_pcolor;

// ******************* Prototypes ******************

void reverse_msg_words(void);
void misc_debug_msg(char *msg);
int  short_nOnes(char k);
void build_show_line(PaneStruct *pane, char cin, int option);
void display_line(PaneStruct *pane);
void display_show_strV2(PaneStruct *pane, char strin[]);

void AddAssignment(int assignedframe, int groupbit, int capcode);
void ConvertGroupcall(int groupbit, char *vtype, int capcode);
void SortGroupCall(int groupbit);
void Check4_MissedGroupcalls(void);
void Remove_MissedGroupcall(int groupbit);

char LogFileHandling(int file, char *szFileName, int action);

bool PlayWaveFile(bool bMONITOR_ONLY, bool bFILTERED, bool bPlay);
void CollectLogfileLine(char *string, bool bFilterfile);
void ActivateCommandFile();
int  Check_4_Filtermatch();
int  CompareMessage(int item, int mon_or_filt);
char *MakeFilterLabel(char *szLabel, char *szCapcode, char *szNewLabel);

void CountBiterrors(int errors);
//void Update_RX_Quality();
void InvertData(void);

#endif
