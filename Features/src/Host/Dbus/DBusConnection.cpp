#include <Host/Dbus/DbusConnection.h>

namespace P1 {
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
        QSettings registry("HKEY_CURRENT_USER\\Software\\ProtocolOne\\Launcher\\Host", QSettings::NativeFormat);
        return  QDBusConnection::connectToPeer(registry.value("DBusAddress").toString(), "UI");
#endif
      }
    }
  }
}
