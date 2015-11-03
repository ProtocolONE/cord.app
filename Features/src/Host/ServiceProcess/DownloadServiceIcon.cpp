#include <Host/ServiceProcess/DownloadServiceIcon.h>

#include <Settings/Settings.h>
#include <Features/GameNetDownloader.h>

#include <QtCore/QCoreApplication>
#include <QtCore/QFile>
#include <QtCore/QDebug>
#include <QtCore/QTimer>
#include <QtCore/QStandardPaths>

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
        this->_downloadQueue.enqueue(QPair<QString, QString>(url, serviceId));
      }

      void DownloadServiceIcon::start()
      {
        this->processNext();
      }

      void DownloadServiceIcon::processNext()
      {
        if (this->_downloadQueue.size() == 0) {
          emit this->finished();
          return;
        }

        QPair<QString, QString> item = this->_downloadQueue.dequeue();
        this->download(item.first, item.second);
      }

      void DownloadServiceIcon::download(const QString& url, const QString& serviceId)
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
          if (httpCode != 304 && httpCode != 200) {
            this->requestNext();
            return;
          }

          QString lastModified = QString::fromLatin1(reply->rawHeader(QByteArray("Last-Modified")));
          QString filePath = QString("%1/icons/%2.ico")
            .arg(QStandardPaths::writableLocation(QStandardPaths::DataLocation)).arg(serviceId);

          bool changed = !QFile::exists(filePath) || httpCode == 200;
          if (!changed) {
            this->requestNext();
            return;
          }

          GameNetDownloader* downloader = new GameNetDownloader(this);
          auto* baseClass = this;
          QString localServiceId = serviceId;

          QObject::connect(downloader, &GameNetDownloader::error,
            [baseClass, downloader](){
            downloader->deleteLater();
            qDebug() << "Download icon warning";
            baseClass->requestNext();
          });

          QObject::connect(downloader, &GameNetDownloader::downloaded,
            [baseClass, downloader, localServiceId, lastModified](){
            baseClass->saveLastModifed(localServiceId, lastModified);
            downloader->deleteLater();
            baseClass->requestNext();
          });

          downloader->download(url, filePath);
        });
      }

      void DownloadServiceIcon::requestNext()
      {
        QTimer::singleShot(0, this, &DownloadServiceIcon::processNext);
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