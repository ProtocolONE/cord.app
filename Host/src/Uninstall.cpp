#include <Uninstall.h>

#include <Application/ArgumentParser.h>
#include <Helper/TerminateProcess.h>

#include <QtCore/QFile>
#include <QtCore/QCoreApplication>
#include <QtCore/QProcess>

Uninstall::Uninstall()
{
}

Uninstall::~Uninstall()
{
}

void Uninstall::run(const QStringList& arguments)
{
  Features::terminateProcessByName("gamenet.ui.exe");

  QString program = QString("%1/gamenet.ui.exe").arg(QCoreApplication::applicationDirPath());
  QProcess uiProcess;
  uiProcess.start(program, arguments);
  uiProcess.waitForFinished();
}

