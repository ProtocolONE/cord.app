#include <Host/CommandLineManager.h>
#include <Application/ArgumentParser.h>

#include <QtCore/QCoreApplication>

using GGS::Core::Service;

namespace GameNet {
  namespace Host {

    CommandLineManager::CommandLineManager(QObject *parent /*= 0*/)
      : QObject(parent)
      , _commandLineArguments(new GGS::Application::ArgumentParser(this))
    {
      this->_commandLineArguments->parse(QCoreApplication::arguments());
    }

    CommandLineManager::~CommandLineManager()
    {
    }

    void CommandLineManager::commandRecieved(const QString& name, const QStringList& arguments)
    {
      if (name == "quit") {
        emit this->shutdown();
        return;
      }

      bool ignoreCommand = name == "empty"
        || name == "skip-ui"
        || name == "gamepts"
        || name == "gametest";

      if (ignoreCommand)
        return;

      emit this->uiCommand(name, arguments);
    }

    Service::Area CommandLineManager::gameArea()
    {
      Service::Area result = Service::Live;
      if (this->_commandLineArguments->contains("gamepts"))
        result = Service::Pts;

      if (this->_commandLineArguments->contains("gametest"))
        result = Service::Tst;

      return result;
    }

    QString CommandLineManager::startingService()
    {
      if (!this->_commandLineArguments->contains("startservice"))
        return "0";

      QStringList arguments = this->_commandLineArguments->commandArguments("startservice");
      if (arguments.count() > 0)
        return arguments.at(0);

      return "0";
    }

    bool CommandLineManager::skipUi()
    {
      return this->_commandLineArguments->contains("skip-ui");
    }


  }
}