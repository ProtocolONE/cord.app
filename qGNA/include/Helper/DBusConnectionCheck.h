#pragma once

#include <QtCore/QObject>
#include <QtCore/QString>

#if defined(Q_OS_WIN) && defined(interface)
#undef interface
#endif

class QDBusConnectionInterface;

namespace GameNet {

  class DBusConnectionCheck: public QObject 
  {
    Q_OBJECT
  public:
    explicit DBusConnectionCheck::DBusConnectionCheck(const QString& dbusService);
    virtual DBusConnectionCheck::~DBusConnectionCheck();

    bool checkConnection();

  private slots:
    void serviceUnregistered(QString service);

  signals:
    void serviceDisconnected();

  private:
    QDBusConnectionInterface* _iface;
    QString _dbusService;
  };
}