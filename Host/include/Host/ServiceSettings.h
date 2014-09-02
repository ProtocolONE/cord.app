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

      bool isDefaultInstallPath(const QString& serviceId) const;
      bool isDefaultDownloadPath(const QString& serviceId) const;
      bool hasDownloadPath(const QString& serviceId) const;
      
      QString downloadPath(const QString& serviceId) const;
      void setDownloadPath(const QString& serviceId, const QString& path);
      
      QString installPath(const QString& serviceId) const;
      void setInstallPath(const QString& serviceId, const QString& path);

      int gameSize(const QString& serviceId) const;
      QString name(const QString& serviceId) const;

    private:
      ServiceLoader *_services;
      GGS::GameDownloader::GameDownloadService *_downloader;
    };

  }
}