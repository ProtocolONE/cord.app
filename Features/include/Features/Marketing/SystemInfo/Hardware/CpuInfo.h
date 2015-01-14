#pragma once

#include <QtCore/QObject>
#include <QtCore/QXmlStreamWriter>

namespace Features {
  namespace Marketing {
    namespace SystemInfo {
      namespace Hardware {

        class CpuInfo : public QObject
        {
          Q_OBJECT
        public:
          explicit CpuInfo(QObject *parent = 0);
          virtual ~CpuInfo();

          void write(QXmlStreamWriter *writer);
        };

      }
    }
  }
}