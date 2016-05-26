#include <Features/RenderRateHack.h>
#include <assert.h>
#include <QtCore/QDebug>
#include <QtGui/QGuiApplication>
#include <QtGui/QScreen>
#include <excpt.h>

namespace Features {
  RenderRateHack::RenderRateHack(QObject* parent /*= 0*/)
    : QObject(parent)
    , _vsync(nullptr)
    , _originalValue(0x10)
  {
  }

  RenderRateHack::~RenderRateHack()
  {
  }

  int sehFilter(unsigned int code, struct _EXCEPTION_POINTERS *ep)
  {
    return EXCEPTION_EXECUTE_HANDLER;
  }

  bool initBlackMagic(void* window, int** addr, int& value)
  {
    bool result = false;
    __try {
      // INFO тут из QQuickView пытаемся добраться до QSGWindowsRenderLoop::m_vsyncDelta
      *addr = ((int*)(*((void**)(*((void**)window + 1)) + 76)) + 10);
      value = **addr;
      result = true;
    } __except (sehFilter(GetExceptionCode(), GetExceptionInformation())) {
    }

    return result;
  }

  void RenderRateHack::init(QQuickView *window)
  {
    assert(window);
    {
      const char* version = qVersion();
      QString expectedVersion("5.5.1");
      if (expectedVersion != version) {
        qWarning() << "RenderRateHack init failed. Expected qt version is " << expectedVersion
          << "Current version is " << version;
        return;
      }

    }

    // INFO формула из сорцов Qt
    int expect_m_vsyncDelta = 1000 / QGuiApplication::primaryScreen()->refreshRate();
    if (expect_m_vsyncDelta <= 0)
      expect_m_vsyncDelta = 16;

    int* m_vsyncDelta = nullptr;
    int originalValue = 0;

    if (!initBlackMagic(window, &m_vsyncDelta, originalValue))
      return;

    this->_originalValue = originalValue;
    if (originalValue != expect_m_vsyncDelta) {
      qWarning() << "RenderRateHack init failed. Wrong vsyncDelta. Can be wrong qt version";
      return;
    }

    this->_vsync = m_vsyncDelta;
    this->_originalValue = originalValue;
  }

  void RenderRateHack::setVsyncDelay(int value)
  {
    if (!this->_vsync)
      return;

    *this->_vsync = value;
  }

  void RenderRateHack::resetToOriginal()
  {
    this->setVsyncDelay(this->_originalValue);
  }

}