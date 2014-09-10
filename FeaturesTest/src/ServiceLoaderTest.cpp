#include <gmock/gmock.h>
#include <gtest/gtest.h>

#include <Features/Thetta/ThettaMonitor.h>
#include <Features/Thetta/DistrIntegrity.h>

#include <Host/ServiceLoader.h>
#include <Host/ServiceDescription.h>
#include <Host/ServiceSettings.h>
#include <Host/HookFactory.h>
#include <Host/ExecutorHookFactory.h>

#include <Core/Service.h>

#include <GameDownloader/GameDownloadService.h>
#include <GameDownloader/HookBase.h>
#include <GameDownloader/Hooks/InstallDependency.h>

#include <GameExecutor/GameExecutorService.h>
#include <GameExecutor/hookinterface.h>
#include <GameExecutor/Hook/SendPlayingInfo.h>

#include <QtCore/QCoreApplication>
#include <QtCore/QVector>

using GameNet::Host::ServiceLoader;
using GameNet::Host::ServiceDescription;
using GameNet::Host::ServiceSettings;
using GameNet::Host::DownloadHookDescription;
using GameNet::Host::ExecutorHookDescription;
using GameNet::Host::HookFactory;
using GameNet::Host::ExecutorHookFactory;

using GGS::Core::Service;

using GGS::GameExecutor::GameExecutorService;
using GGS::GameExecutor::HookInterface;
using GGS::GameExecutor::Hook::SendPlayingInfo;

using GGS::GameDownloader::GameDownloadService;
using GGS::GameDownloader::HookBase;
using GGS::GameDownloader::Hooks::InstallDependency;

using Features::Thetta::DistrIntegrity;
using Features::Thetta::ThettaMonitor;

class ThettaMonitorMock : public HookInterface 
{
public :
  ThettaMonitorMock() 
    : HookInterface()
  {
  }

  static QString id() { return ThettaMonitor::id(); }
};

class SendPlayingInfoMock : public HookInterface 
{
public :
  SendPlayingInfoMock() 
    : HookInterface()
  {
  }

  static QString id() { return SendPlayingInfo::id(); }
};

class ExecutorTestHook1 : public HookInterface
{
public:
  static QString id() { return "{C789FCAD-5EB9-4B4F-BE88-F82CB698A409}"; }
};

class ExecutorTestHook2 : public HookInterface
{
public:
  static QString id() { return "{9C0E818B-8302-4295-A86C-E4FCC2C00640}"; }
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

  virtual bool addHook(const GGS::Core::Service &service, HookInterface* hook, int priority = 0)
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

class GameDownloadServiceMock : public GameDownloadService
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
      "&downloadCustomFile=UI/GuildMarkWorld1.tga,http://files.gamenet.ru/update/aika,2");
    description.setDependencyList("vcredistr2005,/Q,vcredistr2010,/Q,dxwebsetup.exe,/Q");
    description.setGameSize(2800);

    downloaderHookFactory.setServiceLoader(&loader);
    downloaderHookFactory.setServiceSettings(&settigns);

    executorHookFactory.reg<ThettaMonitorMock>();
    executorHookFactory.reg<SendPlayingInfoMock>();

    loader.setApplicationPath(QCoreApplication::applicationDirPath());
    loader.setGameArea(GGS::Core::Service::Live);

    loader.setExecutor(&executor);
    loader.setDownloader(&downloader);
    loader.setExecuterHookFactory(&executorHookFactory);
    loader.setDownloaderHookFactory(&downloaderHookFactory);
  }

  ServiceDescription description;
  GameExecutorServiceMock executor;
  GameDownloadServiceMock downloader;
  HookFactory downloaderHookFactory;
  ExecutorHookFactory executorHookFactory;
  ServiceSettings settigns;
  ServiceLoader loader;
};

TEST_F(ServiceLoaderTest, gameArea)
{
  loader.setGameArea(Service::Pts);
  ASSERT_EQ(Service::Pts, loader.gameArea());

  loader.setGameArea(Service::Live);
  ASSERT_EQ(Service::Live, loader.gameArea());
}

TEST_F(ServiceLoaderTest, applicationPath)
{
  QString path1 = "C:/qwe/asd/zxc/qwe/asd/zxc/qwe/asd/";
  loader.setApplicationPath(path1);
  ASSERT_EQ(path1, loader.applicationPath());

  QString path2 = "D:/123/234/456/678/wer/67/sdf/234/df/wer";
  loader.setApplicationPath(path2);
  ASSERT_EQ(path2, loader.applicationPath());
}

TEST_F(ServiceLoaderTest, unknownService)
{
  loader.registerService(description);
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
  executorHookFactory.reg<ExecutorTestHook2>();

  ExecutorHookDescription hook1;
  hook1.first = ExecutorTestHook1::id();
  hook1.second = 42;

  ExecutorHookDescription hook2;
  hook2.first = ExecutorTestHook2::id();
  hook2.second = 65;

  QList<ExecutorHookDescription> hooks;
  hooks << hook1 << hook2;
  description.setExecutorHooks(hooks);

  loader.registerService(description);

  ASSERT_LE(2, executor.hooks.count());
  
  ASSERT_EQ(description.id(), executor.hooks[0].service.id());
  ASSERT_EQ(description.id(), executor.hooks[1].service.id());

  ASSERT_EQ(hook1.second, executor.hooks[0].priority);
  ASSERT_EQ(hook2.second, executor.hooks[1].priority);

  ASSERT_NE(nullptr, dynamic_cast<ExecutorTestHook1*>(executor.hooks[0].hook));
  ASSERT_NE(nullptr, dynamic_cast<ExecutorTestHook2*>(executor.hooks[1].hook));
}

TEST_F(ServiceLoaderTest, downloaderHook)
{
  QList<DownloadHookDescription> hooks;
  DownloadHookDescription hook1;
  hook1.first = "B4910801-2FA4-455E-AEAE-A2BAA2D3E4CA";
  hook1.second.first = 10;
  hook1.second.second = 42;

  DownloadHookDescription hook2;
  hook2.first = "36003110-6DC9-4D16-8076-D84FFAFC36B8";
  hook2.second.first = 100;
  hook2.second.second = 18;

  hooks << hook1 << hook2;
  description.setDownloadHooks(hooks);
  loader.registerService(description);

  ASSERT_LE(2, downloader.hooks.count());

  ASSERT_EQ(description.id(), downloader.hooks[0].serviceId);
  ASSERT_EQ(description.id(), downloader.hooks[1].serviceId);

  ASSERT_EQ(hook1.second.first, downloader.hooks[0].preHookPriority);
  ASSERT_EQ(hook2.second.first, downloader.hooks[1].preHookPriority);

  ASSERT_EQ(hook1.second.second, downloader.hooks[0].postHookPriority);
  ASSERT_EQ(hook2.second.second, downloader.hooks[1].postHookPriority);

  ASSERT_NE(nullptr, dynamic_cast<InstallDependency*>(downloader.hooks[0].hook));
  ASSERT_NE(nullptr, dynamic_cast<DistrIntegrity*>(downloader.hooks[1].hook));
}
