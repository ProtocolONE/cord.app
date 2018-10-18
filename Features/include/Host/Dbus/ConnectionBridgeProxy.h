/*
 * This file was generated by qdbusxml2cpp version 0.8
 * Command line was: qdbusxml2cpp -v -p ConnectionBridgeProxy -c ConnectionBridgeProxy ConnectionBridge.interface.xml
 *
 * qdbusxml2cpp is Copyright (C) 2015 The Qt Company Ltd.
 *
 * This is an auto-generated file.
 * Do not edit! All changes made to it will be lost.
 */

#ifndef CONNECTIONBRIDGEPROXY_H
#define CONNECTIONBRIDGEPROXY_H

#include <QtCore/QObject>
#include <QtCore/QByteArray>
#include <QtCore/QList>
#include <QtCore/QMap>
#include <QtCore/QString>
#include <QtCore/QStringList>
#include <QtCore/QVariant>
#include <QtDBus/QtDBus>

/*
 * Proxy class for interface com.protocolone.launcher.dbus.Connection
 */
class ConnectionBridgeProxy: public QDBusAbstractInterface
{
    Q_OBJECT
public:
    static inline const char *staticInterfaceName()
    { return "com.protocolone.launcher.dbus.Connection"; }

public:
    ConnectionBridgeProxy(const QString &service, const QString &path, const QDBusConnection &connection, QObject *parent = 0);

    ~ConnectionBridgeProxy();

public Q_SLOTS: // METHODS
    inline QDBusPendingReply<> close()
    {
        QList<QVariant> argumentList;
        return asyncCallWithArgumentList(QStringLiteral("close"), argumentList);
    }

    inline QDBusPendingReply<> ping()
    {
        QList<QVariant> argumentList;
        return asyncCallWithArgumentList(QStringLiteral("ping"), argumentList);
    }

    inline QDBusPendingReply<> setApplicationName(const QString &applicationName)
    {
        QList<QVariant> argumentList;
        argumentList << QVariant::fromValue(applicationName);
        return asyncCallWithArgumentList(QStringLiteral("setApplicationName"), argumentList);
    }

    inline QDBusPendingReply<> setCredential(const QString &accessToken, const QString &acccessTokenExpiredTime)
    {
        QList<QVariant> argumentList;
        argumentList << QVariant::fromValue(accessToken) << QVariant::fromValue(acccessTokenExpiredTime);
        return asyncCallWithArgumentList(QStringLiteral("setCredential"), argumentList);
    }

    inline QDBusPendingReply<> updateCredential(const QString &accessTokenOld, const QString &acccessTokenExpiredTimeOld, const QString &accessTokenNew, const QString &acccessTokenExpiredTimeNew)
    {
        QList<QVariant> argumentList;
        argumentList << QVariant::fromValue(accessTokenOld) << QVariant::fromValue(acccessTokenExpiredTimeOld) << QVariant::fromValue(accessTokenNew) << QVariant::fromValue(acccessTokenExpiredTimeNew);
        return asyncCallWithArgumentList(QStringLiteral("updateCredential"), argumentList);
    }

Q_SIGNALS: // SIGNALS
    void authorizationError(const QString &accessToken, const QString &acccessTokenExpiredTime);
    void pong();
};

namespace com {
  namespace protocolone {
    namespace launcher {
      namespace dbus {
        typedef ::ConnectionBridgeProxy Connection;
      }
    }
  }
}
#endif
