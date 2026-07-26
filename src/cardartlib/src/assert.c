#include "inttypes.h"
#include <stdio.h>
#include <stdarg.h>
#include <windows.h>
#include <time.h>
#include "defs.h"

// GLOBAL: CARDARTLIB 0x10020210
// GLOBAL: DRAWCARDLIB 0x100f23e0
// GLOBAL: DECKDLL 0x10103d50
// GLOBAL: FACEMAKER 0x00411818
// GLOBAL: SHANDALAR 0x005a9580
char g_assertMessageBuffer[2000];

// GLOBAL: CARDARTLIB 0x10117800
// GLOBAL: DRAWCARDLIB 0x100f35b0
// GLOBAL: DECKDLL 0x10113d30
// GLOBAL: MAGIC 0x777860
// GLOBAL: SHANDALAR 0x0078e5f0
// GLOBAL: FACEMAKER 0x0041ae40
char global_base_directory[0x105];

// MATCHING
// FUNCTION: CARDARTLIB 0x10001000
// FUNCTION: DRAWCARDLIB 0x1000c0f0
// FUNCTION: DECKDLL 0x1000eef0
// FUNCTION: MAGIC 0x004da0e0
// FUNCTION: SHANDALAR 0x0046a520
// FUNCTION: FACEMAKER 0x00401440
void assert(int condition, char *file, int line, char *fmt, ...)
{
  time_t now;
  FILE *logFile;
  va_list args;

  if (condition == 0)
  {
#ifdef FACEMAKER
    logFile = fopen("assertFile.txt", "at");
#else
    char local_114[260];
    strcpy(local_114, global_base_directory);
    strcat(local_114, "\\assertFile.txt");
    logFile = fopen(local_114, "at");
#endif

    va_start(args, fmt);
    if ((file != (char *)0x0) && (line != 0))
    {
      sprintf(g_assertMessageBuffer, "File-> %s, Line-> %d\n", file, line);
    }

    _vsnprintf(g_assertMessageBuffer + strlen(g_assertMessageBuffer), 2000, fmt, args);

    time(&now);

    fprintf(logFile, "%s%s\n", ctime(&now), g_assertMessageBuffer);
    fclose(logFile);

    MessageBoxA((HWND)0x0, g_assertMessageBuffer, "Assertion Error", 0x1000);
    exit(0xff);
  }
}

// MATCHING
// FUNCTION: CARDARTLIB 0x10001116
// FUNCTION: DRAWCARDLIB 0x1000c206
// FUNCTION: DECKDLL 0x1000f044
// FUNCTION: FACEMAKER 0x00401531
void assert_noabort(int condition, char *file, int line, char *fmt, ...)
{
  time_t now;
  FILE *logFile;
  va_list args;

  if (condition == 0)
  {
#ifdef FACEMAKER
    logFile = fopen("assertFile.txt", "at");
#else
    char local_114[260];
    strcpy(local_114, global_base_directory);
    strcat(local_114, "\\assertFile.txt");
    logFile = fopen(local_114, "at");
#endif

    va_start(args, fmt);
    if ((file != (char *)0x0) && (line != 0))
    {
      sprintf(g_assertMessageBuffer, "File-> %s, Line-> %d\n", file, line);
    }

    _vsnprintf(g_assertMessageBuffer + strlen(g_assertMessageBuffer), 2000, fmt, args);

    time(&now);

    fprintf(logFile, "%s%s\n", ctime(&now), g_assertMessageBuffer);
    fclose(logFile);

    MessageBoxA((HWND)0x0, g_assertMessageBuffer, "Assertion Error", 0x1000);
  }
}
