#include <Features/Marketing/SystemInfo/SoftVendor/Ubisoft.h>
#include <Features/Marketing/SystemInfo/SoftVendor/RegistryHelper.h>

#include <QtCore/QSettings>
#include <QtCore/QFile>
#include <QtCore/QRegularExpression>

namespace Features {
  namespace Marketing {
    namespace SystemInfo {
      namespace SoftVendor {

        Ubisoft::Ubisoft(QObject *parent /*= 0*/)
          : GameVendorBase("Ubisoft", parent)
        {
        }

        Ubisoft::~Ubisoft()
        {
        }

        void Ubisoft::writeGames(QXmlStreamWriter *writer)
        {
          QStringList gameList = this->getUbisoftGameList();
          if (gameList.isEmpty())
            return;

          QString gameId;
          Q_FOREACH(const QString& name, gameList)
            this->writeGame(gameId, name, writer);
        }

        QStringList Ubisoft::getUbisoftGameList()
        {
          if (!registryKeyExist(HKEY_LOCAL_MACHINE, L"SOFTWARE\\Ubisoft\\Launcher"))
            return QStringList();

          QString installDir;
          QSettings registry("HKEY_LOCAL_MACHINE\\SOFTWARE\\Ubisoft\\Launcher", QSettings::NativeFormat);
          installDir = registry.value("InstallDir", QString()).toString();

          QFile configFile(QString("%1\\cache\\configuration\\configurations").arg(installDir));
          if (!configFile.open(QIODevice::ReadOnly))
            return QStringList();

          QString config(configFile.readAll());

          QRegularExpression re("root:[\\s\\r\\n]+?name: \\\"(.+?)\\\"");
          QRegularExpressionMatchIterator i = re.globalMatch(config);

          QStringList result;
          while (i.hasNext()) {
            QRegularExpressionMatch match = i.next();
            QString gameName = match.captured(1);
            result << gameName;
          }

          return result;
        }

      }
    }
  }
}