#include <Host/ServiceHandle.h>
#include <Host/Connection.h>

#include <QtCore/QMutexLocker>
#include <QtCore/QDebug>

namespace GameNet {
  namespace Host {

    ServiceHandle::ServiceHandle(QObject *parent /*= 0*/)
      : QObject(parent)
    {
    }

    ServiceHandle::~ServiceHandle()
    {
    }

    bool ServiceHandle::lock(const QString& serviceId, Connection* connection)
    {
      Q_ASSERT(connection);

      QMutexLocker lock(&this->_mutex);

      if (this->_lockedServices.contains(serviceId))
        return this->_lockedServices[serviceId] == connection;

      connection->lockService(serviceId);
      this->_lockedServices[serviceId] = connection;

      emit this->locked(serviceId, connection);
      return true;
    }

    void ServiceHandle::unlock(const QString& serviceId, Connection* connection)
    {
      Q_ASSERT(connection);

      QMutexLocker lock(&this->_mutex);

      if (!this->_lockedServices.contains(serviceId))
        return;

      if (this->_lockedServices[serviceId] != connection)
        return;

      connection->unlockService(serviceId);
      this->_lockedServices.remove(serviceId);

      emit this->unlocked(serviceId, connection);
    }

    void ServiceHandle::unlockAllForConnection(Connection* connection)
    {
      Q_ASSERT(connection);

      QMutexLocker lock(&this->_mutex);
      QList<QString> serviceToUnlock = this->_lockedServices.keys(connection);
      Q_FOREACH(const QString& serviceId, serviceToUnlock) {
        this->_lockedServices.remove(serviceId);
        emit this->unlocked(serviceId, connection);
      }
    }

    Connection* ServiceHandle::connectionLockedService(const QString& serviceId)
    {
      QMutexLocker lock(&this->_mutex);
      return this->_lockedServices.value(serviceId, nullptr);
    }

    QList<QString> ServiceHandle::lockedServices(Connection* connection)
    {
      Q_ASSERT(connection);

      QMutexLocker lock(&this->_mutex);
      return connection->lockedServices();
    }

  }
}
