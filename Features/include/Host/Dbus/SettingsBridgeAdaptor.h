/*
 * This file was generated by qdbusxml2cpp version 0.8
 * Command line was: qdbusxml2cpp -V -a SettingsBridgeAdaptor -c SettingsBridgeAdaptor SettingsBridge.interface.xml
 *
 * qdbusxml2cpp is Copyright (C) 2017 The Qt Company Ltd.
 *
 * This is an auto-generated file.
 * This file may have been hand-edited. Look for HAND-EDIT comments
 * before re-generating it.
 */

#ifndef SETTINGSBRIDGEADAPTOR_H
#define SETTINGSBRIDGEADAPTOR_H

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
 * Adaptor class for interface com.protocolone.launcher.dbus.RemoteSettings
 */
class SettingsBridgeAdaptor: public QDBusAbstractAdaptor
{
    Q_OBJECT
    Q_CLASSINFO("D-Bus Interface", "com.protocolone.launcher.dbus.RemoteSettings")
    Q_CLASSINFO("D-Bus Introspection", ""
"  <interface name=\"com.protocolone.launcher.dbus.RemoteSettings\">\n"
"    <method name=\"setValue\">\n"
"      <arg direction=\"in\" type=\"s\" name=\"path\"/>\n"
"      <arg direction=\"in\" type=\"s\" name=\"key\"/>\n"
"      <arg direction=\"in\" type=\"s\" name=\"data\"/>\n"
"    </method>\n"
"    <method name=\"value\">\n"
"      <arg direction=\"out\" type=\"s\"/>\n"
"      <arg direction=\"in\" type=\"s\" name=\"path\"/>\n"
"      <arg direction=\"in\" type=\"s\" name=\"key\"/>\n"
"      <arg direction=\"in\" type=\"s\" name=\"defValue\"/>\n"
"    </method>\n"
"  </interface>\n"
        "")
public:
    SettingsBridgeAdaptor(QObject *parent);
    virtual ~SettingsBridgeAdaptor();

public: // PROPERTIES
public Q_SLOTS: // METHODS
    void setValue(const QString &path, const QString &key, const QString &data);
    QString value(const QString &path, const QString &key, const QString &defValue);
Q_SIGNALS: // SIGNALS
};

#endif
