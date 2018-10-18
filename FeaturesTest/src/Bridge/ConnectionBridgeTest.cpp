#include <gmock/gmock.h>
#include <gtest/gtest.h>

#include <Host/Connection.h>
#include <Host/Bridge/ConnectionBridge.h>

#include <RestApi/ProtocolOneCredential.h>

#include <QtCore/QString>

using P1::Host::Connection;
using P1::Host::Bridge::ConnectionBridge;

using P1::RestApi::ProtocolOneCredential;

using ::testing::Return;

class ConnectionMock : public Connection
{
public:
  ConnectionMock() 
    : Connection(QDBusConnection("test"))
  {
  }

  MOCK_METHOD1(setApplicationName, void(const QString&));
  MOCK_METHOD1(setCredential, void(const ProtocolOneCredential&));
  
  MOCK_METHOD0(ping, void());
  MOCK_METHOD0(close, void());

  // mock slot for signals
  MOCK_METHOD2(mockOnAuthorizationError, void(const QString& accessToken, const QString& acccessTokenExpiredTime));
  MOCK_METHOD0(onPong, void());
};

class ConnectionBridgeTest : public ::testing::Test 
{
public:
  ConnectionBridgeTest() 
  {
    bridge.setConnection(&connection);

    QDateTime now = QDateTime::currentDateTime();
    now.addSecs(30);

    accessToken = "eyJ0eXAiOiJKV1QiLCJhbGciOiJSUzI1NiJ9.eyJpYXQiOjE1MzkxNTM0MTEsImV4cCI6MTUzOTE1NTI"
      "xMSwicm9sZXMiOlsiUk9MRV9VU0VSIl0sImlkIjoiNWJiZDlkZTMzNDg5ZDI5YzgzMGZkYjBkIn0.SKo"
      "EfLk7b4GshpXWXobtQeF00XVjWh1vQDe41hWVtcHGf_qSEG3WqcRFsgRdWsJ1vBTBSHjmoSupxSdNyHY"
      "6lP04wlnaFwr7w5ofBVeNPaP78bX6qnXvmBpfrvRzLX2IQBJLw89CAqfiCDJgTZzJ9Gu9K8Vwygjwdtj"
      "qEhq3ugEbaBslO_dHQ-6rXwk5CnjsSTsTleyTERCIathB_Zl4L0UUXHgjXZ3gE_4CdTmuCnHjFOjdGmL"
      "td1v80ZXWyuQifbwL32QPV4FniRXCrI7zEEXcBJAlGwF0gPYeOht3G2wgpYw4rD0dADYEX1J5gtAnKuj"
      "xdULdvR1S6Xnq0Rt9rKAkC0OC2Gz9RjWfY92Kw-NPDeFlXHBEaIm0Q6XOvX9ctRSb5S7K4_FbPFTlIBQ"
      "nlzipWt7pXUH6ObSJw0y_ZsmBfvbL0nCgqyZzKCAYOo2T88gjhbdl7hivICjcVNWD6R1cU5llwEC85t2"
      "AyqLpZPDdgpuevwDjXOwCDJLylrYxTK5oL_bi-2fKLzxRqNNhAYQ3Hs4CpAMML1PqmcxKXApN8U7d1jx"
      "gteL8l0XIEK47ZjoyD2kXL9fHCK0HmDeY5GWv1yRpVPExLxATcQiHyTZJQ51vdnWxn1zx9bVU8ZUCuJg"
      "GwbZYB1oVSXNWH7t5y_2Bh6_dfmxeg3ITUWZ0kCI";
    acccessTokenExpiredTime = QString::number(now.toMSecsSinceEpoch() / 1000);

    expectedCredential = ProtocolOneCredential(accessToken, acccessTokenExpiredTime);

    QObject::connect(&bridge, &ConnectionBridge::authorizationError,
      &connection, &ConnectionMock::mockOnAuthorizationError);

    QObject::connect(&bridge, &ConnectionBridge::pong,
      &connection, &ConnectionMock::onPong);
  }

  ConnectionBridge bridge;
  ConnectionMock connection;

  QString accessToken;
  QString acccessTokenExpiredTime;

  
  ProtocolOneCredential expectedCredential;
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
  EXPECT_CALL(connection, setCredential(expectedCredential))
    .Times(1);

  bridge.setCredential(accessToken, acccessTokenExpiredTime);
}

TEST_F(ConnectionBridgeTest, authorizationError)
{
  QString userId("123123123123345");

  EXPECT_CALL(connection, mockOnAuthorizationError(accessToken, acccessTokenExpiredTime))
    .Times(1);

  connection.authorizationError(expectedCredential);
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

