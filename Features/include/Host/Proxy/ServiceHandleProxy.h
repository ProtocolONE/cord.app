#pragma once

#include <QtCore/QObject>

namespace GameNet {
  namespace Host {

    class ServiceHandle;
    class Connection;

    namespace Proxy {

      class ServiceHandleProxy : public QObject
      {
        Q_OBJECT
      public:
        explicit ServiceHandleProxy(QObject *parent = 0);
        virtual ~ServiceHandleProxy();

        void setConnection(Connection *value);
        void setServiceHandle(ServiceHandle *value);

        virtual bool lock(const QString& serviceId);
        virtual void unlock(const QString& serviceId);

      signals:
        void locked(const QString& serviceId);
        void unlocked(const QString& serviceId);
        
      private:
        void onLocked(const QString& serviceId, Connection* connection);
        void onUnlocked(const QString& serviceId, Connection* connection);

        Connection *_connection;
        ServiceHandle *_serviceHandle;
      };

    }
  }
}
