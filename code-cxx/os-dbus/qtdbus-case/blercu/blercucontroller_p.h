#ifndef BLERCUCONTROLLER_IMPL_H
#define BLERCUCONTROLLER_IMPL_H

#include <QDBusContext>
#include <QList>
#include <QObject>
#include <QSharedPointer>
#include <QtDBus>
#include <blercucontroller.h>

// NOTE: this is not pimpl pattern so changed it's filename to impl.h to make it
// clear.

// NOTE: BleRcuController is to provide `signals` and ABC interface.
// What else purpose to prove why we need it??

class DBusAbstractAdaptor;

// NOTE: can use forward declaration for T
class ConfigSettings;

class BleRcuControllerImpl : public BleRcuController, public QDBusContext
{
  Q_OBJECT

private:
  // TODO: "const ConfigSettings"?
  const QSharedPointer<const ConfigSettings> m_config;
  QDBusObjectPath m_objectPath;
  QList<DBusAbstractAdaptor *> m_adaptors;

public:
  // NOTE: do not use parent
  BleRcuControllerImpl(const QSharedPointer<ConfigSettings> &,
                       QObject *parent = nullptr);
  // ~BleRcuControllerImpl() = default;
  virtual ~BleRcuControllerImpl();

  bool isRegisteredToDBus(const QDBusConnection &) const override;
  bool registerToDBus(const QDBusConnection &) override;
  bool unregisterFromDBus(const QDBusConnection &) override;

  bool isValid() const override;
  QString managedDevices() const override;
  bool isPairing() const override;
  quint8 pairingCode() const override;

  bool startPairing(quint8 pairingCode) override;
  void cancelPairing() override;
};

#endif // BLERCUCONTROLLER_IMPL_H
