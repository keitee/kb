#include "client_object.h"

#define SERVICE_NAME "org.example.SenderExample"

#if 0
static bool sendCommand(std::string const &command, void *sender)
{
  qDebug() << "cli::sendCommands called";

  ((QDBusInterface *)sender)->call("sendCommand", "command");

  return true;
}


void run_cli(QDBusInterface *sender)
{
  CommandHandler handler;
  bool running{true};

  handler.addCommand("sendc", "sendCommand", sendCommand, (void*)sender);
  handler.showCommands();

  while (running)
  {
    // prompt
    std::cout << ":> " << std::flush;

    std::string line;
    if (std::getline(std::cin, line))
    {
      auto ret = handler.handle(line);
      if (!ret)
        std::cout << line << " is not handled" << std::endl;
    }
    else if (std::cin.bad())
    {
      std::cout << "std io error" << std::endl;
      break;
    }
    else if (std::cin.eof())
    {
      std::cout << "std eof" << std::endl;
      break;
    }
  } // while
}
#endif

int main(int argc, char **argv)
{
  QCoreApplication app(argc, argv);

  if (!QDBusConnection::sessionBus().isConnected()) {
    fprintf(stderr, "Cannot connect to the D-Bus session bus.\n"
        "To start it, run:\n"
        "\teval `dbus-launch --auto-syntax`\n");
    return 1;
  }

  // connect to the remote signal
  Client client;


#if 0
  // this works as well:
  //
  // QDBusInterface::QDBusInterface(
  //  const QString &service, 
  //  const QString &path,
  //  const QString &interface = QString(), 
  //  const QDBusConnection &connection = QDBusConnection::sessionBus(), 
  //  QObject *parent = nullptr)

  auto sender = new QDBusInterface(SERVICE_NAME,
      "/",
      "org.example.sender",
      QDBusConnection::sessionBus());
  if(!sender->isValid())
  {
    fprintf(stderr, "%s\n",
        qPrintable(QDBusConnection::sessionBus().lastError().message()));
    QCoreApplication::instance()->quit();
  }

  // QDBusInterface::connect()

  sender->connect(sender, SIGNAL(action(QString, QString)),
      &client, SLOT(onSignalReceived(QString, QString)));

  // connect sender signal to quit
  sender->connect(sender, SIGNAL(aboutToQuit()),
      QCoreApplication::instance(), SLOT(quit()));
#endif

  qDebug() << "org.example.client started";

  // this do not work as well since app.exec() didn't run.
  // run_cli(sender);

  app.exec();

  // never get's run since app.exec() loops.
  // run_cli();

  qDebug() << "org.example.client ended";

  return 0;
}
