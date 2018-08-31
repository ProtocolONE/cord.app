/*
 * This file was generated by qdbusxml2cpp version 0.8
 * Command line was: qdbusxml2cpp -V -a UpdateManagerBridgeAdaptor -c UpdateManagerBridgeAdaptor UpdateManagerBridge.interface.xml
 *
 * qdbusxml2cpp is Copyright (C) 2017 The Qt Company Ltd.
 *
 * This is an auto-generated file.
 * Do not edit! All changes made to it will be lost.
 */

#include <Host/Dbus/UpdateManagerBridgeAdaptor.h>
#include <QtCore/QMetaObject>
#include <QtCore/QByteArray>
#include <QtCore/QList>
#include <QtCore/QMap>
#include <QtCore/QString>
#include <QtCore/QStringList>
#include <QtCore/QVariant>

/*
 * Implementation of adaptor class UpdateManagerBridgeAdaptor
 */

UpdateManagerBridgeAdaptor::UpdateManagerBridgeAdaptor(QObject *parent)
    : QDBusAbstractAdaptor(parent)
{
    // constructor
    setAutoRelaySignals(true);
}

UpdateManagerBridgeAdaptor::~UpdateManagerBridgeAdaptor()
{
    // destructor
}

int UpdateManagerBridgeAdaptor::updateState()
{
    // handle method call com.protocolone.launcher.dbus.Updater.updateState
    int out0;
    QMetaObject::invokeMethod(parent(), "updateState", Q_RETURN_ARG(int, out0));
    return out0;
}

