#include <Features/LanguageChangeEventFilter.h>

#include <QtCore/QObject>
#include <QtCore/QCoreApplication>
#include <Windows.h>

namespace P1 {
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