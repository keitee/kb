#include "gmock/gmock.h"

#include <iostream>

// g++ -g -std=c++0x t_override.cpp

using namespace std;
using namespace testing;

// // #define _GNU_SOURCE /* Get '_sys_nerr' and '_sys_errlist' declarations from <stdio.h> */
// #include <sys/syscall.h>
// #include <sys/types.h>
// #include <unistd.h>
#include <pthread.h>

// ={=========================================================================
namespace lpi_30_1 {

/*
LPI-30 
Listing 30-1: Incorrectly incrementing a global variable from two threads
––––––––––––––––––––––––––––––––––––––––––––––––––––– threads/thread_incr.c
*/

static int glob = 0;
static int gloops = 100000000; // 100M

// Loop 'arg' times incrementing 'glob'
static void * threadFunc1(void *arg)
{
  int loops = *((int *) arg);
  int loc, j;
  for (j = 0; j < loops; j++) {
    loc = glob;
    loc++;
    glob = loc;
  }
  return NULL;
}

// Loop 'arg' times incrementing 'glob'
static void * threadFunc2(void *arg)
{
  int loops = *((int *) arg);
  int j;
  for (j = 0; j < loops; j++) {
    ++glob;
  }
  return NULL;
}

#if 0

https://gcc.gnu.org/onlinedocs/gcc-4.9.4/gcc/_005f_005fsync-Builtins.html#_005f_005fsync-Builtins

6.51 Legacy __sync Built-in Functions for Atomic Memory Access

The following built-in functions are intended to be compatible with those
described in the Intel Itanium Processor-specific Application Binary Interface,
section 7.4. As such, they depart from the normal GCC practice of using the
‘__builtin_’ prefix, and further that they are overloaded such that they work on
multiple types.

The definition given in the Intel documentation allows only for the use of the
types int, long, long long as well as their unsigned counterparts. GCC allows
any integral scalar or pointer type that is 1, 2, 4 or 8 bytes in length.

Not all operations are supported by all target processors. If a particular
operation cannot be implemented on the target processor, a warning is generated
and a call an external function is generated. The external function carries the
same name as the built-in version, with an additional suffix ‘_n’ where n is the
size of the data type.  In most cases, these built-in functions are considered a
full barrier. That is, no memory operand is moved across the operation, either
forward or backward. Further, instructions are issued as necessary to prevent
the processor from speculating loads across the operation and from queuing
stores after the operation.

All of the routines are described in the Intel documentation to take “an
optional list of variables protected by the memory barrier”. It's not clear what
is meant by that; it could mean that only the following variables are protected,
or it could mean that these variables should in addition be protected. At
present GCC ignores this list and protects all variables that are globally
accessible. If in the future we make some use of this list, an empty list will
continue to mean all globally accessible variables.

type __sync_fetch_and_add (type *ptr, type value, ...)
type __sync_fetch_and_sub (type *ptr, type value, ...)
type __sync_fetch_and_or (type *ptr, type value, ...)
type __sync_fetch_and_and (type *ptr, type value, ...)
type __sync_fetch_and_xor (type *ptr, type value, ...)
type __sync_fetch_and_nand (type *ptr, type value, ...)

These built-in functions perform the operation suggested by the name, and
returns the value that had previously been in memory. That is,

