#pragma once
#include <Features/Marketing/SystemInfo/SoftVendor/GameVendorBase.h>

namespace Features {
  namespace Marketing {
    namespace SystemInfo {
      namespace SoftVendor {

        class Origin : public GameVendorBase
        {
          Q_OBJECT
        public:
          explicit Origin(QObject *parent = 0);
          virtual ~Origin();

          virtual void writeGames(QXmlStreamWriter *writer) override;
        };

      }
    }
  }
}
