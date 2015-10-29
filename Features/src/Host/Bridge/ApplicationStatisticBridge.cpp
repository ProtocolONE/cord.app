#include <Host/Bridge/ApplicationStatisticBridge.h>
#include <Host/ApplicationStatistic.h>

namespace GameNet {
  namespace Host {
    namespace Bridge {

      ApplicationStatisticBridge::ApplicationStatisticBridge(QObject *parent /*= 0*/)
        : QObject(parent)
        , _applicationStatistic(nullptr)
      {
      }

      ApplicationStatisticBridge::~ApplicationStatisticBridge()
      {
      }

      void ApplicationStatisticBridge::setApplicationStatistic(ApplicationStatistic *value)
      {
        Q_ASSERT(value);
        this->_applicationStatistic = value;
      }

      bool ApplicationStatisticBridge::isGameInstalled(const QString& serviceId) const
      {
        Q_ASSERT(this->_applicationStatistic);
        return this->_applicationStatistic->isGameInstalled(serviceId);
      }

      quint64 ApplicationStatisticBridge::gameInstallDate(const QString& serviceId) const
      {
        Q_ASSERT(this->_applicationStatistic);
        return this->_applicationStatistic->gameInstallDate(serviceId);
      }

      quint64 ApplicationStatisticBridge::gameLastExecutionTime(const QString& serviceId) const
      {
        Q_ASSERT(this->_applicationStatistic);
        return this->_applicationStatistic->gameLastExecutionTime(serviceId);
      }

      int ApplicationStatisticBridge::executeGameSuccessCount(const QString& serviceId) const
      {
        Q_ASSERT(this->_applicationStatistic);
        return this->_applicationStatistic->executeGameSuccessCount(serviceId);
      }

      int ApplicationStatisticBridge::executeGameFailedCount(const QString& serviceId) const
      {
        Q_ASSERT(this->_applicationStatistic);
        return this->_applicationStatistic->executeGameFailedCount(serviceId);
      }

      int ApplicationStatisticBridge::executeGameTotalCount(const QString& serviceId) const
      {
        Q_ASSERT(this->_applicationStatistic);
        return this->_applicationStatistic->executeGameTotalCount(serviceId);
      }

      QString ApplicationStatisticBridge::installWithService() const
      {
        Q_ASSERT(this->_applicationStatistic);
        return this->_applicationStatistic->installWithService();
      }

      quint64 ApplicationStatisticBridge::installDate() const
      {
        Q_ASSERT(this->_applicationStatistic);
        return this->_applicationStatistic->installDate();
      }

    }
  }
}
