#ifndef SETTINGSVIEWMODEL_H
#define SETTINGSVIEWMODEL_H

#include <QObject>

class SettingsViewModel : public QObject
{
    Q_OBJECT
    Q_PROPERTY(int autoStart READ autoStart WRITE setAutoStart NOTIFY autoStartChanged)
    Q_PROPERTY(QString incomingPort READ incomingPort WRITE setIncomingPort NOTIFY incomingPortChanged)
    Q_PROPERTY(QString numConnections READ numConnections WRITE setNumConnections NOTIFY numConnectionsChanged)
    Q_PROPERTY(QString downloadSpeed READ downloadSpeed WRITE setDownloadSpeed NOTIFY downloadSpeedChanged)
    Q_PROPERTY(QString uploadSpeed READ uploadSpeed WRITE setUploadSpeed NOTIFY uploadSpeedChanged)

    Q_PROPERTY(bool notifyWhenStartedGameNet READ notifyWhenStartedGameNet WRITE setNotifyWhenStartedGameNet NOTIFY notifyWhenStartedGameNetChanged)
    Q_PROPERTY(bool notifyStartEndGameDownload READ notifyStartEndGameDownload WRITE setNotifyStartEndGameDownload NOTIFY notifyStartEndGameDownloadChanged)
    Q_PROPERTY(bool notifyDisconnectFriendFromNetwork READ notifyDisconnectFriendFromNetwork WRITE setNotifyDisconnectFriendFromNetwork NOTIFY notifyDisconnectFriendFromNetworkChanged)
    Q_PROPERTY(bool notifyDisconnectFriendFromGame READ notifyDisconnectFriendFromGame WRITE setNotifyDisconnectFriendFromGame NOTIFY notifyDisconnectFriendFromGameChanged)
    Q_PROPERTY(bool notifyNewMessage READ notifyNewMessage WRITE setNotifyNewMessage NOTIFY notifyNewMessageChanged)

    Q_PROPERTY(bool isPublicTestVersion READ isPublicTestVersion NOTIFY isPublicTestVersionChanged)
    Q_PROPERTY(bool seedEnabled READ seedEnabled WRITE setSeedEnabled NOTIFY seedEnabledChanged)

public:
    explicit SettingsViewModel(QObject *parent = 0);
    ~SettingsViewModel();

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

    void setAutoStart(int _autostart);
    int autoStart();

    void setDefaultSettings();
    void addToAutoStart(bool autostart, bool isMinimized); 
    void loadSettings();

    bool isPublicTestVersion();
    void switchClientVersion();

    bool seedEnabled();
    void setSeedEnabled(bool value);

private:
    bool _instantlySave;

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

    void applicationAreaChanged();
    void seedEnabledChanged();
};

#endif // SETTINGSVIEWMODEL_H
