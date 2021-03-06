#include <gtest/gtest.h>
#include <gmock/gmock.h>

#include <LibtorrentWrapper/EventArgs/ProgressEventArgs.h>
#include <Host/Bridge/DownloaderBridge.h>
#include <Host/Proxy/DownloaderProxy.h>
#include <Host/ServiceProcess/ServiceLoader.h>
#include <Host/Application.h>
#include <Core/Service.h>

using namespace P1::Host::Bridge;
using P1::Host::Proxy::DownloaderProxy;
using P1::Host::Application;

using ::testing::Return;

void constructFakeProgressEventArgs(P1::Libtorrent::EventArgs::ProgressEventArgs &args)
{
  args.setId("300012010000000000");
  args.setStatus(P1::Libtorrent::EventArgs::ProgressEventArgs::Downloading);
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

class ServiceLoaderFixture : public P1::Host::ServiceProcess::ServiceLoader
{
public:
  explicit ServiceLoaderFixture() {};
  ~ServiceLoaderFixture(){};

  void setService(P1::Core::Service *service)
  {
    this->_service = service;
  }

  virtual P1::Core::Service *getService(const QString& serviceId)
  {
    return this->_service;
  }

public:
  P1::Core::Service *_service;
};

class DownloadBridgeTestApplicationMock : public Application
{
public:
  MOCK_METHOD1(credential, P1::RestApi::ProtocolOneCredential(const QString&));
};

class GameDownloadServiceMock : public DownloaderProxy
{
public:
  MOCK_METHOD1(isInProgress, bool(const P1::Core::Service *));
  MOCK_METHOD0(isAnyServiceInProgress, bool());
  MOCK_METHOD1(isInstalled, bool(const QString&));
  MOCK_METHOD2(start, void(const P1::Core::Service *, P1::GameDownloader::StartType));
  MOCK_METHOD1(stop, void(const P1::Core::Service *));
  MOCK_METHOD0(pauseSession, void());
  MOCK_METHOD0(resumeSession, void());

  //  fake slots 
  MOCK_METHOD2(onStarted, void(const QString&, int));
  MOCK_METHOD1(onFinished, void(const QString&));
  MOCK_METHOD1(onStopped, void(const QString&));
  MOCK_METHOD1(onStopping, void(const QString&));
  MOCK_METHOD1(onFailed, void(const QString&));
  MOCK_METHOD1(onServiceInstalled, void(const QString&));
  MOCK_METHOD1(onServiceUninstalled, void(const QString&));
  MOCK_METHOD1(onServiceUpdated, void(const QString&));
  MOCK_METHOD2(onStatusMessageChanged, void(const QString&, const QString&));
  MOCK_METHOD2(onTotalProgressChanged, void(const QString&, int));
  MOCK_METHOD3(onDownloadProgressChanged, void(const QString&, int, const P1::Host::Bridge::DownloadProgressArgs&));

  MOCK_METHOD1(onAccessRequired, void(const QString&));
};

class DownloadBridgeTest : public ::testing::Test 
{
public:
  DownloadBridgeTest()
  {
    qRegisterMetaType<const P1::Core::Service *>("const P1::Core::Service *");
    qRegisterMetaType<P1::GameDownloader::StartType>("P1::GameDownloader::StartType");
    qRegisterMetaType<P1::Host::Bridge::DownloadProgressArgs>("P1::Host::Bridge::DownloadProgressArgs");

    this->_service.setId("300012010000000000");
    this->_serviceLoaderFixture.setService(&this->_service);
    this->_downloadBridge.setDownloader(&this->_downloadServiceFixture);
    this->_downloadBridge.setServiceLoader(&this->_serviceLoaderFixture);

    QObject::connect(&this->_downloadBridge, &DownloaderBridge::started, 
      &this->_downloadServiceFixture, &GameDownloadServiceMock::onStarted);

    QObject::connect(&this->_downloadBridge, &DownloaderBridge::finished, 
      &this->_downloadServiceFixture, &GameDownloadServiceMock::onFinished);

    QObject::connect(&this->_downloadBridge, &DownloaderBridge::stopped, 
      &this->_downloadServiceFixture, &GameDownloadServiceMock::onStopped);

    QObject::connect(&this->_downloadBridge, &DownloaderBridge::stopping, 
      &this->_downloadServiceFixture, &GameDownloadServiceMock::onStopping);

    QObject::connect(&this->_downloadBridge, &DownloaderBridge::failed, 
      &this->_downloadServiceFixture, &GameDownloadServiceMock::onFailed);

    QObject::connect(&this->_downloadBridge, &DownloaderBridge::serviceInstalled, 
      &this->_downloadServiceFixture, &GameDownloadServiceMock::onServiceInstalled);

    QObject::connect(&this->_downloadBridge, &DownloaderBridge::serviceUninstalled, 
      &this->_downloadServiceFixture, &GameDownloadServiceMock::onServiceUninstalled);

    QObject::connect(&this->_downloadBridge, &DownloaderBridge::serviceUpdated, 
      &this->_downloadServiceFixture, &GameDownloadServiceMock::onServiceUpdated);

    QObject::connect(&this->_downloadBridge, &DownloaderBridge::statusMessageChanged, 
      &this->_downloadServiceFixture, &GameDownloadServiceMock::onStatusMessageChanged);

    QObject::connect(&this->_downloadBridge, &DownloaderBridge::totalProgress, 
      &this->_downloadServiceFixture, &GameDownloadServiceMock::onTotalProgressChanged);

    QObject::connect(&this->_downloadBridge, &DownloaderBridge::downloadProgress, 
      &this->_downloadServiceFixture, &GameDownloadServiceMock::onDownloadProgressChanged);

    QObject::connect(&this->_downloadBridge, &DownloaderBridge::accessRequired,
      &this->_downloadServiceFixture, &GameDownloadServiceMock::onAccessRequired);
  }
    
  P1::Core::Service _service;
  DownloaderBridge _downloadBridge;
  GameDownloadServiceMock _downloadServiceFixture;
  ServiceLoaderFixture _serviceLoaderFixture;
  DownloadBridgeTestApplicationMock _application;
};

TEST_F(DownloadBridgeTest, EmitStartedSignal)
{
  P1::GameDownloader::StartType startType = P1::GameDownloader::Normal;

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

TEST_F(DownloadBridgeTest, EmitServiceUninstalledSignal)
{
  EXPECT_CALL(_downloadServiceFixture, onServiceUninstalled(_service.id())).Times(1);
  _downloadServiceFixture.serviceUninstalled(&_service);
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
  P1::Libtorrent::EventArgs::ProgressEventArgs libtorrentArgs;
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

TEST_F(DownloadBridgeTest, isInProgress)
{
  EXPECT_CALL(_downloadServiceFixture, isInProgress(&_service))
    .WillOnce(Return(true));

  ASSERT_TRUE(_downloadBridge.isInProgress(_service.id()));
}

TEST_F(DownloadBridgeTest, isAnyServiceInProgress)
{
  EXPECT_CALL(_downloadServiceFixture, isAnyServiceInProgress())
    .WillOnce(Return(true));

  ASSERT_TRUE(_downloadBridge.isAnyServiceInProgress());
}

TEST_F(DownloadBridgeTest, isInstalled)
{
  EXPECT_CALL(_downloadServiceFixture, isInstalled(_service.id()))
    .WillOnce(Return(true));

  ASSERT_TRUE(_downloadBridge.isInstalled(_service.id()));
}

TEST_F(DownloadBridgeTest, start)
{
  P1::GameDownloader::StartType type = P1::GameDownloader::StartType::Shadow;
  EXPECT_CALL(_downloadServiceFixture, start(&_service, type)).Times(1);

  _downloadBridge.start(_service.id(), static_cast<int>(type));
}

TEST_F(DownloadBridgeTest, stop)
{
  EXPECT_CALL(_downloadServiceFixture, stop(&_service)).Times(1);

  _downloadBridge.stop(_service.id());
}

TEST_F(DownloadBridgeTest, pauseSession)
{
  EXPECT_CALL(_downloadServiceFixture, pauseSession()).Times(1);

  _downloadBridge.pauseSession();
}

TEST_F(DownloadBridgeTest, resumeSession)
{
  EXPECT_CALL(_downloadServiceFixture, resumeSession()).Times(1);

  _downloadBridge.resumeSession();
}

TEST_F(DownloadBridgeTest, EmitAccessRequiredSignal)
{
  EXPECT_CALL(_downloadServiceFixture, onAccessRequired(_service.id())).Times(1);
  _downloadServiceFixture.accessRequired(&_service);
}
