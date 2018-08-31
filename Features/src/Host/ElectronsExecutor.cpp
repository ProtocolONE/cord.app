#include <Host/ElectronsExecutor.h>

namespace P1 {
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
