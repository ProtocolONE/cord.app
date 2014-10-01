#include <Host/ApplicationStatistic.h>

#include <GameDownloader/GameDownloadService.h>

#include <Settings/Settings.h>

#include <Application/ArgumentParser.h>

#include <QtCore/QDateTime>

using GGS::Settings::Settings;
using GGS::GameDownloader::GameDownloadService;
using GGS::Application::ArgumentParser;

namespace GameNet {
  namespace Host {

    ApplicationStatistic::ApplicationStatistic(QObject *parent)
      : QObject(parent)
      , _downloader(nullptr)
    {
    }

    ApplicationStatistic::~ApplicationStatistic()
    {
    }

    void ApplicationStatistic::setDownloader(GameDownloadService *value)
    {
      Q_ASSERT(value);
      this->_downloader = value;
    }

    void ApplicationStatistic::setCommandLineArgs(ArgumentParser *value)
    {
      Q_ASSERT(value);
      this->_commandLineArgs = value;
    }

    void ApplicationStatistic::init()
    {
      Q_ASSERT(this->_downloader);
      Q_ASSERT(this->_commandLineArgs);

      quint64 installDate = this->installDate();
      if (installDate != 0)
        return;

      this->setInstallDate(this->startingServiceId());
    }

    bool ApplicationStatistic::isGameInstalled(const QString& serviceId) const
    {
      Q_ASSERT(this->_downloader);
      return this->_downloader->isInstalled(serviceId);
    }

    quint64 ApplicationStatistic::gameInstallDate(const QString& serviceId) const
    {
      Settings settings;
      settings.beginGroup("GameDownloader");
      settings.beginGroup(serviceId);
      QDateTime date = settings.value("installDate", 0).toDateTime();
      return date.toMSecsSinceEpoch();
    }

    quint64 ApplicationStatistic::gameLastExecutionTime(const QString& serviceId) const
    {
      Settings settings;
      settings.beginGroup("gameExecutor");
      settings.beginGroup("serviceInfo");
      settings.beginGroup(serviceId);
      QDateTime date = settings.value("lastExecutionTime", 0).toDateTime();
      return date.toMSecsSinceEpoch();
    }

    int ApplicationStatistic::executeGameSuccessCount(const QString& serviceId) const
    {
      Settings settings;
      settings.beginGroup("gameExecutor");
      settings.beginGroup("serviceInfo");
      settings.beginGroup(serviceId);
      return settings.value("successCount", 0).toInt();
    }

    int ApplicationStatistic::executeGameFailedCount(const QString& serviceId) const
    {
      Settings settings;
      settings.beginGroup("gameExecutor");
      settings.beginGroup("serviceInfo");
      settings.beginGroup(serviceId);
      return settings.value("failedCount", 0).toInt();
    }

    int ApplicationStatistic::executeGameTotalCount(const QString& serviceId) const
    {
      return this->executeGameFailedCount(serviceId) + this->executeGameSuccessCount(serviceId);
    }

    QString ApplicationStatistic::installWithService() const
    {
      Settings settings;
      settings.beginGroup("qGNA");
      return settings.value("installWithService", "0").toString();
    }

    quint64 ApplicationStatistic::installDate() const
    {
      Settings settings;
      settings.beginGroup("qGNA");
      return settings.value("installDate", 0).toULongLong();
    }

    void ApplicationStatistic::setInstallDate(const QString& serviceId)
    {
      quint64 installDate = QDateTime::currentDateTime().toMSecsSinceEpoch() / 1000;
      Settings settings;
      settings.beginGroup("qGNA");
      settings.setValue("installDate", installDate);

      if (!serviceId.isEmpty() && serviceId != "0")
        settings.setValue("installWithService", serviceId);
    }

    QString ApplicationStatistic::startingServiceId()
    {
      Q_ASSERT(this->_commandLineArgs);

      if (!this->_commandLineArgs->contains("startservice"))
        return "0";

      QStringList arguments = this->_commandLineArgs->commandArguments("startservice");
      if (arguments.count() > 0)
        return arguments.at(0);

      return "0";
    }

  }
}
