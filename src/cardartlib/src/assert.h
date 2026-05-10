#ifndef ASSERT_H
#define ASSERT_H

void assert(int condition,char *file,int line,char *fmt,...);
void assert_noabort(int condition, char *file, int line, char *fmt, ...);

extern char global_base_directory[0x105];

#endif
