#pragma once

#include <Host/Bridge/Credential.h>
#include <RestApi/ProtocolOneCredential.h>

namespace P1 {
  namespace Host {
    namespace Bridge {

      Credential createDbusCredential(const P1::RestApi::ProtocolOneCredential& credential);
      P1::RestApi::ProtocolOneCredential createProtocolOneCredential(const Credential& credential);
    }
  }
}