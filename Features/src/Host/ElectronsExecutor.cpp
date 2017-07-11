/****************************************************************************
** This file is a part of Syncopate Limited GameNet Application or it parts.
**
** Copyright (©) 2011 - 2012, Syncopate Limited and/or affiliates.
** All rights reserved.
**
** This file is provided AS IS with NO WARRANTY OF ANY KIND, INCLUDING THE
** WARRANTY OF DESIGN, MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE.
****************************************************************************/

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
