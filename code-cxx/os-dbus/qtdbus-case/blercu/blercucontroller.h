#ifndef BLERCUCONTROLLER_H
#define BLERCUCONTROLLER_H

class BleRcuController : public QObject
{
  Q_OBJECT

public:
  virtual ~BleRcuController() {}

protected:
  BleRcuController(QObject *parent)
      : QObject(parent)
  {}

public:
  virtual bool isRegisteredToDBus(const QDBusConnection &dbusConn) const = 0;
  virtual bool registerToDBus(const QDBusConnection &dbusConn)           = 0;
  virtual bool unregisterFromDBus(const QDBusConnection &dbusConn)       = 0;

public:
  virtual bool isPairing() const     = 0;
  virtual quint8 pairingCode() const = 0;

  virtual bool startPairing(quint8 pairingCode) = 0;
  virtual void cancelPairing()                  = 0;

  virtual QString managedDevices() const = 0;

signals:
  // TODO: can use reference here? const QString &
  void managedDeviceAdded(const QString address);

  // signals:
  // 	void managedDeviceAdded(BleAddress address);
  // 	void managedDeviceRemoved(BleAddress address);
  // 	void pairingStateChanged(bool pairing);
};

#endif // BLERCUCONTROLLER_H
