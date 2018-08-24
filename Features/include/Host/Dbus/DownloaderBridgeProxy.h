/*
 * This file was generated by qdbusxml2cpp version 0.8
 * Command line was: qdbusxml2cpp -V -p DownloaderBridgeProxy -c DownloaderBridgeProxy DownloaderBridge.interface.xml
 *
 * qdbusxml2cpp is Copyright (C) 2017 The Qt Company Ltd.
 *
 * This is an auto-generated file.
 * Do not edit! All changes made to it will be lost.
 */

#ifndef DOWNLOADERBRIDGEPROXY_H
#define DOWNLOADERBRIDGEPROXY_H

#include <QtCore/QObject>
#include <QtCore/QByteArray>
#include <QtCore/QList>
#include <QtCore/QMap>
#include <QtCore/QString>
#include <QtCore/QStringList>
#include <QtCore/QVariant>
#include <QtDBus/QtDBus>

#include <Host/Bridge/DownloaderBridge.h>

/*
 * Proxy class for interface com.gamenet.dbus.Downloader
 */
class DownloaderBridgeProxy: public QDBusAbstractInterface
{
    Q_OBJECT
public:
    static inline const char *staticInterfaceName()
    { return "com.gamenet.dbus.Downloader"; }

public:
    DownloaderBridgeProxy(const QString &service, const QString &path, const QDBusConnection &connection, QObject *parent = nullptr);

    ~DownloaderBridgeProxy();

public Q_SLOTS: // METHODS
    inline QDBusPendingReply<bool> isAnyServiceInProgress()
    {
        QList<QVariant> argumentList;
        return asyncCallWithArgumentList(QStringLiteral("isAnyServiceInProgress"), argumentList);
    }

    inline QDBusPendingReply<bool> isInProgress(const QString &serviceId)
    {
        QList<QVariant> argumentList;
        argumentList << QVariant::fromValue(serviceId);
        return asyncCallWithArgumentList(QStringLiteral("isInProgress"), argumentList);
    }

    inline QDBusPendingReply<bool> isInstalled(const QString &serviceId)
    {
        QList<QVariant> argumentList;
        argumentList << QVariant::fromValue(serviceId);
        return asyncCallWithArgumentList(QStringLiteral("isInstalled"), argumentList);
    }

    inline Q_NOREPLY void pauseSession()
    {
        QList<QVariant> argumentList;
        callWithArgumentList(QDBus::NoBlock, QStringLiteral("pauseSession"), argumentList);
    }

    inline Q_NOREPLY void resumeSession()
    {
        QList<QVariant> argumentList;
        callWithArgumentList(QDBus::NoBlock, QStringLiteral("resumeSession"), argumentList);
    }

    inline Q_NOREPLY void start(const QString &serviceId, int startType)
    {
        QList<QVariant> argumentList;
        argumentList << QVariant::fromValue(serviceId) << QVariant::fromValue(startType);
        callWithArgumentList(QDBus::NoBlock, QStringLiteral("start"), argumentList);
    }

    inline Q_NOREPLY void stop(const QString &serviceId)
    {
        QList<QVariant> argumentList;
        argumentList << QVariant::fromValue(serviceId);
        callWithArgumentList(QDBus::NoBlock, QStringLiteral("stop"), argumentList);
    }

Q_SIGNALS: // SIGNALS
    void accessRequired(const QString &serviceId);
    void downloadProgress(const QString &serviceId, int progress, GameNet::Host::Bridge::DownloadProgressArgs args);
    void failed(const QString &serviceId);
    void finished(const QString &serviceId);
    void serviceInstalled(const QString &serviceId);
    void serviceUninstalled(const QString &serviceId);
    void serviceUpdated(const QString &serviceId);
    void started(const QString &serviceId, int startType);
    void statusMessageChanged(const QString &serviceId, const QString &message);
    void stopped(const QString &serviceId);
    void stopping(const QString &serviceId);
    void totalProgress(const QString &serviceId, int progress);
};

namespace com {
  namespace gamenet {
    namespace dbus {
      typedef ::DownloaderBridgeProxy Downloader;
    }
  }
}
#endif
