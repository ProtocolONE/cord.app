#include <Host/ConnectionManager.h>

#ifndef USE_SESSION_DBUS
#include <Host/Dbus/DBusServer.h>
#endif

#include <Host/Connection.h>
#include <Host/Application.h>
#include <Host/MarketingStatistic.h>
#include <Host/ServiceHandle.h>

#include <Host/Proxy/GameExecutorProxy.h>
#include <Host/Proxy/DownloaderProxy.h>
#include <Host/Proxy/ServiceHandleProxy.h>

#include <Host/Bridge/DownloaderBridge.h>
#include <Host/Bridge/DownloaderSettingsBridge.h>
#include <Host/Bridge/ServiceSettingsBridge.h>
#include <Host/Bridge/ExecutorBridge.h>
#include <Host/Bridge/UpdateManagerBridge.h>
#include <Host/Bridge/ApplicationBridge.h>
#include <Host/Bridge/ApplicationStatisticBridge.h>
#include <Host/Bridge/MessageAdapterBridge.h>
#include <Host/Bridge/ServiceHandleBridge.h>

#include <Host/Dbus/DBusServer.h>
#include <Host/Dbus/DownloaderBridgeAdaptor.h>
#include <Host/Dbus/DownloaderSettingsBridgeAdaptor.h>
#include <Host/Dbus/ServiceSettingsBridgeAdaptor.h>
#include <Host/Dbus/ExecutorBridgeAdaptor.h>
#include <Host/Dbus/UpdateManagerBridgeAdaptor.h>
#include <Host/Dbus/ApplicationBridgeAdaptor.h>
#include <Host/Dbus/ApplicationStatisticBridgeAdaptor.h>
#include <Host/Dbus/MessageAdapterBridgeAdaptor.h>
#include <Host/Dbus/ServiceHandleBridgeAdaptor.h>

#include <RestApi/RestApiManager.h>

#include <QtDBus/QDBusConnection>

using GameNet::Host::DBus::DBusServer;
using GGS::RestApi::RestApiManager;

namespace GameNet {
  namespace Host {

    ConnectionManager::ConnectionManager(QObject *parent /*= 0*/)
      : QObject(parent)
      , _application(nullptr)
    {
    }

    ConnectionManager::~ConnectionManager()
    {
    }

    void ConnectionManager::setApplication(Application *value)
    {
      Q_ASSERT(value);
      this->_application = value;
    }

    bool ConnectionManager::init()
    {
      Q_ASSERT(this->_application);

      qRegisterMetaType<GameNet::Host::Bridge::DownloadProgressArgs>("GameNet::Host::Bridge::DownloadProgressArgs");
      qDBusRegisterMetaType<GameNet::Host::Bridge::DownloadProgressArgs>();

      qRegisterMetaType<GameNet::Host::Bridge::Credential>("GameNet::Host::Bridge::Credential");
      qDBusRegisterMetaType<GameNet::Host::Bridge::Credential>();

#ifdef USE_SESSION_DBUS
      Connection *connection = new Connection(QDBusConnection::sessionBus(), this);

      QObject::connect(connection, &Connection::connectionInfoReceived,
        this, &ConnectionManager::connectionInfoReceived);

      QObject::connect(this->_application->_restApiManager, &RestApiManager::genericErrorEx,
        connection, &Connection::onGenericError);

      // INFO ignored ping-pong disconnect.

      connection->setApplicationName("QGNA");
#else
      DBusServer *server = new DBusServer(this);
      if (!server->isConnected())
        return false;
      
      QObject::connect(server, &DBusServer::newConnection, [this](const QDBusConnection &constConnection) {
        qDebug() << "New IPC connection with name" << constConnection.name() << "accepted";
        
        Connection *connection = new Connection(constConnection, this);

        QObject::connect(connection, &Connection::connectionInfoReceived,
          this, &ConnectionManager::connectionInfoReceived);

        QObject::connect(this->_application->_restApiManager, &RestApiManager::genericErrorEx,
          connection, &Connection::onGenericError);

        QObject::connect(connection, &Connection::disconnected,
          this, &ConnectionManager::onClientDisconnected);
      });
#endif

      return true;
    }

    void ConnectionManager::connectionInfoReceived()
    {
      Connection *connection = qobject_cast<Connection *>(QObject::sender());
      const QString& applicationName = connection->applicationName();
      this->_connections[applicationName] = connection;

      if (applicationName == "QGNA")
        this->registerServicesForQGNA(connection);
    }

    void ConnectionManager::registerServicesForQGNA(Connection * connection)
    {
      this->registerApplicationStatistic(connection);
      this->registerApplication(connection);
      this->registerMessageAdapter(connection);
      this->registerDownloader(connection);
      this->registerDownloaderSettings(connection);
      this->registerServiceSettings(connection);
      this->registerExecutor(connection);
      this->registerUpdateManager(connection);
      this->registerServiceHandle(connection);
    }

    void ConnectionManager::registerApplicationStatistic(Connection* connection)
    {
      Bridge::ApplicationStatisticBridge *applicationStatisticBridge = new Bridge::ApplicationStatisticBridge(connection);
      applicationStatisticBridge->setApplicationStatistic(this->_application->_applicationStatistic);

      new ApplicationStatisticBridgeAdaptor(applicationStatisticBridge);
      connection->registerObject("/applicationStatistic", applicationStatisticBridge);
    }

    void ConnectionManager::registerApplication(Connection* connection)
    {
      Bridge::ApplicationBridge *applicationBridge = new Bridge::ApplicationBridge(connection);
      applicationBridge->setApplication(this->_application);
      applicationBridge->setThetta(this->_application->_thetta);
      applicationBridge->setTranslation(this->_application->_translation);

      new ApplicationBridgeAdaptor(applicationBridge);
      connection->registerObject("/application", applicationBridge);
    }

