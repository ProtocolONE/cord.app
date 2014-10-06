#include <Host/Bridge/Credential.h>

QDBusArgument& operator<<(QDBusArgument &argument, const GameNet::Host::Bridge::Credential arg)
{
  argument.beginStructure();
  argument 
    << arg.userId
    << arg.appKey
    << arg.cookie;
  argument.endStructure();
  return argument;
}

const QDBusArgument& operator>>(const QDBusArgument &argument, GameNet::Host::Bridge::Credential& arg)
{
  argument.beginStructure();
  argument 
    >> arg.userId
    >> arg.appKey
    >> arg.cookie;
  argument.endStructure();
  return argument;
}
