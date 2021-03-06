/*
 * This file was generated by qdbusxml2cpp version 0.8
 * Command line was: qdbusxml2cpp -V -p SettingsBridgeProxy -c SettingsBridgeProxy SettingsBridge.interface.xml
 *
 * qdbusxml2cpp is Copyright (C) 2017 The Qt Company Ltd.
 *
 * This is an auto-generated file.
 * Do not edit! All changes made to it will be lost.
 */

#ifndef SETTINGSBRIDGEPROXY_H
#define SETTINGSBRIDGEPROXY_H

#include <QtCore/QObject>
#include <QtCore/QByteArray>
#include <QtCore/QList>
#include <QtCore/QMap>
#include <QtCore/QString>
#include <QtCore/QStringList>
#include <QtCore/QVariant>
#include <QtDBus/QtDBus>

/*
 * Proxy class for interface com.protocolone.launcher.dbus.RemoteSettings
 */
class SettingsBridgeProxy: public QDBusAbstractInterface
{
    Q_OBJECT
public:
    static inline const char *staticInterfaceName()
    { return "com.protocolone.launcher.dbus.RemoteSettings"; }

public:
    SettingsBridgeProxy(const QString &service, const QString &path, const QDBusConnection &connection, QObject *parent = nullptr);

    ~SettingsBridgeProxy();

public Q_SLOTS: // METHODS
    inline QDBusPendingReply<> setValue(const QString &path, const QString &key, const QString &data)
    {
        QList<QVariant> argumentList;
        argumentList << QVariant::fromValue(path) << QVariant::fromValue(key) << QVariant::fromValue(data);
        return asyncCallWithArgumentList(QStringLiteral("setValue"), argumentList);
    }

    inline QDBusPendingReply<QString> value(const QString &path, const QString &key, const QString &defValue)
    {
        QList<QVariant> argumentList;
        argumentList << QVariant::fromValue(path) << QVariant::fromValue(key) << QVariant::fromValue(defValue);
        return asyncCallWithArgumentList(QStringLiteral("value"), argumentList);
    }

Q_SIGNALS: // SIGNALS
};

namespace com {
  namespace p1 {
    namespace dbus {
      typedef ::SettingsBridgeProxy RemoteSettings;
    }
  }
}
#endif
