#pragma once

#include <QmlMessageAdapter.h>

#include <Features/RememberGameDownloading.h>
#include <Features/SilentMode.h>
#include <Features/TaskBarProgressHelper.h>

#include <Features/Thetta/ThettaInstaller.h>

#include <Marketing/MarketingTarget.h>

#include <Core/Service>

#include <RestApi/FakeCache>
#include <RestApi/GameNetCredential>
#include <RestApi/RestApiManager>

#include <AutoRunHelper/UACHelper.h>

#include <Application/ArgumentParser.h>

#include <QtCore/QDebug>
#include <QtCore/QObject>
#include <QtCore/QUrl>
#include <QtCore/QScopedPointer>
#include <QtWidgets/QApplication>
#include <QtWidgets/QMainWindow>
#include <QtDeclarative/QDeclarativeContext>
#include <QtDeclarative/QDeclarativeView>
#include <QtDeclarative/QDeclarativeEngine>
#include <QDesktopServices>
#include <QMouseEvent>

#include "KeyboardLayoutHelper.h"

#include <Host/Bridge/DownloaderBridge.h>

/*!
\class MQDeclarativeView

\brief Позволяет получить глобальный клик мыши. Т.к. QGNA-60 и к сожалению QTBUG-13007 
http://qt-project.org/forums/viewthread/1468
*/

using namespace Features;

class MQDeclarativeView: public QDeclarativeView    
{
  Q_OBJECT
public:
  MQDeclarativeView(QWidget *parent = 0) 
    : QDeclarativeView(parent)
  {
  }

protected:
  void mousePressEvent(QMouseEvent* event);

signals:
  void leftMouseClick(int globalX, int globalY); 
};

class BestInstallPath;

class DownloaderBridgeProxy;
class DownloaderSettingsBridgeProxy;
class ServiceSettingsBridgeProxy;
class ExecutorBridgeProxy;
class UpdateManagerBridgeProxy;
class ApplicationBridgeProxy;
class ApplicationStatisticBridgeProxy;
class ConnectionBridgeProxy;

class SettingsViewModel;
class GameSettingsViewModel;

namespace GameNet {
  namespace Host {
    class ClientConnection;
  }
}

class MainWindow : public QMainWindow
{
  Q_OBJECT

  Q_PROPERTY(QString language READ language NOTIFY languageChanged)
  Q_PROPERTY(QString fileVersion READ fileVersion NOTIFY fileVersionChanged) 

protected:
  //Next one overloaded due to QGNA-128
  virtual void showEvent(QShowEvent* event);

public:
  MainWindow(QWidget *parent = 0);
  ~MainWindow();

  const QString& installUpdateGnaPath() { return this->_installUpdateGnaPath; }
  const QString& updateArea() { return this->_updateArea; }
  const QString& updateUrl() { return this->_updateUrl; }

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

  bool isDownloading(QString serviceId);  

  void commandRecieved(QString name, QStringList arguments);       
  void updateFinishedSlot();
  void activateWindow();
  bool executeService(QString id);
  bool executeSecondService(QString id, QString userId, QString appKey);
  void terminateSecondService();

  void startBackgroundCheckUpdate();
  bool isWindowVisible();

  bool anyLicenseAccepted();
  bool isLicenseAccepted(const QString& serviceId);
  QString startingService();
  QString getExpectedInstallPath(const QString& serviceId);
  void setServiceInstallPath(const QString& serviceId, const QString& path);
  void acceptFirstLicense(const QString& serviceId);
  void initFinished();
  void initialize();

  void onTaskbarButtonCreated();
  void onProgressUpdated(int progressValue, const QString &status);

  void onLanguageChanged();

  bool silent();
  void onWindowClose();

private:
  void loadPlugin(QString pluginName);

  void translatorsParse();
  void initializeUpdateSettings();
  int checkUpdateInterval();

  void initAutorun();
  void initRestApi();
  void initMarketing();
  bool isUseOpenGLrender();

  void createShortcut(const QString& pathToLnk, const GGS::Core::Service* service);

  GGS::RestApi::FakeCache _fakeCache;
  GGS::RestApi::GameNetCredential _credential;
  GGS::RestApi::RestApiManager _restapiManager;

  QmlMessageAdapter* messageAdapter;
  SettingsViewModel* settingsViewModel;
  EnterNickNameViewModel *_enterNickViewModel;
  GameSettingsViewModel *_gameSettingsViewModel;

