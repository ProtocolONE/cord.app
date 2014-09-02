#pragma once

#include <QtCore/QObject>

namespace GameNet {
  namespace Host {

    class DownloaderSettings;

    namespace Bridge {
      class DownloaderSettingsBridge : public QObject
      {
        Q_OBJECT
        Q_CLASSINFO("Version", "1.0.0.0")
        Q_CLASSINFO("D-Bus Interface", "com.gamenet.dbus.DownloaderSettings")
        Q_PROPERTY(int listeningPort READ listeningPort WRITE setListeningPort NOTIFY listeningPortChanged)
        Q_PROPERTY(int uploadRate READ uploadRate WRITE setUploadRate NOTIFY uploadRateChanged)
        Q_PROPERTY(int downloadRate READ downloadRate WRITE setDownloadRate NOTIFY downloadRateChanged)
        Q_PROPERTY(int maxConnection READ maxConnection WRITE setMaxConnection NOTIFY maxConnectionChanged)
        Q_PROPERTY(bool seedEnabled READ seedEnabled WRITE setSeedEnabled NOTIFY seedEnabledChanged)
      public:
        explicit DownloaderSettingsBridge(QObject *parent = 0);
        virtual ~DownloaderSettingsBridge();

        void setDownloaderSettings(DownloaderSettings* val);

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
        DownloaderSettings *_settings;
      };
    }
  }
}
