#pragma once

#include <QtCore/QObject>
#include <QtCore/QString>
#include <QtCore/QMetaType>

namespace GameNet {
  namespace Host {

    class Application;

    namespace Bridge {

      class ApplicationBridge: public QObject {
        Q_OBJECT
        Q_CLASSINFO("Version", "1.0.0.0")
        Q_CLASSINFO("D-Bus Interface", "com.gamenet.dbus.Application")
      public:
        explicit ApplicationBridge(QObject *parent = 0);
        virtual ~ApplicationBridge();

        void setApplcation(Application* app);

      public slots:
        bool isInitCompleted();

        /*
          Ответ от UI к Host с разрешением перезапуска с нужными параметрами
        */
        void restartApplication(bool isMinimized);

        void switchClientVersion();

      signals:
        void initCompleted();
        /*
          Сигнал от Host к UI с просьбой перезапуска приложения
        */
        void restartUIRequest();      

      private:
        Application* _application;
      };

    }

  }
}