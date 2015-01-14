#pragma once
#include <Features/Marketing/SystemInfo/SoftVendor/VendorBase.h>

namespace Features {
  namespace Marketing {
    namespace SystemInfo {
      namespace SoftVendor {

        class Antivirus : public VendorBase
        {
          Q_OBJECT
        public:
          explicit Antivirus(QObject *parent = 0);
          virtual ~Antivirus();

          virtual void writePayload(QXmlStreamWriter *writer) override;

        private:
          bool queryAntivirus(QString& name);
        };

      }
    }
  }
}