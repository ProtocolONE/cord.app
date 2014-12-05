/****************************************************************************
** This file is a part of Syncopate Limited GameNet Application or it parts.
**
** Copyright (©) 2011 - 2013, Syncopate Limited and/or affiliates. 
** All rights reserved.
**
** This file is provided AS IS with NO WARRANTY OF ANY KIND, INCLUDING THE
** WARRANTY OF DESIGN, MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE.
****************************************************************************/
#pragma once

#include <QtCore/QObject>

#include <atlcomcli.h>
#include <shobjidl.h>

namespace Features {
  
  class TaskBarHelper : public QObject
  {
    Q_OBJECT
  public:
    enum Status {
      StatusUnknown = 0,
      StatusNormal = 1,
      StatusPaused = 2,
      StatusError = 3
    };

    explicit TaskBarHelper(QObject *parent = 0);
    void prepare(HWND hwnd);
    void init();

    unsigned int getTaskbarCreatedMessageId() const;

    void setProgress(int progress);
    void setStatus(Status status);
    void setIcon(const QString &iconSource, const QString &hint = QString());
    void restore();

  private:
    bool isPlatformSupported() const;

  private:
    bool _isPlatformSupported;
    HWND _appMainWindow;
    CComPtr<ITaskbarList3> _taskbarList;
    unsigned int _taskBarCreatedMsgId;

    Status _lastStatus;
    int _lastProgress;
    QString _lastIcon;
  };

}