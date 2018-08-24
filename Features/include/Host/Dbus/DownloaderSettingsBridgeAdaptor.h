/*
 * This file was generated by qdbusxml2cpp version 0.8
 * Command line was: qdbusxml2cpp -V -a DownloaderSettingsBridgeAdaptor -c DownloaderSettingsBridgeAdaptor DownloaderSettingsBridge.interface.xml
 *
 * qdbusxml2cpp is Copyright (C) 2017 The Qt Company Ltd.
 *
 * This is an auto-generated file.
 * This file may have been hand-edited. Look for HAND-EDIT comments
 * before re-generating it.
 */

#ifndef DOWNLOADERSETTINGSBRIDGEADAPTOR_H
#define DOWNLOADERSETTINGSBRIDGEADAPTOR_H

#include <QtCore/QObject>
#include <QtDBus/QtDBus>
QT_BEGIN_NAMESPACE
class QByteArray;
template<class T> class QList;
template<class Key, class Value> class QMap;
class QString;
class QStringList;
class QVariant;
QT_END_NAMESPACE

/*
 * Adaptor class for interface com.gamenet.dbus.DownloaderSettings
 */
class DownloaderSettingsBridgeAdaptor: public QDBusAbstractAdaptor
{
    Q_OBJECT
    Q_CLASSINFO("D-Bus Interface", "com.gamenet.dbus.DownloaderSettings")
    Q_CLASSINFO("D-Bus Introspection", ""
"  <interface name=\"com.gamenet.dbus.DownloaderSettings\">\n"
"    <property access=\"readwrite\" type=\"i\" name=\"listeningPort\"/>\n"
"    <property access=\"readwrite\" type=\"i\" name=\"uploadRate\"/>\n"
"    <property access=\"readwrite\" type=\"i\" name=\"downloadRate\"/>\n"
"    <property access=\"readwrite\" type=\"i\" name=\"maxConnection\"/>\n"
"    <property access=\"readwrite\" type=\"b\" name=\"seedEnabled\"/>\n"
"    <property access=\"readwrite\" type=\"i\" name=\"torrentProfile\"/>\n"
"    <signal name=\"listeningPortChanged\"/>\n"
"    <signal name=\"uploadRateChanged\"/>\n"
"    <signal name=\"downloadRateChanged\"/>\n"
"    <signal name=\"maxConnectionChanged\"/>\n"
"    <signal name=\"seedEnabledChanged\"/>\n"
"    <signal name=\"torrentProfileChanged\"/>\n"
"  </interface>\n"
        "")
public:
    DownloaderSettingsBridgeAdaptor(QObject *parent);
    virtual ~DownloaderSettingsBridgeAdaptor();

public: // PROPERTIES
    Q_PROPERTY(int downloadRate READ downloadRate WRITE setDownloadRate)
    int downloadRate() const;
    void setDownloadRate(int value);

    Q_PROPERTY(int listeningPort READ listeningPort WRITE setListeningPort)
    int listeningPort() const;
    void setListeningPort(int value);

    Q_PROPERTY(int maxConnection READ maxConnection WRITE setMaxConnection)
    int maxConnection() const;
    void setMaxConnection(int value);

    Q_PROPERTY(bool seedEnabled READ seedEnabled WRITE setSeedEnabled)
    bool seedEnabled() const;
    void setSeedEnabled(bool value);

    Q_PROPERTY(int torrentProfile READ torrentProfile WRITE setTorrentProfile)
    int torrentProfile() const;
    void setTorrentProfile(int value);

    Q_PROPERTY(int uploadRate READ uploadRate WRITE setUploadRate)
    int uploadRate() const;
    void setUploadRate(int value);

public Q_SLOTS: // METHODS
Q_SIGNALS: // SIGNALS
    void downloadRateChanged();
    void listeningPortChanged();
    void maxConnectionChanged();
    void seedEnabledChanged();
    void torrentProfileChanged();
    void uploadRateChanged();
};

#endif
