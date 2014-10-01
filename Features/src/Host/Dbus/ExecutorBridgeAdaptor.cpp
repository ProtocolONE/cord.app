/*
 * This file was generated by qdbusxml2cpp version 0.8
 * Command line was: qdbusxml2cpp -v -a ExecutorBridgeAdaptor -c ExecutorBridgeAdaptor ExecutorBridge.interface.xml
 *
 * qdbusxml2cpp is Copyright (C) 2013 Digia Plc and/or its subsidiary(-ies).
 *
 * This is an auto-generated file.
 * Do not edit! All changes made to it will be lost.
 */

#include <Host/Dbus/ExecutorBridgeAdaptor.h>

#include <QtCore/QMetaObject>
#include <QtCore/QByteArray>
#include <QtCore/QList>
#include <QtCore/QMap>
#include <QtCore/QString>
#include <QtCore/QStringList>
#include <QtCore/QVariant>

/*
 * Implementation of adaptor class ExecutorBridgeAdaptor
 */

ExecutorBridgeAdaptor::ExecutorBridgeAdaptor(QObject *parent)
    : QDBusAbstractAdaptor(parent)
{
    // constructor
    setAutoRelaySignals(true);
}

ExecutorBridgeAdaptor::~ExecutorBridgeAdaptor()
{
    // destructor
}

bool ExecutorBridgeAdaptor::canExecuteSecond(const QString &serviceId)
{
    // handle method call com.gamenet.dbus.Executor.canExecuteSecond
    bool out0;
    QMetaObject::invokeMethod(parent(), "canExecuteSecond", Q_RETURN_ARG(bool, out0), Q_ARG(QString, serviceId));
    return out0;
}

void ExecutorBridgeAdaptor::execute(const QString &serviceId, GameNet::Host::Bridge::Credential credential)
{
    // handle method call com.gamenet.dbus.Executor.execute
    QMetaObject::invokeMethod(parent(), "execute", Q_ARG(QString, serviceId), Q_ARG(GameNet::Host::Bridge::Credential, credential));
}

void ExecutorBridgeAdaptor::executeSecond(const QString &serviceId, GameNet::Host::Bridge::Credential credential, GameNet::Host::Bridge::Credential secondCredential)
{
    // handle method call com.gamenet.dbus.Executor.executeSecond
    QMetaObject::invokeMethod(parent(), "executeSecond", Q_ARG(QString, serviceId), Q_ARG(GameNet::Host::Bridge::Credential, credential), Q_ARG(GameNet::Host::Bridge::Credential, secondCredential));
}

bool ExecutorBridgeAdaptor::isAnyGameStarted()
{
    // handle method call com.gamenet.dbus.Executor.isAnyGameStarted
    bool out0;
    QMetaObject::invokeMethod(parent(), "isAnyGameStarted", Q_RETURN_ARG(bool, out0));
    return out0;
}

bool ExecutorBridgeAdaptor::isGameStarted(const QString &serviceId)
{
    // handle method call com.gamenet.dbus.Executor.isGameStarted
    bool out0;
    QMetaObject::invokeMethod(parent(), "isGameStarted", Q_RETURN_ARG(bool, out0), Q_ARG(QString, serviceId));
    return out0;
}

void ExecutorBridgeAdaptor::shutdownSecond()
{
    // handle method call com.gamenet.dbus.Executor.shutdownSecond
    QMetaObject::invokeMethod(parent(), "shutdownSecond");
}
