#pragma once
#include <QtCore/QObject>
#include <QtCore/QXmlStreamWriter>
#include <Features/Marketing/SystemInfo/SoftVendor/VendorBase.h>

namespace Features {
  namespace Marketing {
    namespace SystemInfo {
      namespace SoftVendor {

        class GameVendorBase : public VendorBase
        {
          Q_OBJECT
        public:
          explicit GameVendorBase(const QString& name, QObject *parent = 0);
          virtual ~GameVendorBase();

          virtual void writeToPayload(QXmlStreamWriter *writer);
          virtual void writeGames(QXmlStreamWriter *writer) = 0;

          virtual void writeGame(const QString& id, const QString& name, QXmlStreamWriter *writer);

        private:
          QString _name;
          virtual void writePayload(QXmlStreamWriter *writer) override;

        };

      }
    }
  }
}