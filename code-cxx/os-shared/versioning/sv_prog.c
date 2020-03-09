// gcc -g -c -fPIC -Wall sv_lib_v1.c
// gcc -g -shared -o libsv.so sv_lib_v1.o -Wl,--version-script,sv_v1.map
//
// $ cat sv_v1.map
// VER_1 {
// global: xyz;
// local: *;
// # Hide all other symbols
// };
//
// gcc -g -o p1 sv_prog.c libsv.so
//
// p1 uses xyz() from sv_lib_v1.c
//
// Now, suppose that we want to modify the definition of xyz() within our
// library, while still ensuring that program p1 continues to use the old
// version of this function.
//
// build new so with new map
//
// VER_1 {
//   global: xyz;
//   local: *;
//   # Hide all other symbols
// };
// 
// VER_2 {
//   global: pqr;
// } VER_1;
//
// gcc -g -c -fPIC -Wall sv_lib_v2.c
// gcc -g -shared -o libsv.so sv_lib_v2.o -Wl,--version-script,sv_v2.map
//
// build p2
// gcc -g -o p2 sv_prog.c libsv.so
// NOTE that if build p2 before new libsv.so, then it still has link to VER_1
// since it's get this info at link time.
//
// LD_LIBRARY_PATH=. ./p2
// v2 xyz
//
// LD_LIBRARY_PATH=. ./p1
// v1 xyz
//
// objdump -t p1 | ag xyz
// 0000000000000000       F *UND*  0000000000000000              xyz@@VER_1
//
// objdump -t p2 | ag xyz
// 0000000000000000       F *UND*  0000000000000000              xyz@@VER_2
//
// readelf -s p1 | ag xyz
//      4: 0000000000000000     0 FUNC    GLOBAL DEFAULT  UND xyz@VER_1 (3)
//     58: 0000000000000000     0 FUNC    GLOBAL DEFAULT  UND xyz@@VER_1
//
// readelf -s p2 | ag xyz
//      1: 0000000000000000     0 FUNC    GLOBAL DEFAULT  UND xyz@VER_2 (2)
//     49: 0000000000000000     0 FUNC    GLOBAL DEFAULT  UND xyz@@VER_2

#include <stdlib.h>
int
main(int argc, char *argv[])
{
void xyz(void);
xyz();
exit(EXIT_SUCCESS);
}
