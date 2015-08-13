#include "SingleShotFunctor.h"
#include <QtCore/QTimer>

SingleShotFunctor::SingleShotFunctor(int ms, std::tr1::function<void ()> f, QObject *parent)
  : QObject(parent)
  , _functor(f)
{
  QTimer::singleShot(ms, this, SLOT(internalFunctorCaller()));
}

SingleShotFunctor::SingleShotFunctor(QObject *parent)
  : QObject(parent)
{
  this->_functor = nullptr;
}

SingleShotFunctor::~SingleShotFunctor()
{
}

void SingleShotFunctor::internalFunctorCaller()
{
  if (this->_functor)
    this->_functor();
}

void SingleShotFunctor::setCallback(std::tr1::function<void ()> f)
{
  this->_functor = f;
}

void SingleShotFunctor::start(int msec)
{
  QTimer::singleShot(msec, this, SLOT(internalFunctorCaller()));
}
