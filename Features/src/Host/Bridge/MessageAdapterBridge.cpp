#include <Host/Bridge/MessageAdapterBridge.h>

#include <Host/MessageAdapter.h>

namespace GameNet {
  namespace Host {
    namespace Bridge {

      MessageAdapterBridge::MessageAdapterBridge(QObject *parent /*= 0*/)
        : QObject(parent)
        , _adapter(nullptr)
      {

      }

      MessageAdapterBridge::~MessageAdapterBridge()
      {
      }

      void MessageAdapterBridge::setAdapter(MessageAdapter *value)
      {
        Q_ASSERT(value);
        this->_adapter = value;
        
        QObject::connect(value, &MessageAdapter::messageRequest, 
          this, &MessageAdapterBridge::messageRequest);
      }

      void MessageAdapterBridge::messageRequestResult(int messageId, int buttonId)
      {
        Q_ASSERT(this->_adapter);
        this->_adapter->messageRequestResult(messageId, buttonId);
      }

    }
  }
}