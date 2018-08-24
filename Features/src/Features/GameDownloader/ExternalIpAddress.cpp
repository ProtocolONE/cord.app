#include <Features/GameDownloader/ExternalIpAddress.h>

#include <QtNetwork/QNetworkRequest>

#include <QtCore/QUrl>

namespace Features {
  namespace GameDownloader {

    ExternalIpAddress::ExternalIpAddress(QObject *parent)
      : QObject(parent)
      , _manager(new QNetworkAccessManager(this))
      , _url("https://gnapi.com:8443/restapi")
    {
      connect(this->_manager, SIGNAL(sslErrors(QNetworkReply*, const QList<QSslError>&)), 
        this, SLOT(sslErrors(QNetworkReply *, const QList<QSslError>&)));
    }

    ExternalIpAddress::~ExternalIpAddress()
    {
    }

    void ExternalIpAddress::execute()
    {
      QNetworkReply *reply = this->_manager->head(QNetworkRequest(this->_url));
      connect(reply, SIGNAL(finished()), this, SLOT(requestFinish()));
    }

    void ExternalIpAddress::requestFinish()
    {
      QNetworkReply *reply = qobject_cast<QNetworkReply*>(QObject::sender());
      if (!reply)
        return;

      reply->deleteLater();

      QString remouteIp;
      if (reply->error() == QNetworkReply::NoError && reply->hasRawHeader(QByteArray("Remote-ip")))
        remouteIp = QString::fromLatin1(reply->rawHeader(QByteArray("Remote-ip")));

      this->result(remouteIp);
    }

    void ExternalIpAddress::sslErrors(QNetworkReply *reply, const QList<QSslError> &errors)
    {
      reply->ignoreSslErrors();
    }

    void ExternalIpAddress::setUrl(const QString& url)
    {
      this->_url = QUrl(url);
    }

  }
}