#pragma  once

#include <QtCore/QObject>
#include <QtQml/QQmlNetworkAccessManagerFactory>
#include <QtNetwork/QNetworkAccessManager>

class CacheNetworkManagerFactory : public QObject, public QQmlNetworkAccessManagerFactory
{
  Q_OBJECT
public:
  explicit CacheNetworkManagerFactory(QObject *parent = 0);
  virtual QNetworkAccessManager *create(QObject *parent) override;
};
