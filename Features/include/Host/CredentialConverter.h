#pragma once

#include <Host/Bridge/Credential.h>
#include <RestApi/GameNetCredential.h>

namespace GameNet {
  namespace Host {
    namespace Bridge {

      Credential createDbusCredential(const P1::RestApi::GameNetCredential& credential);
      P1::RestApi::GameNetCredential createGameNetCredential(const Credential& credential);
    }
  }
}