#include <Host/Connection.h>

#include <Host/Bridge/ConnectionBridge.h>
#include <Host/Dbus/ConnectionBridgeAdaptor.h>

#include <QtCore/QDebug>


using P1::RestApi::ProtocolOneCredential;

namespace P1 {
  namespace Host {

    Connection::Connection(const QDBusConnection& connection, QObject *parent /*= 0*/)
      : QObject(parent)
      , _dbusConnection(connection)
      , _maxTimeoutFail(30) // 3 * 10 * 5000 = 3 * 50 sec
      , _timeoutFail(0)
    {
      Bridge::ConnectionBridge *bridge = new Bridge::ConnectionBridge(this);
      bridge->setConnection(this);

      new ConnectionBridgeAdaptor(bridge);

      this->_dbusConnection.registerObject("/connection", bridge);
      this->_dbusConnection.registerService("com.protocolone.launcher.dbus");

      QObject::connect(&this->_timeoutTimer, &QTimer::timeout,
        this, &Connection::timeoutTick);

      this->_timeoutTimer.setInterval(5000);
      this->_timeoutTimer.start();
    }

    Connection::~Connection()
    {
    }

    void Connection::setApplicationName(const QString& value)
    {
      // INFO этот метод в некотором смысле авторизация UI приложения у нас в хосте.
      // поидее тут можно что-то "придумать" для проверки адекватности подключения.

      if (value.isEmpty() || !this->_applicationName.isEmpty()) {
        emit this->disconnected();
        return;
      }

      this->_applicationName = value;
      emit this->connectionInfoReceived();
    }

    void Connection::setCredential(const ProtocolOneCredential& value)
    {
      if (value.isEmpty()) {
        emit this->logoutMain();
      } else {
        this->_credential = value;
        emit this->mainCredentialChanged();
      }
    }

    const ProtocolOneCredential& Connection::credential()
    {
      return this->_credential;
    }

    const QString& Connection::applicationName()
    {
      return this->_applicationName;
    }

    void Connection::registerObject(const QString &path, QObject *object)
    {
      this->_dbusConnection.registerObject(path, object);
    }

    bool Connection::isOwnService(const QString& serviceId)
    {
      return this->_lockedServices.contains(serviceId);
    }

    void Connection::onAuthorizationError(const P1::RestApi::ProtocolOneCredential &credential)
    {
      // UNDONE In current UI credential here and in GameExecutor::execute are same.
      // In a future we should compare token with all used by conenction.

      if (credential.userId() != this->_credential.userId())
        return;

      if (credential != this->_credential && this->_credential.isValid()) {
        this->updateCredential(credential, this->_credential);
        return;
      }

      emit this->authorizationError(credential);
    }

    void Connection::updateCredential(
      const P1::RestApi::ProtocolOneCredential &credentialOld,
      const P1::RestApi::ProtocolOneCredential &credentialNew)
    {
      emit this->credentialUpdated(credentialOld, credentialNew);
    }

    void Connection::lockService(const QString& serviceId)
    {
      this->_lockedServices.insert(serviceId);
    }

    void Connection::unlockService(const QString& serviceId)
    {
      this->_lockedServices.remove(serviceId);
    }

    void Connection::ping()
    {
      this->_timeoutTimer.start();
      this->_timeoutFail = 0;
      emit this->pong();
    }

    void Connection::timeoutTick()
    {
      //this->_timeoutFail++;
      //if (this->_timeoutFail < this->_maxTimeoutFail)
      //  return;

      //this->_timeoutTimer.stop();
      //qDebug() << "Client" << this->_applicationName << "disconnected";
      //emit this->disconnected();
    }

    void Connection::close()
    {
      emit this->disconnected();
    }

    QList<QString> Connection::lockedServices()
    {
      return this->_lockedServices.toList();
    }

  }
}
