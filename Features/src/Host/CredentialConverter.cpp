#include <Host/CredentialConverter.h>

using P1::RestApi::ProtocolOneCredential;

namespace P1 {
  namespace Host {
    namespace Bridge {

      Credential createDbusCredential(const ProtocolOneCredential& credential)
      {
        Credential result;
        result.userId = credential.userId();
        result.appKey = credential.appKey();
        result.cookie = credential.cookie();
        return result;
      }

      ProtocolOneCredential createProtocolOneCredential(const Credential& credential)
      {
        ProtocolOneCredential result;
        result.setUserId(credential.userId);
        result.setAppKey(credential.appKey);
        result.setCookie(credential.cookie);
        return result;
      }

    }
  }
}