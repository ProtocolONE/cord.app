#pragma once

#include <Host/Bridge/Credential.h>

#include <QtCore/QObject>
#include <QtCore/QString>

namespace GameNet {
  namespace Host {

    class Connection;

    namespace Bridge {

      class ConnectionBridge : public QObject
      {
        Q_OBJECT
        Q_CLASSINFO("Version", "1.0.0.0")
        Q_CLASSINFO("D-Bus Interface", "com.gamenet.dbus.Connection")
      public:
        explicit ConnectionBridge(QObject *parent = 0);
        virtual ~ConnectionBridge();

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

        /*
        <method name="setCredential">
          <arg name="credential" type="a(sii)" direction="in"/>
          <annotation name="org.qtproject.QtDBus.QtTypeName.In0" value="GameNet::Host::Bridge::Credential"/>
        </method>
        */

        /**
         * \fn  void ConnectionBridge::setCredential(const Credential& credential);
         *
         * \brief Sets a user's credential.
         *
         * \author  Ilya Tkachenko
         * \date  23.10.2014
         *
         * \param credential  The credential.
         */
        void setCredential(const Credential& credential);


        /*
        <method name="setSecondCredential">
          <arg name="credential" type="a(sii)" direction="in"/>
          <annotation name="org.qtproject.QtDBus.QtTypeName.In0" value="GameNet::Host::Bridge::Credential"/>
        </method>
        */

        /**
         * \fn  void ConnectionBridge::setSecondCredential(const Credential& credential);
         *
         * \brief Sets a user's second credential. It's available only for premium users.
         *
         * \author  Ilya Tkachenko
         * \date  27.10.2014
         *
         * \param credential  The credential.
         */

        void setSecondCredential(const Credential& credential);

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

      signals:

        /**
         * \fn  void ConnectionBridge::wrongCredential(const QString& userId);
         *
         * \brief Wrong credential for user with identifier 'userId'.
         *
         * \author  Ilya Tkachenko
         * \date  27.10.2014
         *
         * \param userId  Identifier for the user.
         */

        void wrongCredential(const QString& userId);

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

      };

    }
  }
}