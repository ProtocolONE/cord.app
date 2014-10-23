#include <Host/Bridge/ServiceHandleBridge.h>
#include <Host/Proxy/ServiceHandleProxy.h>

using GameNet::Host::Proxy::ServiceHandleProxy;

namespace GameNet {
  namespace Host {
    namespace Bridge {

      ServiceHandleBridge::ServiceHandleBridge(QObject *parent /*= 0*/)
        : QObject(parent)
        , _serviceHandle(nullptr)
      {
      }

      ServiceHandleBridge::~ServiceHandleBridge()
      {
      }

      void ServiceHandleBridge::setServiceHandle(ServiceHandleProxy *value)
      {
        Q_ASSERT(value);
        this->_serviceHandle = value;

        QObject::connect(this->_serviceHandle, &ServiceHandleProxy::locked,
          this, &ServiceHandleBridge::locked);

        QObject::connect(this->_serviceHandle, &ServiceHandleProxy::unlocked,
          this, &ServiceHandleBridge::unlocked);
      }

      bool ServiceHandleBridge::lock(const QString& serviceId)
      {
        Q_ASSERT(this->_serviceHandle);
        return this->_serviceHandle->lock(serviceId);
      }

      void ServiceHandleBridge::unlock(const QString& serviceId)
      {
        Q_ASSERT(this->_serviceHandle);
        this->_serviceHandle->unlock(serviceId);
      }

    }
  }
}
