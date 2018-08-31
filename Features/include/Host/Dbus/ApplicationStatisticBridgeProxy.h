/*
 * This file was generated by qdbusxml2cpp version 0.8
 * Command line was: qdbusxml2cpp -V -p ApplicationStatisticBridgeProxy -c ApplicationStatisticBridgeProxy ApplicationStatisticBridge.interface.xml
 *
 * qdbusxml2cpp is Copyright (C) 2017 The Qt Company Ltd.
 *
 * This is an auto-generated file.
 * Do not edit! All changes made to it will be lost.
 */

#ifndef APPLICATIONSTATISTICBRIDGEPROXY_H
#define APPLICATIONSTATISTICBRIDGEPROXY_H

#include <QtCore/QObject>
#include <QtCore/QByteArray>
#include <QtCore/QList>
#include <QtCore/QMap>
#include <QtCore/QString>
#include <QtCore/QStringList>
#include <QtCore/QVariant>
#include <QtDBus/QtDBus>

/*
 * Proxy class for interface com.protocolone.launcher.dbus.ApplicationStatistic
 */
class ApplicationStatisticBridgeProxy: public QDBusAbstractInterface
{
    Q_OBJECT
public:
    static inline const char *staticInterfaceName()
    { return "com.protocolone.launcher.dbus.ApplicationStatistic"; }

public:
    ApplicationStatisticBridgeProxy(const QString &service, const QString &path, const QDBusConnection &connection, QObject *parent = nullptr);

    ~ApplicationStatisticBridgeProxy();

public Q_SLOTS: // METHODS
    inline QDBusPendingReply<int> executeGameFailedCount(const QString &serviceId)
    {
        QList<QVariant> argumentList;
        argumentList << QVariant::fromValue(serviceId);
        return asyncCallWithArgumentList(QStringLiteral("executeGameFailedCount"), argumentList);
    }

    inline QDBusPendingReply<int> executeGameSuccessCount(const QString &serviceId)
    {
        QList<QVariant> argumentList;
        argumentList << QVariant::fromValue(serviceId);
        return asyncCallWithArgumentList(QStringLiteral("executeGameSuccessCount"), argumentList);
    }

    inline QDBusPendingReply<int> executeGameTotalCount(const QString &serviceId)
    {
        QList<QVariant> argumentList;
        argumentList << QVariant::fromValue(serviceId);
        return asyncCallWithArgumentList(QStringLiteral("executeGameTotalCount"), argumentList);
    }

    inline QDBusPendingReply<qulonglong> gameInstallDate(const QString &serviceId)
    {
        QList<QVariant> argumentList;
        argumentList << QVariant::fromValue(serviceId);
        return asyncCallWithArgumentList(QStringLiteral("gameInstallDate"), argumentList);
    }

    inline QDBusPendingReply<qulonglong> gameLastExecutionTime(const QString &serviceId)
    {
        QList<QVariant> argumentList;
        argumentList << QVariant::fromValue(serviceId);
        return asyncCallWithArgumentList(QStringLiteral("gameLastExecutionTime"), argumentList);
    }

    inline QDBusPendingReply<qulonglong> installDate()
    {
        QList<QVariant> argumentList;
        return asyncCallWithArgumentList(QStringLiteral("installDate"), argumentList);
    }

    inline QDBusPendingReply<QString> installWithService()
    {
        QList<QVariant> argumentList;
        return asyncCallWithArgumentList(QStringLiteral("installWithService"), argumentList);
    }

    inline QDBusPendingReply<bool> isFirstStart()
    {
        QList<QVariant> argumentList;
        return asyncCallWithArgumentList(QStringLiteral("isFirstStart"), argumentList);
    }

    inline QDBusPendingReply<bool> isGameInstalled(const QString &serviceId)
    {
        QList<QVariant> argumentList;
        argumentList << QVariant::fromValue(serviceId);
        return asyncCallWithArgumentList(QStringLiteral("isGameInstalled"), argumentList);
    }

Q_SIGNALS: // SIGNALS
};

namespace com {
  namespace p1 {
    namespace dbus {
      typedef ::ApplicationStatisticBridgeProxy ApplicationStatistic;
    }
  }
}
#endif
