#ifndef PDW_LANG_H
#define PDW_LANG_H


struct lang_db_info {
   char label;
   char desc;
   struct lang_db_info *next;
};

extern char lang_name[10][40];
extern int lang_rev[10];
extern int lang_cnt;
extern struct lang_db_info *lang_table[10];


void read_language_tables(void);
BOOL find_read_string(char *name,char *id,int id_len);
struct lang_db_info *read_lang_tbl(int *ok);
void free_lang_tables(void);
void free_lang_db_info(struct lang_db_info *p_info);

char remap_ch(char c);
BOOL in_lang_tbl(char c);

#endif

