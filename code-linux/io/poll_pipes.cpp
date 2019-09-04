// Listing 63-2: Using poll() to monitor multiple file descriptors
// altio/poll_pipes.c

#include <unistd.h>
#include <time.h>
#include <poll.h>
#include <iostream>
#include <cstring>

#define LOG_ERROR(fmt) \
  std::cout << fmt << std::endl;

int main(int argc, char *argv[])
{
  int num_pipes{};
  int ready;
  int rand_pipe{};
  int num_writes{};

  int (*pipe_fds)[2];

  struct pollfd *poll_fds;

  // usuage: ./poll_pipes 10 3
  if (argc < 2 || strcmp(argv[1], "--help") == 0)
  {
    LOG_ERROR("./poll_pipes num-pipes [num-writes]");
    exit(EXIT_FAILURE);
  }

  // allocate the arrays that we use according to the num of pipes specified on
  // command line.

  num_pipes = std::stoi(argv[1]);

  // allocate num_pipes elements of size int[2]
  pipe_fds = (int (*)[2])calloc(num_pipes, sizeof(int [2]));
  // error: pipe_fds = (int *)calloc(num_pipes, sizeof(int [2]));
  if (!pipe_fds)
    LOG_ERROR("failed on calloc");

  poll_fds = (pollfd *)calloc(num_pipes, sizeof(struct pollfd));
  if (!poll_fds)
    LOG_ERROR("failed on calloc");

  // create the number of pipes
  for (int i = 0; i < num_pipes; ++i)
  {
    if (pipe(pipe_fds[i]) == -1)
      LOG_ERROR("failed to create pipe");
  }

  // perform specified writes to random pipe
  num_writes = (argc > 2) ? std::stoi(argv[2]) : 1;

  srandom((int) time(NULL));

  for (int i = 0; i < num_writes; ++i)
  {
    rand_pipe = random() % num_pipes;

    std::cout << "writing to fd: " << pipe_fds[rand_pipe][1]
      << ", you should read from fd: " << pipe_fds[rand_pipe][0] << std::endl;

    if (write(pipe_fds[rand_pipe][1], "a", 1) == -1)
      LOG_ERROR("failed to write on");
  }

  // build fds to be supplied to poll()

  for (int i = 0; i < num_pipes; ++i)
  {
    // polls on a read pipe
    poll_fds[i].fd = pipe_fds[i][0];
    poll_fds[i].events = POLLIN;
  }

  // non-blocking io. 
  // If timeout equals -1, block until one of the file descriptors listed in the
  // fds array is ready (as defined by the corresponding events field) or a
  // signal is caught.

  ready = poll(poll_fds, num_pipes, -1);

  // check which pipes have data available for reading
  for (int i = 0; i < num_pipes; ++i)
  {
    if (poll_fds[i].revents & POLLIN)
      std::cout << "readable: " << i << " fd set, fd: " << poll_fds[i].fd << std::endl;
  }

  exit(EXIT_SUCCESS);
}
