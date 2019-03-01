#include <iostream>
#include <string.h>

using namespace std;


// ={=========================================================================
int main(int argc, char **argv)
{
  (void) argc;
  (void) argv;

  timespec tp;

  clock_gettime(CLOCK_REALTIME, &tp);

  // sec and us (micro sec)
  printf("time %.010lu.%.06lu\n", 
      (unsigned long)tp.tv_sec, (unsigned long) (tp.tv_nsec / 1000));

  auto localtm = localtime(&tp.tv_sec);
  cout << "The local date and time: " << asctime(localtm) << endl;

  return EXIT_SUCCESS;
}


/*
 To build C and embedded 

#include <stdio.h>
#include <time.h>

// ={=========================================================================
int main(int argc, char **argv)
{
  (void) argc;
  (void) argv;

  struct timespec tp;

  clock_gettime(CLOCK_REALTIME, &tp);

  // sec and us (micro sec)
  printf("time %.010lu.%.06lu\n", 
      (unsigned long)tp.tv_sec, (unsigned long) (tp.tv_nsec / 1000));

  struct tm* localtm = localtime(&tp.tv_sec);
  printf("The local date and time: %s\n", asctime(localtm));

  return 0;
}

*/
