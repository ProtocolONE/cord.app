#pragma once

#include <Windows.h>
#include <string>

namespace Features {
  namespace Marketing {
    namespace SystemInfo {
      namespace SoftVendor {

        bool registryKeyExist(HKEY key, const std::wstring& path);

      }
    }
  }
}
