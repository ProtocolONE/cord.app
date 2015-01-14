#include <Features/Marketing/SystemInfo/SoftVendor/GameVendorBase.h>

namespace Features {
  namespace Marketing {
    namespace SystemInfo {
      namespace SoftVendor {

        GameVendorBase::GameVendorBase(const QString& name, QObject *parent /*= 0*/)
          : VendorBase("game", parent)
          , _name(name)
        {
        }

        GameVendorBase::~GameVendorBase()
        {
        }

        void GameVendorBase::writePayload(QXmlStreamWriter *writer)
        {
          writer->writeTextElement("name", this->_name);

          this->writeToPayload(writer);

          writer->writeStartElement("games");
          this->writeGames(writer);
          writer->writeEndElement();
        }

        void GameVendorBase::writeToPayload(QXmlStreamWriter *writer)
        {
        }

        void GameVendorBase::writeGame(const QString& id, const QString& name, QXmlStreamWriter *writer)
        {
          writer->writeStartElement("game");
          writer->writeTextElement("gameId", id);
          writer->writeTextElement("gameName", name);
          writer->writeEndElement();
        }

      }
    }
  }
}