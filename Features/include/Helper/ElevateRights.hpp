#pragma once

#include <AutoRunHelper/UACHelper.h>

#include <QtCore/QDebug>

namespace GameNet {
  namespace Host {

    bool requireAdminRights() 
    {
      using GGS::AutoRunHelper::UACHelper;

#ifndef QGNA_NO_ADMIN_REQUIRED
      if(!GGS::AutoRunHelper::UACHelper::isUserAdminByRole()) {
        if (!GGS::AutoRunHelper::UACHelper::restartToElevateRights()) {    
          qDebug() << "Restart failed. May be user didn't accept UAC.";
          return false;
        }

        return false;
      }
#endif

      return true;
    }

  }
}