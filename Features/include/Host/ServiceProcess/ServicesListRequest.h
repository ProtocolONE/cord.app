#pragma once
#include <QtCore/QObject>
#include <QtCore/QVector>
#include <QtCore/QList>
#include <QtCore/QMap>
#include <QtCore/QString>

#include <Core/Service.h>
#include <RestApi/Commands/Service/GetHosts.h>
#include <Host/ServiceProcess/DownloadServiceIcon.h>

namespace GameNet {
  namespace Host {
    namespace ServiceProcess {

      class ServiceLoader;

      class ServicesListRequest : public QObject
      {
        Q_OBJECT

      public:
        explicit ServicesListRequest(QObject *parent = 0);
        virtual ~ServicesListRequest();

        void setServiceLoader(ServiceLoader* loader);
        void setOverrideWebExecutor(bool value);

        void request();

    signals:
        void finished();
        void additionalResourcesReady();

      private:
        void registerServices(const QList<QMap<QString, QString>>& servicesData);
        int getRetryInterval();
        void getHostsResult(P1::RestApi::CommandBase::CommandResults result);

        ServiceLoader* _serviceLoader;
        int _retryCount;
        QVector<int> _retryIntervals;
        DownloadServiceIcon _downloadIconHelper;
        bool _overrideWebScheme;
      };
    }
  }
}


