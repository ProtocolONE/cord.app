#include <Host/ServiceProcess/ServicesListRequest.h>
#include <Host/ServiceProcess/ServiceLoader.h>

#include <QtCore/QTimer>

#include <Windows.h>

namespace GameNet {
  namespace Host {
    namespace ServiceProcess {

      ServicesListRequest::ServicesListRequest(QObject *parent)
        : _retryCount(0)
        , _serviceLoader(nullptr)
        , QObject(parent)
      {
        this->_retryIntervals << 5000 << 15000 << 30000 << 60000 << 90000;
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
        using GGS::RestApi::Commands::Service::GetHosts;
        GetHosts *cmd = new GetHosts(this);
        QObject::connect(cmd, &GetHosts::result, this, &ServicesListRequest::getHostsResult);

        cmd->execute();
      }

      void ServicesListRequest::getHostsResult(GGS::RestApi::CommandBase::CommandResults result) 
      {
        Q_ASSERT(this->_serviceLoader);

        using GGS::RestApi::Commands::Service::GetHosts;
        GetHosts* cmd = qobject_cast<GetHosts*>(sender());
        if (!cmd)
          return;

        if (result == GGS::RestApi::CommandBase::NoError) {
          this->registerServices(cmd->servicesData());
          emit this->finished();
          cmd->deleteLater();
          return;
        }

        int interval = this->getRetryInterval();
        qCritical() << "Get services from RestApi, result" << result << "error" << cmd->errorCode() << "retry interval" << interval;

        QTimer::singleShot(interval, cmd, SLOT(execute()));
      }

      void ServicesListRequest::registerServices(const QList<QMap<QString, QString>>& servicesData)
      {
        QMap<QString, QString> data;
        Q_FOREACH(data, servicesData) {
          if (data["isPublishedInApp"] != "1" && this->_applicationArea != GGS::Core::Service::Tst)
            continue;

          ServiceDescription serviceDist;
          serviceDist.setId(data["serviceId"]);

          if (serviceDist.id() == "30000000000") {
            data["isBrowserGame"] = "0";
            data["downloadHooks"] = "999:0:81F2D0B8-298E-4041-83B0-EA5D417F580A";
            //data["executeUrl"] = "exe:%gamePath%/bin/BlackDesert32.exe?workingDir=%gamePath%/bin/&args=%userId% %appKey% %token%&executorHelper=0&injectOverlay=0&authsdk=1";
            data["executeUrl"] = "exe:%gamePath%/bin/BlackDesert32.exe?workingDir=%gamePath%/bin/&args=&executorHelper=0&injectOverlay=0&authsdk=1";
          }

          serviceDist.setGameId(data["gameId"]);
          serviceDist.setName(data["folderName"]);
          serviceDist.setDisplayName(data["name"]);
          serviceDist.setTorrentUrl(data["torrentUrl"]);
          serviceDist.setIsDownloadable(data["isBrowserGame"] != "1");
          serviceDist.setHasDownloadPath(data["hasDownloadPath"] == "1");
          serviceDist.setExtractorType(data["extractorType"]);
          serviceDist.setExecuteUrl(data["executeUrl"]);
          serviceDist.setGameSize(data["gameSize"].toInt());

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

      void ServicesListRequest::setApplicationArea(GGS::Core::Service::Area area)
      {
        this->_applicationArea = area;
      }
    }
  }
}