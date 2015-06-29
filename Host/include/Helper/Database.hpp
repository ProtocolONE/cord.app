#pragma once

#include <Helper/FileUtils.h>

#include <Settings/Settings.h>
#include <Settings/InitializeHelper.h>

#include <QtCore/QString>
#include <QtCore/QStandardPaths>

namespace GameNet {
  namespace Host {

    bool initDatabase()
    {
      QString settingsPath = QString("%1/settings/host.sql")
        .arg(QStandardPaths::writableLocation(QStandardPaths::DataLocation));

      Features::FileUtils::createDirectoryForFile(settingsPath);

      GGS::Settings::InitializeHelper helper;
      helper.setFileName(settingsPath);
      if (!helper.init())
        return false;

      GGS::Settings::Settings::setCacheEnabled(true);
      return true;
    }
  }
}