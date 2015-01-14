#include <Features/Marketing/SystemInfo/SoftVendor/FourGame.h>
#include <Features/Marketing/SystemInfo/SoftVendor/RegistryHelper.h>

#include <QtCore/QSettings>
#include <QtCore/QFile>
#include <QtCore/QRegularExpression>

namespace Features {
  namespace Marketing {
    namespace SystemInfo {
      namespace SoftVendor {

        FourGame::FourGame(QObject *parent /*= 0*/)
          : GameVendorBase("4game", parent)
        {
        }

        FourGame::~FourGame()
        {
        }

        void FourGame::writeGames(QXmlStreamWriter *writer)
        {
          if (!registryKeyExist(HKEY_LOCAL_MACHINE, L"SOFTWARE\\4game\\4gameservice\\Games"))
            return;

          QString gameId;

          QSettings registry("HKEY_LOCAL_MACHINE\\SOFTWARE\\4game\\4gameservice\\Games", QSettings::NativeFormat);
          QStringList allGames = registry.childGroups();

          Q_FOREACH(const QString& gameName, allGames) {
            this->writeGame(gameId, gameName, writer);
          }
        }

      }
    }
  }
}