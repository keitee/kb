#include "console.h"

Console::Console(QObject *parent)
  : QObject(parent)
{
  initReadLine();
}

Console::~Console()
{}

void Console::start()
{
  m_readLine.start();
}


void Console::initReadLine()
{
  m_readLine.setPrompt(QStringLiteral("console >"));

  m_readLine.addCommand(
      "sample",
      {"<on/off>", "<code>"},
      "start/stop pairing using pairing code",
      this,
      &Console::onSampleCommand
      );
}


void Console::onSampleCommand(const QStringList &args)
{
  qDebug() << "Console::onSampleCommand";
}
