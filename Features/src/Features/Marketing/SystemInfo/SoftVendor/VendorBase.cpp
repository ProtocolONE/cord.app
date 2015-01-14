#include <Features/Marketing/SystemInfo/SoftVendor/VendorBase.h>

namespace Features {
  namespace Marketing {
    namespace SystemInfo {
      namespace SoftVendor {

        VendorBase::VendorBase(const QString& type, QObject *parent /*= 0*/)
          : QObject(parent)
          , _type(type)
        {
        }

        VendorBase::~VendorBase()
        {
        }

        void VendorBase::write(QXmlStreamWriter *writer)
        {
          writer->writeStartElement("vendor");

          writer->writeTextElement("type", this->_type);

          this->writePayload(writer);

          writer->writeEndElement();
        }

      }
    }
  }
}