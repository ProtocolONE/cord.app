/*
 * This file was generated by qdbusxml2cpp version 0.8
 * Command line was: qdbusxml2cpp -v -a ApplicationBridgeAdaptor -c ApplicationBridgeAdaptor ApplicationBridge.interface.xml
 *
 * qdbusxml2cpp is Copyright (C) 2013 Digia Plc and/or its subsidiary(-ies).
 *
 * This is an auto-generated file.
 * This file may have been hand-edited. Look for HAND-EDIT comments
 * before re-generating it.
 */

#ifndef APPLICATIONBRIDGEADAPTOR_H_1410952954
#define APPLICATIONBRIDGEADAPTOR_H_1410952954

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
"    <signal name=\"initCompleted\"/>\n"
"    <signal name=\"restartUIRequest\"/>\n"
"    <method name=\"isInitCompleted\">\n"
"      <arg direction=\"out\" type=\"b\"/>\n"
"    </method>\n"
"    <method name=\"restartApplication\">\n"
"      <arg direction=\"in\" type=\"b\" name=\"isMinimized\"/>\n"
"    </method>\n"
"    <method name=\"switchClientVersion\"/>\n"
"  </interface>\n"
        "")
public:
    ApplicationBridgeAdaptor(QObject *parent);
    virtual ~ApplicationBridgeAdaptor();

public: // PROPERTIES
public Q_SLOTS: // METHODS
    bool isInitCompleted();
    void restartApplication(bool isMinimized);
    void switchClientVersion();
Q_SIGNALS: // SIGNALS
    void initCompleted();
    void restartUIRequest();
};

#endif
