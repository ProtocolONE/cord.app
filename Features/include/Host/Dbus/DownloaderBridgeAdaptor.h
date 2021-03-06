/*
 * This file was generated by qdbusxml2cpp version 0.8
 * Command line was: qdbusxml2cpp -V -a DownloaderBridgeAdaptor -c DownloaderBridgeAdaptor DownloaderBridge.interface.xml
 *
 * qdbusxml2cpp is Copyright (C) 2017 The Qt Company Ltd.
 *
 * This is an auto-generated file.
 * This file may have been hand-edited. Look for HAND-EDIT comments
 * before re-generating it.
 */

#ifndef DOWNLOADERBRIDGEADAPTOR_H
#define DOWNLOADERBRIDGEADAPTOR_H

#include <Host/Bridge/DownloaderBridge.h>

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
 * Adaptor class for interface com.protocolone.launcher.dbus.Downloader
 */
class DownloaderBridgeAdaptor: public QDBusAbstractAdaptor
{
    Q_OBJECT
    Q_CLASSINFO("D-Bus Interface", "com.protocolone.launcher.dbus.Downloader")
    Q_CLASSINFO("D-Bus Introspection", ""
"  <interface name=\"com.protocolone.launcher.dbus.Downloader\">\n"
"    <signal name=\"started\">\n"
"      <arg direction=\"out\" type=\"s\" name=\"serviceId\"/>\n"
"      <arg direction=\"out\" type=\"i\" name=\"startType\"/>\n"
"    </signal>\n"
"    <signal name=\"finished\">\n"
"      <arg direction=\"out\" type=\"s\" name=\"serviceId\"/>\n"
"    </signal>\n"
"    <signal name=\"stopped\">\n"
"      <arg direction=\"out\" type=\"s\" name=\"serviceId\"/>\n"
"    </signal>\n"
"    <signal name=\"stopping\">\n"
"      <arg direction=\"out\" type=\"s\" name=\"serviceId\"/>\n"
"    </signal>\n"
"    <signal name=\"failed\">\n"
"      <arg direction=\"out\" type=\"s\" name=\"serviceId\"/>\n"
"    </signal>\n"
"    <signal name=\"serviceInstalled\">\n"
"      <arg direction=\"out\" type=\"s\" name=\"serviceId\"/>\n"
"    </signal>\n"
"    <signal name=\"serviceUninstalled\">\n"
"      <arg direction=\"out\" type=\"s\" name=\"serviceId\"/>\n"
"    </signal>\n"
"    <signal name=\"serviceUpdated\">\n"
"      <arg direction=\"out\" type=\"s\" name=\"serviceId\"/>\n"
"    </signal>\n"
"    <signal name=\"statusMessageChanged\">\n"
"      <arg direction=\"out\" type=\"s\" name=\"serviceId\"/>\n"
"      <arg direction=\"out\" type=\"s\" name=\"message\"/>\n"
"    </signal>\n"
"    <signal name=\"totalProgress\">\n"
"      <arg direction=\"out\" type=\"s\" name=\"serviceId\"/>\n"
"      <arg direction=\"out\" type=\"i\" name=\"progress\"/>\n"
"    </signal>\n"
"    <signal name=\"accessRequired\">\n"
"      <arg direction=\"out\" type=\"s\" name=\"serviceId\"/>\n"
"    </signal>\n"
"    <method name=\"isInProgress\">\n"
"      <arg direction=\"out\" type=\"b\"/>\n"
"      <arg direction=\"in\" type=\"s\" name=\"serviceId\"/>\n"
"    </method>\n"
"    <method name=\"isAnyServiceInProgress\">\n"
"      <arg direction=\"out\" type=\"b\"/>\n"
"    </method>\n"
"    <method name=\"isInstalled\">\n"
"      <arg direction=\"out\" type=\"b\"/>\n"
"      <arg direction=\"in\" type=\"s\" name=\"serviceId\"/>\n"
"    </method>\n"
"    <method name=\"start\">\n"
"      <arg direction=\"in\" type=\"s\" name=\"serviceId\"/>\n"
"      <arg direction=\"in\" type=\"i\" name=\"startType\"/>\n"
"      <annotation value=\"true\" name=\"org.freedesktop.DBus.Method.NoReply\"/>\n"
"    </method>\n"
"    <method name=\"stop\">\n"
"      <arg direction=\"in\" type=\"s\" name=\"serviceId\"/>\n"
"      <annotation value=\"true\" name=\"org.freedesktop.DBus.Method.NoReply\"/>\n"
"    </method>\n"
"    <method name=\"pauseSession\">\n"
"      <annotation value=\"true\" name=\"org.freedesktop.DBus.Method.NoReply\"/>\n"
"    </method>\n"
"    <method name=\"resumeSession\">\n"
"      <annotation value=\"true\" name=\"org.freedesktop.DBus.Method.NoReply\"/>\n"
"    </method>\n"
"    <signal name=\"downloadProgress\">\n"
"      <arg direction=\"out\" type=\"s\" name=\"serviceId\"/>\n"
"      <arg direction=\"out\" type=\"i\" name=\"progress\"/>\n"
"      <arg direction=\"out\" type=\"a(sii)\" name=\"args\"/>\n"
"      <annotation value=\"P1::Host::Bridge::DownloadProgressArgs\" name=\"org.qtproject.QtDBus.QtTypeName.In2\"/>\n"
"    </signal>\n"
"  </interface>\n"
        "")
public:
    DownloaderBridgeAdaptor(QObject *parent);
    virtual ~DownloaderBridgeAdaptor();

public: // PROPERTIES
public Q_SLOTS: // METHODS
    bool isAnyServiceInProgress();
    bool isInProgress(const QString &serviceId);
    bool isInstalled(const QString &serviceId);
    Q_NOREPLY void pauseSession();
    Q_NOREPLY void resumeSession();
    Q_NOREPLY void start(const QString &serviceId, int startType);
    Q_NOREPLY void stop(const QString &serviceId);
Q_SIGNALS: // SIGNALS
    void accessRequired(const QString &serviceId);
    void downloadProgress(const QString &serviceId, int progress, P1::Host::Bridge::DownloadProgressArgs args);
    void failed(const QString &serviceId);
    void finished(const QString &serviceId);
    void serviceInstalled(const QString &serviceId);
    void serviceUninstalled(const QString &serviceId);
    void serviceUpdated(const QString &serviceId);
    void started(const QString &serviceId, int startType);
    void statusMessageChanged(const QString &serviceId, const QString &message);
    void stopped(const QString &serviceId);
    void stopping(const QString &serviceId);
    void totalProgress(const QString &serviceId, int progress);
};

#endif
