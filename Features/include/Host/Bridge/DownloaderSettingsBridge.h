#pragma once

#include <QtCore/QObject>

namespace P1 {
  namespace Host {

    class DownloaderSettings;

    namespace Bridge {

      /*!
      \class DownloaderSettingsBridge

      \brief DownloaderSettingsBridge class provides an interface to manipulate Downloader settings.

      */ 
      class DownloaderSettingsBridge : public QObject
      {
        Q_OBJECT
        Q_CLASSINFO("Version", "1.0.0.0")
        Q_CLASSINFO("D-Bus Interface", "com.protocolone.launcher.dbus.DownloaderSettings")

        /*!
        \property int DownloaderSettingsBridge::listeningPort

        Downloader listening port.

        \sa listeningPort(), setListeningPort()
        */
        Q_PROPERTY(int listeningPort READ listeningPort WRITE setListeningPort NOTIFY listeningPortChanged)
        
        /*!
        \property DownloaderSettingsBridge::uploadRate

        Downloader maximum upload rate.

        \sa uploadRate(), setUploadRate()
        */
        Q_PROPERTY(int uploadRate READ uploadRate WRITE setUploadRate NOTIFY uploadRateChanged)

        /*!
        \property DownloaderSettingsBridge::downloadRate

        Downloader maximum download rate.

        \sa downloadRate(), setDownloadRate()
        */Q_PROPERTY(int downloadRate READ downloadRate WRITE setDownloadRate NOTIFY downloadRateChanged)

        /*!
        \property DownloaderSettingsBridge::maxConnection

        Maximum number of connections used by Downloader.

        \sa maxConnection() setMaxConnection()
        */
        Q_PROPERTY(int maxConnection READ maxConnection WRITE setMaxConnection NOTIFY maxConnectionChanged)

        /*!
        \property DownloaderSettingsBridge::seedEnabled

        This property holds whether the Downloader participates seeding

        \sa seedEnabled() setSeedEnabled()
        */Q_PROPERTY(bool seedEnabled READ seedEnabled WRITE setSeedEnabled NOTIFY seedEnabledChanged)

        /*!
        \property DownloaderSettingsBridge::torrentProfile

        Setup torrent session profile used in Downloader.

        \sa torrentProfile() setMaxConnection()
        */
        Q_PROPERTY(int torrentProfile READ torrentProfile WRITE setTorrentProfile NOTIFY torrentProfileChanged)
      public:
        /*!
        \fn explicit DownloaderSettingsBridge(QObject *parent = 0);

        Creates a DownloaderSettingsBridge object with parent parent.
        
        */
        explicit DownloaderSettingsBridge(QObject *parent = 0);
        /*!
        \fn virtual ~DownloaderSettingsBridge();

        Destroys DownloaderSettingsBridge object instance.
        
        */
        virtual ~DownloaderSettingsBridge();

        /// \cond
        void setDownloaderSettings(DownloaderSettings* val);
        /// \endcond

        /*!
        \fn int DownloaderSettingsBridge::listeningPort() const;

        Method returns Downloader listening port.

        */
        int listeningPort() const;
        /*!
        \fn void DownloaderSettingsBridge::setListeningPort(int val);

        Method sets Downloader listening port.

        */
        void setListeningPort(int val);

        /*!
        \fn int DownloaderSettingsBridge::uploadRate() const;

        Method returns Downloader maximum upload rate.

        */
        int uploadRate() const;
        /*!
        \fn void DownloaderSettingsBridge::setUploadRate(int val);

        Method sets Downloader maximum upload rate.

        */
        void setUploadRate(int val);

        /*!
        \fn int DownloaderSettingsBridge::downloadRate() const;

        Method returns Downloader maximum download rate.

        */
        int downloadRate() const;

        /*!
        \fn int DownloaderSettingsBridge::torrentProfile() const;

        Method returns Downloader torrent session profile.

        */
        int torrentProfile() const;

        /*!
        \fn void DownloaderSettingsBridge::setTorrentProfile(int val);

        Method sets Downloader torrent session profile.

        */
        void setTorrentProfile(int val);

        /*!
        \fn void DownloaderSettingsBridge::setDownloadRate(int val);

        Method sets Downloader maximum download rate.

        */
        void setDownloadRate(int val);

        /*!
        \fn int DownloaderSettingsBridge::maxConnection() const;

        Method returns Downloader maximum connection count.

        */
        int maxConnection() const;
        /*!
        \fn void DownloaderSettingsBridge::setMaxConnection(int val);

        Method sets Downloader maximum connection count.

        */
        void setMaxConnection(int val);

        /*!
        \fn bool DownloaderSettingsBridge::seedEnabled() const;

        Method returns true if Downloader participates seeding, false otherwise.

        */
        bool seedEnabled() const;
        /*!
        \fn void DownloaderSettingsBridge::setSeedEnabled(bool val);

        Method sets   

        */
        void setSeedEnabled(bool val);

      signals:
        /*!
        \fn void DownloaderSettingsBridge::listeningPortChanged()

        This signal is emitted when Downloader listening port is changed

        \sa listeningPort() setListeningPort()
        */
        void listeningPortChanged();
        
        /*!
        \fn void DownloaderSettingsBridge::uploadRateChanged();

        This signal is emitted when upload rate is changed

        \sa uploadRate() setUploadRate()
        */
        void uploadRateChanged();
        
        /*!
        \fn void DownloaderSettingsBridge::downloadRateChanged()

        This signal is emitted when download rate is changed

        \sa downloadRate() setDownloadRate()
        */
        void downloadRateChanged();
        
        /*!
        \fn void DownloaderSettingsBridge::maxConnectionChanged()

        This signal is emitted when maximum number of connections is changed

        \sa downloadRate() setDownloadRate()
        */
        void maxConnectionChanged();
        
        /*!
        \fn void DownloaderSettingsBridge::seedEnabledChanged()

        This signal is emitted when seeding is enabled or disabled

        \sa downloadRate() setDownloadRate()
        */
        void seedEnabledChanged();
        
        /*!
        \fn void DownloaderSettingsBridge::torrentProfileChanged()

        This signal is emitted when torrentProfile changed

        \sa torrentProfile() setTorrentProfile()
        */
        void torrentProfileChanged();

      private:
        DownloaderSettings *_settings;
      };
    }
  }
}
