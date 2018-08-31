#pragma once

#include <QtCore/QString>
#include <QtCore/QMetaType>

#include <QtDBus/QDBusArgument>

namespace P1 {
  namespace Host {
    namespace Bridge {

      /**
       * \struct  Credential
       *
       * \brief This structure represents user's ProtocolOne credential.
       *
       * \author  Ilya Tkachenko
       * \date  07.10.2014
       */

      /** \var Credential::userId 

      User's identificator in ProtocolOne Launcher.

      */
      /** \var Credential::appKey 

      User's token for ProtocolOne Web API.

      */

      /** \var Credential::cookie 

      User's global cookie for authorization on ProtocolOne websites.

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
QDBusArgument& operator <<(QDBusArgument &argument, const P1::Host::Bridge::Credential arg);
const QDBusArgument& operator >>(const QDBusArgument &argument, P1::Host::Bridge::Credential& arg);
/// \endcond