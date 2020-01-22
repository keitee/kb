#ifndef BLERCUCONTROLLER_P_H
#define BLERCUCONTROLLER_P_H

class BleRcuControllerImpl : public BleRcuController, public QDBusContext
{
  private:
    // TODO: "const ConfigSettings"?
    const QSharedPointer<const ConfigSettings> m_config;
    QDBusObjectPath m_objectPath;
};

#endif // BLERCUCONTROLLER_P_H

