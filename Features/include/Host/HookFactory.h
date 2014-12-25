#pragma once
#include <QtCore/QObject>
#include <QtCore/QHash>

namespace GGS {
  namespace GameDownloader {
    class HookBase;
  }
}

namespace GameNet {
  namespace Integration {
    namespace ZZima {
      class ZZimaConnection;
    }
  }

  namespace Host {

    class ServiceSettings;

    namespace ServiceProcess {
      class ServiceLoader;
    }

    class HookFactory : public QObject
    {
      Q_OBJECT
    public:
      explicit HookFactory(QObject *parent = 0);
      ~HookFactory();

      void setServiceSettings(ServiceSettings *value);
      void setServiceLoader(ServiceProcess::ServiceLoader *value);
      void setZzimaConnection(::GameNet::Integration::ZZima::ZZimaConnection *value);

      GGS::GameDownloader::HookBase* create(const QString& guid);

    private:
      QHash<QString, GGS::GameDownloader::HookBase*> _cache;
      ServiceSettings *_serviceSettings;
      ServiceProcess::ServiceLoader *_serviceLoader;
      ::GameNet::Integration::ZZima::ZZimaConnection *_zzimaConnection;
    };

  }
}