#include <Host/ClientConnection.h>

#include <Host/Dbus/ConnectionBridgeProxy.h>
#include <Host/Dbus/DbusConnection.h>

#include <Host/CredentialConverter.h>

#include <Core/UI/Message.h>

#include <RestApi/GameNetCredential.h>

#include <QtCore/QDebug>

using GameNet::Host::DBus::DBusConnection;
using GGS::RestApi::GameNetCredential;

using GameNet::Host::Bridge::createDbusCredential;

namespace GameNet {
  namespace Host {

    ClientConnection::ClientConnection(const QString &name, QObject* parent /*= 0*/)
      : QObject(parent)
      , _maxTimeoutFail(30) // 3 * 10 * 5000 = 3 * 50 sec
      , _timeoutFail(0)
      , _connection(nullptr)
      , _appName(name)
    {
      QObject::connect(&this->_timeoutTimer, &QTimer::timeout,
        this, &ClientConnection::timeoutTick);

      this->_timeoutTimer.setInterval(5000);

      QObject::connect(&this->_pingpongTimer, &QTimer::timeout,
        this, &ClientConnection::sendPing);

      this->_pingpongTimer.setInterval(5000);
    }

    ClientConnection::~ClientConnection()
    {
    }

    void ClientConnection::init()
    {
      QDBusConnection &connection = DBusConnection::bus();
      QString dbusService("com.gamenet.dbus");

      this->_connection = new ConnectionBridgeProxy(dbusService, "/connection", connection, this);
      this->_connection->setApplicationName(this->_appName);

      QObject::connect(this->_connection, &ConnectionBridgeProxy::pong,
        this, &ClientConnection::onPong);

      QObject::connect(this->_connection, &ConnectionBridgeProxy::wrongCredential,
        this, &ClientConnection::wrongCredential);

      this->sendPing();
      this->_pingpongTimer.start();
      this->_timeoutTimer.start();
    }

    void ClientConnection::timeoutTick()
    {
      this->_timeoutFail++;
      if (this->_timeoutFail < this->_maxTimeoutFail)
        return;

      this->internalDisconnected();
    }

    void ClientConnection::sendPing()
    {
      Q_ASSERT(this->_connection);
      QDBusPendingReply<> result = this->_connection->ping();

      if (!result.isError())
        return;

      qDebug() << "Ping finished with error:" << result.error().name() 
        << "message:" << result.error().message() 
        << "code:" << result.error().type();

      if (result.error().type() == QDBusError::Disconnected)
        this->internalDisconnected();
    }

    void ClientConnection::onPong()
    {
      this->_timeoutTimer.start();
      this->_timeoutFail = 0;
    }

    void ClientConnection::setCredential(const GGS::RestApi::GameNetCredential& value)
    {
      Q_ASSERT(this->_connection);
      this->_connection->setCredential(createDbusCredential(value));
    }

    void ClientConnection::setSecondCredential(const GGS::RestApi::GameNetCredential& value)
    {
      Q_ASSERT(this->_connection);
      this->_connection->setSecondCredential(createDbusCredential(value));
    }

    void ClientConnection::close()
    {
      Q_ASSERT(this->_connection);
      this->_connection->close();
    }

    void ClientConnection::internalDisconnected()
    {
      this->_timeoutTimer.stop();
      this->_pingpongTimer.stop();

      qDebug() << "Disconnected from host";
      GGS::Core::UI::Message::critical(tr("DBUS_DISCONNECTED_TITLE"), tr("DBUS_DISCONNECTED_TEXT"));

      emit this->disconnected();
    }

  }
}