#pragma once

#include <QtCore/QObject>
#include <QtCore/QString>

namespace GGS {
  namespace GameDownloader {
    class GameDownloadService;
  }

  namespace Application {
    class ArgumentParser;
  }
}

namespace GameNet {
  namespace Host {

    class ApplicationStatistic : public QObject
    {
      Q_OBJECT
    public:
      explicit ApplicationStatistic(QObject *parent = 0);
      virtual ~ApplicationStatistic();

      void setDownloader(GGS::GameDownloader::GameDownloadService *value);
      void setCommandLineArgs(GGS::Application::ArgumentParser *value);

      void init();

    public slots:
      virtual bool isGameInstalled(const QString& serviceId) const;
      virtual quint64 gameInstallDate(const QString& serviceId) const;
      virtual quint64 gameLastExecutionTime(const QString& serviceId) const;
      
      virtual int executeGameSuccessCount(const QString& serviceId) const;
      virtual int executeGameFailedCount(const QString& serviceId) const;
      virtual int executeGameTotalCount(const QString& serviceId) const;

      virtual QString installWithService() const;
      virtual quint64 installDate() const;

    private:
      void setInstallDate(const QString& serviceId);
      QString startingServiceId();

      GGS::Application::ArgumentParser *_commandLineArgs;
      GGS::GameDownloader::GameDownloadService *_downloader;

    };

  }
}