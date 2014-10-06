#pragma once

#include <Host/Bridge/Credential.h>
#include <RestApi/GameNetCredential.h>

namespace GameNet {
  namespace Host {
    namespace Bridge {

      Credential createDbusCredential(const GGS::RestApi::GameNetCredential& credential);
      GGS::RestApi::GameNetCredential createGameNetCredential(const Credential& credential);
    }
  }
}