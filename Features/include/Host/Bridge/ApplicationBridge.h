#pragma once

#include <QtCore/QObject>
#include <QtCore/QString>
#include <QtCore/QMetaType>

#include <QtDBus/QDBusContext>
#include <QtDBus/QDBusArgument>

#include <Host/Bridge/Credential.h>

namespace GameNet {
  namespace Host {
    class Application;
    class Thetta;

    namespace Bridge {

      class ApplicationBridge: public QObject, protected QDBusContext
      {
        Q_OBJECT
        Q_CLASSINFO("Version", "1.0.0.0")
        Q_CLASSINFO("D-Bus Interface", "com.gamenet.dbus.Application")
      public:
        explicit ApplicationBridge(QObject *parent = 0);
        virtual ~ApplicationBridge();

        /**
         * \fn  void ApplicationBridge::setApplcation(Application* app);
         *
         * \brief Sets an applcation.
         *
         * \author Igor Bugaev
         * \date  26.09.2014
         *
         * \param [in,out]  app If non-null, the application.
         */

        void setApplcation(Application* app);

        /**
         * \fn  void ApplicationBridge::setThetta(Thetta *value);
         *
         * \brief Sets a thetta.
         *
         * \author Igor Bugaev
         * \date  26.09.2014
         *
         * \param [in,out]  value If non-null, the value.
         */

        void setThetta(Thetta *value);

      public slots:
        /**
         * \fn  bool ApplicationBridge::isInitCompleted();
         *
         * \brief Query if host init and update completed.
         *
         * \author Igor Bugaev
         * \date  26.09.2014
         *
         * \return  true if initialise completed, false if not.
         */

        bool isInitCompleted();

        /**
         * \fn  void ApplicationBridge::restartApplication(bool isMinimized);
         *
         * \brief Restart application.
         *
         * \author Igor Bugaev  
         * \date  26.09.2014
         *
         * \param isMinimized true if is minimized.
         */

        void restartApplication(bool isMinimized);

        /**
         * \fn  void ApplicationBridge::switchClientVersion();
         *
         * \brief Switch client version.
         *
         * \author  Igor Bugaev
         * \date  26.09.2014
         */

        void switchClientVersion();

        /**
         * \fn  void ApplicationBridge::openBrowser(const QString& url);
         *
         * \brief Открывает ссылку в дефолтном браузере. Необходимо вызывать из приложений 
         *        находящихся под защитой Thetta.
         *
         * \author Ilya Tkachenko
         * \date  19.09.2014
         *
         * \param url URL of the document.
         */
        void openBrowser(const QString& url);


        /*
        <method name="setCredential">
          <arg name="applicationName" type="s" direction="in"/>
          <arg name="credential" type="a(sii)" direction="in"/>
          <annotation name="org.qtproject.QtDBus.QtTypeName.In1" value="GameNet::Host::Bridge::Credential"/>
        </method>
        */
        
        /**
         * \fn  void ApplicationBridge::setCredential( const QString& applicationName,
         *      const Credential& credential);
         *
         * \brief Sets a credential.
         *
         * \author  Ilya Tkachenko
         * \date  06.10.2014
         *
         * \param applicationName Name of the application.
         * \param credential      The credential.
         */
        void setCredential(
          const QString& applicationName,
          const Credential& credential);

    signals:

        /**
         * \fn  void ApplicationBridge::initCompleted();
         *
         * \brief emited this init and update is completed.
         *
         * \author Igor Bugaev
         * \date  26.09.2014
         */

        void initCompleted();

        /**
         * \fn  void ApplicationBridge::restartUIRequest();
         *
         * \brief Restart user interface request.
         *
         * \author Igor Bugaev
         * \date  26.09.2014
         */

        void restartUIRequest();      

      private:
        Application* _application;
        Thetta *_thetta;
      };

    }

  }
}