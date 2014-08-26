#pragma once
#include <QtCore/QObject>

namespace GGS {
  
  namespace GameDownloader {
    class HookBase;
  }

}

namespace GameNet {
  namespace Host {

    class HookFactory : public QObject
    {
      Q_OBJECT
    public:
      explicit HookFactory(QObject *parent);
      ~HookFactory();

      GGS::GameDownloader::HookBase* create(const QString& guid);

    private:

    };

  }
}