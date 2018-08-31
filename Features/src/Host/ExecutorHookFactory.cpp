#include <Host/ExecutorHookFactory.h>
#include <Host/HookFactory.h>
#include <Host/Application.h>

#include <GameDownloader/GameDownloadService.h>

#include <GameExecutor/Hook/ActivateWindow.h>
#include <GameExecutor/Hook/RestoreResolution.h>
#include <GameExecutor/Hook/DisableDEP.h>
#include <GameExecutor/Hook/DownloadCustomFile.h>
#include <GameExecutor/Hook/DisableIEDefalutProxy.h>
#include <GameExecutor/Hook/SendPlayingInfo.h>
#include <GameExecutor/Hook/DisableAeroHook.h>
#include <GameExecutor/Hook/PreventWinXpLaunch.h>

#include <Features/WorkStationLock/RegisterSessionNotificationFilter.h>
#include <Features/WorkStationLock/WorkStationLockHook.h>

#include <Features/GameExecutor/Hook/CheckAsciiPath.h>

#include <QtWidgets/QApplication>
#include <QtWidgets/QDesktopWidget>

using P1::GameDownloader::GameDownloadService;
using P1::GameExecutor::GameExecutorService;
using P1::GameExecutor::HookInterface;

using Features::WorkStationLock::RegisterSessionNotificationFilter;
using Features::WorkStationLock::WorkStationLockHook;

namespace P1 {
  namespace Host {

    ExecutorHookFactory::ExecutorHookFactory(QObject *parent /*= 0*/)
      : QObject(parent)
      , _downloader(nullptr)
      , _executor(nullptr)
      , _downloaderHookFactory(nullptr)
      , _filter(nullptr)
    {
      this->_window.setGeometry(-30000, -30000, 1, 1);
      this->_window.setWindowFlags(Qt::FramelessWindowHint | Qt::ToolTip);
      this->_window.setAttribute(Qt::WA_TranslucentBackground);
      this->_window.setStyleSheet("background:transparent;");
     
      this->_filter = new RegisterSessionNotificationFilter(&this->_window, reinterpret_cast<HWND>(this->_window.winId()));

      QApplication::instance()->installNativeEventFilter(this->_filter);
    }

    ExecutorHookFactory::~ExecutorHookFactory()
    {
      QApplication::instance()->removeNativeEventFilter(this->_filter);
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
      using namespace P1::GameExecutor::Hook;
      using namespace Features;
      using namespace Features::GameExecutor::Hook;

      this->reg<ActivateWindow>([this](ActivateWindow* a){
        a->setWidget(&this->_window);
      });
      this->reg<WorkStationLockHook>([this](WorkStationLockHook* hook){
        hook->setFilter(this->_filter);
      });
            
      this->reg<RestoreResolution>();
      this->reg<DisableDEP>();
      this->reg<DownloadCustomFile>();
      this->reg<DisableIEDefalutProxy>();
      this->reg<SendPlayingInfo>();
      this->reg<DisableAeroHook>();
      this->reg<PreventWinXpLaunch>();
      this->reg<CheckAsciiPath>();
    }

    HookInterface* ExecutorHookFactory::create(const QString& id)
    {
      typedef Factory<HookInterface, QString> Parent;
      HookInterface* result = this->Parent::create(id);
      if (result)
        result->setParent(this);

      return result;
    }

  }
}