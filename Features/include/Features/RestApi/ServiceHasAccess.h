#pragma once 

#include <QObject>
#include <RestApi/CommandBase.h>

class ServiceHasAccess : public GGS::RestApi::CommandBase
{
  Q_OBJECT
public:
    explicit ServiceHasAccess(QObject *parent = 0);
    virtual ~ServiceHasAccess();

    void setServiceId(const QString &serviceId);
    virtual bool callMethod(const QDomDocument& response);

    int hasAccess() const;
private:
    int _hasAccess;

};