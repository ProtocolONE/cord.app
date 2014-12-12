#pragma once

#include <RestApi/GameNetCredential.h>
#include <RestApi/CommandBase.h>

#include <QtDBus/QDBusConnection>

#include <QtCore/QObject>
#include <QtCore/QString>
#include <QtCore/QSet>
#include <QtCore/QTimer>

namespace GameNet {
  namespace Host {

    class Connection : public QObject
    {
      Q_OBJECT
    public:
      friend class ServiceHandle;

      explicit Connection(const QDBusConnection& connection, QObject *parent = 0);
      virtual ~Connection();

      virtual void setApplicationName(const QString& value);

      virtual void setCredential(const GGS::RestApi::GameNetCredential& value);
      virtual void setSecondCredential(const GGS::RestApi::GameNetCredential& value);

      virtual void ping();
      virtual void close();

      virtual const GGS::RestApi::GameNetCredential& credential();
      const QString& applicationName();

      void registerObject(const QString &path, QObject *object);

      bool isOwnService(const QString& serviceId);

      void onGenericError(
        GGS::RestApi::CommandBase::Error error,
        QString message, 
        GGS::RestApi::CommandBase *command);

    signals:
      void pong();
      void connectionInfoReceived();
      void wrongCredential(const QString& userId);
      void disconnected();

      void logoutMain();
      void mainCredentialChanged();

    private:
      void timeoutTick();

      QList<QString> lockedServices();
      void lockService(const QString& serviceId);
      void unlockService(const QString& serviceId);

      QDBusConnection _dbusConnection;

      QString _applicationName;
      GGS::RestApi::GameNetCredential _credential;
      GGS::RestApi::GameNetCredential _secondCredential;
      QSet<QString> _lockedServices;
      QTimer _timeoutTimer;
      int _maxTimeoutFail;
      int _timeoutFail;
    };

  }
}
