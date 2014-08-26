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
      public:
        explicit DownloaderBridge(QObject *parent = 0);
        virtual ~DownloaderBridge();

        void setDownloader(GGS::GameDownloader::GameDownloadService *downloader);
        void setServiceLoader(ServiceLoader *serviceLoader);

      public slots:
        // есть ощущение что эти парамтры должны быть у настроек
        // ”правление настройками торрента теперь осуществ€летс€ через GameDownloadService
        // void setListeningPort(unsigned short port);
        // void setTorrentConfigDirectoryPath(const QString& path);
        // void changeListeningPort(unsigned short port);
        // void setUploadRateLimit(int bytesPerSecond);
        // void setDownloadRateLimit(int bytesPerSecond);
        // void setMaxConnection(int maxConnection);
        // void setSeedEnabled(bool value);

        bool isInProgress(const QString& serviceId);
        bool isAnyServiceInProgress();
        bool isInstalled(const QString& serviceId);

        void start(const QString& serviceId, int startType);
        void stop(const QString& serviceId);
        
        // может как то не так - а например setDirectory(id, path)
        // хот€ папок можно быть несколько - обсудить
        void directoryChanged(const QString& serviceId);

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

        void totalProgress(QString serviceId, int progress);

        /* INFO вот примерно такой элемент надо добавл€ть в xml дл€ этого сигнала
           “ип a(sii) по факту не соответствует действительности. ѕон€ть надо ли это править.
           ¬еро€тно стоит написать батничек дл€ вставки из коментариев сигналов с кастомными параметрами.

        <signal name="downloadProgress">
        <arg name="serviceId" type="s" direction="out"/>
        <arg name="progress" type="i" direction="out"/>
        <arg name="args" type="a(sii)" direction="out"/>
        <annotation name="org.qtproject.QtDBus.QtTypeName.In2" value="DownloadProgressArgs"/>
        </signal>

        */
        void downloadProgress(QString serviceId, int progress, DownloadProgressArgs& args);

      //
      //    void listeningPortChanged(unsigned short port);

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
          GGS::Libtorrent::EventArgs::ProgressEventArgs args);
      };

    }
  }
}


static QDBusArgument& operator <<(QDBusArgument &argument, const GameNet::Host::Bridge::DownloadProgressArgs arg)
{
  argument.beginStructure();
  argument 
    << arg.id
    << arg.status
    << arg.progress

    << arg.downloadRate
    << arg.uploadRate

    << arg.totalWanted
    << arg.totalWantedDone

    << arg.directTotalDownload
    << arg.peerTotalDownload

    << arg.payloadTotalDownload
    << arg.peerPayloadDownloadRate
    << arg.payloadDownloadRate
    << arg.directPayloadDownloadRate
    << arg.payloadUploadRate
    << arg.totalPayloadUpload;

  argument.endStructure();
  return argument;
}

static const QDBusArgument& operator >>(const QDBusArgument &argument, GameNet::Host::Bridge::DownloadProgressArgs& arg)
{
  argument.beginStructure();
  argument 
    >> arg.id
    >> arg.status
    >> arg.progress

    >> arg.downloadRate
    >> arg.uploadRate

    >> arg.totalWanted
    >> arg.totalWantedDone

    >> arg.directTotalDownload
    >> arg.peerTotalDownload

    >> arg.payloadTotalDownload
    >> arg.peerPayloadDownloadRate
    >> arg.payloadDownloadRate
    >> arg.directPayloadDownloadRate
    >> arg.payloadUploadRate
    >> arg.totalPayloadUpload;;
  argument.endStructure();
  return argument;
}

