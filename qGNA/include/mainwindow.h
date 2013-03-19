﻿#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include "viewmodel/LicenseViewModel.h"
#include "viewmodel/SettingsViewModel.h"
#include "viewmodel/SelectMw2Server/SelectMw2ServerViewModel.h"
#include "viewmodel/EnterNickNameViewModel.h"
#include "viewmodel/GameSettingsViewModel.h"

#include "traywindow.h"
#include <ServiceLoader.h>

#include <QmlMessageAdapter.h>
#include <Features/RememberGameDownloading.h>
#include <Features/StopDownloadServiceWhileExecuteAnyGame.h>
#include <Features/Jabber.h>
#include <Features/GameDownloader/GameDownloadStatistics.h>

#include <Marketing/MarketingTarget.h>

#include <Core/Service>

#include <RestApi/FakeCache>
#include <RestApi/GameNetCredential>
#include <RestApi/RestApiManager>
#include <RestApi/HttpCommandRequest>

#include <AutoRunHelper/UACHelper.h>

#include <UpdateSystem/updatemanagerviewmodel.h>
#include <UpdateSystem/CheckUpdateHelper.h>

#include <GameDownloader/Builder.h>
#include <GameDownloader/StartType.h>

#include <LibtorrentWrapper/Wrapper>
#include <LibtorrentWrapper/TorrentConfig>
#include <LibtorrentWrapper/EventArgs/ProgressEventArgs>

#include <GameExecutor/GameExecutorService.h>
#include <GameExecutor/ServiceInfoCounter>

#include <Application/ArgumentParser.h>

#include <QtCore/QDebug>
#include <QtCore/QObject>
#include <QtCore/QUrl>
#include <QtCore/QScopedPointer>
#include <QtGui/QApplication>
#include <QtGui/QGraphicsObject>
#include <QtDeclarative/QDeclarativeContext>
#include <QtDeclarative/qdeclarativeview.h>
#include <QtDeclarative/QDeclarativeEngine>
#include <QDesktopServices>
#include <QGLWidget>
#include <QMainWindow>
#include <QMouseEvent>

/*!
  \class MQDeclarativeView

  \brief Позволяет получить глобальный клик мыши. Т.к. QGNA-60 и к сожалению QTBUG-13007 
         http://qt-project.org/forums/viewthread/1468
*/

using namespace Features;

class MQDeclarativeView: public QDeclarativeView    
{
  Q_OBJECT
protected:
  void	mousePressEvent(QMouseEvent* event){
    if (event->buttons() & Qt::LeftButton)
      emit this->leftMouseClick(event->x(), event->y()); 
    QDeclarativeView::mousePressEvent(event);
  }

signals:
  void leftMouseClick(int globalX, int globalY); 
};

class MainWindow : public QMainWindow
{
  Q_OBJECT
  Q_PROPERTY(QString installUpdateGnaPath READ installUpdateGnaPath NOTIFY installUpdateGnaPathChanged)
  Q_PROPERTY(QString updateArea READ updateArea NOTIFY updateAreaChanged) 
  Q_PROPERTY(QString updateUrl READ updateUrl NOTIFY updateUrlChanged) 

  Q_PROPERTY(QString nickName READ nickName NOTIFY nickNameChanged)
  Q_PROPERTY(QString techName READ techName NOTIFY techNameChanged)
  Q_PROPERTY(QString mediumAvatarUrl READ mediumAvatarUrl NOTIFY mediumAvatarUrlChanged)
  Q_PROPERTY(QString emptyString READ getEmptyString NOTIFY languageChanged)
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

  const QString& nickName() { return this->_nickName; }
  const QString& techName() { return this->_techName; }
  const QString& mediumAvatarUrl() { return this->_mediumAvatarUrl; }
  QString language();
  const QString& fileVersion() { return _fileVersion; }
  Q_INVOKABLE void saveLanguage(const QString& language);
  Q_INVOKABLE void selectLanguage(const QString& language);
  void setFileVersion(const QString& fileVersion) { _fileVersion = fileVersion; }

public slots:
  void restartApplication(bool shouldStartWithSameArguments = true);
  void authSuccessSlot(const QString& userId, const QString& appKey, const QString& cookie);
  void userMainInfoResult(GGS::RestApi::CommandBase::CommandResults);
  void licenseResult(GGS::RestApi::CommandBase::CommandResults);

  void openExternalBrowser(const QString& url);
  void logout();

  void downloadButtonStart(QString serviceId);
  void downloadButtonPause(QString serviceId);
  
  void licenseOkPressed();

  void settingsIncomingPortChangedSlot();
  void settingsNumConnectionsChangedSlot();
  void settingsDownloadSpeedChangedSlot();
  void settingsUploadSpeedChangedSlot();

  bool isDownloading(QString serviceId);  

  void commandRecieved(QString name, QStringList arguments);       
  void updateFinishedSlot();
  void activateWindow();
  void executeService(QString id);

  void setTechName(QString& techName);
  void setNickName(QString& nickName);

  void startBackgroundCheckUpdate();
  bool isWindowVisible();

