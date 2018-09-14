/* Code sample: using ptrace for simple tracing of a child process.
**
** Note: this was originally developed for a 32-bit x86 Linux system; some
** changes may be required to port to x86-64.
**
** Eli Bendersky (http://eli.thegreenplace.net)
** This code is in the public domain.
*/
#include <stdio.h>
#include <stdarg.h>
#include <stdlib.h>
#include <signal.h>
#include <syscall.h>
#include <sys/ptrace.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/reg.h>
#include <sys/user.h>
#include <unistd.h>
#include <errno.h>


/* Print a message to stdout, prefixed by the process ID
*/
void procmsg(const char* format, ...)
{
    va_list ap;
    fprintf(stdout, "[%d] ", getpid());
    va_start(ap, format);
    vfprintf(stdout, format, ap);
    va_end(ap);
}


void run_target(const char* programname)
{
    procmsg("target started. will run '%s'\n", programname);

    /* Allow tracing of this process */
    if (ptrace(PTRACE_TRACEME, 0, 0, 0) < 0) {
        perror("ptrace");
        return;
    }

    /* Replace this process's image with the given program */
    execl(programname, programname, 0);
}

/*

http://flint.cs.yale.edu/cs421/papers/x86-asm/asm.html

Control Flow Instructions

The x86 processor maintains an instruction pointer (EIP) register that is a
32-bit value indicating the location in memory where the current instruction
starts. Normally, it increments to point to the next instruction in memory
begins after execution an instruction. The EIP register cannot be manipulated
directly, but is updated implicitly by provided control flow instructions.


https://keithtech.wordpress.com/2013/10/21/how-to-use-ptrace_getregs/

Ptrace is a system call which can be called in a number of different ways with a
number of different flags. This particular flag allows you to get the current
state of the registers for a certain process.

Ptrace itself is a function which aids in the controlling/observing of a process
from another process. Applications I know of that utilize this include debuggers
and process tracing tools, but I’m sure there are others.

There isn’t a return value from this, but the fourth parameter is filled with
the register data into a struct named user_regs_struct. Simply #include
<sys/user.h> and you’ll be able to use the struct.

Personally I have a 64 bit x86 system, so I’m accessing registers like rip (in
example) and rax instead of (in 32 bit) eip and eax.


/usr/include/x86_64-linux-gnu/sys/user.h

The whole purpose of this file is for GDB and GDB only.  Don't read
   too much into it.  Don't use it for anything other than GDB unless
   you know what you are doing.

#ifdef __x86_64__

struct user_regs_struct
{
  __extension__ unsigned long long int r15;
  __extension__ unsigned long long int r14;
  __extension__ unsigned long long int r13;
  __extension__ unsigned long long int r12;
  __extension__ unsigned long long int rbp;
  __extension__ unsigned long long int rbx;
  __extension__ unsigned long long int r11;
  __extension__ unsigned long long int r10;
  __extension__ unsigned long long int r9;
  __extension__ unsigned long long int r8;
  __extension__ unsigned long long int rax;
  __extension__ unsigned long long int rcx;
  __extension__ unsigned long long int rdx;
  __extension__ unsigned long long int rsi;
  __extension__ unsigned long long int rdi;
  __extension__ unsigned long long int orig_rax;
  __extension__ unsigned long long int rip;
  __extension__ unsigned long long int cs;
  __extension__ unsigned long long int eflags;
  __extension__ unsigned long long int rsp;
  __extension__ unsigned long long int ss;
  __extension__ unsigned long long int fs_base;
  __extension__ unsigned long long int gs_base;
  __extension__ unsigned long long int ds;
  __extension__ unsigned long long int es;
  __extension__ unsigned long long int fs;
  __extension__ unsigned long long int gs;
};

#else
// These are the 32-bit x86 structures.

struct user_regs_struct
{
  long int ebx;
  long int ecx;
  long int edx;
  long int esi;
  long int edi;
  long int ebp;
  long int eax;
  long int xds;
  long int xes;
  long int xfs;
  long int xgs;
  long int orig_eax;
  long int eip;
  long int xcs;
  long int eflags;
  long int esp;
  long int xss;
};

#endif

*/


void run_debugger(pid_t child_pid)
{
    int wait_status;
    unsigned icounter = 0;
    procmsg("debugger started\n");

    /* Wait for child to stop on its first instruction */
    wait(&wait_status);

    while (WIFSTOPPED(wait_status)) {
        icounter++;
        struct user_regs_struct regs;
        ptrace(PTRACE_GETREGS, child_pid, 0, &regs);


        // 32 bits
        // unsigned instr = ptrace(PTRACE_PEEKTEXT, child_pid, regs.eip, 0);
        // procmsg("icounter = %u.  EIP = 0x%08x.  instr = 0x%08x\n",
        //            icounter, regs.eip, instr);

        // 64 bits
        unsigned instr = ptrace(PTRACE_PEEKTEXT, child_pid, regs.rip, 0);
        procmsg("icounter = %u.  EIP = 0x%08x.  instr = 0x%08x\n",
                    icounter, regs.rip, instr);

        /* Make the child execute another instruction */
        if (ptrace(PTRACE_SINGLESTEP, child_pid, 0, 0) < 0) {
            perror("ptrace");
            return;
        }

        /* Wait for child to stop on its next instruction */
        wait(&wait_status);
    }

    procmsg("the child executed %u instructions\n", icounter);
}


int main(int argc, char** argv)
{
    pid_t child_pid;

    if (argc < 2) {
        fprintf(stderr, "Expected a program name as argument\n");
        return -1;
    }

    child_pid = fork();

    // child
    if (child_pid == 0)
        run_target(argv[1]);
    // parent
    else if (child_pid > 0)
        run_debugger(child_pid);
    else {
        perror("fork");
        return -1;
    }

    return 0;
}
