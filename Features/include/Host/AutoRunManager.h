#pragma once

#include <QtCore/QObject>

namespace GameNet {
  namespace Host {

    class AutoRunManager : public QObject
    {
      Q_OBJECT
    public:
      explicit AutoRunManager(QObject *parent = 0);
      virtual ~AutoRunManager();

      void init();

      virtual void setAutoStartMode(int value);
      virtual int autoStartMode() const;

    signals:
      void autoStartModeChanged();

    private:
      void addToAutoStart(int mode);

    };

  }
}

