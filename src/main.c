#include "include/main.h"
#include "include/commands.h"
#include <stdarg.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/personality.h>
#include <sys/ptrace.h>
#include <sys/reg.h>
#include <sys/user.h>
#include <sys/wait.h>
#include <unistd.h>

char* cmd = NULL;
breakpoint_t* brkp = NULL;

// clang-format off
static char 
HEADER[] = 
    "== The Hollow Debugger ==\n\n"
    "Copyright (C) 2023 The Hollow Debugger\n"
    "For help type 'help'\n\n"
    "== The Hollow Debugger ==\n\n";

command_t 
commands[] = {
    { "c", "continue", &__continue },
    { "h", "help", &__help },
    { "b", "breakpoint", &__breakpoint },
};
// clang-format on

static void
__exit(void) {
    free(cmd);
    cmd = NULL;

    free(brkp);
    brkp = NULL;

    exit(0);
}

int
main(int argc, char** argv, __unused char** envp) {
    if (argc < 2)
        panic("Usage: hdbg [binname]\n");

    char* pname = argv[1];
    pid_t cpid;

    switch ((cpid = fork())) {
        case -1:
            panic("ERR: Cannot fork %d@%s\n", getpid(), pname);
            break;
        case 0:  // child process
            /* #ifdef TEST */
            // disable address space randomization
            personality(ADDR_NO_RANDOMIZE);
            /* #endif */
            start_debuggee(pname);
            break;
        default:  // parent process
            debug(cpid, pname);
            break;
    }

    return 0;
}

char*
rcmd() {
    int input, pos = 0, buflen = CMDLEN;

    if (!cmd)
        cmd = malloc(sizeof(char) * buflen);
    else
        memset(cmd, 0, sizeof(buflen));

    printf("hdbg: ");

    while (1) {
        input = getchar();

        if (input == EOF || input == '\n') {
            cmd[pos++] = '\0';
            return cmd;
        } else
            cmd[pos++] = input;

        if (pos >= buflen) {
            buflen += CMDLEN;
            cmd = realloc(cmd, buflen);
        }
    }
}

void
debug(pid_t cpid, char* pname) {
    int debuggee_status;

    wait(&debuggee_status);

    printf("%s", HEADER);
    printf("INFO: Debugger started on process %s@%d\n\n", pname, cpid);

    while (rcmd() && WIFSTOPPED(debuggee_status)) {
        for (long unsigned int i = 0; i < CMDSLEN; i++) {
            // clang-format off
            if (strcmp(strtok(cmd, "\x20"), commands[i].short_opt) == 0 ||
                strcmp(strtok(cmd, "\x20"), commands[i].long_opt) == 0) {
                commands[i].handler();
                break;
            }
            // clang-format on
        }

        if (strcmp(cmd, "exit") == 0)
            __exit();

        /*
        struct user_regs_struct regs;

        ptrace(PTRACE_GETREGS, cpid, 0, &regs);

        uint64_t instr = ptrace(PTRACE_PEEKTEXT, cpid, regs.rip, 0);

        // Make the child execute another instruction
        if (ptrace(PTRACE_SINGLESTEP, cpid, 0, 0) < 0)
            panic("ERR: Cannot trace on SINGLESTEP mode\n");

        // Wait for child to stop on its next instruction
        wait(&debuggee_status);
        */
    }
}

void
panic(const char* fmt, ...) {
    va_list ap;

    va_start(ap, fmt);
    fprintf(stderr, fmt, ap);
    va_end(ap);

    exit(1);
}

void
start_debuggee(char* pname) {
    if (ptrace(PTRACE_TRACEME, 0, NULL, NULL))
        panic("ERR: Cannot trace the current process on parent\n");

    execl(pname, pname, NULL);
}
