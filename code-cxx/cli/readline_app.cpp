#include <iostream>

/* Standard readline include files. */
#include <readline/readline.h>

// readline standalone app

int main(int argc, char **argv)
{
  // in order to read a line of text from the user. The line returned has the
  // final newline removed, so only the text remains.

  // If readline encounters an EOF while reading the line, and the line is empty
  // at that point, then (char *)NULL is returned. Otherwise, the line is ended
  // just as if a newline had been typed.

  while (1)
  {
    char *line = readline("Enter a line: ");

    if(line == nullptr)
      break;

    std::cout << "the entered line : " << line << std::endl;

    free(line);
  }

  return EXIT_SUCCESS;
}
