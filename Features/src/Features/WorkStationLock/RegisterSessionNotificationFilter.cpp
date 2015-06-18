#include <Features/WorkStationLock/RegisterSessionNotificationFilter.h>

#include <Helper/DebugLog.h>

#include <QtCore/QDebug>
#include <QtCore/QTimer>
#include <Wtsapi32.h>

namespace Features {
  namespace WorkStationLock {

    RegisterSessionNotificationFilter::RegisterSessionNotificationFilter(QObject *parent, HWND hwnd)
      : QObject(parent)
      , _hwnd(hwnd)
      , _isLocked(false)
      , _retryCount(120)
    {
      this->tryInit();
    }

    RegisterSessionNotificationFilter::~RegisterSessionNotificationFilter()
    {
      BOOL result = WTSUnRegisterSessionNotification(this->_hwnd);
      Q_ASSERT(result);
    }

    void RegisterSessionNotificationFilter::tryInit()
    {
      this->_retryCount--;
      BOOL result = WTSRegisterSessionNotification(this->_hwnd, NOTIFY_FOR_THIS_SESSION);
      if (!result && this->_retryCount > 0) {
        DWORD errorCode = GetLastError();
        if (errorCode == RPC_S_INVALID_BINDING) {
          QTimer::singleShot(1000, this, SLOT(tryInit()));
          return;
        }

        DEBUG_LOG << "WTSRegisterSessionNotification error" << errorCode;
      }

      Q_ASSERT(result);
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