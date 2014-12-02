#pragma once

#include <QtCore/QObject>

namespace GameNet {
  namespace Host {

    // INFO: https://corp.syncopate.local/!%D0%9F%D1%80%D0%BE%D0%B5%D0%BA%D1%82%D1%8B/QGNA/%D0%9A%D0%BB%D1%8E%D1%87%D0%B8_%D1%80%D0%B5%D0%B5%D1%81%D1%82%D1%80%D0%B0
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

