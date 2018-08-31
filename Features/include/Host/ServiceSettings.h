#pragma once

#include <QtCore/QObject>
#include <QtCore/QString>

namespace P1 {
  namespace Core {
    class Service;
  }

  namespace GameDownloader {
    class GameDownloadService;
  }
}

namespace P1 {
  namespace Host {
    namespace ServiceProcess {
      class ServiceLoader;
    }

    class ServiceSettings : public QObject
    {
      Q_OBJECT
    public:
      explicit ServiceSettings(QObject *parent = 0);
      virtual ~ServiceSettings();

      void setServices(ServiceProcess::ServiceLoader *value);
      void setDownloader(P1::GameDownloader::GameDownloadService *value);

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

      virtual bool isOverlayEnabled(const QString& serviceId) const;
      virtual void setOverlayEnabled(const QString& serviceId, bool enabled);

      virtual bool isPrefer32Bit(const QString& serviceId) const;
      virtual void setPrefer32Bit(const QString& serviceId, bool value);

    private:
      ServiceProcess::ServiceLoader *_services;
      P1::GameDownloader::GameDownloadService *_downloader;
    };

  }
}