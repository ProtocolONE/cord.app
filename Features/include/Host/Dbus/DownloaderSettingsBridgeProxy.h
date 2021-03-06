/*
 * This file was generated by qdbusxml2cpp version 0.8
 * Command line was: qdbusxml2cpp -V -p DownloaderSettingsBridgeProxy -c DownloaderSettingsBridgeProxy DownloaderSettingsBridge.interface.xml
 *
 * qdbusxml2cpp is Copyright (C) 2017 The Qt Company Ltd.
 *
 * This is an auto-generated file.
 * Do not edit! All changes made to it will be lost.
 */

#ifndef DOWNLOADERSETTINGSBRIDGEPROXY_H
#define DOWNLOADERSETTINGSBRIDGEPROXY_H

#include <QtCore/QObject>
#include <QtCore/QByteArray>
#include <QtCore/QList>
#include <QtCore/QMap>
#include <QtCore/QString>
#include <QtCore/QStringList>
#include <QtCore/QVariant>
#include <QtDBus/QtDBus>

/*
 * Proxy class for interface com.protocolone.launcher.dbus.DownloaderSettings
 */
class DownloaderSettingsBridgeProxy: public QDBusAbstractInterface
{
    Q_OBJECT
public:
    static inline const char *staticInterfaceName()
    { return "com.protocolone.launcher.dbus.DownloaderSettings"; }

public:
    DownloaderSettingsBridgeProxy(const QString &service, const QString &path, const QDBusConnection &connection, QObject *parent = nullptr);

    ~DownloaderSettingsBridgeProxy();

    Q_PROPERTY(int downloadRate READ downloadRate WRITE setDownloadRate)
    inline int downloadRate() const
    { return qvariant_cast< int >(property("downloadRate")); }
    inline void setDownloadRate(int value)
    { setProperty("downloadRate", QVariant::fromValue(value)); }

    Q_PROPERTY(int listeningPort READ listeningPort WRITE setListeningPort)
    inline int listeningPort() const
    { return qvariant_cast< int >(property("listeningPort")); }
    inline void setListeningPort(int value)
    { setProperty("listeningPort", QVariant::fromValue(value)); }

    Q_PROPERTY(int maxConnection READ maxConnection WRITE setMaxConnection)
    inline int maxConnection() const
    { return qvariant_cast< int >(property("maxConnection")); }
    inline void setMaxConnection(int value)
    { setProperty("maxConnection", QVariant::fromValue(value)); }

    Q_PROPERTY(bool seedEnabled READ seedEnabled WRITE setSeedEnabled)
    inline bool seedEnabled() const
    { return qvariant_cast< bool >(property("seedEnabled")); }
    inline void setSeedEnabled(bool value)
    { setProperty("seedEnabled", QVariant::fromValue(value)); }

    Q_PROPERTY(int torrentProfile READ torrentProfile WRITE setTorrentProfile)
    inline int torrentProfile() const
    { return qvariant_cast< int >(property("torrentProfile")); }
    inline void setTorrentProfile(int value)
    { setProperty("torrentProfile", QVariant::fromValue(value)); }

    Q_PROPERTY(int uploadRate READ uploadRate WRITE setUploadRate)
    inline int uploadRate() const
    { return qvariant_cast< int >(property("uploadRate")); }
    inline void setUploadRate(int value)
    { setProperty("uploadRate", QVariant::fromValue(value)); }

public Q_SLOTS: // METHODS
Q_SIGNALS: // SIGNALS
    void downloadRateChanged();
    void listeningPortChanged();
    void maxConnectionChanged();
    void seedEnabledChanged();
    void torrentProfileChanged();
    void uploadRateChanged();
};

namespace com {
  namespace p1 {
    namespace dbus {
      typedef ::DownloaderSettingsBridgeProxy DownloaderSettings;
    }
  }
}
#endif
