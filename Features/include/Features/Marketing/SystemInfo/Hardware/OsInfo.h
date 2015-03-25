#pragma once

#include <QtCore/QObject>
#include <QtCore/QXmlStreamWriter>

namespace Features {
  namespace Marketing {
    namespace SystemInfo {
      namespace Hardware {

        class OsInfo : public QObject
        {
          Q_OBJECT
        public:
          explicit OsInfo(QObject *parent = 0);
          virtual ~OsInfo();

          void write(QXmlStreamWriter *writer);

        private:
          void writeWmiVersion(QXmlStreamWriter *writer);

          bool isWow64();
        };

      }
    }
  }
}
