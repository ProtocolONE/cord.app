#pragma once
#include <QtCore/QObject>
#include <QtCore/QMap>
#include <windows.h>

namespace P1 {
  class KeyboardLayoutHelper: public QObject {
    Q_OBJECT
      Q_PROPERTY(QString keyboardLayout READ keyboardLayout NOTIFY keyboardLayoutChanged)
      Q_PROPERTY(bool capsLockEnabled READ capsLockEnabled NOTIFY keyboardCapslockChanged)

  public:
    KeyboardLayoutHelper();
    ~KeyboardLayoutHelper();

  public slots:
    QString keyboardLayout();
    bool capsLockEnabled();
    void changeLayout();

    void update();

  signals:
    void keyboardLayoutChanged();
    void keyboardCapslockChanged();

  private:
    HKL _layoutCode;
    bool _capsEnabled;
    QString _layoutName;
  };
}