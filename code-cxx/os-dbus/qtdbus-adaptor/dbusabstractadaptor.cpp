#include "dbusabstractadaptor.h"

DBusAbstractAdaptor::DBusAbstractAdaptor(QObject *parent)
  : QDBusAbstractAdaptor(parent)
    , m_parentContext(nullptr)
{
	// sanity check that the parent object is publically inherited from
	// QDBusContext, we need this so that we can get the QDBusConnection object
	// that sent the request
  // https://doc.qt.io/qt-5/qdbuscontext.html

  if (parent)
  {
    void *ptr = parent->qt_metacast("QDBusContext");
    m_parentContext = reinterpret_cast<QDBusContext *>(ptr);
  }

  if (!m_parentContext)
    qError("failed to get dbus context object of the parent");
}
