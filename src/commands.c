#include "include/commands.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/ptrace.h>
#include <sys/user.h>
#include <unistd.h>

#define SEP "\x20"

// forward to main.c
extern char* cmd;
extern breakpoint_t* brkp;

// clang-format off
static char 
HELP[] = 
    "help(h)                    this help message\n"
    "breakpoint(b) [addr]       set a breakpoint on given address\n"
    "continue(c)                continue execution\n"
    "exit                       exit debugging\n";
// clang-format on

void
__help(void) {
    printf("%s", HELP);
}

void
__breakpoint(void) {
    if (!brkp)
        brkp = malloc(sizeof(breakpoint_t));

    strtok(cmd, SEP);
    void* memaddr = (void*) strtok(NULL, SEP);

    printf("INFO: Setting breakpoint at %p\n", memaddr);

    long int data = ptrace(PTRACE_PEEKDATA, getpid(), memaddr, NULL);

    brkp->pid = getpid();
    brkp->addr = (intptr_t) memaddr;
    brkp->bcopy = (data & 0xff);
    brkp->enable = true;

    ptrace(PTRACE_POKEDATA, getpid(), memaddr, ((data & ~0xff) | /*INT 3 instruction=*/0xcc));
}

void
__continue(void) {
    long int data = ptrace(PTRACE_PEEKDATA, brkp->pid, brkp->addr, NULL);
    ptrace(PTRACE_POKEDATA, brkp->pid, brkp->addr, ((data & ~0xff) | brkp->bcopy));

    brkp->enable = false;
}

void
__registers(void) {
    struct user_regs_struct regs;

    ptrace(PTRACE_GETREGS, getpid(), 0, &regs);
}
