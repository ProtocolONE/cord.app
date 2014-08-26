#include <Host/HookFactory.h>
#include <GameDownloader/HookBase.h>

using GGS::GameDownloader::HookBase;

namespace GameNet {
  namespace Host {

    HookFactory::HookFactory(QObject *parent)
      : QObject(parent)
    {

    }

    HookFactory::~HookFactory()
    {

    }

    HookBase* HookFactory::create(const QString& guid)
    {
      return nullptr;
    }

  }
}