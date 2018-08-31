﻿#pragma once

#include <QmlMessageAdapter.h>

#include <Features/TaskBarProgressHelper.h>

#include <Marketing/MarketingTarget.h>

#include <Core/Service.h>

#include <RestApi/FakeCache.h>
#include <RestApi/ProtocolOneCredential.h>
#include <RestApi/RestApiManager.h>

#include <AutoRunHelper/UACHelper.h>

#include <Application/ArgumentParser.h>

#include <QtCore/QDebug>
#include <QtCore/QObject>
#include <QtCore/QUrl>
#include <QtCore/QScopedPointer>
#include <QtWidgets/QApplication>
#include <QtWidgets/QMainWindow>

#include <QQuickView>

#include <QDesktopServices>
#include <QMouseEvent>

#include "KeyboardLayoutHelper.h"

#include <Host/Bridge/DownloaderBridge.h>

/*!
\class MQDeclarativeView

\brief Позволяет получить глобальный клик мыши. Т.к. к сожалению QTBUG-13007 
http://qt-project.org/forums/viewthread/1468
*/

using namespace Features;

class BestInstallPath;

class DownloaderBridgeProxy;
class DownloaderSettingsBridgeProxy;
class ServiceSettingsBridgeProxy;
class ExecutorBridgeProxy;
class UpdateManagerBridgeProxy;
class ApplicationBridgeProxy;
class ApplicationStatisticBridgeProxy;
class ConnectionBridgeProxy;
class LicenseManagerBridgeProxy;

class SettingsViewModel;
class GameSettingsViewModel;

namespace Features {
  class RenderRateHack;
}

namespace P1 {
  namespace Host {
    class ClientConnection;
  }
}

class MainWindow : public QQuickView
{
  Q_OBJECT
  Q_PROPERTY(QString language READ language NOTIFY languageChanged)
  Q_PROPERTY(QString fileVersion READ fileVersion NOTIFY fileVersionChanged) 


public:
  explicit MainWindow(QWindow *parent = 0);
  virtual ~MainWindow();

  QString language();
  const QString& fileVersion() { return _fileVersion; }
  Q_INVOKABLE void saveLanguage(const QString& language);
  Q_INVOKABLE void selectLanguage(const QString& language);
  void setFileVersion(const QString& fileVersion) { _fileVersion = fileVersion; }

  Q_INVOKABLE void setTaskbarIcon(const QString &iconSource);

public slots:
  /*
    Возвращает true если Host приложение проинициализировано, и обновлено
  */
  bool isInitCompleted();

  void switchClientVersion();

  void restartApplication(bool shouldStartWithSameArguments = true);
  void authSuccessSlot(const QString& userId, const QString& appKey, const QString& cookie);

  void openExternalUrlWithAuth(const QString& url);
  void openExternalUrl(const QString& url);
  void logout();

  void downloadButtonStart(QString serviceId);
  void downloadButtonPause(QString serviceId);

  void uninstallService(const QString serviceId);
  void cancelServiceUninstall(const QString serviceId);

  bool isDownloading(QString serviceId);  

  void commandRecieved(QString name, QStringList arguments);       
  void updateFinishedSlot();
  void activateWindow();
  bool executeService(QString id);
  bool executeSecondService(QString id, QString userId, QString appKey);
  void terminateSecondService();

  bool isWindowVisible();

  bool anyLicenseAccepted();
  bool isLicenseAccepted(const QString& serviceId);
  QString startingService();
  QString getExpectedInstallPath(const QString& serviceId);
  QString getBestInstallPath(const QString& serviceId);
  void setServiceInstallPath(const QString& serviceId, const QString& path);
  void acceptFirstLicense(const QString& serviceId);
  void initFinished();
  void initialize();

  void hideToTaskBar();

  void onTaskbarButtonCreated();
  void onProgressUpdated(int progressValue, const QString &status);

  void onLanguageChanged();

  void onWindowClose();
  void restartUISlot(bool minimized);
  void shutdownUISlot();

  void terminateGame(const QString& serviceId);

signals:
  /*
    Вызывается когда хост закончил инициализацию и обновление
  */
  void initCompleted();
  void restartUIRequest();
  void shutdownUIRequest();

  void nickNameChanged();
  void nickNameValueChanged(QString &value);
  void techNameChanged();
  void techNameValueChanged(QString &value);

  void mediumAvatarUrlChanged(); 
  void languageChanged();
  void fileVersionChanged();

  void navigate(QString page);

  void totalProgressChanged(QString serviceId, int progress);
  void rehashProgressChanged(QString serviceId, int progress, int rehashProgress);
  void downloadProgressChanged(QString serviceId, int progress, qint64 totalWantedDone, qint64 totalWanted,
    qint64 directTotalDownload,
    qint64 peerTotalDownload,
    qint64 payloadTotalDownload,
    int peerPayloadDownloadRate,
    int payloadDownloadRate,
    int directPayloadDownloadRate,
    int payloadUploadRate,
    qint64 totalPayloadUpload);

