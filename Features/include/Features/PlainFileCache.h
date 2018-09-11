#pragma once

#include <QtCore/QObject>
#include <RestApi/CacheInterface.h>

namespace Features {

  class PlainFileCache : public QObject, public P1::RestApi::CacheInterface
  {
  public:
    explicit PlainFileCache(QObject* parent = 0);
    virtual ~PlainFileCache();


    virtual bool tryGet(const QString& key, QString& value) override;
    virtual void put(const QString& key, const QString& value, int timeInSec) override;

  private:
  };

}