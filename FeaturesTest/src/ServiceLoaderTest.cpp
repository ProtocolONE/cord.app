#include <gmock/gmock.h>
#include <gtest/gtest.h>

#include <Features/WorkStationLock/WorkStationLockHook.h>

#include <Host/ServiceProcess/ServiceLoader.h>
#include <Host/ServiceProcess/ServiceDescription.h>
#include <Host/ServiceSettings.h>
#include <Host/HookFactory.h>
#include <Host/ExecutorHookFactory.h>

#include <Core/Service.h>

#include <GameDownloader/GameDownloadService.h>
#include <GameDownloader/HookBase.h>
#include <GameDownloader/Hooks/InstallDependency.h>

#include <GameExecutor/GameExecutorService.h>
#include <GameExecutor/hookinterface.h>
#include <GameExecutor/Hook/ActivateWindow.h>

#include <QtCore/QCoreApplication>
#include <QtCore/QVector>

using P1::Host::ServiceProcess::ServiceLoader;
using P1::Host::ServiceProcess::ServiceDescription;
using P1::Host::ServiceSettings;
using P1::Host::ServiceProcess::DownloadHookDescription;
using P1::Host::ServiceProcess::ExecutorHookDescription;
using P1::Host::HookFactory;
using P1::Host::ExecutorHookFactory;

using P1::Core::Service;

using P1::GameExecutor::GameExecutorService;
using P1::GameExecutor::HookInterface;
using P1::GameExecutor::Hook::ActivateWindow;

using P1::GameDownloader::GameDownloadService;
using P1::GameDownloader::HookBase;
using P1::GameDownloader::Hooks::InstallDependency;

using Features::WorkStationLock::WorkStationLockHook;

class ExecutorTestHook1 : public HookInterface
{
public:
  static QString id() { return "{C789FCAD-5EB9-4B4F-BE88-F82CB698A409}"; }
};

class ExecutorHookArgs 
{
public:
  ExecutorHookArgs() 
    : hook(nullptr)
    , priority(0)
  {
  }

  Service service;
  HookInterface* hook;
  int priority;
};

class GameExecutorServiceMock : public GameExecutorService
{
public:
  GameExecutorServiceMock()
    : GameExecutorService()
  {
  }

  virtual bool addHook(const P1::Core::Service &service, HookInterface* hook, int priority = 0)
  {
    ExecutorHookArgs args;
    args.service = service;
    args.hook = hook;
    args.priority = priority;
    this->hooks.push_back(args);
    return this->GameExecutorService::addHook(service, hook, priority);
  }

  QVector<ExecutorHookArgs> hooks;
};

class DownloadHookArgs 
{
public:
  DownloadHookArgs() 
    : hook(nullptr)
  {
  }

  QString serviceId;
  HookBase* hook;
  int preHookPriority;
  int postHookPriority;
};

class ServiceLoaderTestGameDownloadServiceMock : public GameDownloadService
{
public:
  virtual void registerHook(const QString& serviceId, int preHookPriority, int postHookPriority, HookBase *hook)
  {
    DownloadHookArgs args;
    args.serviceId = serviceId;
    args.preHookPriority = preHookPriority;
    args.postHookPriority = postHookPriority;
    args.hook = hook;
    hooks.push_back(args);
  }

  QVector<DownloadHookArgs> hooks;
};

class ServiceLoaderTest : public ::testing::Test
{
public:
  void SetUp() 
  {
    description.setId("{627EBBE5-0E0E-40B3-9414-8DE42E984AF3}");
    description.setGameId("123");
    description.setName("FlawlessGame");
    description.setTorrentUrl("http://site/game.torrent");
    description.setHasDownloadPath(true);
    description.setIsDownloadable(true); // UNDONE
    description.setHasDownloadPath(true);
    description.setExtractorType("D9E40EE5-806F-4B7D-8D5C-B6A4BF0110E9");
    description.setExecuteUrl(
      "exe:%gamePath%/aikaru.exe"
      "?workingDir=%gamePath%/"
      "&args=%login% %token% 300002010000000000 login"
      "&executorHelper=1"
      "&downloadCustomFile=UI/GuildMarkWorld1.tga,http://files.protocol.one/update/aika,2");
    description.setDependencyList("vcredistr2005,/Q,vcredistr2010,/Q,dxwebsetup.exe,/Q");
    description.setGameSize(2800);

    downloaderHookFactory.setServiceLoader(&loader);
    downloaderHookFactory.setServiceSettings(&settigns);

    executorHookFactory.reg<ActivateWindow>();
    executorHookFactory.reg<WorkStationLockHook>();
    
    loader.setExecutor(&executor);
    loader.setDownloader(&downloader);
    loader.setExecuterHookFactory(&executorHookFactory);
    loader.setDownloaderHookFactory(&downloaderHookFactory);
  }

  ServiceDescription description;
  GameExecutorServiceMock executor;

  ServiceLoaderTestGameDownloadServiceMock downloader;
  HookFactory downloaderHookFactory;
  ExecutorHookFactory executorHookFactory;
  ServiceSettings settigns;
  ServiceLoader loader;
};

