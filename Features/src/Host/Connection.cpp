#include <Host/Connection.h>

#include <Host/Bridge/ConnectionBridge.h>
#include <Host/Dbus/ConnectionBridgeAdaptor.h>

#include <QtCore/QDebug>

using GGS::RestApi::CommandBase;
using GGS::RestApi::GameNetCredential;

namespace GameNet {
  namespace Host {

    Connection::Connection(const QDBusConnection& connection, QObject *parent /*= 0*/)
      : QObject(parent)
      , _dbusConnection(connection)
    {
      Bridge::ConnectionBridge *bridge = new Bridge::ConnectionBridge(this);
      bridge->setConnection(this);

      new ConnectionBridgeAdaptor(bridge);

      this->_dbusConnection.registerObject("/connection", bridge);
      this->_dbusConnection.registerService("com.gamenet.dbus");

      QObject::connect(&this->_timeoutTimer, &QTimer::timeout,
        this, &Connection::timeoutTick);

      this->_timeoutTimer.setInterval(45000);
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

    void Connection::setCredential(const GameNetCredential& value)
    {
      this->_credential = value;
    }

    void Connection::setSecondCredential(const GameNetCredential& value)
    {
      this->_secondCredential = value;
    }

    const GameNetCredential& Connection::credential()
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

    void Connection::lockService(const QString& serviceId)
    {
      this->_lockedServices.insert(serviceId);
    }

    void Connection::unlockService(const QString& serviceId)
    {
      this->_lockedServices.remove(serviceId);
    }

    void Connection::onGenericError(CommandBase::Error error, QString message, CommandBase *command)
    {
      switch(error) {
      case CommandBase::AuthorizationFailed: // break пропущен не спроста
      case CommandBase::AccountNotExists:
      case CommandBase::AuthorizationLimitExceed:
      case CommandBase::UnknownAccountStatus:
        break;
      default:
        return; // ignore all other error 
      }
      
      const QMap<QString, QString>* params = command->commandParameters();
      QString userId = params->value("userId", "");
      if (userId.isEmpty())
        return;

      if (this->_credential.userId() == userId || this->_secondCredential.userId() == userId)
        emit this->wrongCredential(userId);
    }

    void Connection::ping()
    {
      this->_timeoutTimer.start();
      emit this->pong();
    }

    void Connection::timeoutTick()
    {
      this->_timeoutTimer.stop();
      qDebug() << "Client" << this->_applicationName << "disconnected";
      emit this->disconnected();
    }

    void Connection::terminate()
    {
      emit this->terminateSignal();
    }

  }
}
