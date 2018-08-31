#pragma once

#include <RestApi/ProtocolOneCredential.h>
#include <RestApi/CommandBase.h>

#include <QtDBus/QDBusConnection>

#include <QtCore/QObject>
#include <QtCore/QString>
#include <QtCore/QSet>
#include <QtCore/QTimer>

namespace P1 {
  namespace Host {

    class Connection : public QObject
    {
      Q_OBJECT
    public:
      friend class ServiceHandle;

      explicit Connection(const QDBusConnection& connection, QObject *parent = 0);
      virtual ~Connection();

      virtual void setApplicationName(const QString& value);

      virtual void setCredential(const P1::RestApi::ProtocolOneCredential& value);
      virtual void setSecondCredential(const P1::RestApi::ProtocolOneCredential& value);

      virtual void ping();
      virtual void close();

      virtual const P1::RestApi::ProtocolOneCredential& credential();
      const QString& applicationName();

      void registerObject(const QString &path, QObject *object);

      bool isOwnService(const QString& serviceId);

      void onGenericError(
        P1::RestApi::CommandBase::Error error,
        QString message, 
        P1::RestApi::CommandBase *command);

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
      P1::RestApi::ProtocolOneCredential _credential;
      P1::RestApi::ProtocolOneCredential _secondCredential;
      QSet<QString> _lockedServices;
      QTimer _timeoutTimer;
      int _maxTimeoutFail;
      int _timeoutFail;
    };

  }
}
