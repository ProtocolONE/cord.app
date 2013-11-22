#ifndef _GGS_GAMEDOWNLOADER_TEST_TESTEVENTLOOPFINISHER_H_
#define _GGS_GAMEDOWNLOADER_TEST_TESTEVENTLOOPFINISHER_H_

#include <QtCore/QObject>
#include <QtCore/QEventLoop>
#include <QtCore/QTimer>
class TestEventLoopFinisher : public QObject
{
  Q_OBJECT
public:
  TestEventLoopFinisher(QEventLoop *loop, int timeout);
  ~TestEventLoopFinisher();
  
  bool isKilledByTimeout() const { return this->_timeoutKill; }
  bool setTerminateSignal(QObject *sender, const char* signal);

public slots:
  void terminateLoop();
  
private slots:
  void timeoutTick();

private:
  bool _timeoutKill;
  QTimer _timer;
  QEventLoop *_loop;
};

#endif // _GGS_GAMEDOWNLOADER_TEST_TESTEVENTLOOPFINISHER_H_