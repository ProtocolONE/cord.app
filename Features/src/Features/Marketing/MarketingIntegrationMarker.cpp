#include <Features/Marketing/MarketingIntegrationMarker.h>

#include <QtCore/QSettings>
#include <QtCore/QCoreApplication>

namespace Features {
  namespace Marketing {

    MarketingIntegrationMarker::MarketingIntegrationMarker()
    {
    }

    MarketingIntegrationMarker::~MarketingIntegrationMarker()
    {
    }

    void MarketingIntegrationMarker::init()
    {
      QSettings settings(
        QSettings::NativeFormat,
        QSettings::UserScope,
        QCoreApplication::organizationName(),
        QCoreApplication::applicationName());

      settings.beginGroup("Integration");
      settings.setValue("IMV", "3.8");
    }

    bool MarketingIntegrationMarker::isActiveUser()
    {
      bool result = true;
      QSettings settings(
        QSettings::NativeFormat,
        QSettings::UserScope,
        QCoreApplication::organizationName(),
        QCoreApplication::applicationName());

      settings.beginGroup("Integration");
      result &= settings.value("IMV", QString()).toString() == "3.8";

      return result;
    }

  }
}