          { tmp = *ptr; *ptr op= value; return tmp; }
          { tmp = *ptr; *ptr = ~(tmp & value); return tmp; }   // nand

Note: GCC 4.4 and later implement __sync_fetch_and_nand as *ptr = ~(tmp & value)
instead of *ptr = ~tmp & value. 

//Source: http://golubenco.org/atomic-operations.html
#ifndef _ATOMIC_H
#define _ATOMIC_H

/* Check GCC version, just to be safe */
#if !defined(__GNUC__) || (__GNUC__ < 4) || (__GNUC_MINOR__ < 1)
# error atomic.h works only with GCC newer than version 4.1
#endif /* GNUC >= 4.1 */

/**
 * Atomic type.
 */
typedef struct {
	volatile int counter;
} atomic_t;

#define ATOMIC_INIT(i)  { (i) }

/**
 * Read atomic variable
 * @param v pointer of type atomic_t
 *
 * Atomically reads the value of @v.
 */
#define atomic_read(v) ((v)->counter)

/**
 * Set atomic variable
 * @param v pointer of type atomic_t
 * @param i required value
 */
#define atomic_set(v,i) (((v)->counter) = (i))

/**
 * Add to the atomic variable
 * @param i integer value to add
 * @param v pointer of type atomic_t
 */
static inline void atomic_add( int i, atomic_t *v )
{
	(void)__sync_add_and_fetch(&v->counter, i);
}

/**
 * Subtract the atomic variable
 * @param i integer value to subtract
 * @param v pointer of type atomic_t
 *
 * Atomically subtracts @i from @v.
 */
static inline void atomic_sub( int i, atomic_t *v )
{
	(void)__sync_sub_and_fetch(&v->counter, i);
}

/**
 * Subtract value from variable and test result
 * @param i integer value to subtract
 * @param v pointer of type atomic_t
 *
 * Atomically subtracts @i from @v and returns
 * true if the result is zero, or false for all
 * other cases.
 */
static inline int atomic_sub_and_test( int i, atomic_t *v )
{
	return !(__sync_sub_and_fetch(&v->counter, i));
}

/**
 * Increment atomic variable
 * @param v pointer of type atomic_t
 *
 * Atomically increments @v by 1.
 */
static inline void atomic_inc( atomic_t *v )
{
	(void)__sync_fetch_and_add(&v->counter, 1);
}

/**
 * @brief decrement atomic variable
 * @param v: pointer of type atomic_t
 *
 * Atomically decrements @v by 1.  Note that the guaranteed
 * useful range of an atomic_t is only 24 bits.
 */
static inline void atomic_dec( atomic_t *v )
{
	(void)__sync_fetch_and_sub(&v->counter, 1);
}

/**
 * @brief Decrement and test
 * @param v pointer of type atomic_t
 *
 * Atomically decrements @v by 1 and
 * returns true if the result is 0, or false for all other
 * cases.
 */
static inline int atomic_dec_and_test( atomic_t *v )
{
	return !(__sync_sub_and_fetch(&v->counter, 1));
}

/**
 * @brief Increment and test
 * @param v pointer of type atomic_t
 *
 * Atomically increments @v by 1
 * and returns true if the result is zero, or false for all
 * other cases.
 */
static inline int atomic_inc_and_test( atomic_t *v )
{
	return !(__sync_add_and_fetch(&v->counter, 1));
}

/**
 * @brief add and test if negative
 * @param v pointer of type atomic_t
 * @param i integer value to add
 *
 * Atomically adds @i to @v and returns true
 * if the result is negative, or false when
 * result is greater than or equal to zero.
 */
static inline int atomic_add_negative( int i, atomic_t *v )
{
	return (__sync_add_and_fetch(&v->counter, i) < 0);
}

#endif

#endif

/**
 * Atomic type.
 */
typedef struct {
	volatile int counter;
} atomic_t;

/**
 * Increment atomic variable
 * @param v pointer of type atomic_t
 *
 * Atomically increments @v by 1.
 */
static inline void atomic_inc( atomic_t *v )
{
	(void)__sync_fetch_and_add(&v->counter, 1);
}


static atomic_t gcount;

// Loop 'arg' times incrementing 'glob'
static void * threadFunc3(void *arg)
{
  int loops = *((int *) arg);
  int j;
  for (j = 0; j < loops; j++) {
    atomic_inc(&gcount);
  }
  return NULL;
}


pthread_mutex_t mtx = PTHREAD_MUTEX_INITIALIZER;

static void * threadFunc4(void *arg)
{
  int loops = *((int *) arg);
  int j;
  for (j = 0; j < loops; j++) {
    pthread_mutex_lock(&mtx);
    ++glob;
    pthread_mutex_unlock(&mtx);
  }
  return NULL;
}

} // namespace

TEST(Async, RaceCondition)
{
  using namespace lpi_30_1;

  pthread_t t1, t2;

  int s;

  glob = 0;

  s = pthread_create(&t1, NULL, threadFunc1, &gloops);
  EXPECT_EQ(s, 0);

  s = pthread_create(&t2, NULL, threadFunc1, &gloops);
  EXPECT_EQ(s, 0);

  s = pthread_join(t1, NULL);
  EXPECT_EQ(s, 0);

  s = pthread_join(t2, NULL);
  EXPECT_EQ(s, 0);

  // expects 200M if there's no race
  cout << "glob = " << glob << endl;
}

TEST(Async, IsNotRaceCondition)
{
  using namespace lpi_30_1;

  pthread_t t1, t2;

  int s;

  glob = 0;

  s = pthread_create(&t1, NULL, threadFunc2, &gloops);
  EXPECT_EQ(s, 0);

  s = pthread_create(&t2, NULL, threadFunc2, &gloops);
  EXPECT_EQ(s, 0);

  s = pthread_join(t1, NULL);
  EXPECT_EQ(s, 0);

  s = pthread_join(t2, NULL);
  EXPECT_EQ(s, 0);

  // expects 200M if there's no race
  cout << "glob = " << glob << endl;
}

TEST(Async, UseAtomic)
{
  using namespace lpi_30_1;

  pthread_t t1, t2;

  int s;

  s = pthread_create(&t1, NULL, threadFunc3, &gloops);
  EXPECT_EQ(s, 0);

  s = pthread_create(&t2, NULL, threadFunc3, &gloops);
  EXPECT_EQ(s, 0);

  s = pthread_join(t1, NULL);
  EXPECT_EQ(s, 0);

  s = pthread_join(t2, NULL);
  EXPECT_EQ(s, 0);

  // expects 200M if there's no race
  cout << "glob = " << gcount.counter << endl;
}

TEST(Async, UseMutex)
{
  using namespace lpi_30_1;

  pthread_t t1, t2;

  int s;

  glob = 0;

  s = pthread_create(&t1, NULL, threadFunc4, &gloops);
  EXPECT_EQ(s, 0);

  s = pthread_create(&t2, NULL, threadFunc4, &gloops);
  EXPECT_EQ(s, 0);

  s = pthread_join(t1, NULL);
  EXPECT_EQ(s, 0);

  s = pthread_join(t2, NULL);
  EXPECT_EQ(s, 0);

  // expects 200M if there's no race
  cout << "glob = " << gcount.counter << endl;
}


// ={=========================================================================
int main(int argc, char **argv)
{
  testing::InitGoogleMock(&argc, argv);
  return RUN_ALL_TESTS();
}
