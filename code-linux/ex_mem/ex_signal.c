#include <cstdio>
#include <signal.h>

void my_sigint_handler(int signum)
{
  printf("I received signal %d (that's SIGINT to you).\n", signum);
  puts("Tee Hee! That tickles!\n");
}

int main()
{
  char choicestr[20];
  int choice;

  while(1)
  {
    puts("1. ignore control C");
    puts("2. custom handle control C");
    puts("3. use the default handler control C");
    puts("4. raise a SIGSEGV on myself");
    printf("enter your choice: ");

    fgets(choicestr, 20, stdin);
    sscanf(choicestr, "%d", &choice);

    // ignore SIGNIT
    if (choice == 1)
      signal(SIGINT, SIG_IGN);
    else if (choice == 2)
      signal(SIGINT, my_sigint_handler);
    else if (choice == 3)
      signal(SIGINT, SIG_DFL);
    else if (choice == 4)
      raise(SIGSEGV);
    else
      puts("whatever you say, guv'nor.\n\n");
  }

  return 0;
}

