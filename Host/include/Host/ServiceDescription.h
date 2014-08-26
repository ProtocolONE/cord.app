#pragma once

#include <QtCore/QObject>
#include <QtCore/QPair>

namespace GameNet {
  namespace Host {

    typedef QPair<QString, QPair<int, int> > DownloadHookDescription;
    

    class ServiceDescription : public QObject
    {
      Q_OBJECT
    public:
      explicit ServiceDescription(QObject *parent = 0);
      virtual ~ServiceDescription();

      QString id() const;
      void setId(QString val);
      
      QString gameId() const;
      void setGameId(QString val);

      QString name() const;
      void setName(QString val);

      QString torrentUrl() const;
      void setTorrentUrl(QString val);

      bool hasDownloadPath() const;
      void setHasDownloadPath(bool val);

      bool isDownloadable() const;
      void setIsDownloadable(bool val);

      QString extractorType() const;
      void setExtractorType(QString val);

      QString executeUrl() const;
      void setExecuteUrl(QString val);

      QString dependencyList() const;
      void setDependencyList(QString val);

      int gameSize() const;
      void setGameSize(int val);

      QList<DownloadHookDescription> downloadHooks() const;
      void setDownloadHooks(QList<DownloadHookDescription> val);

    private:
      QString _id;
      QString _gameId;
      QString _name;
      QString _torrentUrl;
      bool _isDownloadable;
      bool _hasDownloadPath;
      QString _extractorType;
      // exe://%1/%2/aikaru.exe?workingDir=%3/%4/&args=%login% %token% 300002010000000000 login&downloadCustomFile=UI/GuildMarkWorld1.tga,http://files.gamenet.ru/update/aika,2
      QString _executeUrl;
      QString _dependencyList;
      int _gameSize;
      QList<DownloadHookDescription> _downloadHooks;
    };

  }
}