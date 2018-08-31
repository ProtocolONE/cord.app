#include <Host/CommandLineManager.h>
#include <Application/ArgumentParser.h>

#include <RestApi/ProtocolOneCredential.h>

#include <QtCore/QCoreApplication>

#include <Helper/ApplicationArea.hpp>

using P1::Core::Service;
using P1::RestApi::ProtocolOneCredential;

namespace P1 {
  namespace Host {

    CommandLineManager::CommandLineManager(QObject *parent /*= 0*/)
      : QObject(parent)
      , _commandLineArguments(new P1::Application::ArgumentParser(this))
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

      if (name == "goprotocolonemoney") {
        this->goprotocolonemoney(name, arguments);
        return;
      }

      if (name == "goprotocolonehelper") {
        this->goprotocolonehelper(name, arguments);
        return;
      }

      if (name == "update") {
        if (arguments.count() > 0) {
          P1::ApplicationArea requestedArea;
          requestedArea.parse(arguments.at(0));
          requestedArea.save();
        }
        
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

    bool CommandLineManager::forceElectronExecutor() const
    {
      return this->_commandLineArguments->contains("electron");
    }

    bool CommandLineManager::forceWebBrowserExecutor() const
    {
      return this->_commandLineArguments->contains("web-browser");
    }

    void CommandLineManager::goprotocolonemoney(const QString& name, const QStringList& arguments)
    {
      ProtocolOneCredential credential;
      bool seendToUi = this->shouldSendToUi(credential);

      if (seendToUi) {
        emit this->uiCommand(name, arguments);
        return;
      }

      this->openUrlWithAuth("http://www.protocol.one/money", credential);
    }

    void CommandLineManager::goprotocolonehelper(const QString& name, const QStringList& arguments)
    {
      if (arguments.count() < 1)
        return;

      ProtocolOneCredential credential;
      if (this->shouldSendToUi(credential)) {
        emit this->uiCommand(name, arguments);
        return;
      }

      QString gameId = arguments.at(0);
      QString url = QString("http://www.protocol.one/games/%1/guides").arg(gameId);
      this->openUrlWithAuth(url, credential);
    }

    void CommandLineManager::gocombatarmsrating(const QString& name, const QStringList& arguments)
    {
      ProtocolOneCredential credential;
      if (this->shouldSendToUi(credential)) {
        emit this->uiCommand(name, arguments);
        return;
      }

      this->openUrlWithAuth("http://www.combatarms.ru/ratings/user/", credential);
    }

    bool CommandLineManager::shouldSendToUi(ProtocolOneCredential& credential)
    {
      QString applicationName;
      bool gameExecuted = this->_executedGameCredential(credential, applicationName);
      return !gameExecuted || (gameExecuted && applicationName == "Launcher");
    }

    void CommandLineManager::openUrlWithAuth(const QString& url, ProtocolOneCredential& credential)
    {
      QString authUrl;
      if (credential.isEmpty()) {
        authUrl = url;
      } else {
        authUrl = "https://gnlogin.ru/?auth=";
        authUrl.append(credential.cookie());
        authUrl.append("&rp=");
        authUrl.append(QUrl::toPercentEncoding(url));
      }

      authUrl.append('\0');

      emit this->openBrowser(authUrl);
    }

  }
}