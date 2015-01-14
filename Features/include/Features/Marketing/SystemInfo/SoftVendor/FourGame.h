#pragma once
#include <Features/Marketing/SystemInfo/SoftVendor/GameVendorBase.h>
#include <QtCore/QStringList>

namespace Features {
  namespace Marketing {
    namespace SystemInfo {
      namespace SoftVendor {

        class FourGame : public GameVendorBase
        {
          Q_OBJECT
        public:
          explicit FourGame(QObject *parent = 0);
          virtual ~FourGame();

          virtual void writeGames(QXmlStreamWriter *writer) override;
        };

      }
    }
  }
}
