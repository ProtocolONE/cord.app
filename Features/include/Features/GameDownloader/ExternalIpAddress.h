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