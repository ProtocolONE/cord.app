/*
 * This file was generated by qdbusxml2cpp version 0.8
 * Command line was: qdbusxml2cpp -v -a ConnectionBridgeAdaptor -c ConnectionBridgeAdaptor ConnectionBridge.interface.xml
 *
 * qdbusxml2cpp is Copyright (C) 2015 The Qt Company Ltd.
 *
 * This is an auto-generated file.
 * This file may have been hand-edited. Look for HAND-EDIT comments
 * before re-generating it.
 */

#ifndef CONNECTIONBRIDGEADAPTOR_H
#define CONNECTIONBRIDGEADAPTOR_H

#include <QtCore/QObject>
#include <QtDBus/QtDBus>
QT_BEGIN_NAMESPACE
class QByteArray;
template<class T> class QList;
template<class Key, class Value> class QMap;
class QString;
class QStringList;
class QVariant;
QT_END_NAMESPACE

/*
 * Adaptor class for interface com.protocolone.launcher.dbus.Connection
 */
class ConnectionBridgeAdaptor: public QDBusAbstractAdaptor
{
    Q_OBJECT
    Q_CLASSINFO("D-Bus Interface", "com.protocolone.launcher.dbus.Connection")
    Q_CLASSINFO("D-Bus Introspection", ""
"  <interface name=\"com.protocolone.launcher.dbus.Connection\">\n"
"    <signal name=\"authorizationError\">\n"
"      <arg direction=\"out\" type=\"s\" name=\"accessToken\"/>\n"
"      <arg direction=\"out\" type=\"s\" name=\"acccessTokenExpiredTime\"/>\n"
"    </signal>\n"
"    <signal name=\"pong\"/>\n"
"    <method name=\"setApplicationName\">\n"
"      <arg direction=\"in\" type=\"s\" name=\"applicationName\"/>\n"
"    </method>\n"
"    <method name=\"setCredential\">\n"
"      <arg direction=\"in\" type=\"s\" name=\"accessToken\"/>\n"
"      <arg direction=\"in\" type=\"s\" name=\"acccessTokenExpiredTime\"/>\n"
"    </method>\n"
"    <method name=\"updateCredential\">\n"
"      <arg direction=\"in\" type=\"s\" name=\"accessTokenOld\"/>\n"
"      <arg direction=\"in\" type=\"s\" name=\"acccessTokenExpiredTimeOld\"/>\n"
"      <arg direction=\"in\" type=\"s\" name=\"accessTokenNew\"/>\n"
"      <arg direction=\"in\" type=\"s\" name=\"acccessTokenExpiredTimeNew\"/>\n"
"    </method>\n"
"    <method name=\"ping\"/>\n"
"    <method name=\"close\"/>\n"
"  </interface>\n"
        "")
public:
    ConnectionBridgeAdaptor(QObject *parent);
    virtual ~ConnectionBridgeAdaptor();

public: // PROPERTIES
public Q_SLOTS: // METHODS
    void close();
    void ping();
    void setApplicationName(const QString &applicationName);
    void setCredential(const QString &accessToken, const QString &acccessTokenExpiredTime);
    void updateCredential(const QString &accessTokenOld, const QString &acccessTokenExpiredTimeOld, const QString &accessTokenNew, const QString &acccessTokenExpiredTimeNew);
Q_SIGNALS: // SIGNALS
    void authorizationError(const QString &accessToken, const QString &acccessTokenExpiredTime);
    void pong();
};

#endif
