#include <ctype.h> // isprint
#include <getopt.h>
#include <stdio.h>

#define printable(ch) (isprint((unsigned char)ch) ? ch : '#')

int main(int argc, char **argv)
{
  // clang-format off
  struct option longopts[] = {
    {"help",    no_argument,        nullptr, (int)'h'},
    {"address", required_argument,  nullptr, (int)'a'},
    {"service", required_argument,  nullptr, (int)'s'},
    {nullptr,   0,                  nullptr, 0       }
  };
  // clang-format on

  int opt{}, longindex{};

  // An alternative method of preventing getopt() from permuting
  // command-line arguments is to make the first character of optstring a
  // plus sign ( + ).
  //
  // (If we want to also suppress getopt() error messages as described above,
  // then the first two characters of optstring should be +: , in that order.)

  // do not supress error messages from getopt
  // while (-1 != (opt = getopt_long(argc, argv, "+ha:s:", longopts, &longindex)))

  // supress error messages
  while (-1 != (opt = getopt_long(argc, argv, "+:h:a:s", longopts, &longindex)))
  {
    printf("opt = %3d (%c); optind = %d", opt, printable(opt), optind);

    switch (opt)
    {
      case 'h':
        printf(", got h\n");
        break;

      case 'a':
        printf(", got a with option{%s}\n", optarg);
        break;

      case 's':
        printf(", got s with option{%s}\n", optarg);
        break;

        // error cases
      case '?':
        printf(", unknown opt. optopt = %3d (%c)\n", optopt, printable(optopt));
        break;

      default:
        printf(", unknown opt\n");
        break;
    }
  } // while

  if (optind < argc)
    printf("\nfirst nonoption argument is %s at argv[%d]\n",
           argv[optind],
           optind);
}
