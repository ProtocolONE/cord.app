#pragma once
#include <Features/Marketing/SystemInfo/SoftVendor/GameVendorBase.h>

#include <QtCore/QString>
#include <QtCore/QStringList>

namespace Features {
  namespace Marketing {
    namespace SystemInfo {
      namespace SoftVendor {
    
        class Steam : public GameVendorBase
        {
          Q_OBJECT
        public:
          explicit Steam(QObject *parent = 0);
          virtual ~Steam();

          virtual void writeToPayload(QXmlStreamWriter *writer) override;
          virtual void writeGames(QXmlStreamWriter *writer) override;

        private:
          QString getSteamRootDirectory();
          QStringList getSteamIdList();
          QStringList getSteamGameList();
        };

      }
    }
  }
}

