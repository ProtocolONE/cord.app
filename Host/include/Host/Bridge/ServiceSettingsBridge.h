#pragma once

#include <QtCore/QObject>

namespace GameNet {
  namespace Host {

    class ServiceSettings;
    namespace Bridge {

      class ServiceSettingsBridge : public QObject
      {
        Q_OBJECT
        Q_CLASSINFO("Version", "1.0.0.0")
        Q_CLASSINFO("D-Bus Interface", "com.gamenet.dbus.ServiceSettings")
      public:
        explicit ServiceSettingsBridge(QObject *parent = 0);
        virtual ~ServiceSettingsBridge();

        void setSettings(ServiceSettings *value);

      public slots:

        bool isDefaultInstallPath(const QString& serviceId) const;
        bool isDefaultDownloadPath(const QString& serviceId) const;
        bool hasDownloadPath(const QString& serviceId) const;

        QString downloadPath(const QString& serviceId) const;
        void setDownloadPath(const QString& serviceId, const QString& path);

        QString installPath(const QString& serviceId) const;
        void setInstallPath(const QString& serviceId, const QString& path);

        int gameSize(const QString& serviceId) const;
        QString name(const QString& serviceId) const;

      private:
        ServiceSettings *_settings;
      };

    }
  }
}