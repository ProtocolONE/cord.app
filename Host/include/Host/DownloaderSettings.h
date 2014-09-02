#pragma once

#include <QtCore/QObject>

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

    class DownloaderSettings : public QObject
    {
      Q_OBJECT
      Q_PROPERTY(int listeningPort READ listeningPort WRITE setListeningPort NOTIFY listeningPortChanged)
      Q_PROPERTY(int uploadRate READ uploadRate WRITE setUploadRate NOTIFY uploadRateChanged)
      Q_PROPERTY(int downloadRate READ downloadRate WRITE setDownloadRate NOTIFY downloadRateChanged)
      Q_PROPERTY(int maxConnection READ maxConnection WRITE setMaxConnection NOTIFY maxConnectionChanged)
      Q_PROPERTY(bool seedEnabled READ seedEnabled WRITE setSeedEnabled NOTIFY seedEnabledChanged)
    public:
      explicit DownloaderSettings(QObject *parent = 0);
      virtual ~DownloaderSettings();

      void setDownloader(GGS::GameDownloader::GameDownloadService *val);
      void init();

      int listeningPort() const;
      void setListeningPort(int val);

      int uploadRate() const;
      void setUploadRate(int val);

      int downloadRate() const;
      void setDownloadRate(int val);

      int maxConnection() const;
      void setMaxConnection(int val);

      bool seedEnabled() const;
      void setSeedEnabled(bool val);

    signals:
      void listeningPortChanged();
      void uploadRateChanged();
      void downloadRateChanged();
      void maxConnectionChanged();
      void seedEnabledChanged();

    private:
      GGS::GameDownloader::GameDownloadService *_downloader;
      
      void internalListeningPortChanged(unsigned short port);
      int readInt(const QString& key, int defaultValue) const;
    };

  }
}