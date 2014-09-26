#pragma once

#include <QtCore/QObject>

class QDBusServer;
class QDBusConnection;
class QDBusError;

namespace GameNet {
    namespace Host {
      namespace DBus {
        class DBusServer: public QObject
        {
          Q_OBJECT
        public:
          explicit DBusServer(QObject *parent = 0);
          virtual ~DBusServer();

          bool isConnected() const;
          QDBusError lastError() const;

        signals:
          void newConnection(const QDBusConnection &connection);
          
        private:
          QString _name; 
          QDBusServer *_server;
        };
      }
    }
}