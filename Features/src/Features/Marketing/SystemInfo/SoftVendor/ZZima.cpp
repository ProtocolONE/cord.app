#include <Features/Marketing/SystemInfo/SoftVendor/ZZima.h>
#include <Features/Marketing/SystemInfo/SoftVendor/RegistryHelper.h>

#include <QtCore/QSettings>
#include <QtCore/QFile>
#include <QtCore/QRegularExpression>

#include <QtCore/QUrl>

#include <Shlobj.h>

namespace Features {
  namespace Marketing {
    namespace SystemInfo {
      namespace SoftVendor {

        ZZima::ZZima(QObject *parent /*= 0*/)
          : GameVendorBase("ZZima", parent)
        {
        }

        ZZima::~ZZima()
        {
        }

        void ZZima::writeGames(QXmlStreamWriter *writer)
        {
          QStringList gameList = this->getZZimaGames();
          if (gameList.isEmpty())
            return;

          QString name;
          Q_FOREACH(const QString& gameId, gameList)
            this->writeGame(gameId, name, writer);
        }

        QStringList ZZima::getZZimaGames()
        {
          wchar_t appDataPath[MAX_PATH] = {0};
          if (SHGetFolderPath(NULL, CSIDL_APPDATA, NULL, SHGFP_TYPE_CURRENT, appDataPath) != S_OK)
            return QStringList();

          QString configPath = QString("%1\\nloader\\nloader.ini").arg(QString::fromWCharArray(appDataPath));
          if (!QFile::exists(configPath))
            return QStringList();

          QFile configFile(configPath);
          if (!configFile.open(QIODevice::ReadOnly))
            return QStringList();

          QString config(configFile.readAll());

          QRegularExpression re("\\[(.+?)\\]");
          QRegularExpressionMatchIterator i = re.globalMatch(config);

          QStringList result;
          while (i.hasNext()) {
            QRegularExpressionMatch match = i.next();
            QString gameName = match.captured(1);
            result << this->clearGameName(gameName);
          }

          return result;
        }

        QString ZZima::clearGameName(const QString& torrentUrl)
        {
          QString result = torrentUrl;

          int index = result.lastIndexOf('/');
          if (index > 0 && index < result.length())
            result = result.right(result.length() - index - 1);

          QLatin1String extension(".torrent");
          if (result.endsWith(extension))
            result = result.left(result.length() - extension.size());

          return result.isEmpty() ? torrentUrl : result;
        }

      }
    }
  }
}