#include <gtest/gtest.h>

#include <QtCore/QList>
#include <QtCore/QVariant>

#include <QtTest/QSignalSpy>

#include <TestEventLoopFinisher.h>

#include <Core\Service.h>
#include <LibtorrentWrapper/EventArgs/ProgressEventArgs>

#include <Fixtures/GameDownloadServiceFixture.h>
#include <Fixtures/ServiceLoaderFixture.h>

#include <Host/Bridge/DownloaderBridge.h>

using namespace GameNet::Host::Bridge;

void constructFakeDownloadProgressArgs(GGS::Libtorrent::EventArgs::ProgressEventArgs &args)
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

class DownloadBridgeTest : public ::testing::Test 
{
public:
  void SetUp()
  {
    qRegisterMetaType<GameNet::Host::Bridge::DownloadProgressArgs>("GameNet::Host::Bridge::DownloadProgressArgs");

    this->_service.setId("300012010000000000");
    this->_downloadServiceFixture.setService(&this->_service);
    this->_downloadBridge.setDownloader(&this->_downloadServiceFixture);
    this->_downloadBridge.setServiceLoader(&this->_serviceLoaderFixture);
  }

  GGS::Core::Service _service;
  GameDownloadServiceFixture _downloadServiceFixture;
  ServiceLoaderFixture _serviceLoaderFixture;
  DownloaderBridge _downloadBridge;
};

TEST_F(DownloadBridgeTest, TestEmitStartedSignal)
{
  QEventLoop loop;
  TestEventLoopFinisher killer(&loop, 5000);
  
  QSignalSpy spy(&_downloadBridge, SIGNAL(started(const QString&, int)));
  QObject::connect(&_downloadBridge, SIGNAL(started(const QString&, int)), &killer, SLOT(terminateLoop()));

  _downloadServiceFixture.emitStarted(GGS::GameDownloader::Normal);

  loop.exec();

  ASSERT_EQ(1, spy.count());
}

TEST_F(DownloadBridgeTest, TestEmitFinishedSignal)
{
  QEventLoop loop;
  TestEventLoopFinisher killer(&loop, 5000);

  QSignalSpy spy(&_downloadBridge, SIGNAL(finished(const QString&)));
  QObject::connect(&_downloadBridge, SIGNAL(finished(const QString&)), &killer, SLOT(terminateLoop()));

  _downloadServiceFixture.emitFinished();

  loop.exec();

  ASSERT_EQ(1, spy.count());
}

TEST_F(DownloadBridgeTest, TestEmitStoppedSignal)
{
  QEventLoop loop;
  TestEventLoopFinisher killer(&loop, 5000);

  QSignalSpy spy(&_downloadBridge, SIGNAL(stopped(const QString&)));
  QObject::connect(&_downloadBridge, SIGNAL(stopped(const QString&)), &killer, SLOT(terminateLoop()));

  _downloadServiceFixture.emitStopped();

  loop.exec();

  ASSERT_EQ(1, spy.count());
}

TEST_F(DownloadBridgeTest, TestEmitStoppingSignal)
{
  QEventLoop loop;
  TestEventLoopFinisher killer(&loop, 5000);

  QSignalSpy spy(&_downloadBridge, SIGNAL(stopping(const QString&)));
  QObject::connect(&_downloadBridge, SIGNAL(stopping(const QString&)), &killer, SLOT(terminateLoop()));

  _downloadServiceFixture.emitStopping();

  loop.exec();

  ASSERT_EQ(1, spy.count());
}

TEST_F(DownloadBridgeTest, TestEmitFailedSignal)
{
  QEventLoop loop;
  TestEventLoopFinisher killer(&loop, 5000);

  QSignalSpy spy(&_downloadBridge, SIGNAL(failed(const QString&)));
  QObject::connect(&_downloadBridge, SIGNAL(failed(const QString&)), &killer, SLOT(terminateLoop()));

  _downloadServiceFixture.emitFailed();

  loop.exec();

  ASSERT_EQ(1, spy.count());
}

TEST_F(DownloadBridgeTest, TestEmitServiceInstalledSignal)
{
  QEventLoop loop;
  TestEventLoopFinisher killer(&loop, 5000);

  QSignalSpy spy(&_downloadBridge, SIGNAL(serviceInstalled(const QString&)));
  QObject::connect(&_downloadBridge, SIGNAL(serviceInstalled(const QString&)), &killer, SLOT(terminateLoop()));

  _downloadServiceFixture.emitServiceInstalled();

  loop.exec();

  ASSERT_EQ(1, spy.count());
}

TEST_F(DownloadBridgeTest, TestEmitServiceUpdatedSignal)
{
  QEventLoop loop;
  TestEventLoopFinisher killer(&loop, 5000);

  QSignalSpy spy(&_downloadBridge, SIGNAL(serviceUpdated(const QString&)));
  QObject::connect(&_downloadBridge, SIGNAL(serviceUpdated(const QString&)), &killer, SLOT(terminateLoop()));

  _downloadServiceFixture.emitServiceUpdated();

  loop.exec();

  ASSERT_EQ(1, spy.count());
}

TEST_F(DownloadBridgeTest, TestEmitStatusMessageChangedSignal)
{
  QEventLoop loop;
  TestEventLoopFinisher killer(&loop, 5000);

  QSignalSpy spy(&_downloadBridge, SIGNAL(statusMessageChanged(const QString&, const QString& )));
  QObject::connect(&_downloadBridge, SIGNAL(statusMessageChanged(const QString&, const QString& )), &killer, SLOT(terminateLoop()));

  _downloadServiceFixture.emitStatusMessageChanged("-- Test status message --");

  loop.exec();

  ASSERT_EQ(1, spy.count());
}

TEST_F(DownloadBridgeTest, TestEmitTotalProgressSignal)
{
  QEventLoop loop;
  TestEventLoopFinisher killer(&loop, 5000);

  QSignalSpy spy(&_downloadBridge, SIGNAL(totalProgress(const QString&, int)));
  QObject::connect(&_downloadBridge, SIGNAL(totalProgress(const QString&, int)), &killer, SLOT(terminateLoop()));

  _downloadServiceFixture.emitTotalProgressChanged(50);

  loop.exec();

  ASSERT_EQ(1, spy.count());
}

TEST_F(DownloadBridgeTest, TestEmitDownloadProgressSignal)
{
  QEventLoop loop;
  TestEventLoopFinisher killer(&loop, 5000);

  QSignalSpy spy(&_downloadBridge, SIGNAL(downloadProgress(const QString&, int, const GameNet::Host::Bridge::DownloadProgressArgs&)));
  QObject::connect(&_downloadBridge, SIGNAL(downloadProgress(const QString&, int, const GameNet::Host::Bridge::DownloadProgressArgs&)), &killer, SLOT(terminateLoop()));

  GGS::Libtorrent::EventArgs::ProgressEventArgs args;
  constructFakeDownloadProgressArgs(args);

  _downloadServiceFixture.emitDownloadProgressChanged(50, args);

  loop.exec();

  ASSERT_EQ(1, spy.count());
}