#pragma once

#include <QtCore/QObject>
#include <QtNetwork/QNetworkReply>

class QNetworkAccessManager;

namespace GameNet {

  class GameNetDownloader : public QObject
  {
    Q_OBJECT
  public:
    explicit GameNetDownloader(QObject *parent = 0);
    virtual ~GameNetDownloader();

  public slots:
    void download(const QString &url, const QString &path);

  signals:
    void error(QNetworkReply::NetworkError err);
    void downloaded(const QString &path);

  private slots:
    void fileDownloaded(QNetworkReply* reply);

  private:
    QString _targetFilePath;
    QNetworkAccessManager *_networkAccessManager;
  };

}