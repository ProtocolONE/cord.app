#include <Features/Marketing/SystemInfo/SoftVendor/Origin.h>
#include <Features/Marketing/SystemInfo/SoftVendor/RegistryHelper.h>

#include <QtCore/QSettings>
#include <QtCore/QFile>
#include <QtCore/QRegularExpression>

namespace Features {
  namespace Marketing {
    namespace SystemInfo {
      namespace SoftVendor {

        Origin::Origin(QObject *parent /*= 0*/)
          : GameVendorBase("Origin", parent)
        {
        }

        Origin::~Origin()
        {
        }

        void Origin::writeGames(QXmlStreamWriter *writer)
        {
          if (!registryKeyExist(HKEY_LOCAL_MACHINE, L"SOFTWARE\\Origin Games"))
            return;

          QSettings registry("HKEY_LOCAL_MACHINE\\SOFTWARE\\Origin Games", QSettings::NativeFormat);
          QStringList allGames = registry.childGroups();
          Q_FOREACH(const QString& appId, allGames) {
            registry.beginGroup(appId);

            QString name = registry.value("DisplayName", QString()).toString();
            if (name.isEmpty())
              continue;

            this->writeGame(appId, name, writer);
            registry.endGroup();
          }
        }


      }
    }
  }
}