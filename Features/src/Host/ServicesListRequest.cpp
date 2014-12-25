#include <Host/ServicesListRequest.h>
#include <Host/ServiceLoader.h>
#include <Settings/Settings.h>
#include <Features/GameNetDownloader.h>

#include <QCoreApplication>
#include <QtCore/QTimer>
#include <QtCore/QDebug>
#include <QtCore/QFile>

namespace GameNet {
  namespace Host {

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
        serviceDist.setGameId(data["gameId"]);
        serviceDist.setName(data["folderName"]);
        serviceDist.setIsDownloadable(data["isBrowserGame"] != "1");
        serviceDist.setHasDownloadPath(data["hasDownloadPath"] == "1");
        serviceDist.setExtractorType(data["extractorType"]);
        serviceDist.setExecuteUrl(data["executeUrl"]);
        serviceDist.setGameSize(data["gameSize"].toInt());

        if (!data["iconInApp"].isEmpty()) {
          this->downloadIconRequest(data["iconInApp"], data["serviceId"]);
        }
       
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
              downloadHook = downloadHook.remove(0, priority.count() + 1);
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

    void ServicesListRequest::downloadIconRequest(const QString& url, const QString& serviceId)
    {
      GameNetDownloader* downloader = new GameNetDownloader(this);

      QObject::connect(downloader, &GameNetDownloader::downloaded,
        downloader, &QObject::deleteLater);

      QObject::connect(downloader, &GameNetDownloader::error,
        this, &ServicesListRequest::downloadFailed);

      QString filePath = QString("%1/Assets/Images/icons/%2.ico").arg(QCoreApplication::applicationDirPath()).arg(serviceId);
      QString lastModified = this->readLastModified(filePath);

      QNetworkRequest request(url);
      if (!lastModified.isEmpty())
        request.setRawHeader("If-Modified-Since", lastModified.toLatin1());

      QNetworkReply *reply = this->_networkManager.head(request);
      QObject::connect(reply, &QNetworkReply::finished, [this, filePath, url, downloader, reply](){
        reply->deleteLater();

        int httpCode = reply->attribute(QNetworkRequest::HttpStatusCodeAttribute).toInt();
        // Http codes defined by rfc: http://www.w3.org/Protocols/rfc2616/rfc2616-sec10.html
        // 200 OK
        // 304 Not Modified
        if (httpCode != 304 && httpCode != 200) {
          return;
        }

        QString lastModified = QString::fromLatin1(reply->rawHeader(QByteArray("Last-Modified")));

        bool changed = !QFile::exists(filePath) || httpCode == 200;

        if (httpCode != 304) {
          ServicesListRequest* baseClass = this;
          QString localFilePath = filePath;

          QObject::connect(downloader, &GameNetDownloader::downloaded,
            [baseClass, localFilePath, lastModified](){
              baseClass->saveLastModifed(localFilePath, lastModified);
          });
        }

        if (changed) {
          downloader->download(url, filePath);
          return;
        }

        downloader->deleteLater();
      });
    }

    QString ServicesListRequest::readLastModified(const QString& path)
    {
      GGS::Settings::Settings settings; 
      settings.beginGroup("ServicesListRequest");
      settings.beginGroup("Icons");
      settings.beginGroup(path);
      return settings.value("lastModified", "").toString();
    }

    void ServicesListRequest::saveLastModifed(const QString& path, const QString& value)
    {
      GGS::Settings::Settings settings; 
      settings.beginGroup("ServicesListRequest");
      settings.beginGroup("Icons");
      settings.beginGroup(path);
      settings.setValue("lastModified", value);
    }

    void ServicesListRequest::downloadFailed()
    {
      GameNetDownloader* cmd = qobject_cast<GameNetDownloader*>(sender());
      if (cmd) 
        cmd->deleteLater();

      qDebug() << "Download icon warning";
    }

  }
}