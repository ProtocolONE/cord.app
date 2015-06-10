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

#include <Application/ArgumentParser.h>
#include <Thetta/Driver.h>
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
  uninstallDriver();

  Features::terminateProcessByName("gamenet.ui.exe");

  QString program = QString("%1/gamenet.ui.exe").arg(QCoreApplication::applicationDirPath());
  QProcess uiProcess;
  uiProcess.start(program, arguments);
  uiProcess.waitForFinished();
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
