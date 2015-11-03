/*
 * This file was generated by qdbusxml2cpp version 0.8
 * Command line was: qdbusxml2cpp -v -a ApplicationBridgeAdaptor -c ApplicationBridgeAdaptor ApplicationBridge.interface.xml
 *
 * qdbusxml2cpp is Copyright (C) 2015 The Qt Company Ltd.
 *
 * This is an auto-generated file.
 * This file may have been hand-edited. Look for HAND-EDIT comments
 * before re-generating it.
 */

#ifndef APPLICATIONBRIDGEADAPTOR_H
#define APPLICATIONBRIDGEADAPTOR_H

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
 * Adaptor class for interface com.gamenet.dbus.Application
 */
class ApplicationBridgeAdaptor: public QDBusAbstractAdaptor
{
    Q_OBJECT
    Q_CLASSINFO("D-Bus Interface", "com.gamenet.dbus.Application")
    Q_CLASSINFO("D-Bus Introspection", ""
"  <interface name=\"com.gamenet.dbus.Application\">\n"
"    <property access=\"readwrite\" type=\"s\" name=\"language\"/>\n"
"    <property access=\"readwrite\" type=\"i\" name=\"autoStartMode\"/>\n"
"    <signal name=\"initCompleted\"/>\n"
"    <signal name=\"restartUIRequest\"/>\n"
"    <signal name=\"shutdownUIRequest\"/>\n"
"    <signal name=\"languageChanged\"/>\n"
"    <signal name=\"autoStartModeChanged\"/>\n"
"    <signal name=\"uninstallServiceRequest\">\n"
"      <arg direction=\"out\" type=\"s\" name=\"serviceId\"/>\n"
"    </signal>\n"
"    <signal name=\"additionalResourcesReady\"/>\n"
"    <method name=\"isInitCompleted\">\n"
"      <arg direction=\"out\" type=\"b\"/>\n"
"    </method>\n"
"    <method name=\"restartApplication\">\n"
"      <arg direction=\"in\" type=\"b\" name=\"isMinimized\"/>\n"
"    </method>\n"
"    <method name=\"shutdownUIResult\"/>\n"
"    <method name=\"switchClientVersion\"/>\n"
"    <method name=\"openBrowser\">\n"
"      <arg direction=\"in\" type=\"s\" name=\"url\"/>\n"
"    </method>\n"
"    <method name=\"cancelUninstallServiceRequest\">\n"
"      <arg direction=\"in\" type=\"s\" name=\"serviceId\"/>\n"
"    </method>\n"
"  </interface>\n"
        "")
public:
    ApplicationBridgeAdaptor(QObject *parent);
    virtual ~ApplicationBridgeAdaptor();

public: // PROPERTIES
    Q_PROPERTY(int autoStartMode READ autoStartMode WRITE setAutoStartMode)
    int autoStartMode() const;
    void setAutoStartMode(int value);

    Q_PROPERTY(QString language READ language WRITE setLanguage)
    QString language() const;
    void setLanguage(const QString &value);

public Q_SLOTS: // METHODS
    void cancelUninstallServiceRequest(const QString &serviceId);
    bool isInitCompleted();
    void openBrowser(const QString &url);
    void restartApplication(bool isMinimized);
    void shutdownUIResult();
    void switchClientVersion();
Q_SIGNALS: // SIGNALS
    void additionalResourcesReady();
    void autoStartModeChanged();
    void initCompleted();
    void languageChanged();
    void restartUIRequest();
    void shutdownUIRequest();
    void uninstallServiceRequest(const QString &serviceId);
};

#endif
