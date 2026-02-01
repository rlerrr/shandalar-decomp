#include <stdio.h>
#include <stdarg.h>
#include <stdlib.h>

// FUNCTION: DRAWCARDLIB 0x1000c0f0
void FUN_1000c0f0(int ok,const char *file,int line,const char *fmt,...)
{
  va_list args;
  if (ok) {
    return;
  }

  if (file != (const char *)0x0) {
    fprintf(stderr,"%s(%d): ",file,line);
  }

  if (fmt != (const char *)0x0) {
    va_start(args,fmt);
    vfprintf(stderr,fmt,args);
    va_end(args);
  }

  fputc('\n',stderr);
  exit(1);
}
