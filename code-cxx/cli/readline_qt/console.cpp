#include "console.h"
#include <future>
#include <iostream>
#include <sstream>

Console::Console(QObject *parent)
    : QObject(parent)
{
  initReadLine();
}

Console::~Console() {}

void Console::start()
{
  m_readLine.start();
}

void Console::initReadLine()
{
  m_readLine.setPrompt(QStringLiteral("readline > "));

  m_readLine.addCommand("sample",                                // command
                        {"<on/off>", "<code>"},                  // args
                        "start/stop pairing using pairing code", // desc
                        this,                                    // receiver
                        &Console::onSampleCommand);

  m_readLine.addCommand("ch",                     // command
                        {"up/down"},              // args
                        "change channel up/down", // desc
                        this,                     // receiver
                        &Console::onChange);
}

void Console::onSampleCommand(const QStringList &args)
{
  qDebug() << "runs Console::onSampleCommand";
}

void Console::onChange(const QStringList &argList)
{
  bool argsOK = false;

  if (!argList[0].compare("up"))
  {
    qDebug() << "runs Console::onChange(up)";
    argsOK = true;
  }
  else if (!argList[0].compare("down"))
  {
    qDebug() << "runs Console::onChange(down)";
    argsOK = true;
  }

  if (!argsOK)
  {
    qDebug() << "runs Console::onChange: args not okay";
    return;
  }

  qDebug() << "runs Console::onChange";

  return;
}
