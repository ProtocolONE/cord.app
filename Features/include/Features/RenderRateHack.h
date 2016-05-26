#pragma once

#include <QtCore/QObject>

class QQuickView;
namespace Features {

  class RenderRateHack : public QObject
  {
    Q_OBJECT
  public:
    explicit RenderRateHack(QObject* parent = 0);
    virtual ~RenderRateHack();

    void init(QQuickView *window);
    void setVsyncDelay(int value);
    void resetToOriginal();

  private:
    int *_vsync;
    int _originalValue;
  };

}