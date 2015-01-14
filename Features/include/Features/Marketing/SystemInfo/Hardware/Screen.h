#pragma once

#include <QtCore/QObject>
#include <QtCore/QXmlStreamWriter>

namespace Features {
  namespace Marketing {
    namespace SystemInfo {
      namespace Hardware {

        class Screen : public QObject
        {
          Q_OBJECT
        public:
          explicit Screen(QObject *parent = 0);
          virtual ~Screen();

          void write(QXmlStreamWriter *writer);
        };

      }
    }
  }
}
