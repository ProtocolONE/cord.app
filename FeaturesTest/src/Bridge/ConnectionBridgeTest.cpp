#include <gmock/gmock.h>
#include <gtest/gtest.h>

#include <Host/Connection.h>
#include <Host/CredentialConverter.h>
#include <Host/Bridge/ConnectionBridge.h>

#include <RestApi/GameNetCredential.h>

#include <QtCore/QString>

using GameNet::Host::Connection;
using GameNet::Host::Bridge::ConnectionBridge;
using GameNet::Host::Bridge::Credential;
using GameNet::Host::Bridge::createGameNetCredential;

using P1::RestApi::GameNetCredential;

using ::testing::Return;

class ConnectionMock : public Connection
{
public:
  ConnectionMock() 
    : Connection(QDBusConnection("test"))
  {
  }

  MOCK_METHOD1(setApplicationName, void(const QString&));
  MOCK_METHOD1(setCredential, void(const GameNetCredential&));
  MOCK_METHOD1(setSecondCredential, void(const GameNetCredential&));
  MOCK_METHOD0(ping, void());
  MOCK_METHOD0(close, void());

  // mock slot for signals
  MOCK_METHOD1(onWrongCredential, void(const QString&));
  MOCK_METHOD0(onPong, void());
};

class ConnectionBridgeTest : public ::testing::Test 
{
public:
  ConnectionBridgeTest() 
  {
    bridge.setConnection(&connection);
    credential.userId = "123123qwe123";
    credential.appKey = "lksdflkqerqwelkqwemqwklem";
    credential.cookie = "wklermwklrm234mlkmrklqwr90ads";
    gnCredential = createGameNetCredential(credential);

    QObject::connect(&bridge, &ConnectionBridge::wrongCredential,
      &connection, &ConnectionMock::onWrongCredential);

    QObject::connect(&bridge, &ConnectionBridge::pong,
      &connection, &ConnectionMock::onPong);
  }

  ConnectionBridge bridge;
  ConnectionMock connection;

  Credential credential;
  GameNetCredential gnCredential;
};

TEST_F(ConnectionBridgeTest, setApplicationName)
{
  QString name("Application");
  EXPECT_CALL(connection, setApplicationName(name))
    .Times(1);

  bridge.setApplicationName(name);
}

TEST_F(ConnectionBridgeTest, setCredential)
{
  EXPECT_CALL(connection, setCredential(gnCredential))
    .Times(1);

  bridge.setCredential(credential);
}

TEST_F(ConnectionBridgeTest, setSecondCredential)
{
  EXPECT_CALL(connection, setSecondCredential(gnCredential))
    .Times(1);

  bridge.setSecondCredential(credential);
}

TEST_F(ConnectionBridgeTest, wrongCredential)
{
  QString userId("123123123123345");

  EXPECT_CALL(connection, onWrongCredential(userId))
    .Times(1);

  connection.wrongCredential(userId);
}

TEST_F(ConnectionBridgeTest, ping)
{
  EXPECT_CALL(connection, ping())
    .Times(1);

  bridge.ping();
}

TEST_F(ConnectionBridgeTest, pong)
{
  EXPECT_CALL(connection, onPong())
    .Times(1);

  connection.pong();
}

TEST_F(ConnectionBridgeTest, close)
{
  EXPECT_CALL(connection, close())
    .Times(1);

  bridge.close();
}

