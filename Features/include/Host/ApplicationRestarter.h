#pragma once

#include <QtCore/QObject>
#include <QtCore/QString>

namespace GameNet {
  namespace Host {

    class ShutdownManager;

    class ApplicationRestarter: public QObject
    {
      Q_OBJECT
    public:
      explicit ApplicationRestarter::ApplicationRestarter(QObject* parent = 0);
      virtual ApplicationRestarter::~ApplicationRestarter();

      void setShutdownManager(ShutdownManager *value);
      void restartApplication(bool shouldStartWithSameArguments, bool isMinimized);

    private:
       bool executeFile(const QString& path, const QString& fileName, const QString& args);
       void internalRestartApplication(bool shouldStartWithSameArguments, bool isMinimized);

       ShutdownManager *_shutdown;
    };

  }
}