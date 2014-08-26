#include <Host/ServiceDescription.h>

namespace GameNet {
  namespace Host {

    ServiceDescription::ServiceDescription(QObject *parent /*= 0*/)
    {

    }

    ServiceDescription::~ServiceDescription(void)
    {
    }

    QString ServiceDescription::id() const
    {
      return this->_id;
    }

    void ServiceDescription::setId(QString val)
    {
      this->_id = val;
    }

    QString ServiceDescription::gameId() const
    {
      return this->_gameId;
    }

    void ServiceDescription::setGameId(QString val)
    {
      this->_gameId = val;
    }

    QString ServiceDescription::name() const
    {
      return this->_name;
    }

    void ServiceDescription::setName(QString val)
    {
      this->_name = val;
    }

    QString ServiceDescription::torrentUrl() const
    {
      return this->_torrentUrl;
    }

    void ServiceDescription::setTorrentUrl(QString val)
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

    void ServiceDescription::setExtractorType(QString val)
    {
      this->_extractorType = val;
    }

    QString ServiceDescription::executeUrl() const
    {
      return this->_executeUrl;
    }

    void ServiceDescription::setExecuteUrl(QString val)
    {
      this->_executeUrl = val;
    }

    QString ServiceDescription::dependencyList() const
    {
      return this->_dependencyList;
    }

    void ServiceDescription::setDependencyList(QString val)
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

    QList<DownloadHookDescription> ServiceDescription::downloadHooks() const
    {
      return this->_downloadHooks;
    }

    void ServiceDescription::setDownloadHooks(QList<DownloadHookDescription> val)
    {
      this->_downloadHooks = val;
    }

  }
}