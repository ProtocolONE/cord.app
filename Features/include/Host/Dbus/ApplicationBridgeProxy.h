/*
 * This file was generated by qdbusxml2cpp version 0.8
 * Command line was: qdbusxml2cpp -v -p ApplicationBridgeProxy -c ApplicationBridgeProxy ApplicationBridge.interface.xml
 *
 * qdbusxml2cpp is Copyright (C) 2013 Digia Plc and/or its subsidiary(-ies).
 *
 * This is an auto-generated file.
 * Do not edit! All changes made to it will be lost.
 */

#ifndef APPLICATIONBRIDGEPROXY_H_1414563432
#define APPLICATIONBRIDGEPROXY_H_1414563432

#include <QtCore/QObject>
#include <QtCore/QByteArray>
#include <QtCore/QList>
#include <QtCore/QMap>
#include <QtCore/QString>
#include <QtCore/QStringList>
#include <QtCore/QVariant>
#include <QtDBus/QtDBus>

/*
 * Proxy class for interface com.gamenet.dbus.Application
 */
class ApplicationBridgeProxy: public QDBusAbstractInterface
{
    Q_OBJECT
public:
    static inline const char *staticInterfaceName()
    { return "com.gamenet.dbus.Application"; }

public:
    ApplicationBridgeProxy(const QString &service, const QString &path, const QDBusConnection &connection, QObject *parent = 0);

    ~ApplicationBridgeProxy();

    Q_PROPERTY(QString language READ language WRITE setLanguage)
    inline QString language() const
    { return qvariant_cast< QString >(property("language")); }
    inline void setLanguage(const QString &value)
    { setProperty("language", QVariant::fromValue(value)); }

public Q_SLOTS: // METHODS
    inline QDBusPendingReply<bool> isInitCompleted()
    {
        QList<QVariant> argumentList;
        return asyncCallWithArgumentList(QLatin1String("isInitCompleted"), argumentList);
    }

    inline QDBusPendingReply<> openBrowser(const QString &url)
    {
        QList<QVariant> argumentList;
        argumentList << QVariant::fromValue(url);
        return asyncCallWithArgumentList(QLatin1String("openBrowser"), argumentList);
    }

    inline QDBusPendingReply<> restartApplication(bool isMinimized)
    {
        QList<QVariant> argumentList;
        argumentList << QVariant::fromValue(isMinimized);
        return asyncCallWithArgumentList(QLatin1String("restartApplication"), argumentList);
    }

    inline QDBusPendingReply<> switchClientVersion()
    {
        QList<QVariant> argumentList;
        return asyncCallWithArgumentList(QLatin1String("switchClientVersion"), argumentList);
    }

Q_SIGNALS: // SIGNALS
    void initCompleted();
    void languageChanged();
    void restartUIRequest();
};

namespace com {
  namespace gamenet {
    namespace dbus {
      typedef ::ApplicationBridgeProxy Application;
    }
  }
}
#endif
