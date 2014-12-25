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
}

namespace GameNet {
  namespace Host {
    namespace ServiceProcess {
      class ServiceLoader;
    }

    namespace Proxy {
      class DownloaderProxy;
    }

    namespace Bridge {

      /** \struct DownloadProgressArgs
      
      \brief Downloader state
      
      This structure represents Downloader state for a particular service (with Service Identification Number == id)

      */
      /** \var DownloadProgressArgs::id 

      Service identification number

      */
      /** \var DownloadProgressArgs::status 
      
      Current service status.

      May be one of the following:
      <ul>
      <li>0 - Downloader is waiting for check downloaded files
      <li>1 - Downloader is currently checking downloaded files
      <li>2 - Downloader is downloading service information
      <li>3 - downloading is in progress
      <li>4 - Downloader has finished downloading a particular service
      <li>5 - Downloader is currently uploading files to another peers
      <li>6 - Downloader is currently allocating hard disk space
      <li>7 - Downloader is checking download metadata, needed for downloading process to resume
      </ul>

      */
      /** \var DownloadProgressArgs::progress 
      
      Overall downloading progress.

      */
      /** \var DownloadProgressArgs::downloadRate 
      
      Total download speed in bytes, including payload and protocol overhead.

      */
      /** \var DownloadProgressArgs::uploadRate 

      Total upload speed in bytes, including payload and protocol overhead.
      
      */
      /** \var DownloadProgressArgs::totalWanted 
      
      Total amount of bytes to download.
      
      */
      /** \var DownloadProgressArgs::totalWantedDone 
      
      Amount of bytes already downloaded.
      
      */
      /** \var DownloadProgressArgs::directTotalDownload 

      Amount of bytes downloaded from http file servers.
      
      */
      /** \var DownloadProgressArgs::peerTotalDownload
      
      Amount of bytes downloaded from peers.

      */
      /** \var DownloadProgressArgs::payloadTotalDownload 
      
      Amount of bytes downloaded from peers and from http file servers both.

      */
      /** \var DownloadProgressArgs::peerPayloadDownloadRate 
      
      Download rate from peers.

      */
      /** \var DownloadProgressArgs::payloadDownloadRate 
      
      Overall download rate, involving peerPayloadDownloadRate and directPayloadDownloadRate.
      
      */
      /** \var DownloadProgressArgs::directPayloadDownloadRate 
      
      Only direct download speed (that means downloading directly from http file servers).

      */
      /** \var DownloadProgressArgs::payloadUploadRate 
      
      Upload speed in bytes per second.

      */
      /** \var DownloadProgressArgs::totalPayloadUpload 

      Total amount of bytes uploaded.
      
      */

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
      Q_DECLARE_METATYPE(DownloadProgressArgs);

      bool operator==(const DownloadProgressArgs &lhs, const DownloadProgressArgs &rhs);


      /*!
      \class DownloaderBridge

      \brief DownloaderBridge class provides an interface to control all necessary downloading activities.

      DownloaderBridge is a central part of service downloading subsystem. It provides methods for start\stop downloading a particular service.
      
      */ 
      class DownloaderBridge : public QObject
      {
        Q_OBJECT
        Q_CLASSINFO("Version", "1.0.0.0")
        Q_CLASSINFO("D-Bus Interface", "com.gamenet.dbus.Downloader")
      public:
        /*!
        \fn explicit DownloaderBridge(QObject *parent = 0);

        Creates a DownloaderBridge object with parent parent.
        
        */
        explicit DownloaderBridge(QObject *parent = 0);
        /*!
        \fn virtual ~DownloaderBridge();

        Destroys DownloaderBridge object instance.
        
        */
        virtual ~DownloaderBridge();

        /// \cond
        void setDownloader(Proxy::DownloaderProxy *downloader);
        void setServiceLoader(ServiceProcess::ServiceLoader *serviceLoader);
        /// \endcond
        
