#include <Uninstall.h>
#include <viewmodel/GameSettingsViewModel.h>
#include <Application/ArgumentParser.h>
#include <Settings/Settings>
#include <QtCore/QFile>

Uninstall::Uninstall()
{
}

Uninstall::~Uninstall()
{
}

void Uninstall::run(const QStringList& arguments) {
	GGS::Application::ArgumentParser argumentsParser;
	argumentsParser.parse(arguments);

	QStringList args = argumentsParser.commandArguments("uninstall");

	GGS::Settings::Settings settings;
	QStringList filesToDelete;

	if (args.isEmpty()) {
		settings.beginGroup("filesToDelete");
		filesToDelete = GameSettingsViewModel::deserialize(settings.value("filesToDelete", QByteArray()).toByteArray());
	} else {
		settings.beginGroup("GameInstallInfo");
		settings.beginGroup(args[0]);
		filesToDelete = GameSettingsViewModel::deserialize(settings.value("iconPaths", QByteArray()).toByteArray());
	}

	Q_FOREACH(QString fileName, filesToDelete)
		QFile::remove(fileName);
}
