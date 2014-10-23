#pragma once

#include <QtCore/QObject>
#include <QtCore/QTimer>

class ConnectionBridgeProxy;

namespace GGS {
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
      explicit ClientConnection(QObject* parent = 0);
      virtual ~ClientConnection();

      void init();

      void setCredential(const GGS::RestApi::GameNetCredential& value);
      void setSecondCredential(const GGS::RestApi::GameNetCredential& value);

    signals:
      void disconnected();
      void wrongCredential(const QString& userId);

    private:
      void timeoutTick();
      void sendPing();
      void onPong();

      QTimer _timeoutTimer;
      QTimer _pingpongTimer;
      ConnectionBridgeProxy *_connection;
    };

  }
}