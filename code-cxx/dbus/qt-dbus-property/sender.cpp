
int main(int argc, char **argv)
{
  QCoreApplication app(argc, argv);

  if (!QDBusConnection::sessionBus().isConnected())
  {
    fprintf(stderr, "Cannot connect to the D-Bus session bus.\n"
        "To start it, run:\n"
        "\teval `dbus-launch --auto-syntax`\n");
    return 1;
  }

  // as with chat example, create Adaptor and register it to dbus
  // class SenderAdaptor: public QDBusAbstractAdaptor;
  new SenderAdaptor(this);
  if (QDBusConnection::sessionBus().registerObject("/", this))
    qDebug() << "org.example.sender is registered";
  else
  {
    qDebug() << "org.example.sender is not registered";
    return 1;
  }


  qDebug() << "org.example.sender started";

  app.exec();

  return 0;
}
