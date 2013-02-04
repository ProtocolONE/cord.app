#include <Features/GameDownloader/ExternalIpAddress.h>

#include <TestEventLoopFinisher.h>

#include <QtCore/QEventLoop>
#include <QtTest/QSignalSpy>

#include <gtest/gtest.h>

class ExternalIpAddressTest : public ::testing::Test 
{
public:
  void SetUp()
  {
    resultSpy = new QSignalSpy(&this->_feature, SIGNAL(result(QString)));
  }

  Features::GameDownloader::ExternalIpAddress _feature;
  QSignalSpy *resultSpy;
};

TEST_F(ExternalIpAddressTest, good)
{
  QEventLoop loop;
  TestEventLoopFinisher killer(&loop, 5000);
  QObject::connect(&_feature, SIGNAL(result(QString)), &killer, SLOT(terminateLoop()));

  _feature.execute();
  loop.exec();

  ASSERT_FALSE(killer.isKilledByTimeout());
  ASSERT_EQ(1, resultSpy->count());
  QString ip = resultSpy->at(0).at(0).value<QString>();
  ASSERT_FALSE(ip.isEmpty());
}

TEST_F(ExternalIpAddressTest, fail)
{
  QEventLoop loop;
  TestEventLoopFinisher killer(&loop, 5000);
  QObject::connect(&_feature, SIGNAL(result(QString)), &killer, SLOT(terminateLoop()));
  _feature.setUrl("http://www.failServiceUrl.com");

  _feature.execute();
  loop.exec();

  ASSERT_FALSE(killer.isKilledByTimeout());
  ASSERT_EQ(1, resultSpy->count());
  QString ip = resultSpy->at(0).at(0).value<QString>();
  ASSERT_TRUE(ip.isEmpty());
}