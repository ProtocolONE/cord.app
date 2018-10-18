#include <Host/Bridge/ConnectionBridge.h>

#include <Host/Connection.h>

#include <RestApi/ProtocolOneCredential.h>

using P1::RestApi::ProtocolOneCredential;

namespace P1 {
  namespace Host {
    namespace Bridge {

      ConnectionBridge::ConnectionBridge(QObject *parent /*= 0*/)
        : QObject(parent)
        , _connection(nullptr)
      {
      }

      void ConnectionBridge::setConnection(Connection* value)
      {
        Q_CHECK_PTR(value);
        this->_connection = value;

        QObject::connect(value, &Connection::authorizationError,
          this, &ConnectionBridge::onAuthorizationError);

        QObject::connect(value, &Connection::pong,
          this, &ConnectionBridge::pong);
      }

      void ConnectionBridge::setApplicationName(const QString& applicationName)
      {
        Q_CHECK_PTR(this->_connection);
        this->_connection->setApplicationName(applicationName);
      }

      void ConnectionBridge::setCredential(const QString& accessToken, const QString& acccessTokenExpiredTime)
      {
        Q_CHECK_PTR(this->_connection);

        ProtocolOneCredential credential(accessToken, acccessTokenExpiredTime);
        this->_connection->setCredential(credential);
      }

      void ConnectionBridge::updateCredential(
        const QString& accessTokenOld, const QString& acccessTokenExpiredTimeOld,
        const QString& accessTokenNew, const QString& acccessTokenExpiredTimeNew)
      {
        Q_CHECK_PTR(this->_connection);

        ProtocolOneCredential credentialOld(accessTokenOld, acccessTokenExpiredTimeOld);
        ProtocolOneCredential credentialNew(accessTokenNew, acccessTokenExpiredTimeNew);
        this->_connection->updateCredential(credentialOld, credentialNew);
      }

      void ConnectionBridge::ping()
      {
        Q_CHECK_PTR(this->_connection);
        this->_connection->ping();
      }

      void ConnectionBridge::close()
      {
        Q_CHECK_PTR(this->_connection);
        this->_connection->close();
      }

      void ConnectionBridge::onAuthorizationError(const P1::RestApi::ProtocolOneCredential& credential)
      {
        emit this->authorizationError(credential.acccessTokent(), credential.accessTokenExpiredTimeAsString());
      }

    }
  }
}