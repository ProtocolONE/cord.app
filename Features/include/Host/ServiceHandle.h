#pragma once

#include <QtCore/QObject>
#include <QtCore/QString>
#include <QtCore/QMap>
#include <QtCore/QMutex>

namespace P1 {
  namespace Host {

    class Connection;

    class ServiceHandle : public QObject
    {
      Q_OBJECT
    public:
      explicit ServiceHandle(QObject *parent = 0);
      virtual ~ServiceHandle();

      virtual bool lock(const QString& serviceId, Connection* connection);
      virtual void unlock(const QString& serviceId, Connection* connection);

      void unlockAllForConnection(Connection* connection);

      Connection* connectionLockedService(const QString& serviceId);
      QList<QString> lockedServices(Connection* connection);
      
    signals:
      void locked(const QString& serviceId, Connection* connection);
      void unlocked(const QString& serviceId, Connection* connection);

    private:
      QMutex _mutex;
      QMap<QString, Connection *> _lockedServices;

    };
  }
}