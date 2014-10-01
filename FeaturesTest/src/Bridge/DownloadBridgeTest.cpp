#include <gtest/gtest.h>
#include <gmock/gmock.h>

#include <LibtorrentWrapper/EventArgs/ProgressEventArgs>
#include <Host/Bridge/DownloaderBridge.h>
#include <Host/ServiceLoader.h>
#include <Core/Service.h>

using namespace GameNet::Host::Bridge;
using ::testing::Return;

void constructFakeProgressEventArgs(GGS::Libtorrent::EventArgs::ProgressEventArgs &args)
{
  args.setId("300012010000000000");
  args.setStatus(GGS::Libtorrent::EventArgs::ProgressEventArgs::Downloading);
  args.setProgress(0.50f);
  args.setDownloadRate(100000);
  args.setUploadRate(50000);
  args.setTotalWanted(1234567890);
  args.setTotalWantedDone(1234567890);
  args.setPayloadTotalDownload(1234567890);
  args.setPayloadDownloadRate(1000000);
  args.setPeerPayloadDownloadRate(999000);
  args.setDirectPayloadDownloadRate(1000);
  args.setPayloadUploadRate(5000);
  args.setTotalPayloadUpload(1234567890);
  args.setDirectTotalDownload(1234567890);
  args.setPeerTotalDownload(1234567890); 
}

class ServiceLoaderFixture : public GameNet::Host::ServiceLoader
{
public:
  explicit ServiceLoaderFixture() {};
  ~ServiceLoaderFixture(){};

  void setService(GGS::Core::Service *service)
  {
    this->_service = service;
  }

  virtual GGS::Core::Service *getService(const QString& serviceId)
  {
    return this->_service;
  }

public:
  GGS::Core::Service *_service;
};

class GameDownloadServiceMock : public GGS::GameDownloader::GameDownloadService
{
public:
  //  fake slots 
  MOCK_METHOD2(onStarted, void(const QString&, int));
  MOCK_METHOD1(onFinished, void(const QString&));
  MOCK_METHOD1(onStopped, void(const QString&));
  MOCK_METHOD1(onStopping, void(const QString&));
  MOCK_METHOD1(onFailed, void(const QString&));
  MOCK_METHOD1(onServiceInstalled, void(const QString&));
  MOCK_METHOD1(onServiceUpdated, void(const QString&));
  MOCK_METHOD2(onStatusMessageChanged, void(const QString&, const QString&));
  MOCK_METHOD2(onTotalProgressChanged, void(const QString&, int));
  MOCK_METHOD3(onDownloadProgressChanged, void(const QString&, int, const GameNet::Host::Bridge::DownloadProgressArgs&));
};

class DownloadBridgeTest : public ::testing::Test 
{
public:
  DownloadBridgeTest()
  {
    qRegisterMetaType<const GGS::Core::Service *>("const GGS::Core::Service *");
    qRegisterMetaType<GGS::GameDownloader::StartType>("GGS::GameDownloader::StartType");
    qRegisterMetaType<GameNet::Host::Bridge::DownloadProgressArgs>("GameNet::Host::Bridge::DownloadProgressArgs");

    this->_service.setId("300012010000000000");
    this->_serviceLoaderFixture.setService(&this->_service);
    this->_downloadBridge.setDownloader(&this->_downloadServiceFixture);
    this->_downloadBridge.setServiceLoader(&this->_serviceLoaderFixture);

    QObject::connect(&this->_downloadBridge, &DownloaderBridge::started, &this->_downloadServiceFixture, &GameDownloadServiceMock::onStarted);
    QObject::connect(&this->_downloadBridge, &DownloaderBridge::finished, &this->_downloadServiceFixture, &GameDownloadServiceMock::onFinished);
    QObject::connect(&this->_downloadBridge, &DownloaderBridge::stopped, &this->_downloadServiceFixture, &GameDownloadServiceMock::onStopped);
    QObject::connect(&this->_downloadBridge, &DownloaderBridge::stopping, &this->_downloadServiceFixture, &GameDownloadServiceMock::onStopping);
    QObject::connect(&this->_downloadBridge, &DownloaderBridge::failed, &this->_downloadServiceFixture, &GameDownloadServiceMock::onFailed);
    QObject::connect(&this->_downloadBridge, &DownloaderBridge::serviceInstalled, &this->_downloadServiceFixture, &GameDownloadServiceMock::onServiceInstalled);
    QObject::connect(&this->_downloadBridge, &DownloaderBridge::serviceUpdated, &this->_downloadServiceFixture, &GameDownloadServiceMock::onServiceUpdated);
    QObject::connect(&this->_downloadBridge, &DownloaderBridge::statusMessageChanged, &this->_downloadServiceFixture, &GameDownloadServiceMock::onStatusMessageChanged);
    QObject::connect(&this->_downloadBridge, &DownloaderBridge::totalProgress, &this->_downloadServiceFixture, &GameDownloadServiceMock::onTotalProgressChanged);
    QObject::connect(&this->_downloadBridge, &DownloaderBridge::downloadProgress, &this->_downloadServiceFixture, &GameDownloadServiceMock::onDownloadProgressChanged);
  }
    
