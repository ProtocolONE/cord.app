#pragma once

#include <QtCore/QObject>
#include <QtCore/QString>

namespace GGS {
  namespace Core {
    class Service;
  }

  namespace GameDownloader {
    class GameDownloadService;
  }
}

namespace GameNet {
  namespace Host {

    class ServiceLoader;
    class ServiceSettings : public QObject
    {
      Q_OBJECT
    public:
      explicit ServiceSettings(QObject *parent = 0);
      virtual ~ServiceSettings();

      void setServices(ServiceLoader *value);
      void setDownloader(GGS::GameDownloader::GameDownloadService *value);

      virtual bool isDownloadable(const QString& serviceId) const;
      virtual bool isDefaultInstallPath(const QString& serviceId) const;
      virtual bool isDefaultDownloadPath(const QString& serviceId) const;
      virtual bool hasDownloadPath(const QString& serviceId) const;
      
      virtual QString downloadPath(const QString& serviceId) const;
      virtual void setDownloadPath(const QString& serviceId, const QString& path);
      
      virtual QString installPath(const QString& serviceId) const;
      virtual void setInstallPath(const QString& serviceId, const QString& path);

      virtual quint64 gameSize(const QString& serviceId) const;
      virtual QString name(const QString& serviceId) const;

    private:
      ServiceLoader *_services;
      GGS::GameDownloader::GameDownloadService *_downloader;
    };

  }
}