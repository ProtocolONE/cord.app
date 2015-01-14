#include <Features/Marketing/SystemInfo/Hardware/Video.h>
#include <Features/WmiQuery.h>

namespace Features {
  namespace Marketing {
    namespace SystemInfo {
      namespace Hardware {

        Video::Video(QObject *parent /*= 0*/)
          : QObject(parent)
        {
        }

        Video::~Video()
        {
        }

        void Video::write(QXmlStreamWriter *writer)
        {
          std::wstring serviceName(L"root\\cimv2");
          std::wstring tableName(L"Win32_VideoController");
          std::vector<std::wstring> keys;
          keys.push_back(L"AdapterCompatibility");
          keys.push_back(L"Caption");
          keys.push_back(L"VideoProcessor");
          keys.push_back(L"AdapterRAM");
          keys.push_back(L"CurrentHorizontalResolution");
          keys.push_back(L"CurrentVerticalResolution");

          std::vector<std::unordered_map<std::wstring, std::wstring> > result;

          WmiQuery query;
          if (!query.getFromTable(serviceName, tableName, keys, result))
            return;

          if (result.size() == 0)
            return;

          writer->writeStartElement("video");

          for (size_t i = 0; i < result.size(); ++i) {
            std::unordered_map<std::wstring, std::wstring>& row(result[i]);

            int width = QString::fromStdWString(row[L"CurrentHorizontalResolution"]).toInt();
            int height = QString::fromStdWString(row[L"CurrentVerticalResolution"]).toInt();
            if (width == 0 || height == 0)
              continue;

            writer->writeStartElement("card");

            writer->writeTextElement("AdapterCompatibility", QString::fromStdWString(row[L"AdapterCompatibility"]));
            writer->writeTextElement("Caption", QString::fromStdWString(row[L"Caption"]));
            writer->writeTextElement("VideoProcessor", QString::fromStdWString(row[L"VideoProcessor"]));
            writer->writeTextElement("AdapterRAM", QString::fromStdWString(row[L"AdapterRAM"]));

            writer->writeEndElement();
          }

          writer->writeEndElement();
        }

      }
    }
  }
}