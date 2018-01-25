#pragma once
#include <QtCore/QObject>
#include <QtCore/QString>
#include <QtCore/QStringList>

namespace GameNet {
  namespace Host {
    class LicenseManager : public QObject
    {
      Q_OBJECT
    public:
      explicit LicenseManager(QObject *parent = 0);
      virtual ~LicenseManager();

      void setServices(const QStringList &value);

      virtual bool hasAcceptedLicense();
      virtual bool hasAcceptedLicense(const QString &serviceId);
      virtual void acceptLicense(const QString &serviceId, const QString &hash);
      
      virtual void acceptWebLicense();
      virtual bool hasAcceptedWebLicense();

    private:
      QStringList _services;
    };
  }
}


