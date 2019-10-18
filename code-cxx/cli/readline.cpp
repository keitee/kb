
#include "readline.h"

ReadLine::ReadLine(QObject *parent)
  : QObject(parent),
  m_private(ReadLinePrivate::instance()),
  m_prompt("> ")
{

}
