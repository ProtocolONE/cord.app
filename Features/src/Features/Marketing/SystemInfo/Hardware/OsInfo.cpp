#include <Features/Marketing/SystemInfo/Hardware/OsInfo.h>
#include <Features/WmiQuery.h>
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
          
          QString version = QString("%1.%2.%3")
            .arg(osvi.dwMajorVersion)
            .arg(osvi.dwMinorVersion)
            .arg(osvi.dwBuildNumber);

          writer->writeTextElement("Version", version);
          this->writeWmiVersion(writer);

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

      }
    }
  }
}