    void ConnectionManager::registerMessageAdapter(Connection* connection)
    {
      Bridge::MessageAdapterBridge *messageAdapterBridge = new Bridge::MessageAdapterBridge(connection);
      messageAdapterBridge->setAdapter(this->_application->_messageAdapter);

      new MessageAdapterBridgeAdaptor(messageAdapterBridge);
      connection->registerObject("/messageAdapter", messageAdapterBridge);
    }

    void ConnectionManager::registerDownloader(Connection* connection)
    {
      Proxy::DownloaderProxy *downloader = new Proxy::DownloaderProxy(connection);
      downloader->setConnection(connection);
      downloader->setDownloader(this->_application->_gameDownloader);
      downloader->setServiceHandle(this->_application->_serviceHandle);

      QObject::connect(downloader, &Proxy::DownloaderProxy::started,
        this->_application->_marketingStatistic, &MarketingStatistic::onGameDownloadStarted);

      QObject::connect(downloader, &Proxy::DownloaderProxy::finished,
        this->_application->_marketingStatistic, &MarketingStatistic::onGameDownloadFinished);

      QObject::connect(downloader, &Proxy::DownloaderProxy::finishedDownloading,
        this->_application->_marketingStatistic, &MarketingStatistic::onGameTorrentDownloadFinished);

      Bridge::DownloaderBridge *downloaderBridge = new Bridge::DownloaderBridge(connection);
      downloaderBridge->setServiceLoader(this->_application->_serviceLoader);
      downloaderBridge->setDownloader(downloader);

      new DownloaderBridgeAdaptor(downloaderBridge);
      connection->registerObject("/downloader", downloaderBridge);
    }

    void ConnectionManager::registerDownloaderSettings(Connection* connection)
    {
      Bridge::DownloaderSettingsBridge *downloaderSettingsBridge = new Bridge::DownloaderSettingsBridge(connection);
      downloaderSettingsBridge->setDownloaderSettings(this->_application->_downloaderSettings);

      new DownloaderSettingsBridgeAdaptor(downloaderSettingsBridge);
      connection->registerObject("/downloader/settings", downloaderSettingsBridge);
    }

    void ConnectionManager::registerServiceSettings(Connection * connection)
    {
      Bridge::ServiceSettingsBridge* serviceSettingsBridge = new Bridge::ServiceSettingsBridge(connection);
      serviceSettingsBridge->setSettings(this->_application->_serviceSettings);

      new ServiceSettingsBridgeAdaptor(serviceSettingsBridge);
      connection->registerObject("/serviceSettings", serviceSettingsBridge);
    }

    void ConnectionManager::registerExecutor(Connection * connection)
    {
      Proxy::GameExecutorProxy *executor = new Proxy::GameExecutorProxy(connection);
      executor->setConnection(connection);
      executor->setExecutor(this->_application->_executor);
      executor->setServiceHandle(this->_application->_serviceHandle);

      QObject::connect(executor, &Proxy::GameExecutorProxy::serviceStarted,
        this->_application->_marketingStatistic, &MarketingStatistic::onServiceStarted);

      QObject::connect(executor, &Proxy::GameExecutorProxy::serviceFinished,
        this->_application->_marketingStatistic, &MarketingStatistic::onServiceFinished);

      QObject::connect(executor, &Proxy::GameExecutorProxy::secondServiceStarted,
        this->_application->_marketingStatistic, &MarketingStatistic::onSecondServiceStarted);

      QObject::connect(executor, &Proxy::GameExecutorProxy::secondServiceFinished,
        this->_application->_marketingStatistic, &MarketingStatistic::onSecondServiceFinished);

      Bridge::ExecutorBridge* excutorBridge = new Bridge::ExecutorBridge(connection);
      excutorBridge->setExecutor(executor);

      new ExecutorBridgeAdaptor(excutorBridge);
      connection->registerObject("/executor", excutorBridge);
    }

    void ConnectionManager::registerUpdateManager(Connection* connection)
    {
      Bridge::UpdateManagerBridge *updateManagerBridge = new Bridge::UpdateManagerBridge(connection);
      updateManagerBridge->setUpdateManager(this->_application->_updater);

      new UpdateManagerBridgeAdaptor(updateManagerBridge);
      connection->registerObject("/updater", updateManagerBridge);
    }

    void ConnectionManager::registerServiceHandle(Connection * connection)
    {
      Proxy::ServiceHandleProxy *serviceHandle = new Proxy::ServiceHandleProxy(connection);
      serviceHandle->setConnection(connection);
      serviceHandle->setServiceHandle(this->_application->_serviceHandle);

      Bridge::ServiceHandleBridge *serviceHandleBridge = new Bridge::ServiceHandleBridge(connection);
      serviceHandleBridge->setServiceHandle(serviceHandle);

      new ServiceHandleBridgeAdaptor(serviceHandleBridge);
      connection->registerObject("/serviceHandle", serviceHandleBridge);
    }

    void ConnectionManager::shutdown()
    {
      //INFO При использовании peer-to-peer подключения нет необходимо дерегистрировать сервис. Он должен быть всякий
      //раз зарегистрирован для каждого нового подключения. Вызовем дерегистрацию, если используется отдельная шина.

      #ifdef USE_SESSION_DBUS
            QDBusConnection::sessionBus().unregisterService("com.gamenet.dbus");
      #endif
    }

    void ConnectionManager::onClientDisconnected()
    {
      Connection* connection = qobject_cast<Connection*>(QObject::sender());
      Q_ASSERT(connection);

      this->_connections.remove(connection->applicationName());
      this->_application->_serviceHandle->unlockAllForConnection(connection);

      connection->deleteLater();
    }

  }
}
