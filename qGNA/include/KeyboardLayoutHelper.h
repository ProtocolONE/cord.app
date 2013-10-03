/****************************************************************************
** This file is a part of Syncopate Limited GameNet Application or it parts.
**
** Copyright (�) 2011 - 2012, Syncopate Limited and/or affiliates. 
** All rights reserved.
**
** This file is provided AS IS with NO WARRANTY OF ANY KIND, INCLUDING THE
** WARRANTY OF DESIGN, MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE.
****************************************************************************/
#pragma once
#include <QtCore/QObject>
#include <QtCore/QMap>
#include <windows.h>

namespace GGS {
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