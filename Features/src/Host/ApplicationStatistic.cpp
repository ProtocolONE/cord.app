#include <Host/ApplicationStatistic.h>

#include <GameDownloader/GameDownloadService.h>

#include <Settings/Settings.h>

#include <Application/ArgumentParser.h>

#include <QtCore/QDateTime>

using P1::GameDownloader::GameDownloadService;
using P1::Application::ArgumentParser;

namespace P1 {
  namespace Host {

    ApplicationStatistic::ApplicationStatistic(QObject *parent)
      : QObject(parent)
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

    void ApplicationStatistic::setStartingGame(const QString& serviceId)
    {
      this->_startingServiceId = serviceId;
    }

    void ApplicationStatistic::init()
    {
      quint64 installDate = this->installDate();
      if (installDate == 0) {
        this->_firstStart = true;
        this->setInstallDate();
      }

      P1::Settings::Settings settings;
      settings.beginGroup("launcher");

      if (this->_firstStart)
        settings.setValue("firstStart", 1);
      else
        this->_firstStart = settings.value("firstStart", 0).toInt() == 1;
    }

    void ApplicationStatistic::applcationStarted()
    {
      // INFO reset flag about first app launch
      P1::Settings::Settings settings;
      settings.beginGroup("launcher");
      settings.setValue("firstStart", 0);
    }

    bool ApplicationStatistic::isGameInstalled(const QString& serviceId) const
    {
      Q_ASSERT(this->_downloader);
      return this->_downloader->isInstalled(serviceId);
    }

    quint64 ApplicationStatistic::gameInstallDate(const QString& serviceId) const
    {
      P1::Settings::Settings settings;
      settings.beginGroup("GameDownloader");
      settings.beginGroup(serviceId);
      QDateTime date = settings.value("installDate", 0).toDateTime();
      return date.toMSecsSinceEpoch();
    }

    quint64 ApplicationStatistic::gameLastExecutionTime(const QString& serviceId) const
    {
      P1::Settings::Settings settings;
      settings.beginGroup("gameExecutor");
      settings.beginGroup("serviceInfo");
      settings.beginGroup(serviceId);
      QDateTime date = settings.value("lastExecutionTime", 0).toDateTime();
      return date.toMSecsSinceEpoch();
    }

    int ApplicationStatistic::executeGameSuccessCount(const QString& serviceId) const
    {
      P1::Settings::Settings settings;
      settings.beginGroup("gameExecutor");
      settings.beginGroup("serviceInfo");
      settings.beginGroup(serviceId);
      return settings.value("successCount", 0).toInt();
    }

    int ApplicationStatistic::executeGameFailedCount(const QString& serviceId) const
    {
      P1::Settings::Settings settings;
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
      P1::Settings::Settings settings;
      settings.beginGroup("launcher");
      return settings.value("installWithService", "0").toString();
    }

    quint64 ApplicationStatistic::installDate() const
    {
      P1::Settings::Settings settings;
      settings.beginGroup("launcher");
      return settings.value("installDate", 0).toULongLong();
    }

    void ApplicationStatistic::setInstallDate()
    {
      quint64 installDate = QDateTime::currentDateTime().toMSecsSinceEpoch() / 1000;
      P1::Settings::Settings settings;
      settings.beginGroup("launcher");
      settings.setValue("installDate", installDate);

      if (this->hasStartingService())
        settings.setValue("installWithService", this->_startingServiceId);
    }

    bool ApplicationStatistic::hasStartingService()
    {
      if (this->_startingServiceId.isEmpty() || this->_startingServiceId == "0")
        return false;

      return true;
    }

    bool ApplicationStatistic::isFirstStart() const
    {
      return this->_firstStart;
    }

  }
}
