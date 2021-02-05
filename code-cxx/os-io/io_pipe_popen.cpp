// #include "lpi_error.h"
#include "slog.h"
#include <cerrno>
#include <cstdlib> // exit
#include <fcntl.h> // Defines O_* constants

#include <iostream>
#include <sstream>
#include <string>

#include <sys/file.h>
#include <sys/wait.h> // Defines mode constants
#include <unistd.h>

#define LOG_EXIT LOG_MSG

/*

From case example.

keitee@kit-ubuntu:~/git/kb/code-cxx/cxx/build$ echo 'vgcassetid=100&vgctoken=200&streamtype=2' | base64
dmdjYXNzZXRpZD0xMDAmdmdjdG9rZW49MjAwJnN0cmVhbXR5cGU9Mgo=

*/

namespace
{
  // constexpr auto BASE64_COMMAND{"/usr/bin/base64 | tr -d '\n'"};
  constexpr auto BASE64_COMMAND{"/usr/bin/base64"};
  constexpr auto BASE64_DECODE_COMMAND{"/usr/bin/base64 -d"};
  constexpr int BASE64_MAX_SIZE{1000};

  // use external bin
  std::string base64_encode_bin(const std::string &input)
  {
    std::ostringstream cmd{};
    std::ostringstream encoded{};
    char read[BASE64_MAX_SIZE]{};

    // echo -n '&streamtype=1' | base64
    cmd << "echo -n '" << input << "' | " << BASE64_COMMAND;

    // std::cout << "cmd : " << cmd.str() << std::endl;

    auto pfd = popen(cmd.str().c_str(), "r");
    if (pfd)
    {
      // if (fgets(encoded, sizeof(encoded), pfd))
      //
      // fgets stores "\n" when read stops
      //
      // fgets()  reads  in  at most one less than size characters from stream
      // and stores them into the buffer pointed to by s.
      //
      // Reading stops after an EOF or a newline. If a newline is read, it is
      // stored into the buffer.
      //
      // terminating null byte ('\0') is stored after the last character in the
      // buffer.
      //
      // fgets() returns s on success, and NULL on error or when end of file
      // occurs while no characters have been read.

      while (NULL != fgets(read, BASE64_MAX_SIZE, pfd))
      {
        // remove "\n"
        read[strlen(read) - 1] = '\0';
        encoded << read;
      }

      pclose(pfd);
    }
    else
    {
      std::cout << "popen failed : " << cmd.str().c_str() << std::endl;
    }

    return encoded.str();
  }

  // NOTE:
  // if reduce read size to 100 for decode, it fails since input for reads can
  // be big but input from base64 tool is wrapped at 76 by default. that's why
  // we need to do while and remove `\n` to make a single line.
  //
  // constexpr int BASE64_MAX_SIZE{100};
  //
  // NOTE: "base64 -d" do not make NEW LINE!
  // keitee@kit-ubuntu:~/git/kb/code-cxx/cxx$ echo -n 'JnN0cmVhbXR5cGU9MQ==' | /usr/bin/base64 -d
  // &streamtype=1keitee@kit-ubuntu:~/git/kb/code-cxx/cxx$ echo 'JnN0cmVhbXR5cGU9MQ==' | /usr/bin/base64 -d
  // &streamtype=1keitee@kit-ubuntu:~/git/kb/code-cxx/cxx$

  std::string base64_decode_bin(const std::string &input)
  {
    std::ostringstream cmd{};
    std::ostringstream decoded{};
    char read[BASE64_MAX_SIZE]{};

    // here do not include NULL and NEW LINE
    cmd << "echo -n '" << input << "' | " << BASE64_DECODE_COMMAND;

    // std::cout << "cmd : " << cmd.str() << std::endl;

    auto pfd = popen(cmd.str().c_str(), "r");
    if (pfd)
    {
      while (NULL != fgets(read, BASE64_MAX_SIZE, pfd))
      {
        // read[strlen(read) - 1] = '\0';
        decoded << read;
      }

      pclose(pfd);
    }
    else
    {
      // LOG_ERROR( "popen failed {%s}", cmd.str().c_str());
      std::cout << "popen failed : " << cmd.str().c_str() << std::endl;
    }

    return std::string{decoded.str()};
  }
} // namespace

int main(int argc, char *argv[])
{
  const unsigned char input[] = "&streamtype=1";
  const std::string expected{"JnN0cmVhbXR5cGU9MQ=="};

  {
    std::ostringstream drmData{};
    drmData << input;

    std::string encodedData = base64_encode_bin(drmData.str());
    if (encodedData != expected)
      LOG_MSG("encodedData not same");
    else
      LOG_MSG("encodedData is same");

    std::string decodedData = base64_decode_bin(encodedData);
    if (decodedData != std::string((const char *)input))
    {
      // std::cout << "decodedData not same\n";
      LOG_MSG("decodedData not same");
    }
    else
    {
      // std::cout << "decodedData is same\n";
      LOG_MSG("decodedData is same");
    }

    std::cout << "ends\n";
  }
}
