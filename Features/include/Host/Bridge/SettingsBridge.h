#pragma once

#include <QtCore/QObject>
#include <QtCore/QString>

namespace P1 {
  namespace Host {

    class SettingsManager;

    namespace Bridge {

      class SettingsBridge : public QObject
      {
        Q_OBJECT
          Q_CLASSINFO("Version", "1.0.0.0")
          Q_CLASSINFO("D-Bus Interface", "com.protocolone.launcher.dbus.RemoteSettings")
      public:
        explicit SettingsBridge(QObject *parent = 0);
        virtual ~SettingsBridge();

        void setProxy(Host::SettingsManager *proxy);

      public slots:
        void setValue(const QString& path, const QString& key, const QString& data);
        QString value(const QString& path, const QString& key, const QString& defValue);

      private:
        Host::SettingsManager *_settingsManager;
      };

    }
  }
}