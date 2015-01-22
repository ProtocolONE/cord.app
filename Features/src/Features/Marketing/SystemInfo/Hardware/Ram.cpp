
#include <Features/Marketing/SystemInfo/Hardware/Ram.h>
#include <Features/WmiQuery.h>

namespace Features {
  namespace Marketing {
    namespace SystemInfo {
      namespace Hardware {

        Ram::Ram(QObject *parent /*= 0*/)
          : QObject(parent)
        {
        }

        Ram::~Ram()
        {
        }

        void Ram::write(QXmlStreamWriter *writer)
        {
          std::wstring serviceName(L"root\\cimv2");
          std::wstring tableName(L"Win32_PhysicalMemory");
          std::vector<std::wstring> keys;
          keys.push_back(L"Manufacturer");
          keys.push_back(L"PartNumber");
          keys.push_back(L"SerialNumber");
          keys.push_back(L"Speed");
          keys.push_back(L"Capacity");

          std::vector<std::unordered_map<std::wstring, std::wstring> > result;

          WmiQuery query;
          if (!query.getFromTable(serviceName, tableName, keys, result))
            return;

          if (result.size() == 0)
            return;
          
          writer->writeStartElement("ram");

          quint64 totalSize = 0;

          for (size_t i = 0; i < result.size(); ++i) {
            writer->writeStartElement("node");
            const std::unordered_map<std::wstring, std::wstring>& row(result[i]);
            for (auto it = row.cbegin(); it != row.cend(); ++it) {
              QString key = QString::fromStdWString(it->first);
              QString value = QString::fromStdWString(it->second);
              writer->writeTextElement(key, value);

              if (key == "Capacity")
                totalSize += value.toULongLong();
            }

            writer->writeEndElement();
          }

          writer->writeTextElement("totalSize", QString::number(totalSize));
          writer->writeEndElement();
        }

      }
    }
  }
}