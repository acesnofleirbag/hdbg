#ifndef HDBG_COMMANDS_H
#define HDBG_COMMANDS_H

#include <stdbool.h>
#include <stdint.h>
#include <sys/types.h>

typedef struct command_s {
    char* short_opt;
    char* long_opt;
    void (*handler)(void);
} command_t;

typedef struct breakpoint_s {
    pid_t pid;
    uint8_t bcopy;  // bottom byte copy
    bool enable;
    intptr_t addr;
} breakpoint_t;

// forward to main.c
extern command_t commands[];

#define CMDSLEN sizeof(commands) / sizeof(command_t)

void __help(void);

// alpha sorted
void __breakpoint(void);
void __continue(void);
void __registers(void);

#endif  // HDBG_COMMANDS_H
