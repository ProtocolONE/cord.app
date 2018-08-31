#include <gmock/gmock.h>
#include <gtest/gtest.h>

#include <Host/ServiceProcess/ServiceDescription.h>

using P1::Host::ServiceProcess::ServiceDescription;
using P1::Host::ServiceProcess::DownloadHookDescription;
using P1::Host::ServiceProcess::ExecutorHookDescription;

class ServiceDescriptionTest : public ::testing::Test
{
public:
  ServiceDescription description;
};

TEST_F(ServiceDescriptionTest, hasValidDefaultValues)
{
  ASSERT_EQ("", description.id());
  ASSERT_EQ("", description.gameId());
  ASSERT_EQ("", description.name());
  ASSERT_EQ("", description.torrentUrl());
  ASSERT_EQ(false, description.isDownloadable());
  ASSERT_EQ(false, description.hasDownloadPath());
  ASSERT_EQ("", description.extractorType());
  ASSERT_EQ("", description.executeUrl());
  ASSERT_EQ("", description.dependencyList());
}

TEST_F(ServiceDescriptionTest, id)
{
  QString id = "123qwe123";
  description.setId(id);
  ASSERT_EQ(id, description.id());
}

TEST_F(ServiceDescriptionTest, gameId)
{
  QString gameId = "123";
  description.setGameId(gameId);
  ASSERT_EQ(gameId, description.gameId());
}

TEST_F(ServiceDescriptionTest, name)
{
  QString name = "Combat Arms";
  description.setName(name);
  ASSERT_EQ(name, description.name());
}

TEST_F(ServiceDescriptionTest, torrentUrl)
{
  QString torrentUrl = "http://protocol.one/game.torrent";
  description.setTorrentUrl(torrentUrl);
  ASSERT_EQ(torrentUrl, description.torrentUrl());
}

TEST_F(ServiceDescriptionTest, isDownloadable)
{
  bool isDownloadable = true;
  description.setIsDownloadable(isDownloadable);
  ASSERT_EQ(isDownloadable, description.isDownloadable());
}

TEST_F(ServiceDescriptionTest, hasDownloadPath)
{
  bool hasDownloadPath = true;
  description.setHasDownloadPath(hasDownloadPath);
  ASSERT_EQ(hasDownloadPath, description.hasDownloadPath());
}

TEST_F(ServiceDescriptionTest, extractorType)
{
  QString extractorType = "{2E69975E-4D7B-4D6F-A894-02F439F1C1AB}";
  description.setExtractorType(extractorType);
  ASSERT_EQ(extractorType, description.extractorType());
}

TEST_F(ServiceDescriptionTest, executeUrl)
{
  QString executeUrl = "exe:C:/Games/calc.exe?args=1+1&workingdir=C:/Games/";
  description.setExecuteUrl(executeUrl);
  ASSERT_EQ(executeUrl, description.executeUrl());
}

TEST_F(ServiceDescriptionTest, dependencyList)
{
  QString dependencyList = "dxwebsetup.exe,/Q";
  description.setDependencyList(dependencyList);
  ASSERT_EQ(dependencyList, description.dependencyList());
}

TEST_F(ServiceDescriptionTest, gameSize)
{
  int gameSize = 1024;
  description.setGameSize(gameSize);
  ASSERT_EQ(gameSize, description.gameSize());
}

TEST_F(ServiceDescriptionTest, downloadHooks)
{
  QList<DownloadHookDescription> hooks;
  DownloadHookDescription hook1;
  hook1.first = "{01DA2407-49B8-4E52-99A5-56624AC6CD84}";
  hook1.second.first = 100;
  hook1.second.second = 42;
  hooks << hook1;

  description.setDownloadHooks(hooks);

  const QList<DownloadHookDescription>& result = description.downloadHooks();
  ASSERT_EQ(1, result.count());
  ASSERT_EQ(hook1.first, result[0].first);
  ASSERT_EQ(hook1.second.first, result[0].second.first);
  ASSERT_EQ(hook1.second.second, result[0].second.second);
}

TEST_F(ServiceDescriptionTest, executorHooks)
{
  QList<ExecutorHookDescription> hooks;

  ExecutorHookDescription hook1;
  hook1.first = "{01DA2407-49B8-4E52-99A5-56624AC6CD84}";
  hook1.second = 100;

  hooks << hook1;

  description.setExecutorHooks(hooks);

  const QList<ExecutorHookDescription>& result = description.executorHooks();
  ASSERT_EQ(1, result.count());
  ASSERT_EQ(hook1.first, result[0].first);
  ASSERT_EQ(hook1.second, result[0].second);
}
