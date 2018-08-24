#pragma once

#include <QtCore/QObject>
#include <QtCore/QTimer>

class ConnectionBridgeProxy;

namespace P1 {
  namespace RestApi {
    class GameNetCredential;
  }
}

namespace GameNet {
  namespace Host {

    class ClientConnection : public QObject
    {
      Q_OBJECT
    public:
      ClientConnection(const QString &name, QObject* parent = 0);
      virtual ~ClientConnection();

      void init();
      void close();

      void setCredential(const P1::RestApi::GameNetCredential& value);
      void setSecondCredential(const P1::RestApi::GameNetCredential& value);

    signals:
      void disconnected();
      void wrongCredential(const QString& userId);

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