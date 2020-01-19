#include <QCoreApplication>
#include <QDebug>
#include <QSharedPointer>

#include <signal.h>

#include <cmdlineoptions.h>
#include <configsettings.h>

int main(int argc, char *argv[])
{
  QCoreApplication app(argc, argv);

  // TODO:
  // disable the SIGPIPE
  // why "it is the most annoying signal in the world"?
  signal(SIGPIPE, SIG_IGN);

  // parse the command line options
  QSharedPointer<CmdLineOptions> options =
    QSharedPointer<CmdLineOptions>::create();

  options->process(app);

#if 0
  // error when call isSet() without process():
  // QCommandLineParser: call process() or parse() before isSet

  if (options->isSet("help"))
    qDebug() << "option help is set";

  if (options->isSet("noconsole"))
  {
    qDebug() << "option noconsole is set, value="
             << options->value("noconsole");
  }
#endif

  // create the config options
  QSharedPointer<ConfigSettings> config = ConfigSettings::defaults();

  // qDebug() << "qtbus-case daemon started";

  // int exitCode = app.exec();

  // qDebug() << "qtbus-case daemon ended";

  // return exitCode;
}
