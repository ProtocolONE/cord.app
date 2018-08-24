#pragma once

#include <QObject>

class DownloaderSettingsBridgeProxy;
class UpdateManagerBridgeProxy;
class ApplicationBridgeProxy;

class SettingsViewModel : public QObject
{
    Q_OBJECT
    Q_PROPERTY(int autoStart READ autoStart WRITE setAutoStart NOTIFY autoStartChanged)
    Q_PROPERTY(QString incomingPort READ incomingPort WRITE setIncomingPort NOTIFY incomingPortChanged)
    Q_PROPERTY(QString numConnections READ numConnections WRITE setNumConnections NOTIFY numConnectionsChanged)
    Q_PROPERTY(QString downloadSpeed READ downloadSpeed WRITE setDownloadSpeed NOTIFY downloadSpeedChanged)
    Q_PROPERTY(QString uploadSpeed READ uploadSpeed WRITE setUploadSpeed NOTIFY uploadSpeedChanged)
    Q_PROPERTY(int torrentProfile READ torrentProfile WRITE setTorrentProfile NOTIFY torrentProfileChanged)

    Q_PROPERTY(bool notifyWhenStartedGameNet READ notifyWhenStartedGameNet WRITE setNotifyWhenStartedGameNet NOTIFY notifyWhenStartedGameNetChanged)
    Q_PROPERTY(bool notifyStartEndGameDownload READ notifyStartEndGameDownload WRITE setNotifyStartEndGameDownload NOTIFY notifyStartEndGameDownloadChanged)
    Q_PROPERTY(bool notifyDisconnectFriendFromNetwork READ notifyDisconnectFriendFromNetwork WRITE setNotifyDisconnectFriendFromNetwork NOTIFY notifyDisconnectFriendFromNetworkChanged)
    Q_PROPERTY(bool notifyDisconnectFriendFromGame READ notifyDisconnectFriendFromGame WRITE setNotifyDisconnectFriendFromGame NOTIFY notifyDisconnectFriendFromGameChanged)
    Q_PROPERTY(bool notifyNewMessage READ notifyNewMessage WRITE setNotifyNewMessage NOTIFY notifyNewMessageChanged)

    Q_PROPERTY(QString updateArea READ updateArea NOTIFY updateAreaChanged)
    Q_PROPERTY(bool seedEnabled READ seedEnabled WRITE setSeedEnabled NOTIFY seedEnabledChanged)

public:
    explicit SettingsViewModel(QObject *parent = 0);
    virtual ~SettingsViewModel();

public slots:

    QString incomingPort();
    void setIncomingPort(const QString& _port);

    QString numConnections();
    void setNumConnections(const QString& _num);

    bool notifyWhenStartedGameNet();
    void setNotifyWhenStartedGameNet(bool _notify);

    bool notifyStartEndGameDownload();
    void setNotifyStartEndGameDownload(bool _notify);

    bool notifyDisconnectFriendFromNetwork();
    void setNotifyDisconnectFriendFromNetwork(bool _notify);

    bool notifyDisconnectFriendFromGame();
    void setNotifyDisconnectFriendFromGame(bool _notify);

    bool notifyNewMessage();
    void setNotifyNewMessage(bool _notify);

    QString downloadSpeed();
    void setDownloadSpeed(const QString& _speed);

    QString uploadSpeed();
    void setUploadSpeed(const QString& _speed);

    void setAutoStart(int value);
    int autoStart();

    void setDefaultSettings();
    void loadSettings();

    QString updateArea();

    bool seedEnabled();
    void setSeedEnabled(bool value);

    void setDownloaderSettings(DownloaderSettingsBridgeProxy *value);
    void setApplicationProxy(ApplicationBridgeProxy *value);

    void setTorrentProfile(int value);
    int torrentProfile(); 

private:
    bool _instantlySave;
    DownloaderSettingsBridgeProxy* _downloaderSettings;
    UpdateManagerBridgeProxy* _updaterProxy;
    ApplicationBridgeProxy *_applicationProxy;

signals:
    void autoStartChanged();
    void incomingPortChanged();
    void numConnectionsChanged();
    void runInMinimizedChanged();
    void notifyWhenStartedGameNetChanged();
    void notifyStartEndGameDownloadChanged();
    void notifyDisconnectFriendFromNetworkChanged();
    void notifyDisconnectFriendFromGameChanged();
    void notifyNewMessageChanged();
    void downloadSpeedChanged();
    void uploadSpeedChanged();
    void isPublicTestVersionChanged();

    void seedEnabledChanged();
    void updateAreaChanged();
    void torrentProfileChanged();
};
