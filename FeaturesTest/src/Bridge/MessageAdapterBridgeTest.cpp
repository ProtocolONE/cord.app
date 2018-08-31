#include <gtest/gtest.h>
#include <gmock/gmock.h>

#include <Host/Bridge/MessageAdapterBridge.h>
#include <Host/MessageAdapter.h>

using P1::Host::MessageAdapter;
using P1::Host::Bridge::MessageAdapterBridge;

class MessageAdapterMock : public MessageAdapter
{
public:
  MOCK_METHOD5(onMessageRequest, void(int, const QString&, const QString&, int, int));
  MOCK_METHOD2(messageRequestResult, void(int, int));
};

class MessageAdapterBridgeTest : public ::testing::Test 
{
public:
  MessageAdapterBridgeTest() 
  {
    bridge.setAdapter(&adapterMock);
    
    QObject::connect(&bridge, &MessageAdapterBridge::messageRequest,
      &adapterMock, &MessageAdapterMock::onMessageRequest);
  }

  MessageAdapterBridge bridge;
  MessageAdapterMock adapterMock;
};

TEST_F(MessageAdapterBridgeTest, messageRequestResult)
{
  EXPECT_CALL(adapterMock, messageRequestResult(123, 42)).Times(1);
  bridge.messageRequestResult(123, 42);
}

TEST_F(MessageAdapterBridgeTest, messageRequest)
{
  QString title("Warning. Cow can kill you.");
  QString text("Be aware if you came too close they definitely can kill you.");

  EXPECT_CALL(adapterMock, onMessageRequest(123, title, text, 42, 65)).Times(1);

  adapterMock.messageRequest(123, title, text, 42, 65);
}