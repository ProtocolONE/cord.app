#include <gmock/gmock.h>
#include <gtest/gtest.h>

#include <Host/ServiceDescription.h>

using GameNet::Host::ServiceDescription;
using GameNet::Host::DownloadHookDescription;
using GameNet::Host::ExecutorHookDescription;

class ServiceDescriptionTest : public ::testing::Test
{
public:
  ServiceDescription description;
};

TEST_F(ServiceDescriptionTest, id)
{
  QString id1 = "123qwe123";
  QString id2 = "kljsfnskldjfnq123";
  description.setId(id1);
  ASSERT_EQ(id1, description.id());

  description.setId(id2);
  ASSERT_EQ(id2, description.id());
}

TEST_F(ServiceDescriptionTest, gameId)
{
  QString gameId1 = "123";
  QString gameId2 = "731";
  description.setGameId(gameId1);
  ASSERT_EQ(gameId1, description.gameId());

  description.setGameId(gameId2);
  ASSERT_EQ(gameId2, description.gameId());
}

TEST_F(ServiceDescriptionTest, name)
{
  QString name1 = "Combat Arms";
  QString name2 = "War Inc";
  description.setName(name1);
  ASSERT_EQ(name1, description.name());

  description.setName(name2);
  ASSERT_EQ(name2, description.name());
}

TEST_F(ServiceDescriptionTest, torrentUrl)
{
  QString torrentUrl1 = "http://gamenet.dev/game.torrent";
  QString torrentUrl2 = "http://gamenet.dev/somOtherGame.torrent";
  description.setTorrentUrl(torrentUrl1);
  ASSERT_EQ(torrentUrl1, description.torrentUrl());

  description.setTorrentUrl(torrentUrl2);
  ASSERT_EQ(torrentUrl2, description.torrentUrl());
}

TEST_F(ServiceDescriptionTest, isDownloadable)
{
  bool isDownloadable1 = true;
  bool isDownloadable2 = false;
  description.setIsDownloadable(isDownloadable1);
  ASSERT_EQ(isDownloadable1, description.isDownloadable());

  description.setIsDownloadable(isDownloadable2);
  ASSERT_EQ(isDownloadable2, description.isDownloadable());
}

TEST_F(ServiceDescriptionTest, hasDownloadPath)
{
  bool hasDownloadPath1 = true;
  bool hasDownloadPath2 = false;
  description.setHasDownloadPath(hasDownloadPath1);
  ASSERT_EQ(hasDownloadPath1, description.hasDownloadPath());

  description.setHasDownloadPath(hasDownloadPath2);
  ASSERT_EQ(hasDownloadPath2, description.hasDownloadPath());
}

TEST_F(ServiceDescriptionTest, extractorType)
{
  QString extractorType1 = "{2E69975E-4D7B-4D6F-A894-02F439F1C1AB}";
  QString extractorType2 = "{9DADD512-7EFF-4BC4-AF85-AFF5C2589EB2}";
  description.setExtractorType(extractorType1);
  ASSERT_EQ(extractorType1, description.extractorType());

  description.setExtractorType(extractorType2);
  ASSERT_EQ(extractorType2, description.extractorType());
}

TEST_F(ServiceDescriptionTest, executeUrl)
{
  QString executeUrl1 = "exe:C:/Games/calc.exe?args=1+1&workingdir=C:/Games/";
  QString executeUrl2 = "http://playga.ru/play?fullscreen=1";
  description.setExecuteUrl(executeUrl1);
  ASSERT_EQ(executeUrl1, description.executeUrl());

  description.setExecuteUrl(executeUrl2);
  ASSERT_EQ(executeUrl2, description.executeUrl());
}

TEST_F(ServiceDescriptionTest, dependencyList)
{
  QString dependencyList1 = "dxwebsetup.exe,/Q";
  QString dependencyList2 = "vcredistr2005,/Q,vcredistr2010,/Q,dxwebsetup.exe,/Q";
  description.setDependencyList(dependencyList1);
  ASSERT_EQ(dependencyList1, description.dependencyList());

  description.setDependencyList(dependencyList2);
  ASSERT_EQ(dependencyList2, description.dependencyList());
}

TEST_F(ServiceDescriptionTest, gameSize)
{
  int gameSize1 = 1024;
  int gameSize2 = 42;
  description.setGameSize(gameSize1);
  ASSERT_EQ(gameSize1, description.gameSize());

  description.setGameSize(gameSize2);
  ASSERT_EQ(gameSize2, description.gameSize());
}

TEST_F(ServiceDescriptionTest, downloadHooks)
{
  QList<DownloadHookDescription> hooks;
  DownloadHookDescription hook1;
  hook1.first = "{01DA2407-49B8-4E52-99A5-56624AC6CD84}";
  hook1.second.first = 100;
  hook1.second.second = 42;

  DownloadHookDescription hook2;
  hook2.first = "{1562B943-0C47-4D27-AC36-83EAF5D6DB6F}";
  hook2.second.first = 113;
  hook2.second.second = 64;

  hooks << hook1;
  hooks << hook2;

  description.setDownloadHooks(hooks);

  const QList<DownloadHookDescription>& result = description.downloadHooks();
  ASSERT_EQ(2, result.count());
  ASSERT_EQ(hook1.first, result[0].first);
  ASSERT_EQ(hook1.second.first, result[0].second.first);
  ASSERT_EQ(hook1.second.second, result[0].second.second);

  ASSERT_EQ(hook2.first, result[1].first);
  ASSERT_EQ(hook2.second.first, result[1].second.first);
  ASSERT_EQ(hook2.second.second, result[1].second.second);
}

TEST_F(ServiceDescriptionTest, executorHooks)
{
  QList<ExecutorHookDescription> hooks;

  ExecutorHookDescription hook1;
  hook1.first = "{01DA2407-49B8-4E52-99A5-56624AC6CD84}";
  hook1.second = 100;

  ExecutorHookDescription hook2;
  hook1.first = "{1562B943-0C47-4D27-AC36-83EAF5D6DB6F}";
  hook1.second = 113;

  hooks << hook1;
  hooks << hook2;

  description.setExecutorHooks(hooks);

  const QList<ExecutorHookDescription>& result = description.executorHooks();
  ASSERT_EQ(2, result.count());
  ASSERT_EQ(hook1.first, result[0].first);
  ASSERT_EQ(hook1.second, result[0].second);

  ASSERT_EQ(hook2.first, result[1].first);
  ASSERT_EQ(hook2.second, result[1].second);
}
