#pragma once

#include <QtCore/QString>

#include <Settings/Settings.h>
#include <Settings/InitializeHelper.h>

namespace GameNet {
  namespace Host {

    bool initDatabase()
    {
      GGS::Settings::InitializeHelper helper;
      helper.setFileName(QString("%1/host.sql").arg(QCoreApplication::applicationDirPath()));
      if (!helper.init())
        return false;

      if (helper.isRecreated())
      {
        //SettingsViewModel settingsViewModel;
        //settingsViewModel.setDefaultSettings();
      }

      GGS::Settings::Settings::setCacheEnabled(true);
      return true;
    }
  }
}