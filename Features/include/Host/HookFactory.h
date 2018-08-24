#pragma once
#include <QtCore/QObject>
#include <QtCore/QHash>

namespace P1 {
  namespace GameDownloader {
    class HookBase;
  }
}

namespace GameNet {
  namespace Host {

    class ServiceSettings;
    class ServiceHandle;

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
      void setServiceHandle(ServiceHandle *value);

      P1::GameDownloader::HookBase* create(const QString& guid);

    private:
      QHash<QString, P1::GameDownloader::HookBase*> _cache;
      ServiceSettings *_serviceSettings;
      ServiceProcess::ServiceLoader *_serviceLoader;
      ServiceHandle *_serviceHandle;
    };

  }
}