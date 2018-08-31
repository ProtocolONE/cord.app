#pragma once

#include <QtCore/QObject>
#include <QtDBus/QtDBus>

namespace P1 {
  namespace Host {
    namespace DBus {

      class DBusConnection: public QObject
      {
        Q_OBJECT
      public:
        explicit DBusConnection(QObject *parent = 0);
        virtual ~DBusConnection();

        static QDBusConnection bus();
      };
    }
  }
}