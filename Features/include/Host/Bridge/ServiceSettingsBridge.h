#pragma once

#include <QtCore/QObject>

#include <QtDBus/QDBusAbstractAdaptor>

namespace P1 {
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
          Q_CLASSINFO("D-Bus Interface", "com.protocolone.launcher.dbus.ServiceSettings")
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
        Q_NOREPLY void setDownloadPath(const QString& serviceId, const QString& path);

        /*!
        \fn QString installPath(const QString& serviceId) const;

        Method returns installation destination path for a service with identification number specified in serviceId parameter.

        */
        QString installPath(const QString& serviceId) const;
        /*!
        \fn void setInstallPath(const QString& serviceId, const QString& path);

        Method sets installation destination path for a service with identification number specified in serviceId parameter.

        */
        Q_NOREPLY void setInstallPath(const QString& serviceId, const QString& path);

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

        /**
         * \fn  bool ServiceSettingsBridge::overlayEnabled(const QString& serviceId) const;
         *
         * \brief Query settings, and return is overlay for serviceId is enabled.
         *
         * \param serviceId Identifier for the service.
         *
         * \return  true if overlay enabled, false if not.
         */

        bool isOverlayEnabled(const QString& serviceId) const;

        /**
         * \fn  void ServiceSettingsBridge::setOverlayEnabled(const QString& serviceId, bool enabled);
         *
         * \brief Sets an overlay enabled.
         *
         * \param serviceId Identifier for the service.
         * \param enabled   true to enable, false to disable.
         */

        Q_NOREPLY void setOverlayEnabled(const QString& serviceId, bool enabled);

        /*!
         \fn  bool ServiceSettingsBridge::isPrefer32Bit(const QString& serviceId) const;
         
         \brief Query settings, and return is prefer 32bit client for serviceId is enabled.
         
         \author  "Ilya Tkachenko"
         \date  13.11.2015
         
         \param serviceId Identifier for the service.
         
         \return  true if prefer 32 bit, false if not.
         */

        bool isPrefer32Bit(const QString& serviceId) const;

        /*!
        \fn void ServiceSettingsBridge::setPrefer32Bit(const QString& serviceId, bool value);
        
        \brief  Sets a prefer 32 bit game with sericeId.
        
        \author "Ilya Tkachenko"
        \date 13.11.2015
        
        \param  serviceId Identifier for the service.
        \param  value     true to value.
         */
        Q_NOREPLY void setPrefer32Bit(const QString& serviceId, bool value);

      private:
        ServiceSettings *_settings;
      };

    }
  }
}