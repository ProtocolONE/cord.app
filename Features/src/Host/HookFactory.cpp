#include <Host/HookFactory.h>
#include <Host/Connection.h>
#include <Host/ServiceHandle.h>

#include <GameDownloader/HookBase.h>
#include <GameDownloader/Hooks/InstallDependency.h>
#include <GameDownloader/Hooks/PreventWinXpDownload.h>
#include <GameDownloader/Hooks/RemoveFileHook.h>

#include <Features/Thetta/DistrIntegrity.h>

#include <Host/GameDownloader/Hook/SaveInstallPath.h>
#include <Host/GameDownloader/Hook/CheckDownload.h>

#include <Integration/ZZima/DADownloaderHook.h>
#include <Integration/ZZima/ZZimaConnection.h>

#include <RestApi/GameNetCredential.h>

#include <Core/UI/Message.h>

using GGS::Core::UI::Message;

using GGS::GameDownloader::HookBase;
using GGS::GameDownloader::Hooks::InstallDependency;
using GGS::GameDownloader::Hooks::PreventWinXpDownload;
using Features::Thetta::DistrIntegrity;
using GameNet::Host::GameDownloader::Hook::SaveInstallPath;
using GameNet::Host::GameDownloader::Hook::CheckDownload;

namespace GameNet {
  namespace Host {

    HookFactory::HookFactory(QObject *parent)
      : QObject(parent)
      , _serviceSettings(nullptr)
      , _serviceLoader(nullptr)
      , _zzimaConnection(nullptr)
      , _serviceHandle(nullptr)
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
      } else if (guid == "81F2D0B8-298E-4041-83B0-EA5D417F580A") {
        CheckDownload *hook = new CheckDownload(this);
        hook->setCredential([this](const QString& serviceId)-> GGS::RestApi::GameNetCredential {
          Connection* connection = this->_serviceHandle->connectionLockedService(serviceId);
          if (!connection) {
            return GGS::RestApi::GameNetCredential();
          }

          return connection->credential();
        });

        QObject::connect(hook, &CheckDownload::internalError, []() {
          // INFO HACK перевод не работает в Checkdownload 
          QString title = tr("RESTAPI_ERROR_CAPTION");
          QString msg = tr("HAS_ACCESS_INTERNAL_ERROR");
          Message::warning(title, msg);
        });

        result = hook;
      } else if (guid == "9F6083BB-D03D-45A9-89FE-2D6EF098544A") {
        GameNet::Integration::ZZima::DADownloaderHook *hook = new GameNet::Integration::ZZima::DADownloaderHook(this);
        hook->setZzimaConnection(this->_zzimaConnection);
        hook->setServiceSettings(this->_serviceSettings);
        result = hook;
      } else if (guid == "65CE6F6B-B21A-4d8b-8FD4-B5B750D556CD") {
        UpdateUninstallInfo *hook = new UpdateUninstallInfo(this);
        result = hook;
      } else if (guid == "F9FD8276-2FEA-4F99-A2AA-1B37627216F5") {
        result = new PreventWinXpDownload(this);
      } else if (guid == "B963B92F-17D5-4DA3-A5C0-942776CE680A") {
        result = new GGS::GameDownloader::Hooks::RemoveFileHook(this);
      }

      Q_ASSERT(result);
      Q_ASSERT(result->hookId() == guid);

      if (result)
        this->_cache[guid] = result;

      return result;
    }

    void HookFactory::setServiceHandle(ServiceHandle *value)
    {
      Q_ASSERT(value);
      this->_serviceHandle = value;
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