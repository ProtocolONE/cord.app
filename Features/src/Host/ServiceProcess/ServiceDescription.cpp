#include <Host/ServiceProcess/ServiceDescription.h>

namespace GameNet {
  namespace Host {
    namespace ServiceProcess {

      ServiceDescription::ServiceDescription(QObject *parent /*= 0*/)
        : QObject(parent)
        , _isDownloadable(false)
        , _hasDownloadPath(false)
      {
      }

      ServiceDescription::~ServiceDescription(void)
      {
      }

      QString ServiceDescription::id() const
      {
        return this->_id;
      }

      void ServiceDescription::setId(const QString& val)
      {
        this->_id = val;
      }

      QString ServiceDescription::gameId() const
      {
        return this->_gameId;
      }

      void ServiceDescription::setGameId(const QString& val)
      {
        this->_gameId = val;
      }

      QString ServiceDescription::name() const
      {
        return this->_name;
      }

      void ServiceDescription::setName(const QString& val)
      {
        this->_name = val;
      }

      QString ServiceDescription::torrentUrl() const
      {
        return this->_torrentUrl;
      }

      void ServiceDescription::setTorrentUrl(const QString& val)
      {
        this->_torrentUrl = val;
      }

      bool ServiceDescription::hasDownloadPath() const
      {
        return this->_hasDownloadPath;
      }

      void ServiceDescription::setHasDownloadPath(bool val)
      {
        this->_hasDownloadPath = val;
      }

      bool ServiceDescription::isDownloadable() const
      {
        return this->_isDownloadable;
      }

      void ServiceDescription::setIsDownloadable(bool val)
      {
        this->_isDownloadable = val;
      }

      QString ServiceDescription::extractorType() const
      {
        return this->_extractorType;
      }

      void ServiceDescription::setExtractorType(const QString& val)
      {
        this->_extractorType = val;
      }

      QString ServiceDescription::executeUrl() const
      {
        return this->_executeUrl;
      }

      void ServiceDescription::setExecuteUrl(const QString& val)
      {
        this->_executeUrl = val;
      }

      QString ServiceDescription::dependencyList() const
      {
        return this->_dependencyList;
      }

      void ServiceDescription::setDependencyList(const QString& val)
      {
        this->_dependencyList = val;
      }

      int ServiceDescription::gameSize() const
      {
        return this->_gameSize;
      }

      void ServiceDescription::setGameSize(int val)
      {
        this->_gameSize = val;
      }

      const QList<DownloadHookDescription>& ServiceDescription::downloadHooks() const
      {
        return this->_downloadHooks;
      }

      void ServiceDescription::setDownloadHooks(const QList<DownloadHookDescription>& val)
      {
        this->_downloadHooks = val;
      }

      const QList<ExecutorHookDescription>& ServiceDescription::executorHooks() const
      {
        return this->_executorHooks;
      }

      void ServiceDescription::setExecutorHooks(const QList<ExecutorHookDescription>& val)
      {
        this->_executorHooks = val;
      }
    }
  }
}