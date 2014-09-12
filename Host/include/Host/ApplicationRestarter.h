#pragma once

#include <QtCore/QObject>
#include <QtCore/QString>

namespace GameNet {
  namespace Host {

    class ApplicationRestarter: public QObject
    {
      Q_OBJECT
    public:
      explicit ApplicationRestarter::ApplicationRestarter(QObject* parent = 0);
      virtual ApplicationRestarter::~ApplicationRestarter();

      void setGameDownloadInitialized();
      void restartApplication(bool shouldStartWithSameArguments, bool isMinimized);

    signals:
      void secondInstanceExecuteRequest();
      void shutdownRequest();

    private:
       bool executeFile(const QString& path, const QString& fileName, const QString& args);
       void internalRestartApplication(bool shouldStartWithSameArguments, bool isMinimized);

       bool _gameDownloadInitialized;
    };

  }
}