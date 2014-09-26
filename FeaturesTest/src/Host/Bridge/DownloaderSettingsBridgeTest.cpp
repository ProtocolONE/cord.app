#include <gtest/gtest.h>

#include <Core\Service.h>
#include <Fixtures/GameDownloadServiceFixture.h>
#include <Host/Bridge/DownloaderSettingsBridge.h>
#include <Host/DownloaderSettings.h>

class DownloaderSettingsBridgeTest : public ::testing::Test 
{
public:
  void SetUp()
  {
    this->_service.setId("300012010000000000");
    this->_downloadServiceFixture.setService(&this->_service);
    this->_downloaderSettings.setDownloader(&this->_downloadServiceFixture);
    this->_downloaderSettingsBridge.setDownloaderSettings(&this->_downloaderSettings);

    //this->_downloadBridge.setDownloader(&this->_downloadServiceFixture);
    //this->_downloadBridge.setServiceLoader(&this->_serviceLoaderFixture);
  }
  
  GameNet::Host::DownloaderSettings _downloaderSettings;
  GGS::Core::Service _service;
  GameDownloadServiceFixture _downloadServiceFixture;
  GameNet::Host::Bridge::DownloaderSettingsBridge _downloaderSettingsBridge;
};

TEST_F(DownloaderSettingsBridgeTest, ListeningPort)
{
  _downloaderSettingsBridge.setListeningPort(1000);
  EXPECT_EQ(1000, _downloaderSettingsBridge.listeningPort());
}
//
//TEST_F(DownloaderSettingsBridgeTest, UploadRate)
//{
//  _downloaderSettingsBridge.setUploadRate(1000000);
//  EXPECT_EQ(1000000, _downloaderSettingsBridge.uploadRate());
//}
//
//TEST_F(DownloaderSettingsBridgeTest, DownloadRate)
//{
//  _downloaderSettingsBridge.setDownloadRate(1000000);
//  EXPECT_EQ(1000000, _downloaderSettingsBridge.downloadRate());
//}
//
//TEST_F(DownloaderSettingsBridgeTest, MaxConnection)
//{
//  _downloaderSettingsBridge.setMaxConnection(100);
//  EXPECT_EQ(100, _downloaderSettingsBridge.maxConnection());
//}
//
//TEST_F(DownloaderSettingsBridgeTest, SeedEnabled)
//{
//  _downloaderSettingsBridge.setSeedEnabled(true);
//  EXPECT_TRUE(_downloaderSettingsBridge.seedEnabled());
//
//  _downloaderSettingsBridge.setSeedEnabled(false);
//  EXPECT_FALSE(_downloaderSettingsBridge.seedEnabled());
//}
//
//
