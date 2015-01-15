#pragma once

#include <QtCore/QObject>
#include <QtCore/QString>
#include <QtCore/QMetaType>

namespace GameNet {
  namespace Host {

    class Application;
    class Thetta;
    class Translation;
    class AutoRunManager;

    namespace Bridge {

      class ApplicationBridge : public QObject, protected QDBusContext
      {
        Q_OBJECT
        Q_CLASSINFO("Version", "1.0.0.0")
        Q_CLASSINFO("D-Bus Interface", "com.gamenet.dbus.Application")

        Q_PROPERTY(QString language READ language WRITE setLanguage NOTIFY languageChanged)
        Q_PROPERTY(int autoStartMode READ autoStartMode WRITE setAutoStartMode NOTIFY autoStartModeChanged)

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

        void setApplication(Application* app);

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

        /**
         * \fn  void ApplicationBridge::setTranslation(Translation *value);
         *
         * \brief Sets a translation.
         *
         * \author  Ilya Tkachenko
         * \date  16.10.2014
         *
         * \param [in,out]  value If non-null, the value.
         */

        void setTranslation(Translation *value);

        QString language() const;
        void setLanguage(const QString& value);

        void setAutoStartMode(int value);
        int autoStartMode() const;

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
         * \fn  void ApplicationBridge::shutdownUIResult();
         *
         * \brief Call it to agree shutdownUIRequest and continue closing application.
         *
         * \author  Ilya Tkachenko
         * \date  15.01.2015
         */

        void shutdownUIResult();

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

        /**
         * \fn  void ApplicationBridge::shutdownUIRequest();
         *
         * \brief Shutdown user interface request.
         *
         * \author  Ilya Tkachenko
         * \date  15.01.2015
         */
        void shutdownUIRequest();

        void languageChanged();
        void autoStartModeChanged();

      private:
        Application* _application;
        Thetta *_thetta;
        Translation *_translation;
        AutoRunManager *_autoRunManager;
      };

    }
  }
}
      class ApplicationBridge : public QObject
        void setAutoRunManager(AutoRunManager *value);

        QString language() const;