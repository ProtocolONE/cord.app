#pragma once
#include <Features/Marketing/SystemInfo/SoftVendor/GameVendorBase.h>
#include <QtCore/QStringList>

namespace Features {
  namespace Marketing {
    namespace SystemInfo {
      namespace SoftVendor {

        class ZZima : public GameVendorBase
        {
          Q_OBJECT
        public:
          explicit ZZima(QObject *parent = 0);
          virtual ~ZZima();

          virtual void writeGames(QXmlStreamWriter *writer) override;

        private:
          QStringList getZZimaGames();

          QString clearGameName(const QString& torrentUrl);
        };

      }
    }
  }
}
