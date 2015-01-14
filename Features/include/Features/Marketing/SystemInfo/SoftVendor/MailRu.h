#pragma once
#include <Features/Marketing/SystemInfo/SoftVendor/GameVendorBase.h>
#include <QtCore/QStringList>

namespace Features {
  namespace Marketing {
    namespace SystemInfo {
      namespace SoftVendor {

        class MailRu : public GameVendorBase
        {
          Q_OBJECT
        public:
          explicit MailRu(QObject *parent = 0);
          virtual ~MailRu();

          virtual void writeGames(QXmlStreamWriter *writer) override;
        };

      }
    }
  }
}
