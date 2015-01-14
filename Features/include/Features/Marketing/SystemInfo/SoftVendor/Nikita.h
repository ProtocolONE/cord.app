#pragma once
#include <Features/Marketing/SystemInfo/SoftVendor/GameVendorBase.h>
#include <QtCore/QStringList>

namespace Features {
  namespace Marketing {
    namespace SystemInfo {
      namespace SoftVendor {

        class Nikita : public GameVendorBase
        {
          Q_OBJECT
        public:
          explicit Nikita(QObject *parent = 0);
          virtual ~Nikita();

          virtual void writeGames(QXmlStreamWriter *writer) override;
        };

      }
    }
  }
}
