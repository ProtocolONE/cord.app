/*
 * This file was generated by qdbusxml2cpp version 0.8
 * Command line was: qdbusxml2cpp -v -a ApplicationStatisticBridgeAdaptor -c ApplicationStatisticBridgeAdaptor ApplicationStatisticBridge.interface.xml
 *
 * qdbusxml2cpp is Copyright (C) 2013 Digia Plc and/or its subsidiary(-ies).
 *
 * This is an auto-generated file.
 * Do not edit! All changes made to it will be lost.
 */

#include <Host/Dbus/ApplicationStatisticBridgeAdaptor.h>
#include <QtCore/QMetaObject>
#include <QtCore/QByteArray>
#include <QtCore/QList>
#include <QtCore/QMap>
#include <QtCore/QString>
#include <QtCore/QStringList>
#include <QtCore/QVariant>

/*
 * Implementation of adaptor class ApplicationStatisticBridgeAdaptor
 */

ApplicationStatisticBridgeAdaptor::ApplicationStatisticBridgeAdaptor(QObject *parent)
    : QDBusAbstractAdaptor(parent)
{
    // constructor
    setAutoRelaySignals(true);
}

ApplicationStatisticBridgeAdaptor::~ApplicationStatisticBridgeAdaptor()
{
    // destructor
}

int ApplicationStatisticBridgeAdaptor::executeGameFailedCount(const QString &serviceId)
{
    // handle method call com.gamenet.dbus.ApplicationStatistic.executeGameFailedCount
    int out0;
    QMetaObject::invokeMethod(parent(), "executeGameFailedCount", Q_RETURN_ARG(int, out0), Q_ARG(QString, serviceId));
    return out0;
}

int ApplicationStatisticBridgeAdaptor::executeGameSuccessCount(const QString &serviceId)
{
    // handle method call com.gamenet.dbus.ApplicationStatistic.executeGameSuccessCount
    int out0;
    QMetaObject::invokeMethod(parent(), "executeGameSuccessCount", Q_RETURN_ARG(int, out0), Q_ARG(QString, serviceId));
    return out0;
}

int ApplicationStatisticBridgeAdaptor::executeGameTotalCount(const QString &serviceId)
{
    // handle method call com.gamenet.dbus.ApplicationStatistic.executeGameTotalCount
    int out0;
    QMetaObject::invokeMethod(parent(), "executeGameTotalCount", Q_RETURN_ARG(int, out0), Q_ARG(QString, serviceId));
    return out0;
}

qulonglong ApplicationStatisticBridgeAdaptor::gameInstallDate(const QString &serviceId)
{
    // handle method call com.gamenet.dbus.ApplicationStatistic.gameInstallDate
    qulonglong out0;
    QMetaObject::invokeMethod(parent(), "gameInstallDate", Q_RETURN_ARG(qulonglong, out0), Q_ARG(QString, serviceId));
    return out0;
}

qulonglong ApplicationStatisticBridgeAdaptor::gameLastExecutionTime(const QString &serviceId)
{
    // handle method call com.gamenet.dbus.ApplicationStatistic.gameLastExecutionTime
    qulonglong out0;
    QMetaObject::invokeMethod(parent(), "gameLastExecutionTime", Q_RETURN_ARG(qulonglong, out0), Q_ARG(QString, serviceId));
    return out0;
}

qulonglong ApplicationStatisticBridgeAdaptor::installDate()
{
    // handle method call com.gamenet.dbus.ApplicationStatistic.installDate
    qulonglong out0;
    QMetaObject::invokeMethod(parent(), "installDate", Q_RETURN_ARG(qulonglong, out0));
    return out0;
}

QString ApplicationStatisticBridgeAdaptor::installWithService()
{
    // handle method call com.gamenet.dbus.ApplicationStatistic.installWithService
    QString out0;
    QMetaObject::invokeMethod(parent(), "installWithService", Q_RETURN_ARG(QString, out0));
    return out0;
}

bool ApplicationStatisticBridgeAdaptor::isGameInstalled(const QString &serviceId)
{
    // handle method call com.gamenet.dbus.ApplicationStatistic.isGameInstalled
    bool out0;
    QMetaObject::invokeMethod(parent(), "isGameInstalled", Q_RETURN_ARG(bool, out0), Q_ARG(QString, serviceId));
    return out0;
}

