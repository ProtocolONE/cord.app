#include <Host/ExecutorHookFactory.h>
#include <Host/HookFactory.h>
#include <Host/Thetta.h>

#include <GameDownloader/GameDownloadService.h>

#include <GameExecutor/Hook/ActivateWindow.h>
#include <GameExecutor/Hook/RestoreResolution.h>
#include <GameExecutor/Hook/DisableDEP.h>
#include <GameExecutor/Hook/DownloadCustomFile.h>
#include <GameExecutor/Hook/DisableIEDefalutProxy.h>
#include <GameExecutor/Hook/SendPlayingInfo.h>
#include <GameExecutor/Hook/DefaultAikaSettings.h>
#include <GameExecutor/Hook/DisableAeroHook.h>
#include <GameExecutor/Hook/BannerDownload.h>

#include <Features/CASettingsFix.h>
#include <Features/Thetta/DistrIntegrityExecutorHook.h>
#include <Features/Thetta/DistrIntegrity.h>
#include <Features/Thetta/ThettaMonitor.h>
#include <Features/Thetta/SaveUserInfo.h>

#include <QtWidgets/QApplication>
#include <QtWidgets/QDesktopWidget>

using GGS::GameDownloader::GameDownloadService;
using GGS::GameExecutor::GameExecutorService;
using GGS::GameExecutor::HookInterface;

using Features::Thetta::SaveUserInfo;

namespace GameNet {
  namespace Host {

    ExecutorHookFactory::ExecutorHookFactory(QObject *parent /*= 0*/)
      : QObject(parent)
      , _downloader(nullptr)
      , _downloaderHookFactory(nullptr)
      , _thetta(nullptr)
      , _saveUserInfo(new SaveUserInfo(this))
    {
      this->_window.setGeometry(-30000, -30000, 1, 1);
      this->_window.setWindowFlags(Qt::FramelessWindowHint | Qt::ToolTip);
      this->_window.setAttribute(Qt::WA_TranslucentBackground);
      this->_window.setStyleSheet("background:transparent;");
    }

    ExecutorHookFactory::~ExecutorHookFactory()
    {
    }

    void ExecutorHookFactory::setDownloader(GameDownloadService *value)
    {
      Q_ASSERT(value);
      this->_downloader = value;
    }

    void ExecutorHookFactory::setDownloaderHookFactory(HookFactory *value)
    {
      Q_ASSERT(value);
      this->_downloaderHookFactory = value;
    }

    void ExecutorHookFactory::setExecutor(GameExecutorService *value)
    {
      Q_ASSERT(value);
      this->_executor = value;
    }

    void ExecutorHookFactory::init()
    {
      using namespace GGS::GameExecutor::Hook;
      using namespace Features;
      using namespace Features::Thetta;

      this->reg<ActivateWindow>([this](ActivateWindow* a){
        a->setWidget(&this->_window);
      });
      this->reg<RestoreResolution>();
      this->reg<DisableDEP>();
      this->reg<DownloadCustomFile>();
      this->reg<DisableIEDefalutProxy>();
      this->reg<SendPlayingInfo>();
      this->reg<DefaultAikaSettings>();
      this->reg<DisableAeroHook>();
      this->reg<ThettaMonitor>([this](ThettaMonitor* h){
        h->setDriverInstaller(this->_thetta->installer());
        h->setSaveUserInfo(this->_saveUserInfo);
      });

      this->reg<CASettingsFix>([this](CASettingsFix *h) {
        int primaryScreen = QApplication::desktop()->primaryScreen();
        h->setResolution(QApplication::desktop()->screenGeometry(primaryScreen));
        QObject::connect(this->_downloader, &GameDownloadService::serviceInstalled,
          h, &Features::CASettingsFix::install);
      });

      this->reg<DistrIntegrityExecutorHook>([this](DistrIntegrityExecutorHook *h) {
        using GameNet::Host::HookFactory;
        using GGS::GameDownloader::HookBase;

        HookBase *hook = this->_downloaderHookFactory->create("36003110-6DC9-4D16-8076-D84FFAFC36B8");
        h->setDistrIntegrity(qobject_cast<Features::Thetta::DistrIntegrity*>(hook));
        h->setGameExecutor(this->_executor);
      });
    }

    HookInterface* ExecutorHookFactory::create(const QString& id)
    {
      typedef Factory<HookInterface, QString> Parent;
      HookInterface* result = this->Parent::create(id);
      if (result)
        result->setParent(this);

      return result;
    }

    void ExecutorHookFactory::setThetta(Thetta *value)
    {
      Q_ASSERT(value);
      this->_thetta = value;
    }

  }
}