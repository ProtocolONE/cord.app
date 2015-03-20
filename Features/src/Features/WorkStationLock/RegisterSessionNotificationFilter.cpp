#include <Features/WorkStationLock/RegisterSessionNotificationFilter.h>

#include <QtCore/QDebug>
#include <Wtsapi32.h>

namespace Features {
  namespace WorkStationLock {

    RegisterSessionNotificationFilter::RegisterSessionNotificationFilter(QObject *parent, HWND hwnd)
      : QObject(parent)
      , _hwnd(hwnd)
      , _isLocked(false)
    {
      Q_ASSERT(WTSRegisterSessionNotification(this->_hwnd, NOTIFY_FOR_THIS_SESSION));
    }

    RegisterSessionNotificationFilter::~RegisterSessionNotificationFilter()
    {
      Q_ASSERT(WTSUnRegisterSessionNotification(this->_hwnd));
    }

    bool Features::WorkStationLock::RegisterSessionNotificationFilter::isLocked() const
    {
      return this->_isLocked;
    }

    bool Features::WorkStationLock::RegisterSessionNotificationFilter::nativeEventFilter(const QByteArray &eventType, void *message, long *result)
    {
      MSG* msg = reinterpret_cast<MSG*>(message);

      if (WM_WTSSESSION_CHANGE != msg->message) {
        return false;
      }

      //INFO More details in https://msdn.microsoft.com/en-us/library/aa376875(VS.85).aspx
      qDebug() << "WM_WTSSESSION_CHANGE" << " lparam " << msg->lParam << "wparam " << msg->wParam;
      switch(msg->wParam) {
      case WTS_SESSION_LOGOFF:
      case WTS_SESSION_LOCK:
        this->_isLocked = true;
        emit this->workStationLocked();
        return true;
      case WTS_SESSION_LOGON:
      case WTS_SESSION_UNLOCK:
        this->_isLocked = false;
        emit this->workStationUnlocked();
        return true;
      }

      return false;
    }
  }
}