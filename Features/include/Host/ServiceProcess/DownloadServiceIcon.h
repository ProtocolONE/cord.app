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
#include <QtCore/QUrl>

#include <QtNetwork/QNetworkAccessManager>

namespace GameNet {
  namespace Host {
    namespace ServiceProcess {

      class DownloadServiceIcon : public QObject
      {
        Q_OBJECT
      public:
        explicit DownloadServiceIcon(QObject *parent = 0);
        virtual ~DownloadServiceIcon();

        void downloadIconRequest(const QString& url, const QString& serviceId);

      private:
        QString readLastModified(const QString& path);
        void saveLastModifed(const QString& path, const QString& value);

        QNetworkAccessManager _networkManager;

      };

    }
  }
}