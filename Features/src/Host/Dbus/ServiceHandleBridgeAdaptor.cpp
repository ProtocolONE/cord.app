/*
 * This file was generated by qdbusxml2cpp version 0.8
 * Command line was: qdbusxml2cpp -V -a ServiceHandleBridgeAdaptor -c ServiceHandleBridgeAdaptor ServiceHandleBridge.interface.xml
 *
 * qdbusxml2cpp is Copyright (C) 2017 The Qt Company Ltd.
 *
 * This is an auto-generated file.
 * Do not edit! All changes made to it will be lost.
 */

#include <Host/Dbus/ServiceHandleBridgeAdaptor.h>
#include <QtCore/QMetaObject>
#include <QtCore/QByteArray>
#include <QtCore/QList>
#include <QtCore/QMap>
#include <QtCore/QString>
#include <QtCore/QStringList>
#include <QtCore/QVariant>

/*
 * Implementation of adaptor class ServiceHandleBridgeAdaptor
 */

ServiceHandleBridgeAdaptor::ServiceHandleBridgeAdaptor(QObject *parent)
    : QDBusAbstractAdaptor(parent)
{
    // constructor
    setAutoRelaySignals(true);
}

ServiceHandleBridgeAdaptor::~ServiceHandleBridgeAdaptor()
{
    // destructor
}

bool ServiceHandleBridgeAdaptor::lock(const QString &serviceId)
{
    // handle method call com.protocolone.launcher.dbus.ServiceHandle.lock
    bool out0;
    QMetaObject::invokeMethod(parent(), "lock", Q_RETURN_ARG(bool, out0), Q_ARG(QString, serviceId));
    return out0;
}

void ServiceHandleBridgeAdaptor::unlock(const QString &serviceId)
{
    // handle method call com.protocolone.launcher.dbus.ServiceHandle.unlock
    QMetaObject::invokeMethod(parent(), "unlock", Q_ARG(QString, serviceId));
}

