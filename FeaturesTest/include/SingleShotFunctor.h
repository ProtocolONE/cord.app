#ifndef _GGS_GAMEDOWNLOADER_TEST_SINGLESHOTFUNCTOR_H_
#define _GGS_GAMEDOWNLOADER_TEST_SINGLESHOTFUNCTOR_H_

#include <QtCore/QObject>
#include <functional>

// ���� ��������� ��� ������������� ����� �������. ����������� ��������� ��������� ����� ���� ������� ���������
// � �� ����� �������� �� �������, �.�. ��������� [=].
#define SINGLESHOTFUNCTOR(x) std::tr1::function<void ()> x = [&]() mutable -> void

class SingleShotFunctor : public QObject
{
  Q_OBJECT
public:
  SingleShotFunctor(int ms, std::tr1::function<void ()> f, QObject *parent = 0);
  SingleShotFunctor(QObject *parent = 0);
  ~SingleShotFunctor();

  void setCallback(std::tr1::function<void ()> f);
  void start(int msec);

private slots:
  void internalFunctorCaller();

private:
  std::tr1::function<void ()> _functor;
};

#endif // _GGS_GAMEDOWNLOADER_TEST_SINGLESHOTFUNCTOR_H_