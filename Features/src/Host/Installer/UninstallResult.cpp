#include <Host/Installer/UninstallResult.h>

#include <QtCore/QSettings>
#include <QtConcurrent/QtConcurrentRun>

#include <Core/Service.h>

class QStringToWChar {
public:
  QStringToWChar(const QString& str) : _data(NULL)
  {
    this->_size = str.size();
    this->_data = new wchar_t[this->_size + 1]();
    str.toWCharArray(this->_data);
  }

  ~QStringToWChar() 
  {
    delete [] _data;
  }

  wchar_t* data() { return this->_data; }
  size_t size() { return this->_size; }

private:
  wchar_t *_data;
  size_t _size;
};


namespace GameNet {
  namespace Host {
    namespace Installer {

      UninstallResult::UninstallResult(const QString &serviceId, QObject *parent /* = 0 */)
        : QObject(parent)
        , _serviceId(serviceId)
        , _eventHandle(NULL)
        , _memoryHandle(NULL)
        , _uninstallResult(nullptr)
      {
        this->_eventName = "Global\\GameNet_UninstallComplete_" + serviceId;
        this->_sharedMemoryName = "Global\\GameNet_UninstallCompleteResult_" + serviceId;
      }

      UninstallResult::~UninstallResult()
      {
        if (this->_eventHandle)
          CloseHandle(this->_eventHandle);

        if (this->_memoryHandle) {
          UnmapViewOfFile(this->_uninstallResult);
          CloseHandle(this->_memoryHandle);
        }
      }

      void UninstallResult::setFinished(int result)
      {
        if (!this->createEvent())
          return;
        
        if (!this->openSharedMemory())
          return;

        *(this->_uninstallResult) = static_cast<quint32>(result);

        ::SetEvent(this->_eventHandle);
      }

      void UninstallResult::asyncWaitResult()
      {
        QtConcurrent::run(this, &UninstallResult::internalWaitUninstall);
      }

      void UninstallResult::onUninstallFinished(const GGS::Core::Service *service)
      {
        Q_ASSERT(service);
        if (this->_serviceId != service->id()) 
          return;

        this->setFinished(UninstallResult::Success);
        this->deleteLater();
      }

      void UninstallResult::onUninstallFailed(const GGS::Core::Service *service)
      {
        Q_ASSERT(service);
        if (this->_serviceId != service->id())
          return;

        this->setFinished(UninstallResult::Failed);
        this->deleteLater();
      }

      void UninstallResult::onUninstallCancelled(const QString &serviceId)
      {
        if (this->_serviceId != serviceId) 
          return;

        this->setFinished(UninstallResult::Canceled);
        this->deleteLater();
      }

      bool UninstallResult::createEvent()
      {
        QStringToWChar eventName(this->_eventName);
        this->_eventHandle = CreateEvent(NULL, TRUE, FALSE, eventName.data());
        if (!this->_eventHandle)
          return false;

        return true;
      }

      bool UninstallResult::openSharedMemory()
      {
        QStringToWChar sharedMemoryName(this->_sharedMemoryName);
        DWORD errCode = 0;

        this->_memoryHandle = OpenFileMapping(
          FILE_MAP_ALL_ACCESS,
          FALSE,
          sharedMemoryName.data());

        if (!this->_memoryHandle) {
          this->_memoryHandle = CreateFileMapping(INVALID_HANDLE_VALUE, 
            NULL,
            PAGE_READWRITE,
            0,
            sizeof(quint32),
            sharedMemoryName.data());

        }

        if (!this->_memoryHandle) 
          return false;

        this->_uninstallResult = (quint32 *)MapViewOfFile(this->_memoryHandle,
          FILE_MAP_READ | FILE_MAP_WRITE,
          0,
          0,
          sizeof(quint32));

        if (!this->_uninstallResult)
          return false;

        return true;
      }

      void UninstallResult::internalWaitUninstall()
      {
        this->createEvent();
        this->openSharedMemory();

        if (!this->_eventHandle)
          return;

        if (!this->_memoryHandle || !this->_uninstallResult)
          return;

        DWORD waitResult = WaitForSingleObject(this->_eventHandle, INFINITE);
        switch (waitResult) {
        case WAIT_OBJECT_0:
          emit this->finished(static_cast<int>(*(this->_uninstallResult)));
          break;
        case WAIT_TIMEOUT:
        case WAIT_FAILED:
          emit this->finished(static_cast<int>(Failed));
          break; 
        }
      }
    }
  }
}