  GGS::Core::Service _service;
  DownloaderBridge _downloadBridge;
  GameDownloadServiceMock _downloadServiceFixture;
  ServiceLoaderFixture _serviceLoaderFixture;
};


TEST_F(DownloadBridgeTest, EmitStartedSignal)
{
  GGS::GameDownloader::StartType startType = GGS::GameDownloader::Normal;

  EXPECT_CALL(_downloadServiceFixture, onStarted(_service.id(), startType)).Times(1);
  _downloadServiceFixture.started(&_service, startType);
}

TEST_F(DownloadBridgeTest, EmitFinishedSignal)
{
  EXPECT_CALL(_downloadServiceFixture, onFinished(_service.id())).Times(1);
  _downloadServiceFixture.finished(&_service);
}

TEST_F(DownloadBridgeTest, EmitStoppedSignal)
{
  EXPECT_CALL(_downloadServiceFixture, onStopped(_service.id())).Times(1);
  _downloadServiceFixture.stopped(&_service);
}

TEST_F(DownloadBridgeTest, EmitStoppingSignal)
{
  EXPECT_CALL(_downloadServiceFixture, onStopping(_service.id())).Times(1);
  _downloadServiceFixture.stopping(&_service);
}

TEST_F(DownloadBridgeTest, EmitFailedSignal)
{
  EXPECT_CALL(_downloadServiceFixture, onFailed(_service.id())).Times(1);
  _downloadServiceFixture.failed(&_service);
}

TEST_F(DownloadBridgeTest, EmitServiceInstalledSignal)
{
  EXPECT_CALL(_downloadServiceFixture, onServiceInstalled(_service.id())).Times(1);
  _downloadServiceFixture.serviceInstalled(&_service);
}

TEST_F(DownloadBridgeTest, EmitServiceUpdatedSignal)
{
  EXPECT_CALL(_downloadServiceFixture, onServiceUpdated(_service.id())).Times(1);
  _downloadServiceFixture.serviceUpdated(&_service);
}

TEST_F(DownloadBridgeTest, EmitStatusMessageChangedSignal)
{
  QString statusMsg = "-- Test status message --";

  EXPECT_CALL(_downloadServiceFixture, onStatusMessageChanged(_service.id(), statusMsg)).Times(1);
  _downloadServiceFixture.statusMessageChanged(&_service, statusMsg);
}

TEST_F(DownloadBridgeTest, EmitTotalProgressSignal)
{
  int progress = 80;

  EXPECT_CALL(_downloadServiceFixture, onTotalProgressChanged(_service.id(), progress)).Times(1);
  _downloadServiceFixture.totalProgressChanged(&_service, progress);
}

TEST_F(DownloadBridgeTest, EmitDownloadProgressSignal)
{
  int progress = 80;
  GGS::Libtorrent::EventArgs::ProgressEventArgs libtorrentArgs;
  constructFakeProgressEventArgs(libtorrentArgs);

  DownloadProgressArgs downloadArgs;
  downloadArgs.id = libtorrentArgs.id();
  downloadArgs.status = libtorrentArgs.status();
  downloadArgs.progress = libtorrentArgs.progress();

  downloadArgs.downloadRate = libtorrentArgs.downloadRate();
  downloadArgs.uploadRate = libtorrentArgs.uploadRate();

  downloadArgs.totalWanted = libtorrentArgs.totalWanted();
  downloadArgs.totalWantedDone = libtorrentArgs.totalWantedDone();

  downloadArgs.directTotalDownload = libtorrentArgs.directTotalDownload();
  downloadArgs.peerTotalDownload = libtorrentArgs.peerTotalDownload();

  downloadArgs.payloadTotalDownload = libtorrentArgs.payloadTotalDownload();
  downloadArgs.peerPayloadDownloadRate = libtorrentArgs.peerPayloadDownloadRate();
  downloadArgs.payloadDownloadRate = libtorrentArgs.payloadDownloadRate();

  downloadArgs.directPayloadDownloadRate = libtorrentArgs.directPayloadDownloadRate();
  downloadArgs.payloadUploadRate = libtorrentArgs.payloadUploadRate();
  downloadArgs.totalPayloadUpload = libtorrentArgs.totalPayloadUpload();

  EXPECT_CALL(_downloadServiceFixture, onDownloadProgressChanged(_service.id(), progress, downloadArgs)).Times(1);
  _downloadServiceFixture.downloadProgressChanged(&_service, progress, libtorrentArgs);
}