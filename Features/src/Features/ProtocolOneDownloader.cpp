#include <Features/ProtocolOneDownloader.h>
#include <Helper/FileUtils.h>

#include <QtCore/QByteArray>
#include <QtCore/QFile>
#include <QtCore/QFileInfo>
#include <QtCore/QDir>
#include <QtCore/QDebug>
#include <QtNetwork/QNetworkAccessManager>
#include <QtNetwork/QNetworkRequest>

namespace P1 {

  ProtocolOneDownloader::ProtocolOneDownloader(QObject *parent)
    : QObject(parent)
    , _networkAccessManager(new QNetworkAccessManager(this))
  {
    connect(this->_networkAccessManager, &QNetworkAccessManager::finished,
      this, &ProtocolOneDownloader::fileDownloaded);
  }

  ProtocolOneDownloader::~ProtocolOneDownloader()
  {
  }

  void ProtocolOneDownloader::download(const QString &url, const QString &path)
  {
    if (!this->_targetFilePath.isEmpty()) {
      emit this->error(QNetworkReply::UnknownNetworkError);
      return;
    }

    this->_targetFilePath = path;
    QNetworkRequest request(url);
    this->_networkAccessManager->get(request);
  }

  void ProtocolOneDownloader::fileDownloaded(QNetworkReply* reply)
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

    Features::FileUtils::createDirectoryForFile(this->_targetFilePath);

    QFile file(this->_targetFilePath);
    if (file.open(QIODevice::WriteOnly | QIODevice::Truncate)) {
      file.write(data);
      file.close();
    }

    emit downloaded(this->_targetFilePath);
  }
}
