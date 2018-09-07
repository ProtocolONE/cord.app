#pragma once

#include <AutoRunHelper/UACHelper.h>

#include <QtCore/QDebug>

namespace P1 {
  namespace Host {

    bool requireAdminRights() 
    {
      using P1::AutoRunHelper::UACHelper;

#ifndef LAUNCHER_NO_ADMIN_REQUIRED
      if(!P1::AutoRunHelper::UACHelper::isUserAdminByRole()) {
        if (!P1::AutoRunHelper::UACHelper::restartToElevateRights()) {    
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