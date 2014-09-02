#pragma once
#include <QtCore/QObject>
#include <QtCore/QHash>

namespace GGS {
  
  namespace GameDownloader {
    class HookBase;
  }

}

namespace GameNet {
  namespace Host {

    class ServiceSettings;
    class ServiceLoader;
    class HookFactory : public QObject
    {
      Q_OBJECT
    public:
      explicit HookFactory(QObject *parent);
      ~HookFactory();

      void setServiceSettings(ServiceSettings *value);
      void setServiceLoader(ServiceLoader *value);

      GGS::GameDownloader::HookBase* create(const QString& guid);

    private:
      QHash<QString, GGS::GameDownloader::HookBase*> _cache;
      ServiceSettings *_serviceSettings;
      ServiceLoader *_services;
    };

  }
}