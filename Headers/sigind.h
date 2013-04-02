#ifndef SIGIND_H
#define SIGIND_H

void DrawSigInd(HWND hwnd);
void FreeSigInd(void);
BOOL LoadSigInd(HINSTANCE hThisInstance);
void UpdateSigInd(int direction_flg);
//void UpdateAudioSigInd(HWND hwnd, int sample_value);
void show_sigind(int new_pos,int old_pos);


#endif
