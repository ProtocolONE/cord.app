#pragma once

#include <RestApi/ProtocolOneCredential.h>

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

      virtual void ping();
      virtual void close();

      virtual const P1::RestApi::ProtocolOneCredential& credential();
      const QString& applicationName();

      void registerObject(const QString &path, QObject *object);

      bool isOwnService(const QString& serviceId);

      void onAuthorizationError(const P1::RestApi::ProtocolOneCredential &credential);
      void updateCredential(const P1::RestApi::ProtocolOneCredential &credentialOld,
        const P1::RestApi::ProtocolOneCredential &credentialNew);

    signals:
      void pong();
      void connectionInfoReceived();

      void authorizationError(const P1::RestApi::ProtocolOneCredential &credential);
      void credentialUpdated(const P1::RestApi::ProtocolOneCredential &credentialOld,
        const P1::RestApi::ProtocolOneCredential &credentialNew);

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

      QSet<QString> _lockedServices;
      QTimer _timeoutTimer;
      int _maxTimeoutFail;
      int _timeoutFail;
    };

  }
}
