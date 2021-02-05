#include "lpi_error.h"
// #include "slog.h"
#include <cerrno>
#include <cstdlib> // exit
#include <fcntl.h> // Defines O_* constants
// #include <sys/file.h>
// #include <sys/wait.h> // Defines mode constants
#include <cstring>
#include <poll.h>
#include <sstream>
#include <unistd.h> // pipe()

/*
Listing 63-2 provides a simple demonstration of the use of poll(). This program
creates a number of pipes (each pipe uses a consecutive pair of file
descriptors), writes bytes to the write ends of randomly selected pipes, and
then performs a poll() to see which pipes have data available for reading.

The following shell session shows an example of what we see when running this
program. The command-line arguments to the program specify that ten pipes
should be created, and writes should be made to three randomly selected pipes.

$ ./poll_pipes 10 3
Writing to fd: 4 (read fd: 3)
Writing to fd: 14 (read fd: 13)
Writing to fd: 14 (read fd: 13)
poll() returned: 2
Readable: 3
Readable: 13

From the above output, we can see that poll() found two pipes had data available
for reading.


keitee@kit-ubuntu:~/git/kb/code-cxx/os-io/build$ ./io_poll_pipes 10 3
created 10 pipes
ok, writes on random pipes
writing to fd:  12 (read fd:  11)
writing to fd:  12 (read fd:  11)
writing to fd:  14 (read fd:  13)
ok, run poll on read pipes
poll returned 2
Readable: 4  11
Readable: 5  13
keitee@kit-ubuntu:~/git/kb/code-cxx/os-io/build$


Listing 63-2: Using poll() to monitor multiple file descriptors
––––––––––––––––––––––––––––––––––––––––––––––––––––––– altio/poll_pipes.c

*/

static void usageError(const char *progName)
{
  fprintf(stderr, "Usage: %s {num-pipes [num-writes]\n", progName);
  fprintf(stderr, "    e.g.: %s 10 3\n", progName);
  exit(EXIT_FAILURE);
}

int main(int argc, char **argv)
{
  struct timeval timeout;
  struct timeval *pto;
  int timeout_value{};
  int fd{};

  if (argc < 2 || strcmp(argv[1], "--help") == 0)
  {
    usageError(argv[0]);
  }

  // allocate the arrays that we use. the array are sized according to the num
  // of pipes
  int num_pipes{};
  std::stringstream{argv[1]} >> num_pipes;

  // The calloc() function allocates memory for an array of nmemb elements of
  // size bytes each and returns a pointer to the allocated memory.  The memory
  // is set to zero.  If nmemb or size is 0, then calloc() returns either NULL,
  // or a unique pointer value that can later be successfully passed to free().
  //
  // num_pipes x 2

  int(*pfds)[2];
  pfds = (int(*)[2])calloc(num_pipes, sizeof(int[2]));
  if (nullptr == pfds)
    errExit("malloc");

  struct pollfd *poll_fds;
  poll_fds = (struct pollfd *)calloc(num_pipes, sizeof(struct pollfd));
  if (nullptr == poll_fds)
    errExit("malloc");

  // create pipes
  for (int j = 0; j < num_pipes; ++j)
  {
    if (pipe(pfds[j]) == -1)
      errExit("pipd %d", j);
  }

  prnMsg("created %d pipes\n", num_pipes);

  // perform num of writes to random pipes
  int num_writes{};
  std::stringstream{argv[2]} >> num_writes;

  srandom((int)time(nullptr));

  prnMsg("ok, writes on random pipes\n");

  // write() do not cause broken pipe and write block since read pipe is already
  // opened from pipe call(?) and PIPE_BUF is not filled up.

  for (int i = 0; i < num_writes; ++i)
  {
    int rand_pipe = random() % num_pipes;

    prnMsg("writing to fd: %3d (read fd: %3d)\n",
           pfds[rand_pipe][1],
           pfds[rand_pipe][0]);

    // write "a" char
    if (write(pfds[rand_pipe][1], "a", 1) == -1)
      errExit("write %d", pfds[rand_pipe][1]);
  }

  // build the file desc list to be supplied to poll(). this list is set to
  // contain the file desc for the read ends of all of the pipes.

  // POLLIN      Data other than high-priority data can be read

  for (int i = 0; i < num_pipes; ++i)
  {
    poll_fds[i].fd     = pfds[i][0]; // read pipe
    poll_fds[i].events = POLLIN;
  }

  prnMsg("ok, run poll on read pipes\n");

  auto ready = poll(poll_fds, num_pipes, -1);
  if (ready == -1)
    errExit("poll");

  prnMsg("poll returned %d\n", ready);

  // check which pipes have data available for reading
  for (int i = 0; i < num_pipes; ++i)
  {
    if (poll_fds[i].revents & POLLIN)
      prnMsg("Readable: %d %3d\n", i, poll_fds[i].fd);
  }

  exit(EXIT_SUCCESS);
}
