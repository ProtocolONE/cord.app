#pragma once

#include <QtCore/QObject>
#include <QtCore/QString>
#include <QtCore/QMap>

namespace Features {
  class StopDownloadServiceWhileExecuteAnyGame;
}

namespace P1 {
  namespace Host {

    class Application;
    class Connection;
    class MutexHandle;

    namespace DBus {
      class DBusServer;
    }

    class ConnectionManager : public QObject
    {
      Q_OBJECT
    public:
      explicit ConnectionManager(QObject *parent = 0);
      virtual ~ConnectionManager();

      void setApplication(Application *value);
      void setDbusServer(DBus::DBusServer* value);
      void setStopDownloadServiceWhileExecuteAnyGame(Features::StopDownloadServiceWhileExecuteAnyGame* value);

      bool init();
      void shutdown();

      bool hasLauncher();

    signals:
      void newConnection(Connection *connection);

      void zzimaDisabled();
      
    private slots:
      void onInitCompleted();

    private:
      void onClientDisconnected();

      void connectionInfoReceived();

      void registerServicesForLauncher(Connection * connection);

      void registerDownloader(Connection* connection);
      void registerMessageAdapter(Connection* connection);
      void registerApplication(Connection* connection);
      void registerApplicationStatistic(Connection* connection);
      void registerDownloaderSettings(Connection* connection);
      void registerServiceSettings(Connection* connection);
      void registerExecutor(Connection* connection);
      void registerUpdateManager(Connection * connection);
      void registerServiceHandle(Connection * connection);
      void registerLicenseManager(Connection * connection);
      void registerSettingsManager(Connection * connection);

#ifdef ZZIMA_INTEGRATION
      void registerServicesForZzima(Connection * connection);
      void registerZzimaExecutor(Connection* connection);
#endif

      Application *_application;
      QMap<QString, Connection*> _connections;
      MutexHandle *_sharedMutex;
      DBus::DBusServer* _server;
      Features::StopDownloadServiceWhileExecuteAnyGame *_stopDownloadServiceOnExecuteGame;
    };

  }
}
