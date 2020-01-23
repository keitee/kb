#ifndef BLERCUCONTROLLER_IMPL_H
#define BLERCUCONTROLLER_IMPL_H

// NOTE: this is not pimpl pattern so changed it's filename to impl.h to make it
// clear.

// BleRcuController is ABC

class BleRcuControllerImpl : public BleRcuController, public QDBusContext
{
  Q_OBJECT

private:
  // TODO: "const ConfigSettings"?
  const QSharedPointer<const ConfigSettings> m_config;
  QDBusObjectPath m_objectPath;
  QList<DBusAbstractAdaptor *> m_adaptors;
  BleRcuError m_lastError;

public:
  BleRcuControllerImpl(const QSharedPointer<ConfigSettings> &, QObject *);
  ~BleRcuControllerImpl() = default;

  bool isRegisteredToDBus(const QDBusConnection &) const override;
  bool registerToDBus(const QDBusConnection &) const override;
  bool unregisterFromDBus(const QDBusConnection &) const override;
};

#endif // BLERCUCONTROLLER_IMPL_H
