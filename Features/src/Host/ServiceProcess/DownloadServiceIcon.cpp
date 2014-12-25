#include <Host/ServiceProcess/DownloadServiceIcon.h>

#include <Settings/Settings.h>
#include <Features/GameNetDownloader.h>

#include <QtCore/QCoreApplication>
#include <QtCore/QFile>
#include <QtCore/QDebug>

namespace GameNet {
  namespace Host {
    namespace ServiceProcess {

      using GameNet::GameNetDownloader;

      DownloadServiceIcon::DownloadServiceIcon(QObject *parent /*= 0*/)
        : QObject(parent)
      {
      }

      DownloadServiceIcon::~DownloadServiceIcon()
      {
      }

      void DownloadServiceIcon::downloadIconRequest(const QString& url, const QString& serviceId)
      {
        QString lastModified = this->readLastModified(serviceId);

        QNetworkRequest request(url);
        if (!lastModified.isEmpty())
          request.setRawHeader("If-Modified-Since", lastModified.toLatin1());

        QNetworkReply *reply = this->_networkManager.head(request);

        QObject::connect(reply, &QNetworkReply::finished, [this, serviceId, url, reply](){
          reply->deleteLater();

          int httpCode = reply->attribute(QNetworkRequest::HttpStatusCodeAttribute).toInt();
          // Http codes defined by rfc: http://www.w3.org/Protocols/rfc2616/rfc2616-sec10.html
          // 200 OK
          // 304 Not Modified
          if (httpCode != 304 && httpCode != 200)
            return;

          QString lastModified = QString::fromLatin1(reply->rawHeader(QByteArray("Last-Modified")));
          QString filePath = QString("%1/Assets/Images/icons/%2.ico").arg(QCoreApplication::applicationDirPath()).arg(serviceId);
          bool changed = !QFile::exists(filePath) || httpCode == 200;
          if (!changed)
            return;

          GameNetDownloader* downloader = new GameNetDownloader(this);
          auto* baseClass = this;
          QString localServiceId  = serviceId;

          QObject::connect(downloader, &GameNetDownloader::error,
            [downloader](){
              downloader->deleteLater();
              qDebug() << "Download icon warning";
          });

          QObject::connect(downloader, &GameNetDownloader::downloaded,
            [baseClass, downloader, localServiceId, lastModified](){
              baseClass->saveLastModifed(localServiceId, lastModified);
              downloader->deleteLater();
          });  

          downloader->download(url, filePath);
        });
      }

      QString DownloadServiceIcon::readLastModified(const QString& path)
      {
        GGS::Settings::Settings settings; 
        settings.beginGroup("DownloadServiceIconHelper");
        settings.beginGroup("Icons");
        settings.beginGroup(path);
        return settings.value("lastModified", "").toString();
      }

      void DownloadServiceIcon::saveLastModifed(const QString& path, const QString& value)
      {
        GGS::Settings::Settings settings; 
        settings.beginGroup("DownloadServiceIconHelper");
        settings.beginGroup("Icons");
        settings.beginGroup(path);
        settings.setValue("lastModified", value);
      }
    }
  }
}