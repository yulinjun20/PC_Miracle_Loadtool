#ifndef _VIEW_AT_FILE_H_
#define _VIEW_AT_FILE_H_


#ifndef BYTE
#define BYTE unsigned char
#endif

#ifndef WORD
#define WORD unsigned short
#endif

#ifndef DWORD
#define DWORD unsigned long
#endif


int CheckApplication(char *strFileName);

int CheckViewAtFont(char *strFileName);

int CheckVOS(char *strFileName);

int CheckParam(char *strFileName);

int CheckBoot(char *strFileName);

int CheckBale(char *strFileName);

#endif


