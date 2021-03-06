#pragma once

#include <QtCore/QObject>
#include <QtCore/QHash>
#include <QtCore/QString>
#include <QtCore/QStringList>
#include <Qtcore/QByteArray>

namespace P1 {
  namespace Core {
    class Service;
  }
  namespace GameDownloader {
    class GameDownloadService;
  }
}

class ServiceSettingsBridgeProxy;
class DownloaderBridgeProxy;
class GameSettingsViewModel : public QObject
{
  Q_OBJECT
  Q_PROPERTY(QString installPath READ installPath WRITE setInstallPath NOTIFY installPathChanged)
  Q_PROPERTY(QString downloadPath READ downloadPath WRITE setDownloadPath NOTIFY downloadPathChanged)
  Q_PROPERTY(bool hasDownloadPath READ hasDownloadPath NOTIFY hasDownloadPathChanged)
public:
  explicit GameSettingsViewModel(QObject *parent = 0);
  ~GameSettingsViewModel();

  void setDownloader(DownloaderBridgeProxy *value);
  void setServiceSettings(ServiceSettingsBridgeProxy *value);

  void createShortcut(const QString& path, const QString& serviceId);
  void removeShortCutByName(const QString& name);

public slots: 
  void createShortcutOnDesktop(const QString& serviceId);
  void createShortcutInMainMenu(const QString& serviceId);
  void removeShortcuts(const QString& serviceId);

  void browseInstallPath(const QString &preferedPath = QString());
  void browseDownloadPath();
  
  void submitSettings();
  void restoreClient();
  void switchGame(const QString& serviceId);

  bool hasDownloadPath();
  void setHasDownloadPath(bool hasDownloadPath);

  const QString& installPath();
  void setInstallPath(const QString& installPath);

  const QString& downloadPath();
  void setDownloadPath(const QString& downloadPath);

  static QStringList deserialize(QByteArray serialized);
  static QByteArray serialize(QStringList stringList);

  bool isOverlayEnabled(const QString& serviceId);
  void setOverlayEnabled(const QString& serviceId, bool enabled);

  bool isPrefer32Bit(const QString& serviceId);
  void setPrefer32Bit(const QString& serviceId, bool enabled);

signals:
  void installPathChanged();
  void downloadPathChanged();
  void hasDownloadPathChanged();

private:
  ServiceSettingsBridgeProxy* _serviceSettings;
  DownloaderBridgeProxy * _downloader;
  
  QString _currentServiceId;
  QString _installPath;
  QString _downloadPath;
  bool _hasDownloadPath;
};
