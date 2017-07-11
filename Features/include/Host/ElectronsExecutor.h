/****************************************************************************
** This file is a part of Syncopate Limited GameNet Application or it parts.
**
** Copyright (©) 2011 - 2012, Syncopate Limited and/or affiliates.
** All rights reserved.
**
** This file is provided AS IS with NO WARRANTY OF ANY KIND, INCLUDING THE
** WARRANTY OF DESIGN, MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE.
****************************************************************************/

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
