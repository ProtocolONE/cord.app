#include <Host/ServiceProcess/ServicesListRequest.h>
#include <Host/ServiceProcess/ServiceLoader.h>

#include <Helper/ApplicationArea.hpp>

#include <RestApi/Api/V1/GetHost.h>

#include <QtCore/QTimer>

namespace P1 {
  namespace Host {
    namespace ServiceProcess {

      ServicesListRequest::ServicesListRequest(QObject *parent)
        : _retryCount(0)
        , _serviceLoader(nullptr)
        , _overrideWebScheme(false)
        , QObject(parent)
      {
        this->_retryIntervals << 5000 << 15000 << 30000 << 60000 << 90000;
        QObject::connect(&this->_downloadIconHelper, &DownloadServiceIcon::finished,
          this, &ServicesListRequest::additionalResourcesReady);
      }

      ServicesListRequest::~ServicesListRequest()
      {
      }

      void ServicesListRequest::setServiceLoader(ServiceLoader* loader)
      {
        Q_ASSERT(loader);
        this->_serviceLoader = loader;
      }

      void ServicesListRequest::request()
      {
        using P1::RestApi::Api::V1::GetHost;
        
        GetHost *cmd = new GetHost(this);
        QObject::connect(cmd, &GetHost::result, this, &ServicesListRequest::getHostsResult);

        cmd->execute();
      }

      void ServicesListRequest::getHostsResult() 
      {
        Q_ASSERT(this->_serviceLoader);

        using P1::RestApi::Api::V1::GetHost;
        GetHost* cmd = qobject_cast<GetHost*>(sender());
        if (!cmd)
          return;

        if (cmd->isSuccess()) {
          this->registerServices(cmd->servicesData());
          this->_downloadIconHelper.start();
          emit this->finished();
          cmd->deleteLater();
          return;
        }

        int interval = this->getRetryInterval();
        qCritical() << "Get services from RestApi, errorCode " << cmd->errorCode()
          << " statusCode " << cmd->statusCode()
          << " retry interval " << interval;

        QTimer::singleShot(interval, cmd, SLOT(execute()));
      }

      void ServicesListRequest::registerServices(const QList<QMap<QString, QString>>& servicesData)
      {
        QMap<QString, QString> data;
        P1::ApplicationArea applicationArea;
        applicationArea.load();

        Q_FOREACH(data, servicesData) {
          if (data["isPublishedInApp"] != "1" && !applicationArea.isTestVersion())
            continue;

          ServiceDescription serviceDist;
          serviceDist.setId(data["serviceId"]);
          serviceDist.setGameId(data["gameId"]);
          serviceDist.setName(data["folderName"]);
          serviceDist.setDisplayName(data["name"]);
          serviceDist.setTorrentUrl(data["torrentUrl"]);
          serviceDist.setIsDownloadable(data["isBrowserGame"] != "1");
          serviceDist.setHasDownloadPath(data["hasDownloadPath"] == "1");
          serviceDist.setExtractorType(data["extractorType"]);

          QString executeUrl = data["executeUrl"];

          if (QSysInfo::WindowsVersion == QSysInfo::WV_XP) {
            if (executeUrl.startsWith("electron://"))
              executeUrl = "http://" + executeUrl.mid(11);

            if (executeUrl.startsWith("electrons://"))
              executeUrl = "https://" + executeUrl.mid(12);

          } else {
            if (this->_overrideWebScheme) {
              if (executeUrl.startsWith("http://"))
                executeUrl = "electron://" + executeUrl.mid(7);

              if (executeUrl.startsWith("https://"))
                executeUrl = "electrons://" + executeUrl.mid(8);
            }

          }

          serviceDist.setExecuteUrl(executeUrl);
          serviceDist.setGameSize(data["gameSize"].toInt());
          serviceDist.setDependencyList(data["dependencyList"]);

          if (!data["iconInApp"].isEmpty())
            this->_downloadIconHelper.downloadIconRequest(data["iconInApp"], data["serviceId"]);

          QList<ExecutorHookDescription> executorHooks;
          if (!data["executorHooks"].isEmpty())
            Q_FOREACH(QString executorHook, data["executorHooks"].split(",")) {
              QString priority = "0";

              if (executorHook.contains(":")) {
                priority = executorHook.left(executorHook.indexOf(":"));
                executorHook = executorHook.remove(0, priority.count() + 1);
              }

              executorHooks << ExecutorHookDescription(executorHook, priority.toInt());
          }

          if (serviceDist.id() == "370000000000")  {
            // Force add Features::GameExecutor::Hook::CheckAsciiPath
            executorHooks << ExecutorHookDescription("DD9C78EE-A95A-4326-A913-F02F75C942EF", 100);
          }

          serviceDist.setExecutorHooks(executorHooks);

          QList<DownloadHookDescription> downloadHooks;
          if (!data["downloadHooks"].isEmpty())
            Q_FOREACH(QString downloadHook, data["downloadHooks"].split(",")) {
              QString priority = "0";
              QString priority2 = "0";

              if (downloadHook.contains(":")) {
                priority = downloadHook.left(downloadHook.indexOf(":"));
                downloadHook = downloadHook.remove(0, priority.count() + 1);
              }

              if (downloadHook.contains(":")) {
                priority2 = downloadHook.left(downloadHook.indexOf(":"));
                downloadHook = downloadHook.remove(0, priority2.count() + 1);
              }

              DownloadHookDescription hook;
              hook.first = downloadHook;
              hook.second.first = priority.toInt();
              hook.second.second = priority2.toInt();

              downloadHooks << hook;
          }

          {
          // HACK P1::GameDownloader::Hooks::RemoveFileHook
            DownloadHookDescription hook;
            hook.first = "B963B92F-17D5-4DA3-A5C0-942776CE680A";
            hook.second.first = 0;
            hook.second.second = 0;

            downloadHooks << hook;
          }

          serviceDist.setDownloadHooks(downloadHooks);

          this->_serviceLoader->registerService(serviceDist);
        }
      }

      int ServicesListRequest::getRetryInterval()
      {
        if (this->_retryIntervals.count() - 1 >= this->_retryCount)
          return this->_retryIntervals[this->_retryCount++];

        this->_retryCount++;
        return this->_retryIntervals[this->_retryIntervals.count() - 1];
      }

      void ServicesListRequest::setOverrideWebExecutor(bool value)
      {
        this->_overrideWebScheme = value;
      }

    }
  }
}