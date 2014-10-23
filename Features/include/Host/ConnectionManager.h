#pragma once

#include <QtCore/QObject>
#include <QtCore/QString>
#include <QtCore/QMap>

namespace GameNet {
  namespace Host {

    class Application;
    class Connection;

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

      bool init();
      void shutdown();

    private:
      void onClientDisconnected();

      void connectionInfoReceived();

      void registerServicesForQGNA(Connection * connection);

      void registerDownloader(Connection* connection);
      void registerMessageAdapter(Connection* connection);
      void registerApplication(Connection* connection);
      void registerApplicationStatistic(Connection* connection);
      void registerDownloaderSettings(Connection* connection);
      void registerServiceSettings(Connection* connection);
      void registerExecutor(Connection* connection);
      void registerUpdateManager(Connection * connection);
      void registerServiceHandle(Connection * connection);

      Application *_application;
      QMap<QString, Connection*> _connections;

    };

  }
}
