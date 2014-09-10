#pragma once

#include <QtCore/QObject>
#include <QtCore/QString>
#include <QtCore/QMetaType>

namespace GameNet {
  namespace Host {
    class Application;
    class Thetta;

    namespace Bridge {

      class ApplicationBridge: public QObject {
        Q_OBJECT
        Q_CLASSINFO("Version", "1.0.0.0")
        Q_CLASSINFO("D-Bus Interface", "com.gamenet.dbus.Application")
      public:
        explicit ApplicationBridge(QObject *parent = 0);
        virtual ~ApplicationBridge();

        void setApplcation(Application* app);
        void setThetta(Thetta *value);

      public slots:
        bool isInitCompleted();

        /*
          Ответ от UI к Host с разрешением перезапуска с нужными параметрами
        */
        void restartApplication(bool isMinimized);

        void switchClientVersion();

        /**
         * \fn  void ApplicationBridge::openBrowser(const QString& url);
         *
         * \brief Открывает ссылку в дефолтном браузере. Необходимо вызывать из приложений 
         *        находящихся под защитой Thetta.
         *
         * \author  Ilya Tkachenko
         * \date  19.09.2014
         *
         * \param url URL of the document.
         */
        void openBrowser(const QString& url);

      signals:
        void initCompleted();
        /*
          Сигнал от Host к UI с просьбой перезапуска приложения
        */
        void restartUIRequest();      

      private:
        Application* _application;
        Thetta *_thetta;
      };

    }

  }
}