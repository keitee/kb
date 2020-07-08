/*

*/

#include <cstring>
#include <iostream>
#include <sstream>
#include <string>

#include <encode.h>

namespace
{
  // constexpr auto BASE64_COMMAND{"/usr/bin/base64 | tr -d '\n'"};
  constexpr auto BASE64_COMMAND{"/usr/bin/base64"};
  constexpr auto BASE64_DECODE_COMMAND{"/usr/bin/base64 -d"};
  constexpr int BASE64_MAX_SIZE{1000};

  std::string base64_encode(const std::string &input)
  {
    std::ostringstream cmd{};
    std::ostringstream encoded{};
    char read[BASE64_MAX_SIZE]{};

    cmd << "echo '" << input << "' | " << BASE64_COMMAND;

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

      while (NULL != fgets(read, sizeof(read), pfd))
      {
        // std::cout << "encoded:" << encoded << ", strlen: " << strlen(encoded)
        //           << std::endl;

        // remove "\n"
        read[strlen(read) - 1] = '\0';

        encoded << read;
      }

      pclose(pfd);
    }
    else
    {
      // LOG_ERROR( "popen failed {%s}", cmd.str().c_str());
      std::cout << "popen failed : " << cmd.str().c_str() << std::endl;
    }

    return std::string{encoded.str()};
  }

  std::string base64_decode(const std::string &input)
  {
    std::ostringstream cmd{};
    char decoded[BASE64_MAX_SIZE]{};

    cmd << "echo '" << input << "' | " << BASE64_DECODE_COMMAND;

    std::cout << "cmd : " << cmd.str() << std::endl;

    auto pfd = popen(cmd.str().c_str(), "r");
    if (pfd)
    {
      if (fgets(decoded, sizeof(decoded), pfd))
      {
        decoded[strlen(decoded) - 1] = '\0';
      }

      pclose(pfd);
    }
    else
    {
      // LOG_ERROR( "popen failed {%s}", cmd.str().c_str());
      std::cout << "popen failed : " << cmd.str().c_str() << std::endl;
    }

    return std::string{decoded};
  }
} // namespace

#if 0
echo "&vgcassetid=F67307ADA1614C35A0BA4DE39F22272&vgctoken=00000040000001A4000000600000001A00000061000000020002000000630000000800B980D500B98675000000700000017A0006B40A223124823D8BB018C766555E7CDC360F8ADE3F1AF7F91B310D0E2F7624828DBFDE2D3D4E59&streamtype=1" | base64 | tr -d '\n'
JnZnY2Fzc2V0aWQ9RjY3MzA3QURBMTYxNEMzNUEwQkE0REUzOUYyMjI3MiZ2Z2N0b2tlbj0wMDAwMDA0MDAwMDAwMUE0MDAwMDAwNjAwMDAwMDAxQTAwMDAwMDYxMDAwMDAwMDIwMDAyMDAwMDAwNjMwMDAwMDAwODAwQjk4MEQ1MDBCOTg2NzUwMDAwMDA3MDAwMDAwMTdBMDAwNkI0MEEyMjMxMjQ4MjNEOEJCMDE4Qzc2NjU1NUU3Q0RDMzYwRjhBREUzRjFBRjdGOTFCMzEwRDBFMkY3NjI0ODI4REJGREUyRDNENEU1OSZzdHJlYW10eXBlPTEK

encoded:JnZnY2Fzc2V0aWQ9RjY3MzA3QURBMTYxNEMzNUEwQkE0REUzOUYyMjI3MiZ2Z2N0b2tlbj0wMDAw
encoded:MDA0MDAwMDAwMUE0MDAwMDAwNjAwMDAwMDAxQTAwMDAwMDYxMDAwMDAwMDIwMDAyMDAwMDAwNjMw
encoded:MDAwMDAwODAwQjk4MEQ1MDBCOTg2NzUwMDAwMDA3MDAwMDAwMTdBMDAwNkI0MEEyMjMxMjQ4MjNE
encoded:OEJCMDE4Qzc2NjU1NUU3Q0RDMzYwRjhBREUzRjFBRjdGOTFCMzEwRDBFMkY3NjI0ODI4REJGREUy
encoded:RDNENEU1OSZzdHJlYW10eXBlPTEK

        JnZnY2Fzc2V0aWQ9RjY3MzA3QURBMTYxNEMzNUEwQkE0REUzOUYyMjI3MiZ2Z2N0b2tlbj0wMDAw
        MDA0MDAwMDAwMUE0MDAwMDAwNjAwMDAwMDAxQTAwMDAwMDYxMDAwMDAwMDIwMDAyMDAwMDAwNjMw
        MDAwMDAwODAwQjk4MEQ1MDBCOTg2NzUwMDAwMDA3MDAwMDAwMTdBMDAwNkI0MEEyMjMxMjQ4MjNE
        OEJCMDE4Qzc2NjU1NUU3Q0RDMzYwRjhBREUzRjFBRjdGOTFCMzEwRDBFMkY3NjI0ODI4REJGREUy
        RDNENEU1OSZzdHJlYW10eXBlPTEK

