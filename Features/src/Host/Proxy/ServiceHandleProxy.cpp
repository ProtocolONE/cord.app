#include <Host/Proxy/ServiceHandleProxy.h>

#include <Host/Connection.h>
#include <Host/ServiceHandle.h>

namespace P1 {
  namespace Host {

    namespace Proxy {

      ServiceHandleProxy::ServiceHandleProxy(QObject *parent /*= 0*/)
        : QObject(parent)
        , _connection(nullptr)
        , _serviceHandle(nullptr)
      {
      }

      ServiceHandleProxy::~ServiceHandleProxy()
      {
      }

      void ServiceHandleProxy::setConnection(Connection *value)
      {
        Q_ASSERT(value);
        this->_connection = value;
      }

      void ServiceHandleProxy::setServiceHandle(ServiceHandle *value)
      {
        Q_ASSERT(value);
        this->_serviceHandle = value;

        QObject::connect(this->_serviceHandle, &ServiceHandle::locked,
          this, &ServiceHandleProxy::onLocked);

        QObject::connect(this->_serviceHandle, &ServiceHandle::unlocked,
          this, &ServiceHandleProxy::onUnlocked);

      }

      bool ServiceHandleProxy::lock(const QString& serviceId)
      {
        Q_ASSERT(this->_serviceHandle);
        return this->_serviceHandle->lock(serviceId, this->_connection);
      }

      void ServiceHandleProxy::unlock(const QString& serviceId)
      {
        Q_ASSERT(this->_serviceHandle);
        this->_serviceHandle->unlock(serviceId, this->_connection);
      }

      void ServiceHandleProxy::onLocked(const QString& serviceId, Connection* connection)
      {
        if (this->_connection == connection)
          return;

        emit this->locked(serviceId);
      }

      void ServiceHandleProxy::onUnlocked(const QString& serviceId, Connection* connection)
      {
        if (this->_connection == connection)
          return;

        emit this->unlocked(serviceId);
      }

    }
  }
}
