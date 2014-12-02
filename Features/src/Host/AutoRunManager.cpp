#include <Host/AutoRunManager.h>

#include <AutoRunHelper/AutoRunHelper.h>

#include <QtCore/QSettings>
#include <QtCore/QDebug>

namespace GameNet {
  namespace Host {

    AutoRunManager::AutoRunManager(QObject *parent /*= 0*/)
      : QObject(parent)
    {
    }

    AutoRunManager::~AutoRunManager()
    {
    }

    void AutoRunManager::init()
    {
      this->addToAutoStart(this->autoStartMode());
    }

    void AutoRunManager::setAutoStartMode(int value)
    {
      if (!(value == 0 || value == 1 || value == 2))
        return;

      int tmp = this->autoStartMode();
      if (tmp == value)
        return;

      QSettings settings("HKEY_LOCAL_MACHINE\\Software\\GGS\\QGNA", QSettings::NativeFormat);
      settings.setValue("AutoRun", value);
      this->addToAutoStart(value);
      emit this->autoStartModeChanged();
    }

    int AutoRunManager::autoStartMode() const
    {
      QSettings settings("HKEY_LOCAL_MACHINE\\Software\\GGS\\QGNA", QSettings::NativeFormat);
      bool ok;
      int result = settings.value("AutoRun", 0).toInt(&ok);
      return ok ? result : 0;
    }

    void AutoRunManager::addToAutoStart(int mode)
    {
      bool autostart = mode == 1 || mode == 2;
      bool isMinimized = mode == 2;

      GGS::AutoRunHelper::AutoRunHelper autorunHelper;
      autorunHelper.setTaskName("GameNet");

      if (!autostart) {
        qDebug() << "remove from autorun result: " << autorunHelper.removeFromAutoRun();
        return;
      }

      autorunHelper.setPathToExe(QCoreApplication::applicationFilePath());
      autorunHelper.setCommandLineArguments(isMinimized ? QString("/minimized") : QString(""));
      autorunHelper.setTaskDescription(tr("AUTORUN_TASK_DESCRIPTION"));
      qDebug() << "add to autorun result: " << autorunHelper.addToAutoRun();
    }

  }
}
