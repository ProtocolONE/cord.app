#pragma once

#include <QtCore/QObject>
#include <QtCore/QTimer>

class ConnectionBridgeProxy;

namespace P1 {
  namespace RestApi {
    class ProtocolOneCredential;
  }
}

namespace P1 {
  namespace Host {

    class ClientConnection : public QObject
    {
      Q_OBJECT
    public:
      ClientConnection(const QString &name, QObject* parent = 0);
      virtual ~ClientConnection();

      void init();
      void close();

      void setCredential(const P1::RestApi::ProtocolOneCredential& value);
      void updateCredential(const P1::RestApi::ProtocolOneCredential& valueOld, const P1::RestApi::ProtocolOneCredential& valueNew);

    signals:
      void disconnected();

      void authorizationError(const QString& accessToken, const QString &acccessTokenExpiredTime);

    private:
      void timeoutTick();
      void internalDisconnected();
      void sendPing();
      void onPong();

      QTimer _timeoutTimer;
      QTimer _pingpongTimer;
      int _maxTimeoutFail;
      int _timeoutFail;

      ConnectionBridgeProxy *_connection;
      QString _appName;
    };

  }
}