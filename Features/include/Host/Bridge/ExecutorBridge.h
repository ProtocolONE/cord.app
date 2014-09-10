#pragma once

#include <QtCore/QObject>
#include <QtCore/QString>
#include <QtCore/QMetaType>
#include <QtDBus/QDBusArgument>

namespace GameNet {
  namespace Host {

    class GameExecutor;

    namespace Bridge {

      typedef struct {
        QString userId;
        QString appKey;
        QString cookie;
      } Credential;
      Q_DECLARE_METATYPE(Credential)

      class ExecutorBridge : public QObject
      {
        Q_OBJECT
        Q_CLASSINFO("Version", "1.0.0.0")
        Q_CLASSINFO("D-Bus Interface", "com.gamenet.dbus.Executor")

      public:
        explicit ExecutorBridge(QObject *parent = 0);
        virtual ~ExecutorBridge();

        void setExecutor(GameExecutor *value);
        
      public slots:

        /*
        <method name="execute">
          <arg name="serviceId" type="s" direction="in"/>
          <arg name="credential" type="a(sii)" direction="in"/>
          <annotation name="org.qtproject.QtDBus.QtTypeName.In1" value="GameNet::Host::Bridge::Credential"/>
        </method>
        */
        void execute(const QString& serviceId, const Credential& credential);
        
        /*
        <method name="executeSecond">
          <arg name="serviceId" type="s" direction="in"/>
          <arg name="credential" type="a(sii)" direction="in"/>
          <annotation name="org.qtproject.QtDBus.QtTypeName.In1" value="GameNet::Host::Bridge::Credential"/>
          <arg name="secondCredential" type="a(sii)" direction="in"/>
          <annotation name="org.qtproject.QtDBus.QtTypeName.In2" value="GameNet::Host::Bridge::Credential"/>
        </method>
        */
        void executeSecond(const QString& serviceId, const Credential& credential, const Credential& secondCredential);

        bool isGameStarted(const QString& serviceId) const;
        bool isAnyGameStarted() const;
        bool canExecuteSecond(const QString& serviceId) const;
        void shutdownSecond();

      signals:
        void serviceStarted(const QString& serviceId);
        void serviceFinished(const QString& serviceId, int finishState);
        void secondServiceStarted(const QString& serviceId);
        void secondServiceFinished(const QString& serviceId, int finishState);
        
      private:
        GameExecutor *_executor;
      };

    }
  }
}

static QDBusArgument& operator <<(QDBusArgument &argument, const GameNet::Host::Bridge::Credential arg)
{
  argument.beginStructure();
  argument 
    << arg.userId
    << arg.appKey
    << arg.cookie;
  argument.endStructure();
  return argument;
}

static const QDBusArgument& operator >>(const QDBusArgument &argument, GameNet::Host::Bridge::Credential& arg)
{
  argument.beginStructure();
  argument 
    >> arg.userId
    >> arg.appKey
    >> arg.cookie;
  argument.endStructure();
  return argument;
}

