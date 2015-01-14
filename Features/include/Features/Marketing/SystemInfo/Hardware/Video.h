#pragma once

#include <QtCore/QObject>
#include <QtCore/QXmlStreamWriter>

namespace Features {
  namespace Marketing {
    namespace SystemInfo {
      namespace Hardware {

        class Video : public QObject
        {
          Q_OBJECT
        public:
          explicit Video(QObject *parent = 0);
          virtual ~Video();

          void write(QXmlStreamWriter *writer);
        };

      }
    }
  }
}
