#pragma once

#include <Host/ServiceDescription.h>

#include <Core/Service.h>

#include <QtCore/QObject>
#include <QtCore/QHash>
#include <QtCore/QString>

namespace GGS {
  namespace Core {
    class Service;
  }
}

namespace GameNet {
  namespace Host {

    class ServiceLoader : public QObject
    {
      Q_OBJECT

    public:
      explicit ServiceLoader(QObject *parent = 0);
      ~ServiceLoader();

      GGS::Core::Service::Area gameArea() const;
      void setGameArea(GGS::Core::Service::Area val);

      GGS::Core::Service::Area applicationArea() const;
      void setApplicationArea(GGS::Core::Service::Area val);

      QString applicationPath() const;
      void setApplicationPath(QString val);

      void registerService(const ServiceDescription& description);
      
      GGS::Core::Service *getService(const QString& serviceId);

    private:
      QString _applicationPath;
      
      GGS::Core::Service::Area _gameArea;
      GGS::Core::Service::Area _applicationArea;
      QHash<QString, GGS::Core::Service *> _serviceMap;

      QString getServiceInstallPath(const QString& id);
      QString getServiceDownloadPath(const QString& id);
    };

  }
}