#pragma once

#include <Host/ElectronExecutor.h>

namespace GameNet {
  namespace Host {

    class ElectronsExecutor : public ElectronExecutor
    {
      Q_OBJECT
    public:
      explicit ElectronsExecutor(QObject *parent = 0);
      virtual ~ElectronsExecutor();
    };
  }
}
