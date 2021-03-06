#include <Host/ClientConnection.h>

#include <Host/Dbus/ConnectionBridgeProxy.h>
#include <Host/Dbus/DbusConnection.h>

#include <Core/UI/Message.h>

#include <RestApi/ProtocolOneCredential.h>

#include <QtCore/QDebug>

using P1::Host::DBus::DBusConnection;
using P1::RestApi::ProtocolOneCredential;

namespace P1 {
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
      QString dbusService("com.protocolone.launcher.dbus");

      this->_connection = new ConnectionBridgeProxy(dbusService, "/connection", connection, this);
      this->_connection->setApplicationName(this->_appName);

      QObject::connect(this->_connection, &ConnectionBridgeProxy::pong,
        this, &ClientConnection::onPong);

      QObject::connect(this->_connection, &ConnectionBridgeProxy::authorizationError,
        this, &ClientConnection::authorizationError);

      this->sendPing();
      this->_pingpongTimer.start();
      this->_timeoutTimer.start();
    }

    void ClientConnection::timeoutTick()
    {
      // HACK disabled. Debug works very poor.
      //this->_timeoutFail++;
      //if (this->_timeoutFail < this->_maxTimeoutFail)
      //  return;

      //this->internalDisconnected();
    }

    void ClientConnection::sendPing()
    {
      Q_CHECK_PTR(this->_connection);
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

      DEBUG_LOG << "!!!! ClientConnection::onPong()";

      this->_timeoutTimer.start();
      this->_timeoutFail = 0;
    }

    void ClientConnection::setCredential(const P1::RestApi::ProtocolOneCredential& value)
    {
      Q_CHECK_PTR(this->_connection);
      this->_connection->setCredential(value.acccessTokent(), value.accessTokenExpiredTimeAsString());
    }

    void ClientConnection::updateCredential(
      const P1::RestApi::ProtocolOneCredential& valueOld,
      const P1::RestApi::ProtocolOneCredential& valueNew)
    {
      Q_CHECK_PTR(this->_connection);
      this->_connection->updateCredential(
          valueOld.acccessTokent(), valueOld.accessTokenExpiredTimeAsString()
        , valueNew.acccessTokent(), valueNew.accessTokenExpiredTimeAsString());
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
      P1::Core::UI::Message::critical(tr("DBUS_DISCONNECTED_TITLE"), tr("DBUS_DISCONNECTED_TEXT"));

      emit this->disconnected();
    }

  }
}