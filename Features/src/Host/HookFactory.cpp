#include <Host/HookFactory.h>
#include <GameDownloader/HookBase.h>
#include <GameDownloader/Hooks/InstallDependency.h>

#include <Features/Thetta/DistrIntegrity.h>

#include <Host/GameDownloader/Hook/SaveInstallPath.h>

#include <Integration/ZZima/DADownloaderHook.h>
#include <Integration/ZZima/ZZimaConnection.h>

using GGS::GameDownloader::HookBase;
using GGS::GameDownloader::Hooks::InstallDependency;
using Features::Thetta::DistrIntegrity;
using GameNet::Host::GameDownloader::Hook::SaveInstallPath;

namespace GameNet {
  namespace Host {

    HookFactory::HookFactory(QObject *parent)
      : QObject(parent)
      , _serviceSettings(nullptr)
      , _serviceLoader(nullptr)
      , _zzimaConnection(nullptr)
    {
    }

    HookFactory::~HookFactory()
    {
    }

    HookBase* HookFactory::create(const QString& guid)
    {
      Q_ASSERT(this->_serviceSettings);
      Q_ASSERT(this->_serviceLoader);

      if (this->_cache.contains(guid))
        return this->_cache[guid];

      HookBase* result = nullptr;

      if (guid == "B4910801-2FA4-455E-AEAE-A2BAA2D3E4CA") {
        result = new InstallDependency(this);
      } else if (guid == "36003110-6DC9-4D16-8076-D84FFAFC36B8") {
        result = new DistrIntegrity(this);
      } else if (guid == "D4D358CD-DFF5-4B56-AF30-349CCAE86EED") {
        SaveInstallPath *hook = new SaveInstallPath(this);
        hook->setServiceSettings(this->_serviceSettings);
        hook->setServiceLoader(this->_serviceLoader);
        result = hook;
      } else if (guid == "9F6083BB-D03D-45A9-89FE-2D6EF098544A") {
        GameNet::Integration::ZZima::DADownloaderHook *hook = new GameNet::Integration::ZZima::DADownloaderHook(this);
        hook->setZzimaConnection(this->_zzimaConnection);
        hook->setServiceSettings(this->_serviceSettings);
        result = hook;
      }

      Q_ASSERT(result);
      Q_ASSERT(result->hookId() == guid);

      if (result)
        this->_cache[guid] = result;

      return result;
    }

    void HookFactory::setServiceSettings(ServiceSettings *value)
    {
      Q_ASSERT(value);
      this->_serviceSettings = value;
    }

    void HookFactory::setServiceLoader(ServiceProcess::ServiceLoader *value)
    {
      Q_ASSERT(value);
      this->_serviceLoader = value;
    }

    void HookFactory::setZzimaConnection(::GameNet::Integration::ZZima::ZZimaConnection *value)
    {
      Q_ASSERT(value);
      this->_zzimaConnection = value;
    }

  }
}