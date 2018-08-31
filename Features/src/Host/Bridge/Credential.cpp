#include <Host/Bridge/Credential.h>

QDBusArgument& operator<<(QDBusArgument &argument, const P1::Host::Bridge::Credential arg)
{
  argument.beginStructure();
  argument 
    << arg.userId
    << arg.appKey
    << arg.cookie;
  argument.endStructure();
  return argument;
}

const QDBusArgument& operator>>(const QDBusArgument &argument, P1::Host::Bridge::Credential& arg)
{
  argument.beginStructure();
  argument 
    >> arg.userId
    >> arg.appKey
    >> arg.cookie;
  argument.endStructure();
  return argument;
}
