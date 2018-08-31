#include <Host/HookFactory.h>
#include <Host/Connection.h>
#include <Host/ServiceHandle.h>

#include <GameDownloader/HookBase.h>
#include <GameDownloader/Hooks/InstallDependency.h>
#include <GameDownloader/Hooks/PreventWinXpDownload.h>
#include <GameDownloader/Hooks/RemoveFileHook.h>
#include <GameDownloader/Hooks/PreventWin32Download.h>

#include <Host/GameDownloader/Hook/SaveInstallPath.h>
#include <Host/GameDownloader/Hook/CheckDownload.h>

#include <RestApi/ProtocolOneCredential.h>

#include <Core/UI/Message.h>

using P1::Core::UI::Message;

using P1::GameDownloader::HookBase;
using P1::GameDownloader::Hooks::InstallDependency;
using P1::GameDownloader::Hooks::PreventWinXpDownload;
using P1::GameDownloader::Hooks::PreventWin32Download;
using P1::Host::GameDownloader::Hook::SaveInstallPath;
using P1::Host::GameDownloader::Hook::UpdateUninstallInfo;
using P1::Host::GameDownloader::Hook::CheckDownload;

namespace P1 {
  namespace Host {

    HookFactory::HookFactory(QObject *parent)
      : QObject(parent)
      , _serviceSettings(nullptr)
      , _serviceLoader(nullptr)
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
      } else if (guid == "D4D358CD-DFF5-4B56-AF30-349CCAE86EED") {
        SaveInstallPath *hook = new SaveInstallPath(this);
        hook->setServiceSettings(this->_serviceSettings);
        hook->setServiceLoader(this->_serviceLoader);
        result = hook;
      } else if (guid == "81F2D0B8-298E-4041-83B0-EA5D417F580A") {
        CheckDownload *hook = new CheckDownload(this);
        hook->setCredential([this](const QString& serviceId)-> P1::RestApi::ProtocolOneCredential
        {
          Connection* connection = this->_serviceHandle->connectionLockedService(serviceId);
          if (!connection) {
            return P1::RestApi::ProtocolOneCredential();
          }

          return connection->credential();
        });

        QObject::connect(hook, &CheckDownload::internalError, []()
        {
          // INFO HACK ������� �� �������� � Checkdownload 
          QString title = tr("RESTAPI_ERROR_CAPTION");
          QString msg = tr("HAS_ACCESS_INTERNAL_ERROR");
          Message::warning(title, msg);
        });

        result = hook;
      } else if (guid == "65CE6F6B-B21A-4d8b-8FD4-B5B750D556CD") {
        UpdateUninstallInfo *hook = new UpdateUninstallInfo(this);
        result = hook;
      } else if (guid == "F9FD8276-2FEA-4F99-A2AA-1B37627216F5") {
        result = new PreventWinXpDownload(this);
      } else if (guid == "B963B92F-17D5-4DA3-A5C0-942776CE680A") {
        result = new P1::GameDownloader::Hooks::RemoveFileHook(this);
      } else if (guid == "1A097DFD-7660-4619-8B1D-8A1FE5440300") {
        result = new PreventWin32Download(this);
      }

      Q_ASSERT(result && result->hookId() == guid);

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


  }
}