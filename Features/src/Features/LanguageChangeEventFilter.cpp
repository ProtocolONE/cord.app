/****************************************************************************
** This file is a part of Syncopate Limited GameNet Application or it parts.
**
** Copyright (©) 2011 - 2014, Syncopate Limited and/or affiliates. 
** All rights reserved.
**
** This file is provided AS IS with NO WARRANTY OF ANY KIND, INCLUDING THE
** WARRANTY OF DESIGN, MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE.
****************************************************************************/
#include <Features/LanguageChangeEventFilter.h>

#include <QtCore/QObject>
#include <QtCore/QCoreApplication>
#include <Windows.h>

namespace GGS {
  namespace Application {
    LanguageChangeEventFilter::LanguageChangeEventFilter(QObject *parent)
      : QObject(parent)
    {
    }

    bool LanguageChangeEventFilter::nativeEventFilter(const QByteArray &eventType, void *message, long *result)
    {
      MSG* msg = reinterpret_cast<MSG*>(message);
      if (WM_INPUTLANGCHANGE == msg->message) {
        emit this->languageChanged();
        return true;
      }
      return false;
    }
  }
}