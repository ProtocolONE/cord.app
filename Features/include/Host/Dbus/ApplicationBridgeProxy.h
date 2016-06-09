/*
 * This file was generated by qdbusxml2cpp version 0.8
 * Command line was: qdbusxml2cpp -v -p ApplicationBridgeProxy -c ApplicationBridgeProxy ApplicationBridge.interface.xml
 *
 * qdbusxml2cpp is Copyright (C) 2015 The Qt Company Ltd.
 *
 * This is an auto-generated file.
 * Do not edit! All changes made to it will be lost.
 */

#ifndef APPLICATIONBRIDGEPROXY_H
#define APPLICATIONBRIDGEPROXY_H

#include <QtCore/QObject>
#include <QtCore/QByteArray>
#include <QtCore/QList>
#include <QtCore/QMap>
#include <QtCore/QString>
#include <QtCore/QStringList>
#include <QtCore/QVariant>
#include <QtDBus/QtDBus>

/*
 * Proxy class for interface com.gamenet.dbus.Application
 */
class ApplicationBridgeProxy: public QDBusAbstractInterface
{
    Q_OBJECT
public:
    static inline const char *staticInterfaceName()
    { return "com.gamenet.dbus.Application"; }

public:
    ApplicationBridgeProxy(const QString &service, const QString &path, const QDBusConnection &connection, QObject *parent = 0);

    ~ApplicationBridgeProxy();

    Q_PROPERTY(int autoStartMode READ autoStartMode WRITE setAutoStartMode)
    inline int autoStartMode() const
    { return qvariant_cast< int >(property("autoStartMode")); }
    inline void setAutoStartMode(int value)
    { setProperty("autoStartMode", QVariant::fromValue(value)); }

    Q_PROPERTY(QString language READ language WRITE setLanguage)
    inline QString language() const
    { return qvariant_cast< QString >(property("language")); }
    inline void setLanguage(const QString &value)
    { setProperty("language", QVariant::fromValue(value)); }

public Q_SLOTS: // METHODS
    inline Q_NOREPLY void cancelUninstallServiceRequest(const QString &serviceId)
    {
        QList<QVariant> argumentList;
        argumentList << QVariant::fromValue(serviceId);
        callWithArgumentList(QDBus::NoBlock, QStringLiteral("cancelUninstallServiceRequest"), argumentList);
    }

    inline QDBusPendingReply<bool> isInitCompleted()
    {
        QList<QVariant> argumentList;
        return asyncCallWithArgumentList(QStringLiteral("isInitCompleted"), argumentList);
    }

    inline Q_NOREPLY void openBrowser(const QString &url)
    {
        QList<QVariant> argumentList;
        argumentList << QVariant::fromValue(url);
        callWithArgumentList(QDBus::NoBlock, QStringLiteral("openBrowser"), argumentList);
    }

    inline Q_NOREPLY void restartApplication(bool isMinimized)
    {
        QList<QVariant> argumentList;
        argumentList << QVariant::fromValue(isMinimized);
        callWithArgumentList(QDBus::NoBlock, QStringLiteral("restartApplication"), argumentList);
    }

    inline Q_NOREPLY void shutdownUIResult()
    {
        QList<QVariant> argumentList;
        callWithArgumentList(QDBus::NoBlock, QStringLiteral("shutdownUIResult"), argumentList);
    }

    inline Q_NOREPLY void switchClientVersion()
    {
        QList<QVariant> argumentList;
        callWithArgumentList(QDBus::NoBlock, QStringLiteral("switchClientVersion"), argumentList);
    }

Q_SIGNALS: // SIGNALS
    void additionalResourcesReady();
    void autoStartModeChanged();
    void initCompleted();
    void languageChanged();
    void restartUIRequest();
    void shutdownUIRequest();
    void uninstallServiceRequest(const QString &serviceId);
};

namespace com {
  namespace gamenet {
    namespace dbus {
      typedef ::ApplicationBridgeProxy Application;
    }
  }
}
#endif
