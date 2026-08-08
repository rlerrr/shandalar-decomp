#ifndef ASSERT_H
#define ASSERT_H

void assert(int condition, const char *file, int line, const char *fmt, ...);
void assert_noabort(int condition, const char *file, int line, const char *fmt, ...);

extern char global_base_directory[0x105];

#endif
