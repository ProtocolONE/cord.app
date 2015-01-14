#pragma once

#include <QtCore/QObject>
#include <QtCore/QXmlStreamWriter>

namespace Features {
  namespace Marketing {
    namespace SystemInfo {
      namespace Hardware {

        class Ram : public QObject
        {
          Q_OBJECT
        public:
          explicit Ram(QObject *parent = 0);
          virtual ~Ram();

          void write(QXmlStreamWriter *writer);
        };

      }
    }
  }
}
