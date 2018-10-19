#include <Features/Marketing/MarketingIntegrationMarker.h>

#include <QtCore/QSettings>

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
      QSettings settings("HKEY_LOCAL_MACHINE\\Software\\GGS\\QGNA\\Integration", QSettings::NativeFormat);

      // INFO QGNA-1562 ��� InstallMonster �������� ������� �������� ������������, ���
      // ��������� �� ��� �������� ��������� ����������.
      settings.setValue("IMV", "3.8");
    }

  }
}