keitee@kit-ubuntu:~/git/kb/code-cxx/cxx/build$ echo "&vgcassetid=F67307ADA1614C35A0BA4DE39F22272&vgctoken=00000040000001A4000000600000001A00000061000000020002000000630000000800B980D500B98675000000700000017A0006B40A223124823D8BB018C766555E7CDC360F8ADE3F1AF7F91B310D0E2F7624828DBFDE2D3D4E59&streamtype=1" | base64
JnZnY2Fzc2V0aWQ9RjY3MzA3QURBMTYxNEMzNUEwQkE0REUzOUYyMjI3MiZ2Z2N0b2tlbj0wMDAw
MDA0MDAwMDAwMUE0MDAwMDAwNjAwMDAwMDAxQTAwMDAwMDYxMDAwMDAwMDIwMDAyMDAwMDAwNjMw
MDAwMDAwODAwQjk4MEQ1MDBCOTg2NzUwMDAwMDA3MDAwMDAwMTdBMDAwNkI0MEEyMjMxMjQ4MjNE
OEJCMDE4Qzc2NjU1NUU3Q0RDMzYwRjhBREUzRjFBRjdGOTFCMzEwRDBFMkY3NjI0ODI4REJGREUy
RDNENEU1OSZzdHJlYW10eXBlPTEK

encoded drmData {
JnZnY2Fzc2V0aWQ9RjY3MzA3QURBMTYxNEMzNUEwQkE0REUzOUYyMjI3MiZ2Z2N0b2tlbj0wMDAw
MDA0MDAwMDAwMUE0MDAwMDAwNjAwMDAwMDAxQTAwMDAwMDYxMDAwMDAwMDIwMDAyMDAwMDAwNjMw
MDAwMDAwODAwQjk4MEQ1MDBCOTg2NzUwMDAwMDA3MDAwMDAwMTdBMDAwNkI0MEEyMjMxMjQ4MjNE
OEJCMDE4Qzc2NjU1NUU3Q0RDMzYwRjhBREUzRjFBRjdGOTFCMzEwRDBFMkY3NjI0ODI4REJGREUy
RDNENEU1OSZzdHJlYW10eXBlPTEK}

#endif

TEST(CxxCaseBase64, check_decode_5)
{
  using namespace cxx_case_base64_bin;

  const char input[]{
    "&vgcassetid=F67307ADA1614C35A0BA4DE39F22272&vgctoken="
    "00000040000001A4000000600000001A00000061000000020002000000630000000800B980"
    "D500B98675000000700000017A0006B40A223124823D8BB018C766555E7CDC360F8ADE3F1A"
    "F7F91B310D0E2F7624828DBFDE2D3D4E59&streamtype=1"};

  std::string expected{
    "JnZnY2Fzc2V0aWQ9RjY3MzA3QURBMTYxNEMzNUEwQkE0REUzOUYyMjI3MiZ2Z2N0b2tlbj0wMD"
    "AwMDA0MDAwMDAwMUE0MDAwMDAwNjAwMDAwMDAxQTAwMDAwMDYxMDAwMDAwMDIwMDAyMDAwMDAw"
    "NjMwMDAwMDAwODAwQjk4MEQ1MDBCOTg2NzUwMDAwMDA3MDAwMDAwMTdBMDAwNkI0MEEyMjMxMj"
    "Q4MjNEOEJCMDE4Qzc2NjU1NUU3Q0RDMzYwRjhBREUzRjFBRjdGOTFCMzEwRDBFMkY3NjI0ODI4"
    "REJGREUyRDNENEU1OSZzdHJlYW10eXBlPTEK"};

  std::ostringstream drmData{};

  drmData << input;

  // encode drmData using base64
  // printf("input drmData {%s}\n", drmData.str().c_str());
  // std::cout << "input drmData : " << drmData.str().c_str() << std::endl;

  std::string encodedData = base64_encode(drmData.str());

  // printf("encoded drmData {%s}\n", encodedData.c_str());
  // std::cout << "encoded drmData : " << encodedData << std::endl;

  EXPECT_THAT(encodedData, expected);
}

// ={=========================================================================
int main(int argc, char **argv)
{
  testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}
