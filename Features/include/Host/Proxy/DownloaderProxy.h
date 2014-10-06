#pragma once

#include <GameDownloader/GameDownloadService.h>

#include <RestApi/GameNetCredential.h>

#include <QtCore/QObject>
#include <QtCore/QHash>

namespace GGS {
  namespace Core {
    class Service;
  }
}

namespace GameNet {
  namespace Host {

    class Application;

    namespace Proxy {

      class DownloaderProxy : public GGS::GameDownloader::GameDownloadService
      {
        Q_OBJECT
      public:
        explicit DownloaderProxy(QObject *parent = 0);
        virtual ~DownloaderProxy();

        void init();

        void setApplication(Application* value);

        virtual void startWithName(
          const QString& connectionName,
          const GGS::Core::Service *service,
          GGS::GameDownloader::StartType startType);

      signals:
        void startedWithCredential(
          const QString& serviceId, 
          const GGS::RestApi::GameNetCredential credetial);

        void finishedDownloadingWithCredential(
          const QString& serviceId, 
          const GGS::RestApi::GameNetCredential credetial);

        void finishedWithCredential(
          const QString& serviceId, 
          const GGS::RestApi::GameNetCredential credetial);

      private:
        void internalFinished(const GGS::Core::Service* service);
        void internalStarted(const GGS::Core::Service* service);
        void internalDownloaded(const GGS::Core::Service* service);

        Application *_application;
        QHash<QString, GGS::RestApi::GameNetCredential> _credentialMap;
      };

    }
  }
}