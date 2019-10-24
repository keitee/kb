#ifndef COMPLEXPONG_H
#define COMPLEXPONG_H

#include <QtCore/QObject>
#include <QtDBus/QDBusAbstractAdaptor>
#include <QtDBus/QDBusVariant>

class Pong : public QDBusAbstractAdaptor
{
  Q_OBJECT
    Q_CLASSINFO("D-Bus Interface",
        "org.example.QtDBus.ComplexPong.Pong")
    Q_PROPERTY(QString value READ value WRITE setValue)

  public:
    QString m_value;
    QString value() const;
    void setValue(const QString &newValue);

    Pong(QObject *obj)
        : QDBusAbstractAdaptor(obj)
        , m_value("pong")
    {}

    void really_quit();

  signals:
    void aboutToQuit();

  public slots:
    QDBusVariant query(const QString &query);
    Q_NOREPLY void quit();
};

#endif // COMPLEXPONG_H
