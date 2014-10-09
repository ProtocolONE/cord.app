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
      void shutdown();
      void uiCommand(const QString& name, const QStringList& arguments);

    private:
      GGS::Application::ArgumentParser* _commandLineArguments;

    };

  }
}
