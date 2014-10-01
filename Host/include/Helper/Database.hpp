#pragma once

#include <QtCore/QString>
#include <QtCore/QFile>

#include <Settings/Settings.h>
#include <Settings/InitializeHelper.h>

namespace GameNet {
  namespace Host {
    void migrateSettings(const QString& targetPath) 
    {
      QString oldSettingsPath = QString("%1/settings.sql").arg(QCoreApplication::applicationDirPath());
      QFile::copy(oldSettingsPath, targetPath);
    }

    bool initDatabase()
    {
      QString settingsPath = QString("%1/host.sql").arg(QCoreApplication::applicationDirPath());
      if (!QFile::exists(settingsPath))
        migrateSettings(settingsPath);

      GGS::Settings::InitializeHelper helper;
      helper.setFileName(settingsPath);
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