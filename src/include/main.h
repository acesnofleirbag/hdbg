#ifndef HDBG_H
#define HDBG_H

#include <sys/types.h>

#define __unused __attribute__((unused))
#define CMDLEN 256

char* rcmd();
void debug(pid_t cpid, char* pname);
void panic(const char* fmt, ...);
void start_debuggee(char* pname);

#endif  // HDBG_H
