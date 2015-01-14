#pragma once
#include <QtCore/QObject>
#include <QtCore/QXmlStreamWriter>

namespace Features {
  namespace Marketing {
    namespace SystemInfo {
      namespace SoftVendor {

        class VendorBase : public QObject
        {
          Q_OBJECT
        public:
          explicit VendorBase(const QString& type, QObject *parent = 0);
          virtual ~VendorBase();

          void write(QXmlStreamWriter *writer);
          virtual void writePayload(QXmlStreamWriter *writer) = 0;

        private:
          QString _type;
        };

      }
    }
  }
}