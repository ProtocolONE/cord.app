#pragma once

#include <QtCore/QObject>
#include <QtCore/QPair>

namespace GameNet {
  namespace Host {

    // INFO: ID, priority<Before, After>
    typedef QPair<QString, QPair<int, int> > DownloadHookDescription;

    // INFO: ID, priority
    typedef QPair<QString, int> ExecutorHookDescription;

    class ServiceDescription : public QObject
    {
      Q_OBJECT
    public:
      explicit ServiceDescription(QObject *parent = 0);
      virtual ~ServiceDescription();

      QString id() const;
      void setId(const QString& val);
      
      QString gameId() const;
      void setGameId(const QString& val);

      QString name() const;
      void setName(const QString& val);

      QString torrentUrl() const;
      void setTorrentUrl(const QString& val);

      bool hasDownloadPath() const;
      void setHasDownloadPath(bool val);

      bool isDownloadable() const;
      void setIsDownloadable(bool val);

      QString extractorType() const;
      void setExtractorType(const QString& val);

      QString executeUrl() const;
      void setExecuteUrl(const QString& val);

      QString dependencyList() const;
      void setDependencyList(const QString& val);

      int gameSize() const;
      void setGameSize(int val);

      const QList<DownloadHookDescription>& downloadHooks() const;
      void setDownloadHooks(const QList<DownloadHookDescription>& val);

      const QList<ExecutorHookDescription>& executorHooks() const;
      void setExecutorHooks(const QList<ExecutorHookDescription>& val);

    private:
      QString _id;
      QString _gameId;
      QString _name;
      QString _torrentUrl;
      bool _isDownloadable;
      bool _hasDownloadPath;
      QString _extractorType;
      QString _executeUrl;
      QString _dependencyList;
      int _gameSize;
      QList<DownloadHookDescription> _downloadHooks;
      QList<ExecutorHookDescription> _executorHooks;
    };

  }
}