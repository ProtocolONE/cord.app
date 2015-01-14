#include <Features/Marketing/SystemInfo/SoftVendor/RegistryHelper.h>

namespace Features {
  namespace Marketing {
    namespace SystemInfo {
      namespace SoftVendor {

        bool registryKeyExist(HKEY key, const std::wstring& path)
        {
          HKEY subKey = nullptr;
          LONG result = RegOpenKeyEx(
            key,
            path.c_str(),
            0,
            KEY_READ | KEY_WOW64_32KEY,
            &subKey);

          if (result == ERROR_SUCCESS) {
            RegCloseKey(subKey);
            return true;
          }

          return false;
        }

      }
    }
  }
}
