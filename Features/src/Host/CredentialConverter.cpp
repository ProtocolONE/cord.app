#include <Host/CredentialConverter.h>

using P1::RestApi::GameNetCredential;

namespace GameNet {
  namespace Host {
    namespace Bridge {

      Credential createDbusCredential(const GameNetCredential& credential)
      {
        Credential result;
        result.userId = credential.userId();
        result.appKey = credential.appKey();
        result.cookie = credential.cookie();
        return result;
      }

      GameNetCredential createGameNetCredential(const Credential& credential)
      {
        GameNetCredential result;
        result.setUserId(credential.userId);
        result.setAppKey(credential.appKey);
        result.setCookie(credential.cookie);
        return result;
      }

    }
  }
}