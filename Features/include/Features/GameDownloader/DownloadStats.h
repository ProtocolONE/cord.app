/****************************************************************************
** This file is a part of Syncopate Limited GameNet Application or it parts.
**
** Copyright (©) 2011 - 2012, Syncopate Limited and/or affiliates. 
** All rights reserved.
**
** This file is provided AS IS with NO WARRANTY OF ANY KIND, INCLUDING THE
** WARRANTY OF DESIGN, MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE.
****************************************************************************/

#pragma once

#include <QtCore/QObject>
#include <QtCore/QString>
#include <QtCore/QDateTime>

namespace Features {
  namespace GameDownloader {

    class DownloadStats : public QObject
    {
      Q_OBJECT
    public:
      explicit DownloadStats(QObject *parent = 0);
      explicit DownloadStats(const DownloadStats &obj);
      ~DownloadStats();

      DownloadStats &operator = (const DownloadStats &obj);

      void setId(const QString& id);

      void load();
      void save();

      void start();
      void stop();

      void update(int downloadSpeed, int uploadSpeed, qint64 totalSize);

      unsigned int downloadTime() const;
      unsigned int applicationRestartCount() const;
      int maxDownloadSpeed() const;
      int maxUploadSpeed() const;
      qint64 totalSize() const;

    protected:
      QString _id;

      bool _initialized;
      unsigned int _downloadTime;
      unsigned int _downloadTimeAtStart;
      qint64 _startTime;

      unsigned int _applicationRestartCount;
      bool _applicationRestartCountChanged;

      bool _started;

      int _maxDownloadSpeed;
      int _maxUploadSpeed;
      qint64 _totalSize;
    };
  }
}