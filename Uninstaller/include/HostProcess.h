#pragma  once

#include <QtCore/QObject>
#include <QtCore/QString>
#include <QtCore/QTimer>

#include <Windows.h>

class HostProcess: public QObject 
{
  Q_OBJECT
public:
  explicit HostProcess(QObject* parent = 0);
  virtual ~HostProcess();

  void start();

signals:
  void uninstallFinished(int result);

private slots:
  void onUninstallFinished(int result);
  void testQGNAReady();

private:
  void qgnaStarted();

  int _waitAttempts;
  QTimer _waitTimer;
  QString _serviceId;
};
