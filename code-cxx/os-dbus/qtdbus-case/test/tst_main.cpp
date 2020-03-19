
#include <gmock/gmock.h>
#include <signal.h>

#include <QCoreApplication>
#include <QDebug>
#include <QTimer>

// ={=========================================================================

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
