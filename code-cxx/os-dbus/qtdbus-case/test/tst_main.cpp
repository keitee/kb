
#include <gmock/gmock.h>
#include <signal.h>

#include <QCoreApplication>
#include <QDebug>
#include <QTimer>

#if 0

// Use this function in main() to run all tests.  It returns 0 if all
// tests are successful, or 1 otherwise.
//
// RUN_ALL_TESTS() should be invoked after the command line has been
// parsed by InitGoogleTest().
//
// This function was formerly a macro; thus, it is in the global
// namespace and has an all-caps name.
int RUN_ALL_TESTS() GTEST_MUST_USE_RESULT_;

inline int RUN_ALL_TESTS() {
  return ::testing::UnitTest::GetInstance()->Run();
}

This is the usual form to run gtest

int main(int argc, char **argv)
{
  testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}

However, need to change this to use Qt system since we need to use event loop
and all that.

#endif

static void GoogleTestRunner(int argc, char **argv)
{
  // Since Google Mock depends on Google Test, InitGoogleMock() is
  // also responsible for initializing Google Test. Therefore there's
  // no need for calling testing::InitGoogleTest() separately.
  testing::InitGoogleMock(&argc, argv);
  int result = RUN_ALL_TESTS();

  QCoreApplication::exit(result);
}

int main(int argc, char **argv)
{
  QCoreApplication app(argc, argv);

  // Disable sigpipe as we run a few tests that deliberately close remote
  // ends of pipes / sockets.
  signal(SIGPIPE, SIG_IGN);

  // This will run the functor from the application event loop.
  QTimer::singleShot(0, &app, std::bind(&GoogleTestRunner, argc, argv));

  return app.exec();
}
