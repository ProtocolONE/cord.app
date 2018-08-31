/*
 * This file was generated by qdbusxml2cpp version 0.8
 * Command line was: qdbusxml2cpp -V -a ConnectionBridgeAdaptor -c ConnectionBridgeAdaptor ConnectionBridge.interface.xml
 *
 * qdbusxml2cpp is Copyright (C) 2017 The Qt Company Ltd.
 *
 * This is an auto-generated file.
 * Do not edit! All changes made to it will be lost.
 */

#include <Host/Dbus/ConnectionBridgeAdaptor.h>
#include <QtCore/QMetaObject>
#include <QtCore/QByteArray>
#include <QtCore/QList>
#include <QtCore/QMap>
#include <QtCore/QString>
#include <QtCore/QStringList>
#include <QtCore/QVariant>

/*
 * Implementation of adaptor class ConnectionBridgeAdaptor
 */

ConnectionBridgeAdaptor::ConnectionBridgeAdaptor(QObject *parent)
    : QDBusAbstractAdaptor(parent)
{
    // constructor
    setAutoRelaySignals(true);
}

ConnectionBridgeAdaptor::~ConnectionBridgeAdaptor()
{
    // destructor
}

void ConnectionBridgeAdaptor::close()
{
    // handle method call com.protocolone.launcher.dbus.Connection.close
    QMetaObject::invokeMethod(parent(), "close");
}

void ConnectionBridgeAdaptor::ping()
{
    // handle method call com.protocolone.launcher.dbus.Connection.ping
    QMetaObject::invokeMethod(parent(), "ping");
}

void ConnectionBridgeAdaptor::setApplicationName(const QString &applicationName)
{
    // handle method call com.protocolone.launcher.dbus.Connection.setApplicationName
    QMetaObject::invokeMethod(parent(), "setApplicationName", Q_ARG(QString, applicationName));
}

void ConnectionBridgeAdaptor::setCredential(P1::Host::Bridge::Credential credential)
{
    // handle method call com.protocolone.launcher.dbus.Connection.setCredential
    QMetaObject::invokeMethod(parent(), "setCredential", Q_ARG(P1::Host::Bridge::Credential, credential));
}

void ConnectionBridgeAdaptor::setSecondCredential(P1::Host::Bridge::Credential credential)
{
    // handle method call com.protocolone.launcher.dbus.Connection.setSecondCredential
    QMetaObject::invokeMethod(parent(), "setSecondCredential", Q_ARG(P1::Host::Bridge::Credential, credential));
}

