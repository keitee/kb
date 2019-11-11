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
Listing 57-3: A simple UNIX domain stream socket server
––––––––––––––––––––––––––––––––––––––––––––––––––––––sockets/us_xfr_sv.c
*/

#define BACKLOG (5)

int main(int argc, char **argv)
{
  struct sockaddr_un addr;
  int sfd{}, cfd{};
  ssize_t num_read{};
  char buf[BUF_SIZE];

  LOG_MSG("server starts");

  sfd = socket(AF_UNIX, SOCK_STREAM, 0);
  if (sfd == -1)
    LOG_EXIT("failed to create socket");

  // construct server socket address, bind socket to it, and make this a
  // listening socket

  // REMOVE(3)
  //
  // NAME
  //        remove - remove a file or directory
  //
  // SYNOPSIS
  //        #include <stdio.h>
  //        int remove(const char *pathname);
  //
  // RETURN VALUE
  //       On success, zero is returned.  On error, -1 is returned, and errno is
  //       set appropriately.
  //
  // #define ENOENT 2 // No such file or directory

  // okay to fail to remove when file do not exist
  if (remove(SV_SOCK_PATH) == -1 && errno != ENOENT)
    LOG_EXIT("failed to delete (%s)", SV_SOCK_PATH);

  memset(&addr, 0, sizeof(struct sockaddr_un));
  addr.sun_family = AF_UNIX;
  strncpy(addr.sun_path, SV_SOCK_PATH, sizeof(sockaddr_un) - 1);

  // When used to bind a UNIX domain socket, bind() creates an entry in the file
  // system. (Thus, a directory specified as part of the socket pathname needs
  // to be accessible and writable.) The ownership of the file is determined
  // according to the usual rules for file creation (Section 15.3.1). The file
  // is marked as a socket. When stat() is applied to this pathname, it returns
  // the value S_IFSOCK in the file-type component of the st_mode field of the
  // stat structure (Section 15.1). When listed with ls –l, a UNIX domain socket
  // is shown with the type s in the first column, and ls –F appends an equal
  // sign (=) to the socket pathname.
  //
  // It is usual to bind a socket to an absolute pathname, so that the socket
  // resides at a fixed address in the file system. Using a relative pathname is
  // possible, but unusual, because it requires an application that wants to
  // connect() to this socket to know the current working directory of the
  // application that performs the bind().

  if (bind(sfd, (struct sockaddr *)&addr, sizeof(struct sockaddr_un)) == -1)
    LOG_EXIT("failed to bind");

  if (listen(sfd, BACKLOG) == -1)
    LOG_EXIT("failed to listen");

  // handle client connections iteratively
  for (;;)
  {

    // Accept a connection. The connection is returned on a new socket, 'cfd';
    // the listening socket ('sfd') remains open and can be used to accept
    // further connections.

    cfd = accept(sfd, NULL, NULL);
    if (cfd == -1)
      LOG_EXIT("failed to accept");

    // read data from connected socket, client, and output it to stdout until
    // EOF
    while ((num_read = read(cfd, buf, BUF_SIZE)) > 0)
      if (num_read != write(STDOUT_FILENO, buf, num_read))
        LOG_EXIT("partial or failed write");

    if (num_read == -1)
      LOG_EXIT("failed to read from client");

    if (close(cfd) == -1)
      LOG_EXIT("failed to close");

  } // for

  LOG_MSG("server ends");

  exit(EXIT_SUCCESS);
}
