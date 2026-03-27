#include "inttypes.h"
#include <stdio.h>
#include <stdarg.h>
#include <windows.h>
#include <time.h>
#include "defs.h"

// GLOBAL: CARDARTLIB 0x1001d05c
// GLOBAL: DRAWCARDLIB 0x10026a94
// GLOBAL: DECKDLL 0x100331c0
char s__assertFile_txt_1001d05c[] = "\\assertFile.txt";

// GLOBAL: CARDARTLIB 0x1001d070
// GLOBAL: DRAWCARDLIB 0x10026aa8
// GLOBAL: DECKDLL 0x100331d4
char s_File__s__Line__d_1001d070[] = "File-> %s, Line-> %d\n";

// GLOBAL: CARDARTLIB 0x1001d088
// GLOBAL: DRAWCARDLIB 0x10026ac0
// GLOBAL: DECKDLL 0x100331ec
char s__s_s_1001d088[] = "%s%s\n";

// GLOBAL: CARDARTLIB 0x1001d06c
// GLOBAL: DRAWCARDLIB 0x10026aa4
// GLOBAL: DECKDLL 0x100331d0
char s_at_1001d06c[] = "at";

// GLOBAL: CARDARTLIB 0x1001d090
// GLOBAL: DRAWCARDLIB 0x10026ac8
// GLOBAL: DECKDLL 0x10033208
char s_Assertion_Error_1001d090[] = "Assertion Error";

// GLOBAL: CARDARTLIB 0x1001d0a0
// GLOBAL: DRAWCARDLIB 0x10026ad8
// GLOBAL: DECKDLL 0x10033218
char s__assertFile_txt_1001d0a0[] = "\\assertFile.txt";

// GLOBAL: CARDARTLIB 0x1001d0b0
// GLOBAL: DRAWCARDLIB 0x10026ae8
// GLOBAL: DECKDLL 0x10033228
char s_at_1001d0b0[] = "at";

// GLOBAL: CARDARTLIB 0x1001d0b4
// GLOBAL: DRAWCARDLIB 0x10026aec
// GLOBAL: DECKDLL 0x1003322c
char s_File__s__Line__d_1001d0b4[] = "File-> %s, Line-> %d\n";

// GLOBAL: CARDARTLIB 0x1001d0cc
// GLOBAL: DRAWCARDLIB 0x10026b04
// GLOBAL: DECKDLL 0x10033244
char s__s_s_1001d0cc[] = "%s%s\n";

// GLOBAL: CARDARTLIB 0x1001d0d4
// GLOBAL: DRAWCARDLIB 0x10026b0c
// GLOBAL: DECKDLL 0x10033260
char s_Assertion_Error_1001d0d4[] = "Assertion Error";

// GLOBAL: CARDARTLIB 0x10020210
// GLOBAL: DRAWCARDLIB 0x100f23e0
// GLOBAL: DECKDLL 0x10103d50
char g_assertMessageBuffer[2000];

// GLOBAL: CARDARTLIB 0x10117800
// GLOBAL: DRAWCARDLIB 0x100f35b0
// GLOBAL: DECKDLL 0x10113d30
// GLOBAL: MAGIC 0x777860
char global_base_directory[0x105];

// MATCHING
// FUNCTION: CARDARTLIB 0x10001000
// FUNCTION: DRAWCARDLIB 0x1000c0f0
// FUNCTION: DECKDLL 0x1000eef0
void assert(int condition,char *file,int line,char *fmt,...)
{
  char local_114 [260];
  time_t now;
  FILE *logFile;
  va_list args;

  if (condition == 0) {
    strcpy(local_114,global_base_directory);
    strcat(local_114,s__assertFile_txt_1001d05c);
    logFile = fopen(local_114,s_at_1001d06c);

    va_start(args,fmt);
    if ((file != (char *)0x0) && (line != 0)) {
      sprintf(g_assertMessageBuffer,s_File__s__Line__d_1001d070,file,line);
    }

    _vsnprintf(g_assertMessageBuffer + strlen(g_assertMessageBuffer),2000,fmt,args);
    
    time(&now);

    fprintf(logFile,s__s_s_1001d088,ctime(&now),g_assertMessageBuffer);
    fclose(logFile);

    MessageBoxA((HWND)0x0,g_assertMessageBuffer,s_Assertion_Error_1001d090,0x1000);
    exit(0xff);  
  }
}

// MATCHING
// FUNCTION: CARDARTLIB 0x10001116
// FUNCTION: DRAWCARDLIB 0x1000c206
// FUNCTION: DECKDLL 0x1000f044
void assert_noabort(int condition,char *file,int line,char *fmt,...)
{
  char local_114 [260];
  time_t now;
  FILE *logFile;
  va_list args;

  if (condition == 0) {
    strcpy(local_114,global_base_directory);
    strcat(local_114,s__assertFile_txt_1001d0a0);
    logFile = fopen(local_114,s_at_1001d0b0);

    va_start(args,fmt);
    if ((file != (char *)0x0) && (line != 0)) {
      sprintf(g_assertMessageBuffer,s_File__s__Line__d_1001d0b4,file,line);
    }

    _vsnprintf(g_assertMessageBuffer + strlen(g_assertMessageBuffer),2000,fmt,args);
    
    time(&now);

    fprintf(logFile,s__s_s_1001d0cc,ctime(&now),g_assertMessageBuffer);
    fclose(logFile);

    MessageBoxA((HWND)0x0,g_assertMessageBuffer,s_Assertion_Error_1001d0d4,0x1000);
  }
}
