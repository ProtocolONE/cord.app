#ifndef _GGS_GAMESETTINGSVIEWMODEL_H_
#define _GGS_GAMESETTINGSVIEWMODEL_H_

#include <QtCore/QObject>
#include <QtCore/QHash>
#include <QtCore/QString>

namespace GGS {
  namespace Core {
    class Service;
  }
  namespace GameDownloader {
    class GameDownloadService;
  }
}

class GameSettingsViewModel : public QObject
{
  Q_OBJECT
  Q_PROPERTY(QString installPath READ installPath WRITE setInstallPath NOTIFY installPathChanged)
  Q_PROPERTY(QString downloadPath READ downloadPath WRITE setDownloadPath NOTIFY downloadPathChanged)
  Q_PROPERTY(bool hasDownloadPath READ hasDownloadPath NOTIFY hasDownloadPathChanged)
public:
  explicit GameSettingsViewModel(QObject *parent = 0);
  ~GameSettingsViewModel();

  void setServiceList(QHash<QString, GGS::Core::Service *> *serviceList);
  void setGameDownloader(GGS::GameDownloader::GameDownloadService *gameDownloader);

  void createShortcutOnDesktop(GGS::Core::Service *service);
  void createShortcutInMainMenu(GGS::Core::Service *service);
  void createShortcut(const QString& path, GGS::Core::Service *service);
  void removeShortCutByName(const QString& name);

public slots:
  void createShortcutOnDesktop();
  void createShortcutInMainMenu();

  void browseInstallPath();
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

signals:
  void installPathChanged();
  void downloadPathChanged();
  void hasDownloadPathChanged();

private:

  QHash<QString, GGS::Core::Service *> *_serviceList;
  GGS::GameDownloader::GameDownloadService *_gameDownloader;

  QString _currentServiceId;
  QString _installPath;
  QString _downloadPath;
  bool _hasDownloadPath;
};

#endif // _GGS_GAMESETTINGSVIEWMODEL_H_