  bool anyLicenseAccepted();
  QString startingService();
  QString getExpectedInstallPath(const QString& serviceId);
  void setServiceInstallPath(const QString& serviceId, const QString& path, bool createShortcuts);
  void acceptFirstLicense(const QString& serviceId);
  void initFinished();

private:
  const QString getEmptyString() { return ""; }
  void loadPlugin(QString pluginName);
  void setMediumAvatarUrl(const QString& mediumAvatarUrl);

  void translatorsParse();
  void initializeUpdateSettings();
  int checkUpdateInterval();

  void initAutorun();
  bool isUseOpenGLrender();

  void createShortcut(const QString& pathToLnk, const GGS::Core::Service* service);

  GGS::RestApi::FakeCache _fakeCache;
  GGS::RestApi::GameNetCredential _credential;
  GGS::RestApi::RestApiManager _restapiManager;

  QmlMessageAdapter* messageAdapter;
  TrayWindow* _trayWindow;
  LicenseViewModel* licenseModel;
  SettingsViewModel* settingsViewModel;
  SelectMw2ServerViewModel *_selectMw2ServerViewModel;
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
  
signals:
  void nickNameChanged();
  void nickNameValueChanged(QString &value);
  void techNameChanged();
  void techNameValueChanged(QString &value);
  
  void mediumAvatarUrlChanged(); 
  void languageChanged();
  void fileVersionChanged();
  void installUpdateGnaPathChanged();
  void updateAreaChanged();
  void updateUrlChanged();
  void progressbarChange(QString serviceId, int progress, qint64 totalWantedDone, qint64 totalWanted);
  void progressbarExtractionChange(QString serviceId, int progress, qint64 totalWantedDone, qint64 totalWanted);

  void secondInstanceExecuteRequest();

  void selectService(QString serviceId);
  void needAuth();
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
  
  void serviceStarted(QString service);
  void serviceFinished(QString service, int serviceState);
  void serviceInstalled(QString serviceId);

  void authBeforeStartGameRequest(QString serviceId);
  void authGuestConfirmRequest(QString serviceId);

private slots:
  void onServiceStarted(const GGS::Core::Service &service);
  void onServiceFinished(const GGS::Core::Service &service, GGS::GameExecutor::FinishState state);

  void onSystemBarPressed(int MouseX, int MouseY);
  void onSystemBarReleased(int MouseX, int MouseY);
  void onSystemBarPositionChanged(int MouseX, int MouseY);
  void onWindowClose();
  void onForceWindowClose();
  void menuItemTrigger(int index);

  void progressChanged(QString serviceId, qint8 progress);
  void progressDownloadChanged(QString serviceId, qint8 progress, GGS::Libtorrent::EventArgs::ProgressEventArgs args); 
  void progressExtractionChanged(QString serviceId, qint8 progress, qint64 current, qint64 total);

  void gameDownloaderStarted(const GGS::Core::Service *service, GGS::GameDownloader::StartType startType);
  void gameDownloaderStopped(const GGS::Core::Service *service);
  void gameDownloaderStopping(const GGS::Core::Service *service);     
  void gameDownloaderFailed(const GGS::Core::Service *service); 
  void gameDownloaderFinished(const GGS::Core::Service *service);
  void gameDownloaderStatusMessageChanged(const GGS::Core::Service *service, const QString& message);
  void torrentListenPortChangedSlot(unsigned short port);
  void gameDownloaderServiceInstalled(const GGS::Core::Service *service); 
  void gameDownloaderServiceUpdated(const GGS::Core::Service *service); 

  void shutdownCompleted();
  void checkUpdateHelperFinished(GGS::UpdateSystem::CheckUpdateHelper::Results result);

  void restApiGenericError(GGS::RestApi::CommandBase::Error, QString message);

private:
  void initServices();

  void checkLicense(const QString& serviceId);
  void startGame(const QString& serviceId);

  void prepairGameDownloader();
  void initializeStopDownloadServiceOnExecuteGameFeature();
  void postUpdateInit();

  GGS::Core::Service* getService(const QString& id);

  ServiceLoader _serviceLoader;

  QMap<QString,QTranslator*> translators;
  GGS::GameDownloader::Builder _gameDownloaderBuilder;

  GGS::GameExecutor::GameExecutorService _gameExecutorService;
  GGS::GameExecutor::ServiceInfoCounter _gameExecutorServiceInfoCounter;
  QString _installUpdateGnaPath;
  QString _updateArea;
  QString _updateUrl;
  GGS::UpdateSystem::CheckUpdateHelper _checkUpdateHelper;

  RememberGameDownloading _rembrGameFeature;
  GGS::Marketing::MarketingTarget _marketingTargetFeatures;
  Features::StopDownloadServiceWhileExecuteAnyGame _stopDownloadServiceOnExecuteGame;
  Features::Jabber _jabber;
  Features::GameDownloader::GameDownloadStatistics _downloadStatistics;

protected:
  void closeEvent(QCloseEvent* event);
};

#endif // MAINWINDOW_H
