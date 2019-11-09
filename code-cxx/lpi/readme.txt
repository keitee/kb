
––––––––––––––––––––––––––––––––––––––––––––––––––––––
57.2 Stream Sockets in the UNIX Domain

o The server must be terminated manually (e.g., by sending it a signal).

The client program (Listing 57-4) performs the following steps:
keitee@kit-hdebi:~/git/kb/code-cxx/lpi/build$ ./lpi-socket-unix-server > received &
[1] 24895
LOG| F:lpi_socket_unix_server.cpp C:int main(int, char**) L:00035 : server starts


keitee@kit-hdebi:~/git/kb/code-cxx/lpi/build$ ls -alF /tmp/us_xfr
srwxr-xr-x 1 keitee kitp 0 Nov  9 02:08 /tmp/us_xfr=

keitee@kit-hdebi:~/git/kb/code-cxx/lpi/build$ ./lpi-socket-unix-client < CMakeCache.txt
LOG| F:lpi_socket_unix_client.cpp C:int main(int, char**) L:00035 : client starts
LOG| F:lpi_socket_unix_client.cpp C:int main(int, char**) L:00063 : client ends

o The diff command produces no output, indicating that the input and output
  files are identical.

o Note that after the server terminates, the socket pathname continues to exist.

keitee@kit-hdebi:~/git/kb/code-cxx/lpi/build$ diff received CMakeCache.txt
keitee@kit-hdebi:~/git/kb/code-cxx/lpi/build$ ll


