#pragma once
#include <QtCore/QObject>
#include <QtCore/QVector>
#include <QtCore/QList>
#include <QtCore/QMap>
#include <QtCore/QString>
#include <QNetworkAccessManager>

#include <Core/Service.h>
#include <RestApi/Commands/Service/GetHosts.h>

namespace GameNet {
  namespace Host {

    class ServiceLoader;

    class ServicesListRequest : public QObject
    {
      Q_OBJECT

    public:
      explicit ServicesListRequest(QObject *parent = 0);
      virtual ~ServicesListRequest();

      void setApplicationArea(GGS::Core::Service::Area area);
      void setServiceLoader(ServiceLoader* loader);
      void request();

    signals:
      void finished();

    private:
      void downloadFailed();

      void downloadIconRequest(const QString& url, const QString& serviceId);

      QString readLastModified(const QString& path);
      void saveLastModifed(const QString& path, const QString& value);

      void registerServices(const QList<QMap<QString, QString>>& servicesData);
      int getRetryInterval();
      void getHostsResult(GGS::RestApi::CommandBase::CommandResults result);

      GGS::Core::Service::Area _applicationArea;
      ServiceLoader* _serviceLoader;
      int _retryCount;
      QVector<int> _retryIntervals;
      QNetworkAccessManager _networkManager;
    };

  }
}


