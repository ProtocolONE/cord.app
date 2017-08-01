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
        || name == "gametest"
        || name == "minimized";

      if (ignoreCommand)
        return;

      if (name == "gogamenetmoney") {
        this->gogamenetmoney(name, arguments);
        return;
      }

      if (name == "gogamenethelper") {
        this->gogamenethelper(name, arguments);
        return;
      }

      if (name == "gocombatarmsrating") {
        this->gocombatarmsrating(name, arguments);
        return;
      }

      if (name == "update") {
        this->updateRequested();
        return;
      }

      if (name == "uninstall" && !arguments.empty()) {
        QString serviceId = arguments.at(0);
        if (!serviceId.isEmpty()) {
          emit this->uninstallService(serviceId);
          return;
        }
      }

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

    bool CommandLineManager::overrideWebExecutor() const
    {
      return this->_commandLineArguments->contains("electron");
    }

    void CommandLineManager::gogamenetmoney(const QString& name, const QStringList& arguments)
    {
      GameNetCredential credential;
      bool seendToUi = this->shouldSendToUi(credential);

      if (arguments.size() > 0 && arguments.at(0) == "darkage") {
        this->openUrlWithAuth("https://gamenet.ru/games/da/exchange", credential);
        return;
      }

      if (seendToUi) {
        emit this->uiCommand(name, arguments);
        return;
      }

  }
}