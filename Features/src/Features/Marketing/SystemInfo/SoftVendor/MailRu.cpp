#include <Features/Marketing/SystemInfo/SoftVendor/MailRu.h>
#include <Features/Marketing/SystemInfo/SoftVendor/RegistryHelper.h>

#include <QtCore/QSettings>
#include <QtCore/QFile>
#include <QtCore/QDir>
#include <QtCore/QRegularExpression>

namespace Features {
  namespace Marketing {
    namespace SystemInfo {
      namespace SoftVendor {

        MailRu::MailRu(QObject *parent /*= 0*/)
          : GameVendorBase("MailRu", parent)
        {
        }

        MailRu::~MailRu()
        {
        }

        void MailRu::writeGames(QXmlStreamWriter *writer)
        {
          if (!registryKeyExist(HKEY_CURRENT_USER, L"Software\\Mail.Ru\\GameCenterMailRu"))
            return;

          QSettings registry("HKEY_CURRENT_USER\\Software\\Mail.Ru\\GameCenterMailRu", QSettings::NativeFormat);
          QString centerPath = registry.value("1", QString()).toString();
          if (!QFile::exists(centerPath))
            return;

          QFileInfo centerExeInfo(centerPath);
          QString centerConfigPath = QString("%1/GameCenter@Mail.Ru.ini").arg(centerExeInfo.absolutePath());

          QFile configFile(centerConfigPath);
          if (!configFile.open(QIODevice::ReadOnly))
            return;

          QString config(configFile.readAll());

          QString installPath;
          QRegExp rxGameInstallPath("GamesInstallPath=(.+)[\r\n]");
          rxGameInstallPath.setMinimal(true);
          if (rxGameInstallPath.indexIn(config) != -1)
            installPath = rxGameInstallPath.cap(1);

          QDir installDir(installPath);
          if (!installDir.exists())
            QStringList();

          QStringList gameDirs = installDir.entryList(QDir::NoDotAndDotDot | QDir::AllDirs);
          Q_FOREACH(const QString& dir, gameDirs) {
            QString lastFilePath = QString("%1/%2/-gup-/last.xml").arg(installPath, dir);

            QFile lastFile(lastFilePath);
            if (!lastFile.open(QIODevice::ReadOnly))
              continue;

            QString last(lastFile.readAll());

            QString gameId;
            QRegExp rxGameId("GAMEID=\\\"([\\d\\.]+)\\\"");
            rxGameId.setMinimal(true);
            if (rxGameId.indexIn(last) != -1)
              gameId = rxGameId.cap(1);

            if (gameId.isEmpty())
              continue;

            this->writeGame(gameId, dir, writer);
          }
        }

      }
    }
  }
}