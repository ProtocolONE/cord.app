#include <Features/Marketing/SystemInfo/Hardware/Hdd.h>

#include <vector>

#include <Windows.h>

namespace Features {
  namespace Marketing {
    namespace SystemInfo {
      namespace Hardware {

        Hdd::Hdd(QObject *parent /*= 0*/)
          : QObject(parent)
        {
        }

        Hdd::~Hdd()
        {
        }

        void Hdd::write(QXmlStreamWriter *writer)
        {
          writer->writeStartElement("hddList");

          DWORD size = GetLogicalDriveStringsW(0, NULL);
          std::vector<wchar_t> disks(size + 1, L'\0');
          GetLogicalDriveStringsW(size, disks.data());

          for (unsigned int i = 0; i < size; i += 4) {
            const wchar_t *currentDisk = disks.data() + i;
            if (GetDriveTypeW(currentDisk) != DRIVE_FIXED)
              continue;

            ULARGE_INTEGER freeBytes = {0};
            ULARGE_INTEGER totalBytes = {0};

            if (!GetDiskFreeSpaceEx(currentDisk, &freeBytes, &totalBytes, NULL))
              continue;

            writer->writeStartElement("hdd");
            writer->writeTextElement("name", QString::fromWCharArray(currentDisk));
            writer->writeTextElement("free", QString::number(freeBytes.QuadPart));
            writer->writeTextElement("total", QString::number(totalBytes.QuadPart));
            writer->writeEndElement();
          }

          writer->writeEndElement();
        }

      }
    }
  }
}