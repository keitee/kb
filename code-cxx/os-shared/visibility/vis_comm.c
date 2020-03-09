// $ gcc -g -c -fPIC -Wall vis_comm.c vis_f1.c vis_f2.c
// $ g++ -g -c -fPIC -Wall vis_comm.c vis_f1.c vis_f2.c
//
// $ gcc -g -shared -o vis.so vis_comm.o vis_f1.o vis_f2.o
// $ g++ -g -shared -o vis.so vis_comm.o vis_f1.o vis_f2.o
// 
// If we use the following readelf command to list the dynamic symbols exported
// by the library, we see the following:
//
// $ readelf --syms --use-dynamic vis.so | grep vis_
//
// use gcc to see symbol names which is not mangled.
//
// This shared library exported three symbols: vis_comm(), vis_f1(), and
// vis_f2(). How- ever, we would like to ensure that only the symbols vis_f1()
// and vis_f2() are exported by the library. We can achieve this result using
// the following version script:
//
// $ cat vis.map
// VER_1 {
// global:
// vis_f1;
// vis_f2;
// local:
// *;
// };
//
// $ readelf --syms --use-dynamic vis.so | grep vis_
//     7   0: 00000000000006aa    29 FUNC    GLOBAL DEFAULT  12 vis_f2
//     9   0: 000000000000067a    19 FUNC    GLOBAL DEFAULT  12 vis_comm
//    13   2: 000000000000068d    29 FUNC    GLOBAL DEFAULT  12 vis_f1
//
// $ gcc -g -c -fPIC -Wall vis_comm.c vis_f1.c vis_f2.c
// $ gcc -g -shared -o vis.so vis_comm.o vis_f1.o vis_f2.o -Wl,--version-script,vis.map
//
// $ readelf --syms --use-dynamic vis.so | grep vis_
//     6   0: 000000000000060a    29 FUNC    GLOBAL DEFAULT  13 vis_f2
//     8   2: 00000000000005ed    29 FUNC    GLOBAL DEFAULT  13 vis_f1

// #include <iostream>
#include <stdio.h>

void vis_comm()
{
  // std::cout << "this is vis_comm" << std::endl;
  printf("this is vis_comm\n");
}
