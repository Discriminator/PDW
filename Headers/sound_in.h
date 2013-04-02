#ifndef SOUND_IN_H
#define SOUND_IN_H

#define DEFAULT_HI_AUDIO	1
#define DEFAULT_LO_AUDIO	0

#define INDEX512	0
#define INDEX1200	1
#define INDEX2400	2
#define INDEX1600	3
#define INDEX3200	4

extern bool bCapturing;
extern char high_audio;
extern char low_audio;
extern long BaudRate;
extern int  cross_over;

void CALLBACK Callback_Function(HWAVEIN hwi, UINT uMsg, DWORD dwInstance, DWORD dwParam1, DWORD dwParam2);
void Process_ReadyBuffers(HWND hwnd);
void free_audio_buffers(void);
BOOL Stop_Capturing(void);
BOOL Start_Capturing(void);

void Audio_To_Bits  (char *lpAudioBuffer, long LenAudioBuffer);
void MOBITEX_To_Bits(char *lpAudioBuffer, long LenAudioBuffer);
void ACARS_To_Bits  (char *lpAudioBuffer, long LenAudioBuffer);
void ERMES_To_Bits  (char *lpAudioBuffer, long LenAudioBuffer); // PH: test

void Reset_ATB(void);
void SetAudioConfig(int sac_type);
int Get_Percent(int x,int percent);

#endif


