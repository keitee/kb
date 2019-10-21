#ifndef READLINE_H
#define READLINE_H

class ReadLine : public QObject
{
  Q_OBJECT

  public:
    ReadLine(QObject *parent = nullptr);
    ~ReadLine();

  private:
    QPointer<ReadLinePrivate> m_private;
};

#endif /* ifndef READLINE_H
