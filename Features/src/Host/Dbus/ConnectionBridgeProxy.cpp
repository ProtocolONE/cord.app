/*
 * This file was generated by qdbusxml2cpp version 0.8
 * Command line was: qdbusxml2cpp -v -p ConnectionBridgeProxy -c ConnectionBridgeProxy ConnectionBridge.interface.xml
 *
 * qdbusxml2cpp is Copyright (C) 2013 Digia Plc and/or its subsidiary(-ies).
 *
 * This is an auto-generated file.
 * This file may have been hand-edited. Look for HAND-EDIT comments
 * before re-generating it.
 */

#include <Host/Dbus/ConnectionBridgeProxy.h>

/*
 * Implementation of interface class ConnectionBridgeProxy
 */

ConnectionBridgeProxy::ConnectionBridgeProxy(const QString &service, const QString &path, const QDBusConnection &connection, QObject *parent)
    : QDBusAbstractInterface(service, path, staticInterfaceName(), connection, parent)
{
}

ConnectionBridgeProxy::~ConnectionBridgeProxy()
{
}

