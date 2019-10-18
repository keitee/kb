#ifndef READLINE_H
#define READLINE_H

class ReadLine : public QObject
{
  Q_OBJECT

  public:
    ReadLine(QObject *parent = nullptr);
    ~ReadLine();
};

#endif /* ifndef READLINE_H
