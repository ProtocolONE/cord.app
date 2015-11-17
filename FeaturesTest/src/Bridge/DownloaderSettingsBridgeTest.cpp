#include <gtest/gtest.h>
#include <gmock/gmock.h>

#include <Host/DownloaderSettings.h>
#include <Host/Bridge/DownloaderSettingsBridge.h>

using ::testing::Return;

class DownloaderSettingsMock : public GameNet::Host::DownloaderSettings
{
public:
  MOCK_METHOD1(setListeningPort, void(int));
  MOCK_CONST_METHOD0(listeningPort, int());

  MOCK_METHOD1(setUploadRate, void(int));
  MOCK_CONST_METHOD0(uploadRate, int());

  MOCK_METHOD1(setDownloadRate, void(int));
  MOCK_CONST_METHOD0(downloadRate, int());

  MOCK_METHOD1(setMaxConnection, void(int));
  MOCK_CONST_METHOD0(maxConnection, int());

  MOCK_METHOD1(setSeedEnabled, void(bool));
  MOCK_CONST_METHOD0(seedEnabled, bool());

  MOCK_METHOD1(setTorrentProfile, void(int));
  MOCK_CONST_METHOD0(torrentProfile, int());
};

class DownloaderSettingsBridgeTest : public ::testing::Test 
{
public:
  DownloaderSettingsBridgeTest()
  {
    this->_downloaderSettingsBridge.setDownloaderSettings(&this->_downloaderSettingsMock);
  }
  
  GameNet::Host::Bridge::DownloaderSettingsBridge _downloaderSettingsBridge;
  DownloaderSettingsMock _downloaderSettingsMock;
};

TEST_F(DownloaderSettingsBridgeTest, ListeningPort)
{
  int port = 10000;

  EXPECT_CALL(_downloaderSettingsMock, setListeningPort(port)).Times(1);

  EXPECT_CALL(_downloaderSettingsMock, listeningPort())
    .WillOnce(Return(port));
  
  _downloaderSettingsBridge.setListeningPort(port);
  ASSERT_EQ(port, _downloaderSettingsBridge.listeningPort());
}

TEST_F(DownloaderSettingsBridgeTest, UploadRate)
{
  int rate = 2000;

  EXPECT_CALL(_downloaderSettingsMock, setUploadRate(rate)).Times(1);

  EXPECT_CALL(_downloaderSettingsMock, uploadRate())
    .WillOnce(Return(rate));

  _downloaderSettingsBridge.setUploadRate(rate);
  ASSERT_EQ(rate, _downloaderSettingsBridge.uploadRate());
}

TEST_F(DownloaderSettingsBridgeTest, DownloadRate)
{
  int rate = 2000;

  EXPECT_CALL(_downloaderSettingsMock, setDownloadRate(rate)).Times(1);

  EXPECT_CALL(_downloaderSettingsMock, downloadRate())
    .WillOnce(Return(rate));

  _downloaderSettingsBridge.setDownloadRate(rate);
  ASSERT_EQ(rate, _downloaderSettingsBridge.downloadRate());
}

TEST_F(DownloaderSettingsBridgeTest, MaxConnection)
{
  int connections = 2000;

  EXPECT_CALL(_downloaderSettingsMock, setMaxConnection(connections)).Times(1);

  EXPECT_CALL(_downloaderSettingsMock, maxConnection())
    .WillOnce(Return(connections));

  _downloaderSettingsBridge.setMaxConnection(connections);
  ASSERT_EQ(connections, _downloaderSettingsBridge.maxConnection());
}

TEST_F(DownloaderSettingsBridgeTest, DisableSeedEnabled)
{
  EXPECT_CALL(_downloaderSettingsMock, setSeedEnabled(false)).Times(1);

  EXPECT_CALL(_downloaderSettingsMock, seedEnabled())
    .WillOnce(Return(false));

  _downloaderSettingsBridge.setSeedEnabled(false);
  ASSERT_EQ(false, _downloaderSettingsBridge.seedEnabled());
}

TEST_F(DownloaderSettingsBridgeTest, EnableSeedEnabled)
{
  EXPECT_CALL(_downloaderSettingsMock, setSeedEnabled(true)).Times(1);

  EXPECT_CALL(_downloaderSettingsMock, seedEnabled())
    .WillOnce(Return(true));

  _downloaderSettingsBridge.setSeedEnabled(true);
  ASSERT_EQ(true, _downloaderSettingsBridge.seedEnabled());
}

TEST_F(DownloaderSettingsBridgeTest, TorrentProfile)
{
  int profile = 1;

  EXPECT_CALL(_downloaderSettingsMock, setTorrentProfile(profile)).Times(1);
  EXPECT_CALL(_downloaderSettingsMock, torrentProfile()).WillOnce(Return(profile));

  _downloaderSettingsBridge.setTorrentProfile(profile);
  ASSERT_EQ(profile, _downloaderSettingsBridge.torrentProfile());
}