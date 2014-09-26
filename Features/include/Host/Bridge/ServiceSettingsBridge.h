#pragma once

#include <QtCore/QObject>

namespace GameNet {
  namespace Host {

    class ServiceSettings;
    namespace Bridge {

      /*!
      \class ServiceSettingsBridge

      \brief ServiceSettingsBridge class provides an interface to manipulate settings of particular service.

      ServiceSettingsBridge class provides a 

      */ 
      class ServiceSettingsBridge : public QObject
      {
        Q_OBJECT
          Q_CLASSINFO("Version", "1.0.0.0")
          Q_CLASSINFO("D-Bus Interface", "com.gamenet.dbus.ServiceSettings")
      public:
        /*!
        \fn explicit ServiceSettingsBridge(QObject *parent = 0);

        Creates a ServiceSettingsBridge object with parent parent.

        */
        explicit ServiceSettingsBridge(QObject *parent = 0);
        /*!
        \fn virtual ~ServiceSettingsBridge();

        Destroys ServiceSettingsBridge object instance.

        */
        virtual ~ServiceSettingsBridge();

        /// \cond
        void setSettings(ServiceSettings *value);
        /// \endcond

      public slots:
        /*!
        \fn bool isDownloadable(const QString& serviceId) const;

        Method returns true if service needs installation on user PC.

        */
        bool isDownloadable(const QString& serviceId) const;
        /*!
        \fn bool isDefaultInstallPath(const QString& serviceId) const;

        Method returns true if service installation path is equal to one proposed by application.

        */
        bool isDefaultInstallPath(const QString& serviceId) const;
        /*!
        \fn bool isDefaultDownloadPath(const QString& serviceId) const;

        Method returns true if service download destination path is equal to one proposed by application.

        */
        bool isDefaultDownloadPath(const QString& serviceId) const;
        /*!
        \fn bool hasDownloadPath(const QString& serviceId) const;

        Method returns true if service installation path should differ from it's download path.

        */
        bool hasDownloadPath(const QString& serviceId) const;

        /*!
        \fn QString downloadPath(const QString& serviceId) const;

        Method returns download destination path for a service with identification number specified in serviceId parameter.

        */
        QString downloadPath(const QString& serviceId) const;
        /*!
        \fn void setDownloadPath(const QString& serviceId, const QString& path);

        Method sets download destination path for a service with identification number specified in serviceId parameter.

        */
        void setDownloadPath(const QString& serviceId, const QString& path);

        /*!
        \fn QString installPath(const QString& serviceId) const;

        Method returns installation destination path for a service with identification number specified in serviceId parameter.

        */
        QString installPath(const QString& serviceId) const;
        /*!
        \fn void setInstallPath(const QString& serviceId, const QString& path);

        Method sets installation destination path for a service with identification number specified in serviceId parameter.

        */
        void setInstallPath(const QString& serviceId, const QString& path);

        /*!
        \fn int gameSize(const QString& serviceId) const;

        Method returns approximate size of service in megabytes.

        */
        quint64 gameSize(const QString& serviceId) const;
        /*!
        \fn QString name(const QString& serviceId) const;

        Method returns human-readable service name.

        */
        QString name(const QString& serviceId) const;

      private:
        ServiceSettings *_settings;
      };

    }
  }
}