      public slots:
        /*!
        \fn bool isInProgress(const QString& serviceId) const;

        Method returns true if Downloader is currently downloading service with id == serviceId.
        
        \param  serviceId  service identification number
        */
        bool isInProgress(const QString& serviceId) const;

        /*!
        \fn bool isAnyServiceInProgress() const;

        Method returns true if one or more downloader tasks are running in a certain moment, false otherwise.
        Can be used for examining if any downloader activity is taking place.

        */
        bool isAnyServiceInProgress() const;

        /*!
        \fn bool isInstalled(const QString& serviceId) const;

        Method returns true if service with id == serviceId is installed, false otherwise.

        */
        bool isInstalled(const QString& serviceId) const;

        /*!
        \fn void start(const QString& serviceId, int startType);

        Starts (or resumes, if previously stopped) downloading of 

        \param serviceId  service identification number
        \param startType  additional parameter, describing start behavior. May be one of the following:
        <ul>
        <li> 0 - Normal
        <li> 1 - Fast check downloading metadata and then continue downloading
        <li> 2 - Rehash all downloaded data and then continue downloading
        </ul>

        */
        void start(const QString& serviceId, int startType);

        /*!
        \fn void stop(const QString& serviceId);

        Stops downloading service with specified id

        \param serviceId  service identification number

        */
        void stop(const QString& serviceId);

        /*!
        \fn void pauseSession();

        Pauses all downloader activity.

        */
        void pauseSession();

        /*!
        \fn bool void resumeSession();

        Resumes downloader activity.

        */
        void resumeSession();
        
      signals:
        /*!
        \fn void started(const QString& serviceId, int startType);

        This signal is emitted when service downloading is started.

        \param serviceId  service identification number
        \param startType  previously specified start behavior.

        \sa void start(const QString& serviceId, int startType);

        */
        void started(const QString& serviceId, int startType);

        /*!
        \fn void finished(const QString& serviceId);

        This signal is emitted when service downloading is completed.

        */
        void finished(const QString& serviceId);

        /*!
        \fn void stopped(const QString& serviceId);

        This signal is emitted when service downloading is stopped.
        Note that this signal is emitted when downloading is actually stopped.

        \sa void stopping(const QString& serviceId)

        */
        void stopped(const QString& serviceId);

        /*!
        \fn void stopping(const QString& serviceId);

        This signal is emitted as a reaction to stop command. 
        Service is not actually stopped at this moment.

        */
        void stopping(const QString& serviceId);

        /*!
        \fn void failed(const QString& serviceId);

        This signal is emitted as a reaction to stop command. 

        \sa void stopped(const QString& serviceId)

        */
        void failed(const QString& serviceId);


        /*!
        \fn void serviceInstalled(const QString& serviceId);

        Signal is emitted when service downloading is complete for the first time and it's ready to run.

        */
        void serviceInstalled(const QString& serviceId);

        /*!
        \fn void serviceUpdated(const QString& serviceId);

        Signal is emitted when previously installed service downloading is complete and it's ready to run.

        */
        void serviceUpdated(const QString& serviceId);
          
        /*!
        \fn void statusMessageChanged(const QString& serviceId, const QString& message);

        This signal is emitted when download status message for particular service (with id == serviceId) is changed.

        */
        void statusMessageChanged(const QString& serviceId, const QString& message);


        /*!
        \fn void totalProgress(const QString& serviceId, int progress);

        The signal is emitted when service overall progress changes. This is a cumulative value, including progress of download 

        */
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

        /*!
        \fn downloadProgress(
              const QString& serviceId,
              int progress, 
              const GameNet::Host::Bridge::DownloadProgressArgs& args);

        This signal is emitted when downloader state for particular service is changed.
        It may occur due to change of download progress, download/upload speed change, total downloaded size change, etc.

        */
        void downloadProgress(
          const QString& serviceId,
          int progress, 
          const GameNet::Host::Bridge::DownloadProgressArgs& args);
      
      private:
        Proxy::DownloaderProxy *_downloader;
        ServiceProcess::ServiceLoader *_serviceLoader;

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

