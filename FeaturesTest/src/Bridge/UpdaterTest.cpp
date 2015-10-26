#include <gmock/gmock.h>
#include <gtest/gtest.h>
 
#include <Host/Updater.h>
#include <Host/Bridge/UpdateManagerBridge.h>

#include <QtTest/QSignalSpy>
#include <QtCore/QEventLoop>
#include <QtCore/QTimer>

using ::testing::Return;

using GameNet::Host::Bridge::UpdateManagerBridge;

class UpdaterMock : public GameNet::Host::Updater
{
public:
  using GameNet::Host::Updater::updateState;
  MOCK_METHOD0(updateState, int());

  // slots for signals
  MOCK_METHOD2(onDownloadUpdateProgress, void(quint64,quint64));
  MOCK_METHOD1(onAllCompleted, void(bool));
  MOCK_METHOD1(onUpdateError, void(int));
  MOCK_METHOD1(onUpdateStateChanged, void(int));
  MOCK_METHOD0(onNoUpdatesFound, void());
  MOCK_METHOD0(onUpdatesFound, void());
  MOCK_METHOD1(onDownloadRetryNumber, void(int));
};

class UpdaterBridgeTest : public ::testing::Test 
{
public:
  UpdaterBridgeTest()
  {
    bridge.setUpdateManager(&updaterMock);

    QObject::connect(&bridge, &UpdateManagerBridge::downloadUpdateProgress,
                     &updaterMock, &UpdaterMock::onDownloadUpdateProgress);

    QObject::connect(&bridge, &UpdateManagerBridge::allCompleted,
      &updaterMock, &UpdaterMock::onAllCompleted);

    QObject::connect(&bridge, &UpdateManagerBridge::updateError,
      &updaterMock, &UpdaterMock::onUpdateError);

    QObject::connect(&bridge, &UpdateManagerBridge::updateStateChanged,
      &updaterMock, &UpdaterMock::onUpdateStateChanged);

    QObject::connect(&bridge, &UpdateManagerBridge::updatesFound,
      &updaterMock, &UpdaterMock::onUpdatesFound);

    QObject::connect(&bridge, &UpdateManagerBridge::noUpdatesFound,
      &updaterMock, &UpdaterMock::onNoUpdatesFound);

    QObject::connect(&bridge, &UpdateManagerBridge::downloadRetryNumber,
      &updaterMock, &UpdaterMock::onDownloadRetryNumber);
  }

  UpdateManagerBridge bridge;
  UpdaterMock updaterMock;
};

TEST_F(UpdaterBridgeTest, downloadUpdateProgress)
{
  EXPECT_CALL(updaterMock, onDownloadUpdateProgress(100500, 500100)).Times(1);
  updaterMock.downloadUpdateProgress(100500, 500100);
}

TEST_F(UpdaterBridgeTest, allCompleted)
{
  EXPECT_CALL(updaterMock, onAllCompleted(true)).Times(1);
  updaterMock.allCompleted(true);
}

TEST_F(UpdaterBridgeTest, updateError)
{
  EXPECT_CALL(updaterMock, onUpdateError(1)).Times(1);
  updaterMock.updateError(1);
}

TEST_F(UpdaterBridgeTest, updateStateChanged)
{
  EXPECT_CALL(updaterMock, onUpdateStateChanged(1)).Times(1);
  updaterMock.updateStateChanged(1);
}

TEST_F(UpdaterBridgeTest, onNoUpdatesFound)
{
  EXPECT_CALL(updaterMock, onNoUpdatesFound()).Times(1);
  updaterMock.noUpdatesFound();
}

TEST_F(UpdaterBridgeTest, updatesFound)
{
  EXPECT_CALL(updaterMock, onUpdatesFound()).Times(1);
  updaterMock.updatesFound();
}

TEST_F(UpdaterBridgeTest, downloadRetryNumber)
{
  EXPECT_CALL(updaterMock, onDownloadRetryNumber(1)).Times(1);
  updaterMock.downloadRetryNumber(1);
}

TEST_F(UpdaterBridgeTest, updateStateSlot) 
{
  EXPECT_CALL(updaterMock, updateState())
     .WillOnce(Return(1));

  ASSERT_EQ(1, bridge.updateState());
}