  void secondInstanceExecuteRequest();

  void selectService(QString serviceId);
  void needPakkanenVerification(QString serviceId);

  void downloaderStarted(QString service, int startType);
  void downloaderFinished(QString service);
  void downloaderStopped(QString service);  
  void downloaderStopping(QString service);
  void downloaderFailed(QString service);
  void downloaderServiceStatusMessageChanged(QString service, QString message);

  void torrentListenPortChanged(QString port);

  void downloadButtonStartSignal(QString serviceId);
  void updateFinished();

  void leftMousePress(int globalX, int globalY); 
  void leftMouseRelease(int globalX, int globalY);  

  void closeMainWindow();

  void serviceStarted(QString service);
  void serviceFinished(QString service, int serviceState);
  void serviceInstalled(QString serviceId);

  void secondServiceStarted(QString service);
  void secondServiceFinished(QString service, int serviceState);

  void authBeforeStartGameRequest(QString serviceId);
  void authGuestConfirmRequest(QString serviceId);

  void taskBarButtonMsgRegistered(unsigned int msgId);

  void showLicense(QString serviceId);
  void showWebLicense(QString serviceId);
  void quit();
  void wrongCredential(const QString& userId);

  void gameDownloaderAccessRequired(const QString& serviceId);
  void uninstallServiceRequest(QString serviceId);
  void additionalResourcesReady();

private slots:
  void onServiceStarted(const QString &serviceId);
  void onServiceFinished(const QString &serviceId, int state);

  void onSecondServiceStarted(const QString &serviceId);
  void onSecondServiceFinished(const QString &serviceId, int state);

  void downloadGameTotalProgressChanged(const QString& serviceId, int progress);
  void downloadGameProgressChanged(
    const QString& serviceId, 
    int progress, 
    P1::Host::Bridge::DownloadProgressArgs args);

  void gameDownloaderStarted(const QString& serviceId, int startType);
  void gameDownloaderStopped(const QString& serviceId);
  void gameDownloaderStopping(const QString& serviceId);
  void gameDownloaderFailed(const QString& serviceId); 
  void gameDownloaderFinished(const QString& serviceId);
  void gameDownloaderStatusMessageChanged(const QString& serviceId, const QString& message);
  void gameDownloaderServiceInstalled(const QString& serviceId); 
  void gameDownloaderServiceUpdated(const QString& serviceId); 
  void removeStartGame(QString serviceId);

  void restApiGenericError(
    P1::RestApi::CommandBase::Error error,
    QString message,
    P1::RestApi::CommandBase *command);

private:
  void sendStartingMarketing();
  
  void initRestApi();
  void initMarketing();

  void checkDesktopDepth();

  void startGame(const QString& serviceId);

  void prepairGameDownloader();
  void postUpdateInit();
  void onApplicationStateChanged(Qt::ApplicationState state);

  P1::RestApi::FakeCache _fakeCache;
  P1::RestApi::ProtocolOneCredential _credential;
  P1::RestApi::RestApiManager _restapiManager;

  QmlMessageAdapter* messageAdapter;
  SettingsViewModel* settingsViewModel;
  GameSettingsViewModel *_gameSettingsViewModel;

  QString _language;
  QString _fileVersion;

  //MQDeclarativeView *nQMLContainer;

  bool m_WindowState; // false - normal size, true - max size  
  P1::Application::ArgumentParser _commandLineArguments;
  P1::Core::Service::Area _gameArea;
  P1::KeyboardLayoutHelper _keyboardLayoutHelper;

  QMap<QString,QTranslator*> translators;
  P1::GameDownloader::GameDownloadService _gameDownloader;

  P1::Core::Service::Area _applicationArea;

  P1::Marketing::MarketingTarget _marketingTargetFeatures;

  Features::TaskBarHelper _taskBarHelper;

  DownloaderBridgeProxy *_downloader;
  DownloaderSettingsBridgeProxy *_downloaderSettings;
  ServiceSettingsBridgeProxy *_serviceSettings;
  ExecutorBridgeProxy *_executor;
  ApplicationBridgeProxy* _applicationProxy;
  ApplicationStatisticBridgeProxy* _applicationStatistic;
  LicenseManagerBridgeProxy* _licenseManager;
  P1::Host::ClientConnection *_clientConnection;
  Features::RenderRateHack *_renderRateHack;

  BestInstallPath *_bestInstallPath;

protected:
  bool event(QEvent* event);
  bool nativeEvent(const QByteArray & eventType, void * message, long * result) override;

  virtual void showEvent(QShowEvent* event);

  void mousePressEvent(QMouseEvent * event) override;
  void mouseReleaseEvent(QMouseEvent * event) override;

};