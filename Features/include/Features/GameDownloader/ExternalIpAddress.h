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
#include <QtNetwork/QNetworkAccessManager>
#include <QtNetwork/QNetworkReply>
#include <QtNetwork/QSslError>

namespace Features {
  namespace GameDownloader {

    class ExternalIpAddress : public QObject
    {
      Q_OBJECT
    public:
      ExternalIpAddress(QObject *parent = 0);
      ~ExternalIpAddress();

      void execute();
      void setUrl(const QString& url);

    signals:
      void result(QString externalIp);

    private slots:
      void requestFinish();
      void sslErrors(QNetworkReply *reply, const QList<QSslError> &errors);

    private:
      QNetworkAccessManager *_manager;
      QUrl _url;
    };

  }
}