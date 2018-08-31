#pragma once

#include <QtCore/QObject>
#include <QtNetwork/QNetworkReply>

class QNetworkAccessManager;

namespace P1 {

  class ProtocolOneDownloader : public QObject
  {
    Q_OBJECT
  public:
    explicit ProtocolOneDownloader(QObject *parent = 0);
    virtual ~ProtocolOneDownloader();

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