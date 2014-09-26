#include <Host/Dbus/DbusConnection.h>

namespace GameNet {
  namespace Host {
    namespace DBus {

      DBusConnection::DBusConnection(QObject *parent)
        : QObject(parent)
      {
      }

      DBusConnection::~DBusConnection()
      {
      }

      QDBusConnection DBusConnection::bus()
      {
#ifdef USE_SESSION_DBUS
        return QDBusConnection::sessionBus();
#else
        QSettings registry("HKEY_CURRENT_USER\\Software\\GGS\\QGNA\\Host", QSettings::NativeFormat);
        return  QDBusConnection::connectToPeer(registry.value("DBusAddress").toString(), "UI");
#endif
      }
    }
  }
}
