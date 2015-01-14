#include <Features/Marketing/SystemInfo/SoftVendor/Antivirus.h>

#include <Features/WmiQuery.h>
#include <Windows.h>

namespace Features {
  namespace Marketing {
    namespace SystemInfo {
      namespace SoftVendor {

        Antivirus::Antivirus(QObject *parent)
          : VendorBase("antivirus", parent)
        {
        }

        Antivirus::~Antivirus()
        {
        }

        void Antivirus::writePayload(QXmlStreamWriter *writer)
        {
          QString name;
          this->queryAntivirus(name);
          writer->writeTextElement("name", name);
        }

        bool Antivirus::queryAntivirus(QString& name)
        {
          DWORD osVersion = GetVersion();
          bool isXP = (DWORD)(LOBYTE(LOWORD(osVersion))) == 5;
          
          std::wstring serviceName(isXP ? L"root\\securitycenter" : L"root\\securitycenter2");
          std::wstring tableName(L"AntivirusProduct");
          std::vector<std::wstring> keys;
          keys.push_back(L"displayName");
          
          std::vector<std::unordered_map<std::wstring, std::wstring> > result;

          WmiQuery query;
          if (!query.getFromTable(serviceName, tableName, keys, result))
            return false;

          if (result.size() == 0)
            return false;

          name = QString::fromStdWString(result.at(0)[L"displayName"]);
          return true;
        }

      }
    }
  }
}
