/****************************************************************************
** This file is a part of Syncopate Limited GameNet Application or it parts.
**
** Copyright (©) 2014, Syncopate Limited and/or affiliates. 
** All rights reserved.
**
** This file is provided AS IS with NO WARRANTY OF ANY KIND, INCLUDING THE
** WARRANTY OF DESIGN, MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE.
****************************************************************************/

#include <Features/GameNetDownloader.h>

#include <QtCore/QByteArray>
#include <QtCore/QFile>
#include <QtCore/QFileInfo>
#include <QtCore/QDir>
#include <QtCore/QDebug>
#include <QtNetwork/QNetworkAccessManager>
#include <QtNetwork/QNetworkRequest>

namespace GameNet {

  GameNetDownloader::GameNetDownloader(QObject *parent)
    : QObject(parent)
    , _networkAccessManager(new QNetworkAccessManager(this))
  {
    connect(this->_networkAccessManager, &QNetworkAccessManager::finished,
      this, &GameNetDownloader::fileDownloaded);
  }

  GameNetDownloader::~GameNetDownloader()
  {
  }

  void GameNetDownloader::download(const QString &url, const QString &path)
  {
    if (!this->_targetFilePath.isEmpty()) {
      emit this->error(QNetworkReply::UnknownNetworkError);
      return;
    }

    this->_targetFilePath = path;
    QNetworkRequest request(url);
    this->_networkAccessManager->get(request);
  }

  void GameNetDownloader::fileDownloaded(QNetworkReply* reply)
  {
    QNetworkReply::NetworkError err = reply->error();
    reply->deleteLater();

    if (err != QNetworkReply::NoError) {
      emit this->error(err);
      return;
    }

    int statusCode = reply->attribute(QNetworkRequest::HttpStatusCodeAttribute).toInt();

    if (statusCode == 301 || statusCode == 302) {
      QUrl newUrl = reply->attribute(QNetworkRequest::RedirectionTargetAttribute).toUrl();
      QNetworkRequest newRequest(newUrl);
      this->_networkAccessManager->get(newRequest);
      return;
    }

    QByteArray data = reply->readAll();

    QFileInfo info(this->_targetFilePath);
    QDir dir;
    dir.mkpath(info.absolutePath());

    QFile file(this->_targetFilePath);
    if (file.open(QIODevice::WriteOnly | QIODevice::Truncate)) {
      file.write(data);
      file.close();
    }

    emit downloaded(this->_targetFilePath);
  }
}
