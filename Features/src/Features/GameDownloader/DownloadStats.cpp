/****************************************************************************
** This file is a part of Syncopate Limited GameNet Application or it parts.
**
** Copyright (©) 2011 - 2012, Syncopate Limited and/or affiliates. 
** All rights reserved.
**
** This file is provided AS IS with NO WARRANTY OF ANY KIND, INCLUDING THE
** WARRANTY OF DESIGN, MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE.
****************************************************************************/

#include <Features/GameDownloader/DownloadStats.h>

#include <Core/Service.h>
#include <Settings/Settings.h>

namespace Features {
  namespace GameDownloader {

    DownloadStats::DownloadStats(QObject *parent)
      : QObject(parent)
      , _applicationRestartCountChanged(false)
      , _initialized(false)
      , _started(false)
      , _totalSize(0)
      , _applicationRestartCount(0)
      , _downloadTime(0)
      , _maxDownloadSpeed(0)
      , _maxUploadSpeed(0)
    {
    }

    DownloadStats::DownloadStats(const DownloadStats &obj)
    {
      this->operator=(obj);
    }

    DownloadStats & DownloadStats::operator=(const DownloadStats &obj)
    {
      if (this == &obj)
        return *this; 

      this->_downloadTime = obj._downloadTime;
      this->_applicationRestartCount = obj._applicationRestartCount;
      this->_applicationRestartCountChanged = obj._applicationRestartCountChanged;
      this->_maxDownloadSpeed = obj._maxDownloadSpeed;
      this->_maxUploadSpeed = obj._maxUploadSpeed;
      this->_id = obj._id;
      this->_totalSize = obj._totalSize;
      return *this;
    }

    DownloadStats::~DownloadStats()
    {
    }

    void DownloadStats::start()
    {
      if (this->_started)
        return;

      if (!this->_applicationRestartCountChanged) {
        this->_applicationRestartCount++;
        this->_applicationRestartCountChanged = true; 
      }

      this->_downloadTimeAtStart = this->_downloadTime;
      this->_startTime = QDateTime::currentMSecsSinceEpoch() / 1000;
      this->_started = true;

      this->save();
    }

    void DownloadStats::stop()
    {
      if (!this->_started)
        return;
      this->_started = false;

      int duration = (QDateTime::currentMSecsSinceEpoch() / 1000) - this->_startTime;
      this->_downloadTime = this->_downloadTimeAtStart + duration;

      this->save();
    }

    void DownloadStats::update(int downloadSpeed, int uploadSpeed, qint64 totalSize)
    {
      if (!this->_started)
        return;

      int duration = (QDateTime::currentMSecsSinceEpoch() / 1000) - this->_startTime;
      this->_downloadTime = this->_downloadTimeAtStart + duration;

      if (downloadSpeed > this->_maxDownloadSpeed)
        this->_maxDownloadSpeed = downloadSpeed;

      if (uploadSpeed > this->_maxUploadSpeed)
        this->_maxUploadSpeed = uploadSpeed;
      
      if (totalSize > this->_totalSize)
        this->_totalSize = totalSize;
    }

    void DownloadStats::setId(const QString& id)
    {
      this->_id = id;
    }

    void DownloadStats::load()
    {
      GGS::Settings::Settings settings;
      settings.beginGroup("qGNA");
      settings.beginGroup("Features");
      settings.beginGroup("GameDownloaderStatistic");
      settings.beginGroup(this->_id);
      int downloadTime = settings.value("downloadTime", -1).toInt();
      if (downloadTime > 0) {
        this->_downloadTime = downloadTime;
        this->_applicationRestartCount = settings.value("restartCount", 0).toInt();
        this->_maxDownloadSpeed = settings.value("maxDownloadSpeed", 0).toInt();
        this->_maxUploadSpeed = settings.value("maxUploadSpeed", 0).toInt();
        this->_totalSize = settings.value("totalSize", 0).toLongLong();
        this->_initialized = true;
      } else {
        this->_initialized = false;
      }
    }

    void DownloadStats::save()
    {
      GGS::Settings::Settings settings;
      settings.beginGroup("qGNA");
      settings.beginGroup("Features");
      settings.beginGroup("GameDownloaderStatistic");
      settings.beginGroup(this->_id);
      settings.setValue("downloadTime", this->_downloadTime);
      settings.setValue("restartCount", this->_applicationRestartCount);
      settings.setValue("maxDownloadSpeed", this->_maxDownloadSpeed);
      settings.setValue("maxUploadSpeed", this->_maxUploadSpeed);
      settings.setValue("totalSize", this->_totalSize);
    }

    unsigned int DownloadStats::downloadTime() const
    {
      return this->_downloadTime;
    }

    int DownloadStats::maxDownloadSpeed() const
    {
      return this->_maxDownloadSpeed;
    }

    int DownloadStats::maxUploadSpeed() const
    {
      return this->_maxUploadSpeed;
    }

    unsigned int DownloadStats::applicationRestartCount() const
    {
      return this->_applicationRestartCount;
    }

    qint64 DownloadStats::totalSize() const
    {
      return this->_totalSize;
    }

  }
}