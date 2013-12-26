#include <Features/StopDownloadServiceWhileExecuteAnyGame.h>

#include <Core/Service>

#include <TestEventLoopFinisher.h>

#include <QtCore/QEventLoop>
#include <QtTest/QSignalSpy>
#include <gtest/gtest.h>

class StopDownloadServiceWhileExecuteAnyGameTest: public ::testing::Test
{
public:
  void SetUp()
  {
    qRegisterMetaType<GGS::GameDownloader::StartType>("GGS::GameDownloader::StartType");
    qRegisterMetaType<const GGS::Core::Service *>("const GGS::Core::Service *");

    downloadStopRequestSpy = new QSignalSpy(&_feature, SIGNAL(downloadStopRequest(const GGS::Core::Service *)));
    downloadStartRequestSpy = new QSignalSpy(&_feature, SIGNAL(downloadStartRequest(const GGS::Core::Service *, GGS::GameDownloader::StartType)));
    torrentSessionPauseRequestSpy = new QSignalSpy(&_feature, SIGNAL(torrentSessionPauseRequest()));
    torrentSessionResumeRequestSpy = new QSignalSpy(&_feature, SIGNAL(torrentSessionResumeRequest()));
  }

  QSignalSpy *downloadStopRequestSpy;
  QSignalSpy *downloadStartRequestSpy;
  QSignalSpy *torrentSessionPauseRequestSpy;
  QSignalSpy *torrentSessionResumeRequestSpy;

  Features::StopDownloadServiceWhileExecuteAnyGame _feature;
  GGS::Core::Service service1;
  GGS::Core::Service service2;
};

TEST_F(StopDownloadServiceWhileExecuteAnyGameTest, simple)
{
  QEventLoop loop;
  TestEventLoopFinisher killer(&loop, 5000);
  QObject::connect(&_feature, SIGNAL(torrentSessionPauseRequest()), &killer, SLOT(terminateLoop()));

  _feature.onServiceStartDownload(&service1, GGS::GameDownloader::Normal);
  _feature.onServiceStartDownload(&service2, GGS::GameDownloader::Force);
  _feature.onGameExecuted("fake");

  loop.exec();

  ASSERT_EQ(2, downloadStopRequestSpy->count());
  ASSERT_EQ(0, downloadStartRequestSpy->count());
  ASSERT_EQ(1, torrentSessionPauseRequestSpy->count());
  ASSERT_EQ(0, torrentSessionResumeRequestSpy->count());

  const GGS::Core::Service* s1 = downloadStopRequestSpy->at(0).at(0).value<const GGS::Core::Service*>();
  const GGS::Core::Service* s2 = downloadStopRequestSpy->at(1).at(0).value<const GGS::Core::Service*>();

  ASSERT_TRUE((&service1 == s1 && &service2 == s2) || (&service2 == s1 && &service1 == s2));

  QEventLoop loop1;
  TestEventLoopFinisher killer1(&loop1, 5000);
  QObject::connect(&_feature, SIGNAL(torrentSessionResumeRequest()), &killer1, SLOT(terminateLoop()));
  _feature.onGameFinished("fake");

  loop1.exec();

  ASSERT_EQ(2, downloadStopRequestSpy->count());
  ASSERT_EQ(2, downloadStartRequestSpy->count());
  ASSERT_EQ(1, torrentSessionPauseRequestSpy->count());
  ASSERT_EQ(1, torrentSessionResumeRequestSpy->count());

  const GGS::Core::Service* s3 = downloadStartRequestSpy->at(0).at(0).value<const GGS::Core::Service*>();
  const GGS::Core::Service* s4 = downloadStartRequestSpy->at(1).at(0).value<const GGS::Core::Service*>();
  ASSERT_TRUE((&service1 == s3 && &service2 == s4) || (&service2 == s3 && &service1 == s4));

}