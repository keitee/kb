#ifndef DBUSABSTRACTINTERFACE_H
#define DBUSABSTRACTINTERFACE_H

#include <QObject>
#include <QString>
#include <QVariant>

#include <QDBusAbstractInterface>
#include <QDBusConnection>
#include <QDBusMetaType>
#include <QDBusPendingCall>
#include <QDBusPendingReply>

// https://doc.qt.io/qt-5/qdbusinterface.html
// Inherits: QDBusAbstractInterface

using DBusPropertyMap = QVariantMap;

class DBusAbstractInterface : public QDbusAbstractInterface
{
  Q_OBJECT

protected:
  explicit DBusAbstractInterface(QString const &service,
                                 QString const &path,
                                 char const *interface,
                                 QDBusConnection &connection,
                                 QObject *parent);

  // https://doc.qt.io/qt-5/qtglobal.html#Q_DECL_OVERRIDE
  //
  // Q_DECL_OVERRIDE
  // This macro can be used to declare an overriding virtual function. Use of
  // this markup will allow the compiler to generate an error if the
  // overriding virtual function does not in fact override anything.
  //
  // It expands to "override" if your compiler supports that C++11 contextual
  // keyword, or to nothing otherwise.
  //
  // The macro goes at the end of the function, usually after the const, if
  // any:
  //
  // generate error if this doesn't actually override anything:
  // virtual void MyWidget::paintEvent(QPaintEvent*) override;
  //
  // This macro was introduced in Qt 5.0.

  void connectNotify(QMetaMethod const &signal) Q_DECL_OVERRIDE;
  void disconnectNotify(QMetaMethod const &signal) Q_DECL_OVERRIDE;

public:
  QDBusPendingReply<QDBusVariant> asnycProperty(char const *name) const;
  QDBusPendingReply<> asyncSetProperty(char const *name,
                                       const QVariant &value) const;

public:
  QDBusPendingReply<DBusPropertyMap> asyncGetAllProperties() const;

  inline DBusPropertyMap getAllProperties() const
  {
    QDBusPendingReply<DBusPropertyMap> reply = asyncGetAllProperties();
    reply.waitForFinished();
    return reply.value();
  }

private slots:
  void onPropertiesChanged(QString const &interface,
                           QVariantMap const &changedProperties,
                           QStringList const &invalidatedProperties);

private:
  bool isSignalPropertyNotification(QMetaMethod const &signal) const;
  void invokeNotifySignal(QMetaMethod const &method,
                          QString const &name,
                          QVariant const &value,
                          int propertyType);

private:
  bool m_connected;
  bool m_propertyChangedConnected;

  static QString const m_dbusPropertiesInterface;
  static QString const m_dbusPropertiesChangedSignal;
};

#endif // DBUSABSTRACTINTERFACE_H
