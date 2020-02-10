#ifndef CONSOLE_H
#define CONSOLE_H

#include "readline.h"

class Console : public QObject
{
  Q_OBJECT

public:
  Console(QObject *parent = nullptr);
  ~Console();

public:
  void start();

private slots:
  void onSampleCommand(const QStringList &args);

private:
  void initReadLine();
  ReadLine m_readLine;
};

#endif // CONSOLE_H
