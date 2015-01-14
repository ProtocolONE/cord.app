#pragma once
#include <Features/Marketing/SystemInfo/SoftVendor/GameVendorBase.h>
#include <QtCore/QStringList>

namespace Features {
  namespace Marketing {
    namespace SystemInfo {
      namespace SoftVendor {

        class Ubisoft : public GameVendorBase
        {
          Q_OBJECT
        public:
          explicit Ubisoft(QObject *parent = 0);
          virtual ~Ubisoft();

          virtual void writeGames(QXmlStreamWriter *writer) override;

        private:
          QStringList getUbisoftGameList();
        };

      }
    }
  }
}