TEST_F(ServiceLoaderTest, hasValidDefaultValue)
{
  ASSERT_EQ(Service::Live, loader.gameArea());
  ASSERT_EQ("", loader.applicationPath());
}

TEST_F(ServiceLoaderTest, gameArea)
{
  loader.setGameArea(Service::Pts);
  ASSERT_EQ(Service::Pts, loader.gameArea());
}

TEST_F(ServiceLoaderTest, applicationPath)
{
  QString path = "C:/qwe/asd/zxc/qwe/asd/zxc/qwe/asd/";
  loader.setApplicationPath(path);
  ASSERT_EQ(path, loader.applicationPath());
}

TEST_F(ServiceLoaderTest, unknownService)
{
  Service *service = loader.getService("UNKNOWND SERVICE ID");
  ASSERT_EQ(nullptr, service);
}

TEST_F(ServiceLoaderTest, id)
{
  loader.registerService(description);
  Service *service = loader.getService(description.id());
  ASSERT_NE(nullptr, service);
  ASSERT_EQ(description.id(), service->id());
  ASSERT_TRUE(loader.servicesMap().contains(description.id()));
}

TEST_F(ServiceLoaderTest, gameId)
{
  loader.registerService(description);
  Service *service = loader.getService(description.id());
  ASSERT_NE(nullptr, service);
  ASSERT_EQ(description.gameId(), service->gameId());
}

TEST_F(ServiceLoaderTest, name)
{
  loader.registerService(description);
  Service *service = loader.getService(description.id());
  ASSERT_NE(nullptr, service);
  ASSERT_EQ(description.name(), service->name());
}

TEST_F(ServiceLoaderTest, torrentUrl)
{
  loader.registerService(description);
  Service *service = loader.getService(description.id());
  ASSERT_NE(nullptr, service);
  ASSERT_EQ(description.torrentUrl(), service->torrentUrl().toString());
}

TEST_F(ServiceLoaderTest, hasDownloadPath)
{
  loader.registerService(description);
  Service *service = loader.getService(description.id());
  ASSERT_NE(nullptr, service);
  ASSERT_EQ(description.hasDownloadPath(), service->hashDownloadPath());
}

TEST_F(ServiceLoaderTest, isDownloadable)
{
  loader.registerService(description);
  Service *service = loader.getService(description.id());
  ASSERT_NE(nullptr, service);
  ASSERT_EQ(description.isDownloadable(), service->isDownloadable());
}

TEST_F(ServiceLoaderTest, extractorType)
{
  loader.registerService(description);
  Service *service = loader.getService(description.id());
  ASSERT_NE(nullptr, service);
  ASSERT_EQ(description.extractorType(), service->extractorType());
}

TEST_F(ServiceLoaderTest, executeUrl)
{
  loader.registerService(description);
  Service *service = loader.getService(description.id());
  ASSERT_NE(nullptr, service);
  ASSERT_EQ(description.executeUrl(), service->urlTemplate());
}

TEST_F(ServiceLoaderTest, dependencyList)
{
  loader.registerService(description);
  Service *service = loader.getService(description.id());
  ASSERT_NE(nullptr, service);
  ASSERT_EQ(description.dependencyList(), service->externalDependencyList());
}

TEST_F(ServiceLoaderTest, gameSize)
{
  loader.registerService(description);
  Service *service = loader.getService(description.id());
  ASSERT_NE(nullptr, service);
  ASSERT_EQ(description.gameSize(), service->gameSize());
}

TEST_F(ServiceLoaderTest, executorHook)
{
  executorHookFactory.reg<ExecutorTestHook1>();

  ExecutorHookDescription hook1;
  hook1.first = ExecutorTestHook1::id();
  hook1.second = 42;

  QList<ExecutorHookDescription> hooks;
  hooks << hook1;
  description.setExecutorHooks(hooks);

  loader.registerService(description);

  ASSERT_LE(1, executor.hooks.count());
  ASSERT_EQ(description.id(), executor.hooks[0].service.id());
  ASSERT_EQ(hook1.second, executor.hooks[0].priority);
  ASSERT_NE(nullptr, dynamic_cast<ExecutorTestHook1*>(executor.hooks[0].hook));
}

TEST_F(ServiceLoaderTest, downloaderHook)
{
  QList<DownloadHookDescription> hooks;
  DownloadHookDescription hook1;
  hook1.first = "B4910801-2FA4-455E-AEAE-A2BAA2D3E4CA";
  hook1.second.first = 10;
  hook1.second.second = 42;

  hooks << hook1;
  description.setDownloadHooks(hooks);
  loader.registerService(description);

  ASSERT_LE(1, downloader.hooks.count());

  ASSERT_EQ(description.id(), downloader.hooks[0].serviceId);
  ASSERT_EQ(hook1.second.first, downloader.hooks[0].preHookPriority);
  ASSERT_EQ(hook1.second.second, downloader.hooks[0].postHookPriority);
  ASSERT_NE(nullptr, dynamic_cast<InstallDependency*>(downloader.hooks[0].hook));
}
