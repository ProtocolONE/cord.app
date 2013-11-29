/****************************************************************************
** This file is a part of Syncopate Limited GameNet Application or it parts.
**
** Copyright (©) 2011 - 2012, Syncopate Limited and/or affiliates. 
** All rights reserved.
**
** This file is provided AS IS with NO WARRANTY OF ANY KIND, INCLUDING THE
** WARRANTY OF DESIGN, MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE.
****************************************************************************/
#include <Uninstall.h>
#include <viewmodel/GameSettingsViewModel.h>
#include <Application/ArgumentParser.h>
#include <Settings/Settings>

#include <Thetta/Driver.h>

#include <QtCore/QFile>
#include <QtCore/QCoreApplication>

Uninstall::Uninstall()
{
}

Uninstall::~Uninstall()
{
}

void Uninstall::run(const QStringList& arguments)
{
  uninstallDriver();

  GGS::Application::ArgumentParser argumentsParser;
  argumentsParser.parse(arguments);

  QStringList args = argumentsParser.commandArguments("uninstall");

  GGS::Settings::Settings settings;
  QStringList filesToDelete;

  if (args.isEmpty()) {
    settings.beginGroup("GameInstallInfo");
    filesToDelete = GameSettingsViewModel::deserialize(settings.value("filesToDelete", QByteArray()).toByteArray());
  } else {
    settings.beginGroup("GameInstallInfo");
    settings.beginGroup(args[0]);
    filesToDelete = GameSettingsViewModel::deserialize(settings.value("iconPaths", QByteArray()).toByteArray());
  }

  Q_FOREACH(QString fileName, filesToDelete)
    QFile::remove(fileName);
}

void Uninstall::uninstallDriver()
{
  QString thettaDirectory = QString("%1/Thetta").arg(QCoreApplication::applicationDirPath());
  Thetta::Driver driver(0);
  driver.setPath(thettaDirectory);
  driver.setName(QString("Thetta"));
  driver.unload();
  driver.uninstall();
}
