#pragma once

#include <QtCore/QObject>
#include <QtCore/QString>

namespace P1 {
  namespace RestApi {
    class ProtocolOneCredential;
  }

  namespace Host {

    class Connection;

    namespace Bridge {

      class ConnectionBridge : public QObject
      {
        Q_OBJECT
        Q_CLASSINFO("Version", "1.0.0.0")
        Q_CLASSINFO("D-Bus Interface", "com.protocolone.launcher.dbus.Connection")
      public:
        explicit ConnectionBridge(QObject *parent = 0);
        virtual ~ConnectionBridge() = default;

        void setConnection(Connection* value);

      public slots:

        /**
         * \fn  void ConnectionBridge::setApplicationName(const QString& applicationName);
         *
         * \brief Sets an application name. This method must be call before any call to other DBUS objects.
         *
         * \author  Ilya Tkachenko
         * \date  23.10.2014
         *
         * \param applicationName Name of the application.
         */

        void setApplicationName(const QString& applicationName);

        /**
         * \fn  void ConnectionBridge::setCredential(const QString& accessToken, const QString& acccessTokenExpiredTime);
         *
         * \brief Sets a user's credential.
         *
         * \author  Ilya Tkachenko
         * \date  23.10.2014
         *
         * \param credential  The credential.
         */
        void setCredential(const QString& accessToken, const QString& acccessTokenExpiredTime);

        void updateCredential(
            const QString& accessTokenOld, const QString& acccessTokenExpiredTimeOld
          , const QString& accessTokenNew, const QString& acccessTokenExpiredTimeNew);

        /**
         * \fn  void ConnectionBridge::ping();
         *
         * \brief Send ping request to determine that host still alive.
                  If it's alive host will respond with 'pong' signal.
         *
         * \author  Ilya Tkachenko
         * \date  28.10.2014
         */

        void ping();

        /**
         * \fn  void ConnectionBridge::close();
         *
         * \brief Send close request
         *
         * \author  Igor Bugaev
         * \date  02.12.2014
         */

        void close();

      signals:

        void authorizationError(const QString& accessToken, const QString& acccessTokenExpiredTime);

        /**
         * \fn  void ConnectionBridge::pong();
         *
         * \brief Response to ping request.
         *
         * \author  Ilya Tkachenko
         * \date  28.10.2014
         */

        void pong();

      private:
        Connection *_connection;

        void onAuthorizationError(const P1::RestApi::ProtocolOneCredential& credential);

      };

    }
  }
}