#include <Host/Bridge/DownloaderSettingsBridge.h>
#include <Host/DownloaderSettings.h>

namespace P1 {
  namespace Host {
    namespace Bridge {
      DownloaderSettingsBridge::DownloaderSettingsBridge(QObject *parent /*= 0*/)
        : QObject(parent)
        , _settings(nullptr)
      {
      }

      DownloaderSettingsBridge::~DownloaderSettingsBridge()
      {
      }

      void DownloaderSettingsBridge::setDownloaderSettings(DownloaderSettings* val)
      {
        Q_ASSERT(val);
        this->_settings = val;

        QObject::connect(this->_settings, &DownloaderSettings::listeningPortChanged, 
          this, &DownloaderSettingsBridge::listeningPortChanged);
        QObject::connect(this->_settings, &DownloaderSettings::uploadRateChanged, 
          this, &DownloaderSettingsBridge::uploadRateChanged);
        QObject::connect(this->_settings, &DownloaderSettings::downloadRateChanged, 
          this, &DownloaderSettingsBridge::downloadRateChanged);
        QObject::connect(this->_settings, &DownloaderSettings::maxConnectionChanged, 
          this, &DownloaderSettingsBridge::maxConnectionChanged);
        QObject::connect(this->_settings, &DownloaderSettings::seedEnabledChanged, 
          this, &DownloaderSettingsBridge::seedEnabledChanged);
      }

      int DownloaderSettingsBridge::listeningPort() const
      {
        Q_ASSERT(this->_settings);
        return this->_settings->listeningPort();
      }

      void DownloaderSettingsBridge::setListeningPort(int val)
      {
        Q_ASSERT(this->_settings);
        this->_settings->setListeningPort(val);
      }

      int DownloaderSettingsBridge::uploadRate() const
      {
        Q_ASSERT(this->_settings);
        return this->_settings->uploadRate();
      }

      void DownloaderSettingsBridge::setUploadRate(int val)
      {
        Q_ASSERT(this->_settings);
        return this->_settings->setUploadRate(val);
      }

      int DownloaderSettingsBridge::downloadRate() const
      {
        Q_ASSERT(this->_settings);
        return this->_settings->downloadRate();
      }

      int DownloaderSettingsBridge::torrentProfile() const
      {
        Q_ASSERT(this->_settings);
        return this->_settings->torrentProfile();
      }

      void DownloaderSettingsBridge::setTorrentProfile(int val)
      {
        Q_ASSERT(this->_settings);
        this->_settings->setTorrentProfile(val);
      }

      void DownloaderSettingsBridge::setDownloadRate(int val)
      {
        Q_ASSERT(this->_settings);
        this->_settings->setDownloadRate(val);
      }

      int DownloaderSettingsBridge::maxConnection() const
      {
        Q_ASSERT(this->_settings);
        return this->_settings->maxConnection();
      }

      void DownloaderSettingsBridge::setMaxConnection(int val)
      {
        Q_ASSERT(this->_settings);
        this->_settings->setMaxConnection(val);
      }

      bool DownloaderSettingsBridge::seedEnabled() const
      {
        Q_ASSERT(this->_settings);
        return this->_settings->seedEnabled();
      }

      void DownloaderSettingsBridge::setSeedEnabled(bool val)
      {
        Q_ASSERT(this->_settings);
        this->_settings->setSeedEnabled(val);
      }

    }
  }
}