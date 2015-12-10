#include <Features/Marketing/SystemInfo/Hardware/OsInfo.h>
#include <Features/WmiQuery.h>

#include <Helper/DebugLog.h>

#include <Windows.h>

namespace Features {
  namespace Marketing {
    namespace SystemInfo {
      namespace Hardware {

        OsInfo::OsInfo(QObject *parent /*= 0*/)
          : QObject(parent)
        {
        }

        OsInfo::~OsInfo()
        {
        }

        void OsInfo::write(QXmlStreamWriter *writer)
        {
          writer->writeStartElement("os");

          OSVERSIONINFO osvi = {0};
          osvi.dwOSVersionInfoSize = sizeof(OSVERSIONINFO);
          GetVersionEx(&osvi);

          QString winVersion;
          switch (QSysInfo::windowsVersion()) {
          case QSysInfo::WV_5_1: winVersion = "5.1"; break;
          case QSysInfo::WV_6_0: winVersion = "6.0"; break;
          case QSysInfo::WV_6_1: winVersion = "6.1"; break;
          case QSysInfo::WV_6_2: winVersion = "6.2"; break;
          case QSysInfo::WV_6_3: winVersion = "6.3"; break;
          case QSysInfo::WV_10_0: winVersion = "10.0"; break;
          default: winVersion = "6.1"; break;
          }

          QString version = QString("%1.%2")
            .arg(winVersion)
            .arg(osvi.dwBuildNumber);

          writer->writeTextElement("Version", version);
          this->writeWmiVersion(writer);

          writer->writeTextElement("WOW64", this->isWow64() ? "1" : "0");

          writer->writeEndElement();
        }

        void OsInfo::writeWmiVersion(QXmlStreamWriter *writer)
        {
          std::wstring serviceName(L"root\\cimv2");
          std::wstring tableName(L"Win32_OperatingSystem");
          std::vector<std::wstring> keys;
          keys.push_back(L"Version");
          keys.push_back(L"Caption");
          keys.push_back(L"CountryCode");
          keys.push_back(L"OperatingSystemSKU");

          std::vector<std::unordered_map<std::wstring, std::wstring> > result;

          WmiQuery query;
          if (!query.getFromTable(serviceName, tableName, keys, result))
            return;

          if (result.size() == 0)
            return;

          std::unordered_map<std::wstring, std::wstring>& row(result[0]);

          writer->writeTextElement("WmiVersion", QString::fromStdWString(row[L"Version"]));
          writer->writeTextElement("Caption", QString::fromStdWString(row[L"Caption"]));
          writer->writeTextElement("CountryCode", QString::fromStdWString(row[L"CountryCode"]));
          writer->writeTextElement("OperatingSystemSKU", QString::fromStdWString(row[L"OperatingSystemSKU"]));
        }

        bool OsInfo::isWow64()
        {
          typedef BOOL (APIENTRY *LPFN_ISWOW64PROCESS)(HANDLE, PBOOL);
          LPFN_ISWOW64PROCESS fnIsWow64Process;

          fnIsWow64Process = reinterpret_cast<LPFN_ISWOW64PROCESS>(
            GetProcAddress(GetModuleHandleW(L"kernel32"), "IsWow64Process"));

          if (!fnIsWow64Process)
            return false;

          BOOL bIsWow64 = FALSE;
          if (!fnIsWow64Process(GetCurrentProcess(), &bIsWow64)) {
            DWORD error = GetLastError();
            DEBUG_LOG << "Fail to call IsWow64Process: " << error;
            return false;
          }

          return bIsWow64 != FALSE;
        }

      }
    }
  }
}