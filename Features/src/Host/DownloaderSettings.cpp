
#include <Host/DownloaderSettings.h>

#include <GameDownloader/GameDownloadService.h>
#include <Settings/Settings.h>

#include <QtCore/QDebug>
#include <QtCore/QStandardPaths>
#include <QtCore/QCoreApplication>

using GGS::GameDownloader::GameDownloadService;

namespace GameNet {
  namespace Host {

    DownloaderSettings::DownloaderSettings(QObject * parent /*= 0*/)
      : QObject(parent)
      , _downloader(nullptr)
    {
    }

    DownloaderSettings::~DownloaderSettings()
    {
    }

    void DownloaderSettings::setDownloader(GGS::GameDownloader::GameDownloadService *val)
    {
      Q_ASSERT(val);
      this->_downloader = val;

      QObject::connect(this->_downloader, &GameDownloadService::listeningPortChanged, 
        this, &DownloaderSettings::internalListeningPortChanged);
    }

    void DownloaderSettings::init()
    {
      Q_ASSERT(this->_downloader);

      QString torrentConfigPath = QStandardPaths::writableLocation(QStandardPaths::DataLocation);
      torrentConfigPath.append("/torrents");
      
      this->_downloader->setTorrentConfigDirectoryPath(torrentConfigPath);
      this->_downloader->setTimeoutForResume(600);
      this->_downloader->setSeedEnabled(this->seedEnabled());
      this->_downloader->setDownloadRateLimit(this->downloadRate() * 1024);
      this->_downloader->setUploadRateLimit(this->uploadRate() * 1024);
      this->internalSetTorrentProfile(this->torrentProfile());
    }

    int DownloaderSettings::listeningPort() const
    {
      GGS::Settings::Settings settings;
      QString port = settings.value("qGNA/inPort").toString();
      bool ok = false;
      unsigned short result = port.toUShort(&ok);
      return ok ? result : 11888;
    }

    void DownloaderSettings::setListeningPort(int val)
    {
      Q_ASSERT(this->_downloader);
      this->_downloader->changeListeningPort(val);
    }

    void DownloaderSettings::internalListeningPortChanged(unsigned short port)
    {
      GGS::Settings::Settings settings;
      settings.setValue("qGNA/inPort", QString::number(port));
      emit this->listeningPortChanged();
    }

    int DownloaderSettings::uploadRate() const
    {
      return this->readInt("qGNA/uploadSpeed", 0);
    }

    void DownloaderSettings::setUploadRate(int val)
    {
      Q_ASSERT(this->_downloader);
      int current = this->uploadRate();
      if (current == val)
        return;

      this->_downloader->setUploadRateLimit(val * 1024);
      GGS::Settings::Settings settings;
      settings.setValue("qGNA/uploadSpeed", QString::number(val));
      emit this->uploadRateChanged();
    }

    int DownloaderSettings::downloadRate() const
    {
      return this->readInt("qGNA/downloadSpeed", 0);
    }

    void DownloaderSettings::setDownloadRate(int val)
    {
      Q_ASSERT(this->_downloader);
      int current = this->downloadRate();
      if (current == val)
        return;

      this->_downloader->setDownloadRateLimit(val * 1024);
      GGS::Settings::Settings settings;
      settings.setValue("qGNA/downloadSpeed", QString::number(val));
      emit this->downloadRateChanged();
    }

    int DownloaderSettings::maxConnection() const
    {
      return this->readInt("qGNA/numConnections", 10);
    }

    void DownloaderSettings::setMaxConnection(int val)
    {
      Q_ASSERT(this->_downloader);
      int current = this->downloadRate();
      if (current == val)
        return;

      this->_downloader->setMaxConnection(val);
      GGS::Settings::Settings settings;
      settings.setValue("qGNA/numConnections", QString::number(val));
      emit this->maxConnectionChanged();
    }

    bool DownloaderSettings::seedEnabled() const
    {
      GGS::Settings::Settings settings;
      return settings.value("qGNA/seedEnabled", true).toBool();
    }

    void DownloaderSettings::setSeedEnabled(bool val)
    {
      Q_ASSERT(this->_downloader);
      bool current = this->seedEnabled();
      if (current == val)
        return;

      this->_downloader->setSeedEnabled(val);
      GGS::Settings::Settings settings;
      settings.setValue("qGNA/seedEnabled", val);
      emit this->seedEnabledChanged();
    }

    int DownloaderSettings::torrentProfile() const
    {
      return this->readInt("qGNA/torrentProfile", 1);
    }

    void DownloaderSettings::setTorrentProfile(int val)
    {
      int current = this->torrentProfile();
      if (current == val)
        return;

      this->internalSetTorrentProfile(val);
      
      GGS::Settings::Settings settings;
      settings.setValue("qGNA/torrentProfile", QString::number(val));

      emit this->torrentProfileChanged();
    }
    
    void DownloaderSettings::internalSetTorrentProfile(int val)
    {
      switch (val)
      {
      case 1:
        qDebug() << "Use HIGH_PERFORMANCE_SEED torrent profile";
        this->_downloader->setTorrentProfile(GameDownloadService::HIGH_PERFORMANCE_SEED);
        break;
      case 2:
        qDebug() << "Use MIN_MEMORY_USAGE torrent profile";
        this->_downloader->setTorrentProfile(GameDownloadService::MIN_MEMORY_USAGE);
        break;
      default:
        qDebug() << "Use DEFAULT_PROFILE torrent profile";
        this->_downloader->setTorrentProfile(GameDownloadService::DEFAULT_PROFILE);
        break;
      }
    }

    int DownloaderSettings::readInt(const QString& key, int defaultValue) const
    {
      GGS::Settings::Settings settings;
      QString value = settings.value(key).toString();
      bool ok = false;
      int result = value.toInt(&ok);
      return ok ? result : defaultValue;
    }

  }
}
