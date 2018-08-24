#include <Uninstall.h>
#include <viewmodel/GameSettingsViewModel.h>
#include <Application/ArgumentParser.h>
#include <Settings/Settings.h>

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
  P1::Application::ArgumentParser argumentsParser;
  argumentsParser.parse(arguments);

  QStringList args = argumentsParser.commandArguments("uninstall");

  P1::Settings::Settings settings;
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
