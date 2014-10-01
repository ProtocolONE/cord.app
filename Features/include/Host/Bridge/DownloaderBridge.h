#pragma once

#include <GameDownloader/GameDownloadService.h>

#include <QtCore/QObject>
#include <QtCore/QString>
#include <QtCore/QMetaType>
#include <QtDBus/QDBusArgument>

namespace GGS {
  namespace Core {
    class Service;
  }

  namespace GameDownloader {
    class GameDownloadService;
  }
}

namespace GameNet {
  namespace Host {
    class ServiceLoader;

    namespace Bridge {

      typedef struct {
        QString id;
        int status;
        double progress;

        int downloadRate;
        int uploadRate;

        qint64 totalWanted;
        qint64 totalWantedDone;

        qint64 directTotalDownload;
        qint64 peerTotalDownload;

        qint64 payloadTotalDownload;
        int peerPayloadDownloadRate;
        int payloadDownloadRate;
        int directPayloadDownloadRate;
        int payloadUploadRate;
        qint64 totalPayloadUpload; 
      } DownloadProgressArgs;
      Q_DECLARE_METATYPE(DownloadProgressArgs)

      class DownloaderBridge : public QObject
      {
        Q_OBJECT
        Q_CLASSINFO("Version", "1.0.0.0")
        Q_CLASSINFO("D-Bus Interface", "com.gamenet.dbus.Downloader")
      public:
        explicit DownloaderBridge(QObject *parent = 0);
        virtual ~DownloaderBridge();

        void setDownloader(GGS::GameDownloader::GameDownloadService *downloader);
        void setServiceLoader(ServiceLoader *serviceLoader);

      public slots:
        bool isInProgress(const QString& serviceId) const;
        bool isAnyServiceInProgress() const;
        bool isInstalled(const QString& serviceId) const;

        void start(const QString& serviceId, int startType);
        void stop(const QString& serviceId);

        void pauseSession();
        void resumeSession();
        
      signals:
        void started(const QString& serviceId, int startType);
        void finished(const QString& serviceId);
        void stopped(const QString& serviceId);
        void stopping(const QString& serviceId);
        void failed(const QString& serviceId);

        void serviceInstalled(const QString& serviceId);
        void serviceUpdated(const QString& serviceId);
          
        void statusMessageChanged(const QString& serviceId, const QString& message);

        void totalProgress(const QString& serviceId, int progress);

        /* INFO вот примерно такой элемент надо добавлять в xml для этого сигнала
           Тип a(sii) по факту не соответствует действительности. Понять надо ли это править.
           Вероятно стоит написать батничек для вставки из коментариев сигналов с кастомными параметрами.

        <signal name="downloadProgress">
        <arg name="serviceId" type="s" direction="out"/>
        <arg name="progress" type="i" direction="out"/>
        <arg name="args" type="a(sii)" direction="out"/>
        <annotation name="org.qtproject.QtDBus.QtTypeName.In2" value="GameNet::Host::Bridge::DownloadProgressArgs"/>
        </signal>

        */
        void downloadProgress(
          const QString& serviceId,
          int progress, 
          const GameNet::Host::Bridge::DownloadProgressArgs& args);
      
      private:
        GGS::GameDownloader::GameDownloadService *_downloader;
        ServiceLoader *_serviceLoader;

        void onStarted(const GGS::Core::Service *service, GGS::GameDownloader::StartType startType);
        void onFinished(const GGS::Core::Service *service);
        void onStopped(const GGS::Core::Service *service);
        void onStopping(const GGS::Core::Service *service);
        void onFailed(const GGS::Core::Service *service);
        
        void onServiceInstalled(const GGS::Core::Service *service);
        void onServiceUpdated(const GGS::Core::Service *service);
        void onStatusMessageChanged(const GGS::Core::Service *service, const QString& message);

        void onTotalProgress(const GGS::Core::Service *service, qint8 progress);
        void onDownloadProgress(
          const GGS::Core::Service *service, 
          qint8 progress, 
          const GGS::Libtorrent::EventArgs::ProgressEventArgs& args);
      };

    }
  }
}

QDBusArgument& operator <<(QDBusArgument &argument, const GameNet::Host::Bridge::DownloadProgressArgs& arg);
const QDBusArgument& operator >>(const QDBusArgument &argument, GameNet::Host::Bridge::DownloadProgressArgs& arg);

