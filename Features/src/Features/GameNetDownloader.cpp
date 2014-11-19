#include <Features/GameNetDownloader.h>

#include <QtCore/QByteArray>
#include <QtCore/QFile>
#include <QtCore/QDebug>
#include <QtNetwork/QNetworkRequest>

namespace GameNet {
  GameNetDownloader::GameNetDownloader(QObject *parent)
    : QObject(parent)
  {
    QObject::connect(&this->_networkAccessManager, &QNetworkAccessManager::finished, 
      this, &GameNetDownloader::fileDownloaded);
  }

  GameNetDownloader::~GameNetDownloader()
  {

  }

  void GameNetDownloader::download(const QString &url, const QString &path)
  {
    this->_targetFilePath = path;
    QNetworkRequest request(url);
    this->_networkAccessManager.get(request);
  }

  void GameNetDownloader::fileDownloaded(QNetworkReply* reply)
  {
    QNetworkReply::NetworkError err = reply->error();
    if (err != QNetworkReply::NoError) {
      reply->deleteLater();
      emit this->error(err);
      return;
    }

    if (!this->_targetFilePath.isEmpty()) {
      int statusCode = reply->attribute(QNetworkRequest::HttpStatusCodeAttribute).toInt();

      if (statusCode == 301 || statusCode == 302) {
        QUrl newUrl = reply->attribute(QNetworkRequest::RedirectionTargetAttribute).toUrl();
        QNetworkRequest newRequest(newUrl);
        this->_networkAccessManager.get(newRequest);
        reply->deleteLater();
        return;
      }

      QByteArray data = reply->readAll();

      QFile file(this->_targetFilePath);
      if (file.open(QIODevice::WriteOnly)) {
        file.write(data);
        file.close();
      }
    }

    reply->deleteLater();
    emit downloaded(this->_targetFilePath);
  }
}