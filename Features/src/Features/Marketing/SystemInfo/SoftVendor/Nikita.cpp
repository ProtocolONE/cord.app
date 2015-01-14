#include <Features/Marketing/SystemInfo/SoftVendor/Nikita.h>
#include <Features/Marketing/SystemInfo/SoftVendor/RegistryHelper.h>

#include <QtCore/QSettings>
#include <QtCore/QFile>
#include <QtCore/QFileInfo>
#include <QtCore/QDir>
#include <QtCore/QRegularExpression>

namespace Features {
  namespace Marketing {
    namespace SystemInfo {
      namespace SoftVendor {

        Nikita::Nikita(QObject *parent /*= 0*/)
          : GameVendorBase("Nikita", parent)
        {
        }

        Nikita::~Nikita()
        {
        }

        void Nikita::writeGames(QXmlStreamWriter *writer)
        {
          if (!registryKeyExist(HKEY_CURRENT_USER, L"Software\\GameXP\\Access point\\Games"))
            return;

          QSettings registry("HKEY_CURRENT_USER\\Software\\GameXP\\Access point\\Games", QSettings::NativeFormat);
          QStringList allGames = registry.childKeys();
          Q_FOREACH(const QString& gameId, allGames) {
            QString gameName;
            QString gameExePath = registry.value(gameId, QString()).toString();
            if (!gameExePath.isEmpty()) {
              QFileInfo info(gameExePath);
              gameName = info.dir().dirName();
              if (gameName == ".")
                gameName = "";
            }

            this->writeGame(gameId, gameName, writer);
          }
        }

      }
    }
  }
}