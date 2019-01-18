#include <iostream>
#include <thread>
// #include <future>
#include <list>
#include <mutex>
#include <queue>
#include <stack>
#include <exception>
// #include <condition_variable>
//

#include <string.h>
#include <pthread.h>
#include <sys/syscall.h>
#include <unistd.h>

#if 0
// #include <boost/thread/shared_mutex.hpp>

// #include "gmock/gmock.h"

using namespace std;
// using namespace std::placeholders;
// using namespace testing;


// namespace cxx_condition
// {
  void errExitEN(int errnum, const char *format, ...)
  {
    cout << errnum << ", " << format << endl;
  }
 
  // LPI 30c
  // \tlpi-181022-dist.tar\tlpi-dist\threads\prod_no_condvar.c
  // A simple POSIX threads producer-consumer example that doesn't use a condition
  // variable.

  static pthread_mutex_t mtx = PTHREAD_MUTEX_INITIALIZER;

  // *sync-cond-predicate* which is global
  static int avail = 0;

  // producer

  static void* threadFunc(void *arg)
  {
    int cnt = atoi((char *)arg);
    int s, j;

    // each thread has given the number of unit to produce from main argv

    cout << "cnt: " << cnt << endl;

    for (j = 0; j < cnt; j++) 
    {
      sleep(1);

      /* Code to produce a unit omitted */

      s = pthread_mutex_lock(&mtx);
      if (s != 0)
        errExitEN(s, "pthread_mutex_lock");

      avail++;        /* Let consumer know another unit is available */

      s = pthread_mutex_unlock(&mtx);
      if (s != 0)
        errExitEN(s, "pthread_mutex_unlock");
    }

    return NULL;
  }

// } // namespace

// TEST(CConCondition, ProducerAndConsumer_LPI_No_CondVar)

int main()
{
//  using namespace cxx_condition;

  int const NUM_OF_PRODUCERS = 10;
  int NUM_OF_ITEMS_TO_PRODUCE = 100;

  pthread_t tid;
  int s, j;
  int totRequired;            /* Total number of units that all
                                 threads will produce */
  int numConsumed;            /* Total units so far consumed */
  bool done;
  time_t t;

  t = time(NULL);

  // Create all producers

  totRequired = 0;

  // for (j = 1; j < argc; j++) {
  //   totRequired += atoi(argv[j]);

  //   s = pthread_create(&tid, NULL, threadFunc, argv[j]);
  //   if (s != 0)
  //     errExitEN(s, "pthread_create");
  // }

  for (j = 1; j < NUM_OF_PRODUCERS; j++) {
    totRequired += NUM_OF_ITEMS_TO_PRODUCE;

    s = pthread_create(&tid, NULL, threadFunc, (void *)&NUM_OF_ITEMS_TO_PRODUCE);
    if (s != 0)
    {
      // errExitEN(s, "pthread_create");
      cout << s << ", pthread_create" << endl;
    }

    cout << "produced: " << avail << endl;
  }

  // single consumer
  // Use a polling loop to check for available units

  numConsumed = 0;
  done = false;

  for (;;) {
    s = pthread_mutex_lock(&mtx);
    if (s != 0)
      errExitEN(s, "pthread_mutex_lock");

    // cout << "avil: " << avail << endl;

    // Consume all available units
    while (avail > 0) {

      // Do something with produced unit

      cout << "cousumer: " << endl;

      numConsumed ++;
      avail--;
      printf("T=%ld: numConsumed=%d\n", (long) (time(NULL) - t),
          numConsumed);

      done = numConsumed >= totRequired;
    }

    s = pthread_mutex_unlock(&mtx);
    if (s != 0)
      errExitEN(s, "pthread_mutex_unlock");

    if (done)
      break;

    // Perhaps do other work here that does not require mutex lock
  }

  exit(EXIT_SUCCESS);
}
#endif

static void * threadFunc(void *arg)
{
    const char *s = (char *) arg;
    printf("%s", s);
    printf("sleeps for 30s, getpid=%ld, gettid=%ld\n", getpid(), syscall(SYS_gettid));
    // printf("sleeps for 30s, getpid=%ld\n", getpid());
    sleep(30);
    printf("wakes up\n");
    return (void *) strlen(s);
}

int main(int argc, char *argv[])
{
    pthread_t t1;
    void *res;
    int s;
    s = pthread_create(&t1, NULL, threadFunc, (void*)"Hello world\n");
    if (s != 0)
    {
        printf("pthread_create failed");
        exit(EXIT_FAILURE);
    }

    printf("Message from main() and pid = %ld\n", getpid() );

    s = pthread_join(t1, &res);
    if (s != 0)
    {
        printf("pthread_join failed");
        exit(EXIT_FAILURE);
    }

    printf("Thread returned %ld\n", (long) res);
    exit(EXIT_SUCCESS);
}
