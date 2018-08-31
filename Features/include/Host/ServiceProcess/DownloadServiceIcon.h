#pragma once

#include <QtCore/QObject>
#include <QtCore/QPair>
#include <QtCore/QQueue>
#include <QtCore/QUrl>
#include <QtCore/QString>

#include <QtNetwork/QNetworkAccessManager>

namespace P1 {
  namespace Host {
    namespace ServiceProcess {

      class DownloadServiceIcon : public QObject
      {
        Q_OBJECT
      public:
        explicit DownloadServiceIcon(QObject *parent = 0);
        virtual ~DownloadServiceIcon();

        void downloadIconRequest(const QString& url, const QString& serviceId);
        void start();

      signals:
        void finished();

      private slots:
        void processNext();

      private:
        void download(const QString& url, const QString& serviceId);
        void requestNext();

        QString readLastModified(const QString& path);
        void saveLastModifed(const QString& path, const QString& value);

        QNetworkAccessManager _networkManager;
        QQueue< QPair <QString, QString > > _downloadQueue;

      };

    }
  }
}