/*
 * This file was generated by qdbusxml2cpp version 0.8
 * Command line was: qdbusxml2cpp -v -a ApplicationBridgeAdaptor -c ApplicationBridgeAdaptor ApplicationBridge.interface.xml
 *
 * qdbusxml2cpp is Copyright (C) 2013 Digia Plc and/or its subsidiary(-ies).
 *
 * This is an auto-generated file.
 * Do not edit! All changes made to it will be lost.
 */

#include <Host/DBus/ApplicationBridgeAdaptor.h>
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

bool ApplicationBridgeAdaptor::isInitCompleted()
{
    // handle method call com.gamenet.dbus.Application.isInitCompleted
    bool out0;
    QMetaObject::invokeMethod(parent(), "isInitCompleted", Q_RETURN_ARG(bool, out0));
    return out0;
}

void ApplicationBridgeAdaptor::restartApplication(bool isMinimized)
{
    // handle method call com.gamenet.dbus.Application.restartApplication
    QMetaObject::invokeMethod(parent(), "restartApplication", Q_ARG(bool, isMinimized));
}

void ApplicationBridgeAdaptor::switchClientVersion()
{
    // handle method call com.gamenet.dbus.Application.switchClientVersion
    QMetaObject::invokeMethod(parent(), "switchClientVersion");
}

