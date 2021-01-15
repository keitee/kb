#include <QCoreApplication>
#include <QDebug>

#include "console.h"
#include "scli.h"

// ={=========================================================================

int main(int argc, char **argv)
{
  QCoreApplication app(argc, argv);
  QCoreApplication::setApplicationName("TestConsole");
  QCoreApplication::setApplicationVersion("1.0");

  Console *console = new Console();
  console->start();

  return app.exec();
}
