/*
 * This file was generated by qdbusxml2cpp version 0.8
 * Command line was: qdbusxml2cpp -V -a ApplicationBridgeAdaptor -c ApplicationBridgeAdaptor ApplicationBridge.interface.xml
 *
 * qdbusxml2cpp is Copyright (C) 2017 The Qt Company Ltd.
 *
 * This is an auto-generated file.
 * Do not edit! All changes made to it will be lost.
 */

#include <Host/Dbus/ApplicationBridgeAdaptor.h>
#include <QtCore/QMetaObject>
#include <QtCore/QByteArray>
#include <QtCore/QList>
#include <QtCore/QMap>
#include <QtCore/QString>
#include <QtCore/QStringList>
#include <QtCore/QVariant>

/*
 * Implementation of adaptor class ApplicationBridgeAdaptor
 */

ApplicationBridgeAdaptor::ApplicationBridgeAdaptor(QObject *parent)
    : QDBusAbstractAdaptor(parent)
{
    // constructor
    setAutoRelaySignals(true);
}

ApplicationBridgeAdaptor::~ApplicationBridgeAdaptor()
{
    // destructor
}

int ApplicationBridgeAdaptor::autoStartMode() const
{
    // get the value of property autoStartMode
    return qvariant_cast< int >(parent()->property("autoStartMode"));
}

void ApplicationBridgeAdaptor::setAutoStartMode(int value)
{
    // set the value of property autoStartMode
    parent()->setProperty("autoStartMode", QVariant::fromValue(value));
}

QString ApplicationBridgeAdaptor::language() const
{
    // get the value of property language
    return qvariant_cast< QString >(parent()->property("language"));
}

void ApplicationBridgeAdaptor::setLanguage(const QString &value)
{
    // set the value of property language
    parent()->setProperty("language", QVariant::fromValue(value));
}

void ApplicationBridgeAdaptor::cancelUninstallServiceRequest(const QString &serviceId)
{
    // handle method call com.protocolone.launcher.dbus.Application.cancelUninstallServiceRequest
    QMetaObject::invokeMethod(parent(), "cancelUninstallServiceRequest", Q_ARG(QString, serviceId));
}

bool ApplicationBridgeAdaptor::isInitCompleted()
{
    // handle method call com.protocolone.launcher.dbus.Application.isInitCompleted
    bool out0;
    QMetaObject::invokeMethod(parent(), "isInitCompleted", Q_RETURN_ARG(bool, out0));
    return out0;
}

void ApplicationBridgeAdaptor::restartApplication(bool isMinimized)
{
    // handle method call com.protocolone.launcher.dbus.Application.restartApplication
    QMetaObject::invokeMethod(parent(), "restartApplication", Q_ARG(bool, isMinimized));
}

void ApplicationBridgeAdaptor::shutdownUIResult()
{
    // handle method call com.protocolone.launcher.dbus.Application.shutdownUIResult
    QMetaObject::invokeMethod(parent(), "shutdownUIResult");
}

void ApplicationBridgeAdaptor::switchClientVersion()
{
    // handle method call com.protocolone.launcher.dbus.Application.switchClientVersion
    QMetaObject::invokeMethod(parent(), "switchClientVersion");
}

