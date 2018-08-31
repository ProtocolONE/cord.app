/*
 * This file was generated by qdbusxml2cpp version 0.8
 * Command line was: qdbusxml2cpp -V -p ExecutorBridgeProxy -c ExecutorBridgeProxy ExecutorBridge.interface.xml
 *
 * qdbusxml2cpp is Copyright (C) 2017 The Qt Company Ltd.
 *
 * This is an auto-generated file.
 * Do not edit! All changes made to it will be lost.
 */

#ifndef EXECUTORBRIDGEPROXY_H
#define EXECUTORBRIDGEPROXY_H

#include <Host/Bridge/ExecutorBridge.h>

#include <QtCore/QObject>
#include <QtCore/QByteArray>
#include <QtCore/QList>
#include <QtCore/QMap>
#include <QtCore/QString>
#include <QtCore/QStringList>
#include <QtCore/QVariant>
#include <QtDBus/QtDBus>

/*
 * Proxy class for interface com.protocolone.launcher.dbus.Executor
 */
class ExecutorBridgeProxy: public QDBusAbstractInterface
{
    Q_OBJECT
public:
    static inline const char *staticInterfaceName()
    { return "com.protocolone.launcher.dbus.Executor"; }

public:
    ExecutorBridgeProxy(const QString &service, const QString &path, const QDBusConnection &connection, QObject *parent = nullptr);

    ~ExecutorBridgeProxy();

public Q_SLOTS: // METHODS
    inline QDBusPendingReply<bool> canExecuteSecond(const QString &serviceId)
    {
        QList<QVariant> argumentList;
        argumentList << QVariant::fromValue(serviceId);
        return asyncCallWithArgumentList(QStringLiteral("canExecuteSecond"), argumentList);
    }

    inline QDBusPendingReply<> execute(const QString &serviceId, P1::Host::Bridge::Credential credential)
    {
        QList<QVariant> argumentList;
        argumentList << QVariant::fromValue(serviceId) << QVariant::fromValue(credential);
        return asyncCallWithArgumentList(QStringLiteral("execute"), argumentList);
    }

    inline QDBusPendingReply<> executeSecond(const QString &serviceId, P1::Host::Bridge::Credential credential, P1::Host::Bridge::Credential secondCredential)
    {
        QList<QVariant> argumentList;
        argumentList << QVariant::fromValue(serviceId) << QVariant::fromValue(credential) << QVariant::fromValue(secondCredential);
        return asyncCallWithArgumentList(QStringLiteral("executeSecond"), argumentList);
    }

    inline QDBusPendingReply<bool> isAnyGameStarted()
    {
        QList<QVariant> argumentList;
        return asyncCallWithArgumentList(QStringLiteral("isAnyGameStarted"), argumentList);
    }

    inline QDBusPendingReply<bool> isGameStarted(const QString &serviceId)
    {
        QList<QVariant> argumentList;
        argumentList << QVariant::fromValue(serviceId);
        return asyncCallWithArgumentList(QStringLiteral("isGameStarted"), argumentList);
    }

    inline Q_NOREPLY void shutdownSecond()
    {
        QList<QVariant> argumentList;
        callWithArgumentList(QDBus::NoBlock, QStringLiteral("shutdownSecond"), argumentList);
    }

    inline Q_NOREPLY void terminateGame()
    {
        QList<QVariant> argumentList;
        callWithArgumentList(QDBus::NoBlock, QStringLiteral("terminateGame"), argumentList);
    }

    inline Q_NOREPLY void terminateGame(const QString &serviceId)
    {
        QList<QVariant> argumentList;
        argumentList << QVariant::fromValue(serviceId);
        callWithArgumentList(QDBus::NoBlock, QStringLiteral("terminateGame"), argumentList);
    }

Q_SIGNALS: // SIGNALS
    void secondServiceFinished(const QString &serviceId, int finishState);
    void secondServiceStarted(const QString &serviceId);
    void serviceFinished(const QString &serviceId, int finishState);
    void serviceStarted(const QString &serviceId);
};

namespace com {
  namespace p1 {
    namespace dbus {
      typedef ::ExecutorBridgeProxy Executor;
    }
  }
}
#endif
