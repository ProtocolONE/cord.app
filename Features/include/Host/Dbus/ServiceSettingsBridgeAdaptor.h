/*
 * This file was generated by qdbusxml2cpp version 0.8
 * Command line was: qdbusxml2cpp -v -a ServiceSettingsBridgeAdaptor -c ServiceSettingsBridgeAdaptor ServiceSettingsBridge.interface.xml
 *
 * qdbusxml2cpp is Copyright (C) 2015 The Qt Company Ltd.
 *
 * This is an auto-generated file.
 * This file may have been hand-edited. Look for HAND-EDIT comments
 * before re-generating it.
 */

#ifndef SERVICESETTINGSBRIDGEADAPTOR_H
#define SERVICESETTINGSBRIDGEADAPTOR_H

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
 * Adaptor class for interface com.gamenet.dbus.ServiceSettings
 */
class ServiceSettingsBridgeAdaptor: public QDBusAbstractAdaptor
{
    Q_OBJECT
    Q_CLASSINFO("D-Bus Interface", "com.gamenet.dbus.ServiceSettings")
    Q_CLASSINFO("D-Bus Introspection", ""
"  <interface name=\"com.gamenet.dbus.ServiceSettings\">\n"
"    <method name=\"isDownloadable\">\n"
"      <arg direction=\"out\" type=\"b\"/>\n"
"      <arg direction=\"in\" type=\"s\" name=\"serviceId\"/>\n"
"    </method>\n"
"    <method name=\"isDefaultInstallPath\">\n"
"      <arg direction=\"out\" type=\"b\"/>\n"
"      <arg direction=\"in\" type=\"s\" name=\"serviceId\"/>\n"
"    </method>\n"
"    <method name=\"isDefaultDownloadPath\">\n"
"      <arg direction=\"out\" type=\"b\"/>\n"
"      <arg direction=\"in\" type=\"s\" name=\"serviceId\"/>\n"
"    </method>\n"
"    <method name=\"hasDownloadPath\">\n"
"      <arg direction=\"out\" type=\"b\"/>\n"
"      <arg direction=\"in\" type=\"s\" name=\"serviceId\"/>\n"
"    </method>\n"
"    <method name=\"downloadPath\">\n"
"      <arg direction=\"out\" type=\"s\"/>\n"
"      <arg direction=\"in\" type=\"s\" name=\"serviceId\"/>\n"
"    </method>\n"
"    <method name=\"setDownloadPath\">\n"
"      <arg direction=\"in\" type=\"s\" name=\"serviceId\"/>\n"
"      <arg direction=\"in\" type=\"s\" name=\"path\"/>\n"
"    </method>\n"
"    <method name=\"installPath\">\n"
"      <arg direction=\"out\" type=\"s\"/>\n"
"      <arg direction=\"in\" type=\"s\" name=\"serviceId\"/>\n"
"    </method>\n"
"    <method name=\"setInstallPath\">\n"
"      <arg direction=\"in\" type=\"s\" name=\"serviceId\"/>\n"
"      <arg direction=\"in\" type=\"s\" name=\"path\"/>\n"
"    </method>\n"
"    <method name=\"gameSize\">\n"
"      <arg direction=\"out\" type=\"t\"/>\n"
"      <arg direction=\"in\" type=\"s\" name=\"serviceId\"/>\n"
"    </method>\n"
"    <method name=\"name\">\n"
"      <arg direction=\"out\" type=\"s\"/>\n"
"      <arg direction=\"in\" type=\"s\" name=\"serviceId\"/>\n"
"    </method>\n"
"    <method name=\"isOverlayEnabled\">\n"
"      <arg direction=\"out\" type=\"b\"/>\n"
"      <arg direction=\"in\" type=\"s\" name=\"serviceId\"/>\n"
"    </method>\n"
"    <method name=\"setOverlayEnabled\">\n"
"      <arg direction=\"in\" type=\"s\" name=\"serviceId\"/>\n"
"      <arg direction=\"in\" type=\"b\" name=\"enabled\"/>\n"
"    </method>\n"
"    <method name=\"isPrefer32Bit\">\n"
"      <arg direction=\"out\" type=\"b\"/>\n"
"      <arg direction=\"in\" type=\"s\" name=\"serviceId\"/>\n"
"    </method>\n"
"    <method name=\"setPrefer32Bit\">\n"
"      <arg direction=\"in\" type=\"s\" name=\"serviceId\"/>\n"
"      <arg direction=\"in\" type=\"b\" name=\"value\"/>\n"
"    </method>\n"
"  </interface>\n"
        "")
public:
    ServiceSettingsBridgeAdaptor(QObject *parent);
    virtual ~ServiceSettingsBridgeAdaptor();

public: // PROPERTIES
public Q_SLOTS: // METHODS
    QString downloadPath(const QString &serviceId);
    qulonglong gameSize(const QString &serviceId);
    bool hasDownloadPath(const QString &serviceId);
    QString installPath(const QString &serviceId);
    bool isDefaultDownloadPath(const QString &serviceId);
    bool isDefaultInstallPath(const QString &serviceId);
    bool isDownloadable(const QString &serviceId);
    bool isOverlayEnabled(const QString &serviceId);
    bool isPrefer32Bit(const QString &serviceId);
    QString name(const QString &serviceId);
    void setDownloadPath(const QString &serviceId, const QString &path);
    void setInstallPath(const QString &serviceId, const QString &path);
    void setOverlayEnabled(const QString &serviceId, bool enabled);
    void setPrefer32Bit(const QString &serviceId, bool value);
Q_SIGNALS: // SIGNALS
};

#endif
