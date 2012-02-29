#include <Features/Jabber.h>

#include <QtCore/QCryptographicHash>
#include <QtCore/QDebug>

#include <QXmppConfiguration.h>
#include <QXmppClient.h>
#include <QXmppLogger.h>

namespace Features {

  Jabber::Jabber(QObject *parent) : QObject(parent)
  {
#ifdef _DEBUG
    QXmppLogger::getLogger()->setLoggingType(QXmppLogger::StdoutLogging);
#else
    QXmppLogger::getLogger()->setLoggingType(QXmppLogger::NoLogging);
#endif
        
    this->_client = new QXmppClient(this);
  }

  Jabber::~Jabber()
  {
  }

  void Jabber::authSuccess(const QString& userId, const QString& appKey)
  {
    QCryptographicHash hash(QCryptographicHash::Md5);
    hash.addData(appKey.toAscii());

    QString pass = QString(hash.result().toHex());
    
    QXmppConfiguration config;
    config.setHost("qj.gamenet.ru");
    config.setDomain("qj.gamenet.ru");
    config.setUser(userId);
    config.setPassword(pass);
    config.setKeepAliveInterval(0); //мы не хотим пинговать сервер
    config.setAutoReconnectionEnabled(true);

    this->_client->connectToServer(config);
  }

  void Jabber::logout()
  {
    this->_client->disconnectFromServer();
  }
}

