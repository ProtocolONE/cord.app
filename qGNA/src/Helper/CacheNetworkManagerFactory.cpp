#include <QtCore/QStandardPaths>
#include <QtNetwork/QNetworkDiskCache>

#include <Helper/CacheNetworkManagerFactory.h>

CacheNetworkManagerFactory::CacheNetworkManagerFactory(QObject *parent)
  : QObject(parent)
{
}

/*!
\fn QNetworkAccessManager* CustomNetworkManagerFactory::create(QObject *parent)

\brief this is a virtual method we need to implement this method , most important step
we will create our custom QNetworkAccessManager here and return that
the second important thing we need to do here is to connect the sslErrors signal
from QNetworkAccessManager to our own slot which will ignore the sslErrors

\param [in,out] parent If non-null, the parent.

\return null if it fails, else.
*/

QNetworkAccessManager* CacheNetworkManagerFactory::create(QObject *parent)
{
  QNetworkAccessManager* networkManager = new QNetworkAccessManager(parent);

  QString cacheDirectory = QStandardPaths::writableLocation(QStandardPaths::CacheLocation);

  QNetworkDiskCache* cache = new QNetworkDiskCache(networkManager);
  cache->setCacheDirectory(cacheDirectory);
  cache->setMaximumCacheSize(209715200i64);

  networkManager->setCache(cache);
  return networkManager;
}

