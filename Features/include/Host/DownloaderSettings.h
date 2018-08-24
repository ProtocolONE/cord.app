#pragma once

#include <QtCore/QObject>

namespace P1 {
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
      Q_PROPERTY(int torrentProfile READ torrentProfile WRITE setTorrentProfile NOTIFY torrentProfileChanged)
    public:
      explicit DownloaderSettings(QObject *parent = 0);
      virtual ~DownloaderSettings();

      void setDownloader(P1::GameDownloader::GameDownloadService *val);
      void init();

      virtual int listeningPort() const;
      virtual void setListeningPort(int val);

      virtual int uploadRate() const;
      virtual void setUploadRate(int val);

      virtual int downloadRate() const;
      virtual void setDownloadRate(int val);

      virtual int maxConnection() const;
      virtual void setMaxConnection(int val);

      virtual bool seedEnabled() const;
      virtual void setSeedEnabled(bool val);

      virtual int torrentProfile() const;
      virtual void setTorrentProfile(int val);

    signals:
      void listeningPortChanged();
      void uploadRateChanged();
      void downloadRateChanged();
      void maxConnectionChanged();
      void seedEnabledChanged();
      void torrentProfileChanged();

    private:
      P1::GameDownloader::GameDownloadService *_downloader;

      void internalSetTorrentProfile(int val);

      void internalListeningPortChanged(unsigned short port);
      int readInt(const QString& key, int defaultValue) const;
    };

  }
}