#include <Host/Bridge/ConnectionBridge.h>

#include <Host/Connection.h>
#include <Host/CredentialConverter.h>

namespace GameNet {
  namespace Host {
    namespace Bridge {

      ConnectionBridge::ConnectionBridge(QObject *parent /*= 0*/)
        : QObject(parent)
        , _connection(nullptr)
      {
      }

      ConnectionBridge::~ConnectionBridge()
      {
      }

      void ConnectionBridge::setConnection(Connection* value)
      {
        Q_ASSERT(value);
        this->_connection = value;

        QObject::connect(value, &Connection::wrongCredential,
          this, &ConnectionBridge::wrongCredential);

        QObject::connect(value, &Connection::pong,
          this, &ConnectionBridge::pong);
      }

      void ConnectionBridge::setApplicationName(const QString& applicationName)
      {
        Q_ASSERT(this->_connection);
        this->_connection->setApplicationName(applicationName);
      }

      void ConnectionBridge::setCredential(const Credential& credential)
      {
        Q_ASSERT(this->_connection);
        this->_connection->setCredential(createGameNetCredential(credential));
      }

      void ConnectionBridge::setSecondCredential(const Credential& credential)
      {
        Q_ASSERT(this->_connection);
        this->_connection->setSecondCredential(createGameNetCredential(credential));
      }

      void ConnectionBridge::ping()
      {
        Q_ASSERT(this->_connection);
        this->_connection->ping();
      }

      void ConnectionBridge::close()
      {
        Q_ASSERT(this->_connection);
        this->_connection->close();
      }

    }
  }
}