  QString _nickName;
  QString _techName;
  QString _mediumAvatarUrl;
  QString _language;
  QString _fileVersion;

  MQDeclarativeView *nQMLContainer;

  QPoint mLastMousePosition;
  bool m_WindowState; // false - normal size, true - max size  
  bool _gameDownloadInitialized;
  GGS::Application::ArgumentParser _commandLineArguments;
  GGS::Core::Service::Area _gameArea;
  GGS::KeyboardLayoutHelper _keyboardLayoutHelper;

signals:
  /*
    Вызывается когда хост закончил инициализацию и обновление
  */
  void initCompleted();

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

  void downloaderStarted(QString service);
  void downloaderFinished(QString service);
  void downloaderStopped(QString service);  
  void downloaderStopping(QString service);
  void downloaderFailed(QString service);
  void downloaderServiceStatusMessageChanged(QString service, QString message);

  void torrentListenPortChanged(QString port);

  void downloadButtonStartSignal(QString serviceId);
  void updateFinished();

  void leftMouseClick(int globalX, int globalY); 

  void closeMainWindow();
  void windowDeactivate();
  void windowActivate();

  void serviceStarted(QString service);
  void serviceFinished(QString service, int serviceState);
  void serviceInstalled(QString serviceId);

  void secondServiceStarted(QString service);
  void secondServiceFinished(QString service, int serviceState);

  void authBeforeStartGameRequest(QString serviceId);
  void authGuestConfirmRequest(QString serviceId);

  void taskBarButtonMsgRegistered(unsigned int msgId);

  void showLicense(QString serviceId);
  void quit();
  void wrongCredential(const QString& userId);

private slots:
  void onServiceStarted(const QString &serviceId);
  void onServiceFinished(const QString &serviceId, int state);

  void onSecondServiceStarted(const QString &serviceId);
  void onSecondServiceFinished(const QString &serviceId, int state);

  void onSystemBarPressed(int MouseX, int MouseY);
  void onSystemBarReleased(int MouseX, int MouseY);
  void onSystemBarPositionChanged(int MouseX, int MouseY);

  void downloadGameTotalProgressChanged(const QString& serviceId, int progress);
  void downloadGameProgressChanged(
    const QString& serviceId, 
    int progress, 
    GameNet::Host::Bridge::DownloadProgressArgs args);

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
    GGS::RestApi::CommandBase::Error error,
    QString message,
    GGS::RestApi::CommandBase *command);

private:
  void loadPlugin(QString pluginName);

  void initRestApi();
  void initMarketing();
  bool isUseOpenGLrender();

  void checkDesktopDepth();

  void startGame(const QString& serviceId);

  void prepairGameDownloader();
  void postUpdateInit();

  GGS::RestApi::FakeCache _fakeCache;
  GGS::RestApi::GameNetCredential _credential;
  GGS::RestApi::RestApiManager _restapiManager;

  QmlMessageAdapter* messageAdapter;
  SettingsViewModel* settingsViewModel;
  GameSettingsViewModel *_gameSettingsViewModel;

  QString _language;
  QString _fileVersion;

  MQDeclarativeView *nQMLContainer;

  QPoint mLastMousePosition;
  bool m_WindowState; // false - normal size, true - max size  
  GGS::Application::ArgumentParser _commandLineArguments;
  GGS::Core::Service::Area _gameArea;
  GGS::KeyboardLayoutHelper _keyboardLayoutHelper;

  QMap<QString,QTranslator*> translators;
  GGS::GameDownloader::GameDownloadService _gameDownloader;

  GGS::Core::Service::Area _applicationArea;

  RememberGameDownloading _rememberGameFeature;
  GGS::Marketing::MarketingTarget _marketingTargetFeatures;

  Features::TaskBarHelper _taskBarHelper;
  Features::SilentMode _silentMode;

  DownloaderBridgeProxy *_downloader;
  DownloaderSettingsBridgeProxy *_downloaderSettings;
  ServiceSettingsBridgeProxy *_serviceSettings;
  ExecutorBridgeProxy *_executor;
  ApplicationBridgeProxy* _applicationProxy;
  ApplicationStatisticBridgeProxy* _applicationStatistic;
  GameNet::Host::ClientConnection *_clientConnection;

  BestInstallPath *_bestInstallPath;

protected:
  void closeEvent(QCloseEvent* event);
  bool event(QEvent* event);
  bool nativeEvent(const QByteArray & eventType, void * message, long * result) override;
};
