/*
NOTE: do not work. need to debug

usage: clnt server_machine

when there is no server running: 
kyoupark@kit-debian64:~/git/kb/code-linux/ex_gdb$ ./client_test localhost
client: connect
client: 
client: end


when there is server running:
kyoupark@kit-debian64:~/git/kb/code-linux/ex_gdb$ ./client_test localhost
client: connect
 23:36:41 up  1:35,  2 users,  load average: 0.13, 0.28, 0.30
USER     TTY      FROM             LOGIN@   IDLE   JCPU   PCPU WHAT
kyoupark :0       :0               22:02   ?xdm?  12:59   0.02s gdm-session-worker [pam/gdm-password]
kyoupark pts/0    :0               22:02    2.00s  0.10s  0.00s tmux new -s home
client: 
client: end

*/

#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <string.h>
#include <unistd.h>

#define WPORT 2000 // server port number
#define BUFSIZE 1000

int main(int argc, char **argv)
{ 
  (void)argc;

  int sd,msgsize;

  struct sockaddr_in addr;
  struct hostent *hostptr;
  char buf[BUFSIZE];

  // create socket
  sd = socket(AF_INET,SOCK_STREAM,0);
  addr.sin_family = AF_INET;

  // Now suppose the programmer had forgotten line 26 in the client code, which
  // specifies the port on the server’s system to connect to:

  // addr.sin_port = WPORT;

  hostptr = gethostbyname(argv[1]);
  memcpy(&addr.sin_addr.s_addr,hostptr->h_addr_list[0],hostptr->h_length);

  // OK, now connect
  connect(sd,(struct sockaddr *) &addr,sizeof(addr));

  printf("client: connect\n");
  printf("client: ");
  // read and display response
  msgsize = read(sd,buf,BUFSIZE);
  if (msgsize > 0)
    write(1,buf,msgsize);
  printf("\n");
  printf("client: end\n");
  return 0;
}
