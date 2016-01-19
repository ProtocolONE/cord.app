/*
 * This file was generated by qdbusxml2cpp version 0.8
 * Command line was: qdbusxml2cpp -v -a LicenseManagerBridgeAdaptor -c LicenseManagerBridgeAdaptor LicenseManagerBridge.interface.xml
 *
 * qdbusxml2cpp is Copyright (C) 2015 The Qt Company Ltd.
 *
 * This is an auto-generated file.
 * This file may have been hand-edited. Look for HAND-EDIT comments
 * before re-generating it.
 */

#ifndef LICENSEMANAGERBRIDGEADAPTOR_H
#define LICENSEMANAGERBRIDGEADAPTOR_H

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
 * Adaptor class for interface com.gamenet.dbus.LicenseManager
 */
class LicenseManagerBridgeAdaptor: public QDBusAbstractAdaptor
{
    Q_OBJECT
    Q_CLASSINFO("D-Bus Interface", "com.gamenet.dbus.LicenseManager")
    Q_CLASSINFO("D-Bus Introspection", ""
"  <interface name=\"com.gamenet.dbus.LicenseManager\">\n"
"    <method name=\"hasAcceptedLicense\">\n"
"      <arg direction=\"out\" type=\"b\"/>\n"
"    </method>\n"
"    <method name=\"hasAcceptedLicense\">\n"
"      <arg direction=\"out\" type=\"b\"/>\n"
"      <arg direction=\"in\" type=\"s\" name=\"serviceId\"/>\n"
"    </method>\n"
"    <method name=\"acceptLicense\">\n"
"      <arg direction=\"in\" type=\"s\" name=\"serviceId\"/>\n"
"      <arg direction=\"in\" type=\"s\" name=\"hash\"/>\n"
"    </method>\n"
"  </interface>\n"
        "")
public:
    LicenseManagerBridgeAdaptor(QObject *parent);
    virtual ~LicenseManagerBridgeAdaptor();

public: // PROPERTIES
public Q_SLOTS: // METHODS
    void acceptLicense(const QString &serviceId, const QString &hash);
    bool hasAcceptedLicense(const QString &serviceId);
    bool hasAcceptedLicense();
Q_SIGNALS: // SIGNALS
};

#endif