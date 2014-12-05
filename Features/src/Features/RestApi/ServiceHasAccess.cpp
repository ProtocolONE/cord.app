#include <Features/RestApi/ServiceHasAccess.h>
#include <QtXml/QDomDocument>

ServiceHasAccess::ServiceHasAccess(QObject *parent)
  : CommandBase(parent)
  , _hasAccess(0)
{
  this->appendParameter("method", "service.hasAccess");
  this->appendParameter("version", "1");
  this->appendParameter("lang", "ru");
  this->setAuthRequire(true);
}

ServiceHasAccess::~ServiceHasAccess()
{
}

void ServiceHasAccess::setServiceId(const QString &serviceId)
{
  this->appendParameter("serviceId", serviceId);
}

int ServiceHasAccess::hasAccess() const
{
  return this->_hasAccess;
}

bool ServiceHasAccess::callMethod(const QDomDocument& response)
{
  QDomElement responseElement = response.documentElement();

  if(responseElement.isNull())
    return true;

  QDomElement el = responseElement.firstChildElement("result");
  if(!el.isNull()) {
    this->_hasAccess = el.text().toInt();
    return false;
  }

  return true;
}
