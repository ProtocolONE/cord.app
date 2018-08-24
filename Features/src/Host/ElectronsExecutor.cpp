#include <Host/ElectronsExecutor.h>

namespace GameNet {
  namespace Host {


    ElectronsExecutor::ElectronsExecutor(QObject *parent) : ElectronExecutor(parent)
    {
      this->_scheme = QString("electrons");
    }

    ElectronsExecutor::~ElectronsExecutor()
    {
    }
  }
}
