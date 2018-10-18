
// from SO
// kyoupark@kt-office-debian:~/works$ ./a.out
// 0x40793a ~ 0x407a0d ~ 0x407a19
// si:0
// ip:407a13
// Segmentation fault

#define _GNU_SOURCE 1
#include <iostream>
#include <iomanip>
#include <signal.h>
#include <ucontext.h>

using std::cout;

static volatile int *causecrash;

static void handler(int, siginfo_t *si, void *ptr)
{
   ucontext_t *uc = (ucontext_t *)ptr;

   cout << "si:" << si->si_addr << '\n';

#ifdef __x86_64__
   cout << "ip:" << std::hex << uc->uc_mcontext.gregs[REG_RIP] << '\n';
#else
   cout << "ip:" << std::hex << uc->uc_mcontext.gregs[REG_EIP] << '\n';
#endif
}

int main()
{
begin:
    cout.setf(std::ios::unitbuf);
    cout << &&begin << " ~ " << &&before << " ~ " << &&after << '\n';

    struct sigaction s;
    s.sa_flags = SA_SIGINFO|SA_RESETHAND;
    s.sa_sigaction = handler;
    sigemptyset(&s.sa_mask);
    sigaction(SIGSEGV, &s, 0);

before:
    *causecrash = 0;
after:
    cout << "End.\n";
}
