#include "slog.h"
#include <cerrno>
#include <cstdlib> // exit
#include <sys/syscall.h>
#include <unistd.h>

#define LOG_EXIT LOG_MSG

/*
Listing 57-2: Header file for us_xfr_sv.c and us_xfr_cl.c
––––––––––––––––––––––––––––––––––––––––––––––––––––––––– sockets/us_xfr.h
*/

#include <sys/socket.h>
#include <sys/un.h> // sockaddr_un

#define SV_SOCK_PATH "/tmp/us_xfr"

#define BUF_SIZE (100)

/*
Listing 57-4: A simple UNIX domain stream socket client
––––––––––––––––––––––––––––––––––––––––––––––––––––––#include "us_xfr.h"
*/

#define BACKLOG (5)

int main(int argc, char **argv)
{
  struct sockaddr_un addr;
  int sfd{};
  ssize_t num_read{};
  char buf[BUF_SIZE];

  LOG_MSG("client starts");

  // client socket
  sfd = socket(AF_UNIX, SOCK_STREAM, 0);
  if (sfd == -1)
    LOG_EXIT("failed to create socket");

  // construct server socket address, bind socket to it, and make this a
  // listening socket

  memset(&addr, 0, sizeof(struct sockaddr_un));
  addr.sun_family = AF_UNIX;
  strncpy(addr.sun_path, SV_SOCK_PATH, sizeof(sockaddr_un) - 1);

  if (connect(sfd, (struct sockaddr *)&addr, sizeof(struct sockaddr_un)) == -1)
    LOG_EXIT("failed to connect");

  // for (;;)
  {
    // read data stdio and write it to connected socket, server
    while ((num_read = read(STDIN_FILENO, buf, BUF_SIZE)) > 0)
      if (num_read != write(sfd, buf, num_read))
        LOG_EXIT("partial or failed write");

    if (num_read == -1)
      LOG_EXIT("failed to read from client");
  } // for

  LOG_MSG("client ends");

  exit(EXIT_SUCCESS);
}
