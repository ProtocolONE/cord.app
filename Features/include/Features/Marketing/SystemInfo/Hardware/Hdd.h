#pragma once

#include <QtCore/QObject>
#include <QtCore/QXmlStreamWriter>

namespace Features {
  namespace Marketing {
    namespace SystemInfo {
      namespace Hardware {

        class Hdd : public QObject
        {
          Q_OBJECT
        public:
          explicit Hdd(QObject *parent = 0);
          virtual ~Hdd();

          void write(QXmlStreamWriter *writer);
        };

      }
    }
  }
}
