#pragma once

#include <QtCore/QObject>
#include <QtCore/QString>
#include <QtCore/QStringList>

#include <Core/Service.h>

namespace GGS {
  namespace Application {
    class ArgumentParser;
  }
}

namespace GameNet {
  namespace Host {

    // UNDONE Нужна помощь с названием этого класса.
    class CommandLineManager : public QObject
    {
      Q_OBJECT
    public:
      explicit CommandLineManager(QObject *parent = 0);
      virtual ~CommandLineManager();

      void commandRecieved(const QString& name, const QStringList& arguments);

      GGS::Core::Service::Area gameArea();

      QString startingService();

      bool skipUi();

    signals:

      /**
       * \fn  void CommandLineManager::shutdown();
       *
       * \brief Запрос на закрытие приложения.
       *
       * \author  Ilya Tkachenko
       * \date  17.10.2014
       */

      void shutdown();

      /**
       * \fn  void CommandLineManager::uiCommand(const QString& name, const QStringList& arguments);
       *
       * \brief Комманда для QGNA-UI.
       *
       * \author  Ilya Tkachenko
       * \date  17.10.2014
       *
       * \param name      The name.
       * \param arguments The arguments.
       */

      void uiCommand(const QString& name, const QStringList& arguments);

      /**
       * \fn  void CommandLineManager::openBrowser(const QString& url);
       *
       * \brief Запрос на открытие сайта в внешнем браузере.
       *
       * \author  Ilya Tkachenko
       * \date  17.10.2014
       *
       * \param url URL of the document.
       */
      void openBrowser(const QString& url);

      /**
       * \fn  void CommandLineManager::uninstallService(const QString& name, const QStringList& arguments);
       *
       * \brief Запрос на деинсталляцию сервиса
       *
       * \param url URL of the document.
       */
      void uninstallService(const QString& serviceId);

    private:
      GGS::Application::ArgumentParser* _commandLineArguments;

    };

  }
}
