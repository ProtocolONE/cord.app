#pragma once

#include <QtCore/QString>
#include <QtCore/QMetaType>

#include <QtDBus/QDBusArgument>

namespace GameNet {
  namespace Host {
    namespace Bridge {

      /**
       * \struct  Credential
       *
       * \brief This structure represents user's GameNet credential.
       *
       * \author  Ilya Tkachenko
       * \date  07.10.2014
       */

      /** \var Credential::userId 

      User's identificator in GameNet.

      */
      /** \var Credential::appKey 

      User's token for GameNet Web API.

      */

      /** \var Credential::cookie 

      User's global cookie for authorization on GameNet websites.

      */
      typedef struct {
        QString userId;
        QString appKey;
        QString cookie;
      } Credential;
      Q_DECLARE_METATYPE(Credential)
    }
  }
}

/// \cond
QDBusArgument& operator <<(QDBusArgument &argument, const GameNet::Host::Bridge::Credential arg);
const QDBusArgument& operator >>(const QDBusArgument &argument, GameNet::Host::Bridge::Credential& arg);
/// \endcond