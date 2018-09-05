#include <cstdio>
#include <cstdlib>

/*
insertion sort, several errors

usage: insert_sort num1 num2 num3 ..., where the numi are the numbers to be
sorted Some Preliminaries for Beginners and Pros 19


The only way that the loop at line 36 could have executed no iterations
at all is if the condition j < num_y in line 36 did not hold even when j was
0. Yet you know that num_y is 1, because you are in this function now after
having imposed the condition num_y==1 on the breakpoint. Or at least you
think you know this. Again, you haven’t confirmed it. Check this now:

(gdb) print num_y
$2 = 0

Sure enough, the condition num_y==1 did hold when you entered insert(),
but apparently num_y has changed since then. Somehow num_y became 0 after
you entered this function. But how?

ins.cpp:49:16: warning: suggest parentheses around assignment used as truth value [-Wparentheses]
   if (num_y = 0) { // y empty so far, easy case
                ^
// original

int x[10], // input array
    y[10], // workspace array
    num_inputs, // length of input array
    num_y = 0; // current number of elements in y

void get_args(int ac, char **av)
{ 
  int i;
  num_inputs = ac - 1;
  for (i = 0; i < num_inputs; i++)
    x[i] = atoi(av[i+1]);
}

void scoot_over(int jj)
{ 
  int k;
  for (k = num_y-1; k > jj; k++)
    y[k] = y[k-1];
}

void insert(int new_y)
{ 
  int j;
  if (num_y = 0) { // y empty so far, easy case
    y[0] = new_y;
    return;
  }
  // need to insert just before the first y
  // element that new_y is less than
  for (j = 0; j < num_y; j++) {
    if (new_y < y[j]) {
      // shift y[j], y[j+1],... rightward
      // before inserting new_y
      scoot_over(j);
      y[j] = new_y;
      return;
    }
  }
}

void process_data()
{
  for (num_y = 0; num_y < num_inputs; num_y++)
    // insert new y in the proper place
    // among y[0],...,y[num_y-1]
    insert(x[num_y]);
}

void print_results()
{ 
  int i;
  for (i = 0; i < num_inputs; i++)
    printf("%d\n",y[i]);
}

int main(int argc, char ** argv)
{ 
  get_args(argc,argv);
  process_data();
  print_results();
}

*/


int x[10], // input array
    y[10], // workspace array
    num_inputs, // length of input array
    num_y = 0; // current number of elements in y

void get_args(int ac, char **av)
{ 
  int i;
  num_inputs = ac - 1;
  for (i = 0; i < num_inputs; i++)
    x[i] = atoi(av[i+1]);
}

void scoot_over(int jj)
{ 
  int k;
  for (k = num_y; k > jj; k++)
    y[k] = y[k-1];
}

void insert(int new_y)
{ 
  int j;
  if (num_y == 0) { // y empty so far, easy case
    y[0] = new_y;
    return;
  }
  // need to insert just before the first y
  // element that new_y is less than
  for (j = 0; j < num_y; j++) {
    if (new_y < y[j]) {
      // shift y[j], y[j+1],... rightward
      // before inserting new_y
      scoot_over(j);
      y[j] = new_y;
      return;
    }
  }
}

void process_data()
{
  for (num_y = 0; num_y < num_inputs; num_y++)
    // insert new y in the proper place
    // among y[0],...,y[num_y-1]
    insert(x[num_y]);
}

void print_results()
{ 
  int i;
  for (i = 0; i < num_inputs; i++)
    printf("%d\n",y[i]);
}

int main(int argc, char ** argv)
{ 
  get_args(argc,argv);
  process_data();
  print_results();
}
