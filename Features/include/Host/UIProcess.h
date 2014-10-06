#pragma  once

#include <QtCore/QObject>
#include <QtCore/QString>
#include <QtCore/QStringList>

#include <windows.h>

namespace GameNet {
  namespace Host {

    class UIProcess: public QObject 
    {
      Q_OBJECT
    public:
      explicit UIProcess::UIProcess(QObject* parent = 0);
      virtual UIProcess::~UIProcess();

      bool start(const QString& path, const QString& fileName, const QStringList& args);
      bool isRunning();

    private slots:
      void handleActivated(HANDLE handle);

    private:
      HANDLE _processHandle;
    };


  }
}