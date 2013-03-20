#include <Features/RememberGameDownloading.h>

#include <Settings/Settings>
#include <GameDownloader/GameDownloadService>

#include <QDebug>

namespace Features {
  RememberGameDownloading::RememberGameDownloading(QObject *parent)
    : QObject(parent)
  {
  }

  RememberGameDownloading::~RememberGameDownloading()
  {
  }

  void RememberGameDownloading::started(const GGS::Core::Service *service)
  {
    if (!service->isDownloadable())
      return;
    
    GGS::Settings::Settings settings;
    settings.beginGroup("RememberGameDownloading");
    settings.setValue("service", service->id());
  }

  void RememberGameDownloading::finished(const GGS::Core::Service *service)
  {
    GGS::Settings::Settings settings;
    settings.beginGroup("RememberGameDownloading");
    settings.setValue("service", "");
  }

  void RememberGameDownloading::update()
  {   
    GGS::Settings::Settings settings;
    settings.beginGroup("RememberGameDownloading");
    QString serviceId = settings.value("service", QString()).toString();
    if (serviceId.isEmpty())
      return;

    emit this->startGameRequest(serviceId);
  }
}