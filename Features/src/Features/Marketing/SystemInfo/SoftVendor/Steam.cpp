#include <Features/Marketing/SystemInfo/SoftVendor/Steam.h>
#include <Features/Marketing/SystemInfo/SoftVendor/RegistryHelper.h>

#include <QtCore/QSettings>
#include <QtCore/QDir>
#include <QtCore/QFile>
#include <QtCore/QFileInfo>
#include <QtCore/QRegularExpression>

namespace Features {
  namespace Marketing {
    namespace SystemInfo {
      namespace SoftVendor {

        Steam::Steam(QObject *parent /*= 0*/)
          : GameVendorBase("Steam", parent)
        {
        }

        Steam::~Steam()
        {
        }

        void Steam::writeToPayload(QXmlStreamWriter *writer)
        {
          QStringList steamIds = this->getSteamIdList();
          if (steamIds.count() == 0)
            return;

          writer->writeStartElement("users");

          Q_FOREACH(const QString& steamId, steamIds)
            writer->writeTextElement("id", steamId);

          writer->writeEndElement(); // users
        }

        void Steam::writeGames(QXmlStreamWriter *writer)
        {
          QStringList gameList = this->getSteamGameList();
          if (gameList.isEmpty())
            return;

          QString name;
          Q_FOREACH(const QString& gameId, gameList)
            this->writeGame(gameId, name, writer);
        }

        QString Steam::getSteamRootDirectory()
        {
          if (!registryKeyExist(HKEY_CURRENT_USER, L"Software\\Valve\\Steam"))
            return QString();

          QSettings registry("HKEY_CURRENT_USER\\Software\\Valve\\Steam", QSettings::NativeFormat);
          return registry.value("SteamPath", QString()).toString();
        }

        QStringList Steam::getSteamIdList()
        {
          QString steamRoot = this->getSteamRootDirectory();

          if (steamRoot.isEmpty())
            return QStringList();

          QString configPath = QString("%1/config/config.vdf").arg(steamRoot);
          if (!QFile::exists(configPath))
            return QStringList();

          QFile configFile(configPath);
          if (!configFile.open(QIODevice::ReadOnly | QIODevice::Text))
            return QStringList();

          QString config(configFile.readAll());

          QRegularExpression re("\\\"SteamID\\\"\\s+\\\"(\\d+)\\\"");
          QRegularExpressionMatchIterator i = re.globalMatch(config);

          QStringList result;
          while (i.hasNext()) {
            QRegularExpressionMatch match = i.next();
            QString steamId = match.captured(1);
            result << steamId;
          }

          return result;
        }

        QStringList Steam::getSteamGameList()
        {
          if (!registryKeyExist(HKEY_CURRENT_USER, L"Software\\Valve\\Steam\\Apps"))
            return QStringList();

          QSettings registry("HKEY_CURRENT_USER\\Software\\Valve\\Steam\\Apps", QSettings::NativeFormat);
          QStringList allGames = registry.childGroups();
          QStringList result;
          Q_FOREACH(const QString& appId, allGames) {
            registry.beginGroup(appId);

            if (registry.value("Installed", 0).toUInt() == 1)
              result << appId;

            registry.endGroup();
          }

          return result;
        }

      }
    }
  }
}