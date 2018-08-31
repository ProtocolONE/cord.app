#include <Host/Dbus/DBusServer.h>

#include <QtCore/QRegExp>
#include <QtCore/QString>
#include <QtCore/QStringList>
#include <QtCore/QDebug>
#include <QtCore/QSettings>

#include <QtDBus/QtDBus>

namespace P1 {
  namespace Host {
    namespace DBus {
      DBusServer::DBusServer(QObject *parent)
        : QObject(parent)
        , _server(new QDBusServer("tcp:host=127.0.0.1,port=0,family=ipv4", this))
      {
        QObject::connect(this->_server, &QDBusServer::newConnection, this, &DBusServer::newConnection);
                
        QSettings registry("HKEY_CURRENT_USER\\Software\\ProtocolOne\\Launcher\\Host", QSettings::NativeFormat);
        registry.setValue("DBusAddress", this->_server->address());
        
        qDebug() << "DBus server started with address" << _server->address();
      }

      DBusServer::~DBusServer()
      {
      }

      bool DBusServer::isConnected() const
      {
        return this->_server->isConnected();
      }

      QDBusError DBusServer::lastError() const
      {
        return this->_server->lastError(); 
      }
    }
  }
}

