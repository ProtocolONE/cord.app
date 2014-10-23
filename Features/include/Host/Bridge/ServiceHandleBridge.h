#pragma once

#include <QtCore/QObject>
#include <QtCore/QString>

namespace GameNet {
  namespace Host {

    namespace Proxy {
      class ServiceHandleProxy;
    }

    namespace Bridge {

      class ServiceHandleBridge : public QObject
      {
        Q_OBJECT
        Q_CLASSINFO("Version", "1.0.0.0")
        Q_CLASSINFO("D-Bus Interface", "com.gamenet.dbus.ServiceHandle")
      public:
        explicit ServiceHandleBridge(QObject *parent = 0);
        virtual ~ServiceHandleBridge();

        void setServiceHandle(Proxy::ServiceHandleProxy *value);

      public slots:

        /**
         * \fn  virtual bool ServiceHandleBridge::lock(const QString& serviceId);
         *
         * \brief Take ownership for game with 'serviceId'. 
         *        This method must be call before download or execute request.
         *        It's prevent interaction with this game from other application.
         *
         * \author  Ilya Tkachenko
         * \date  27.10.2014
         *
         * \param serviceId Identifier for the game.
         *
         * \return  true if it succeeds, false if it fails.
         */

        virtual bool lock(const QString& serviceId);

        /**
         * \fn  virtual void ServiceHandleBridge::unlock(const QString& serviceId);
         *
         * \brief Unlocks game for interaction from other applications.
         *
         * \author  Ilya Tkachenko
         * \date  27.10.2014
         *
         * \param serviceId Identifier for the game.
         */

        virtual void unlock(const QString& serviceId);

      signals:

        /**
         * \fn  void ServiceHandleBridge::locked(const QString& serviceId);
         *
         * \brief Game locked from another application.
         *
         * \author  Ilya Tkachenko
         * \date  27.10.2014
         *
         * \param serviceId Identifier for the service.
         */

        void locked(const QString& serviceId);

        /**
         * \fn  void ServiceHandleBridge::unlocked(const QString& serviceId);
         *
         * \brief Game unlocked from another application.
         *
         * \author  Ilya Tkachenko
         * \date  27.10.2014
         *
         * \param serviceId Identifier for the service.
         */

        void unlocked(const QString& serviceId);

      private:
        Proxy::ServiceHandleProxy *_serviceHandle;
      };

    }
  }
}
