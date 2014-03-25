#ifndef LICENSEVIEWMODEL_H
#define LICENSEVIEWMODEL_H

#include <QObject>
#include <Core/Service>

class LicenseViewModel : public QObject
{
  Q_OBJECT
  Q_PROPERTY(bool shurtCutInDesktop READ shurtCutInDesktop WRITE setShurtCutInDesktop NOTIFY shurtCutInDesktopChanged)
  Q_PROPERTY(bool licenseAccepted READ licenseAccepted WRITE setLicenseAccepted NOTIFY licenseAcceptedChanged)
  Q_PROPERTY(bool shurtCutInStart READ shurtCutInDesktop WRITE setShurtCutInStart NOTIFY shurtCutInStartChanged)
  Q_PROPERTY(QString pathToInstall READ pathToInstall WRITE setPathToInstall NOTIFY pathToInstallChanged)
  Q_PROPERTY(QString license READ license WRITE setLicense NOTIFY licenseChanged)
public:
    LicenseViewModel(QObject *parent); 
    ~LicenseViewModel();

    const QString& hash() const { return this->_hash; }

public slots:
    QString serviceId() const { return this->_serviceId; }
    const bool shurtCutInDesktop() const { return this->_isShurtCutInDesktop; }
    const bool licenseAccepted() const { return this->_isLicenseAccepted; }
    const bool shurtCutInStart() const { return this->_isShurtCutInStart; }  
    const QString& pathToInstall() const { return this->_pathToInstall; } 
    const QString& license() const { return this->_license; }

    void setShurtCutInDesktop(const bool shurtCutInDesktop);
    void setLicenseAccepted(const bool licenseAccepted);
    void setShurtCutInStart(const bool shurtCutInStart); 
    void setPathToInstall(const QString& pathToInstall);
    void setLicense(const QString& license);

    void okPressed();
    void searchPressed();
    void openLicense(GGS::Core::Service* service, const QString& hash);
    void closeLicense();

private:
  bool _isShurtCutInDesktop;
  bool _isLicenseAccepted; 
  bool _isShurtCutInStart;
  QString _pathToInstall;
  QString _license;

  QString _serviceId;
  QString _hash;

  GGS::Core::Service* _service;

signals:
  void shurtCutInDesktopChanged();
  void licenseAcceptedChanged();
  void shurtCutInStartChanged();
  void pathToInstallChanged(); 
  void licenseChanged();

  void result();
  void openLicenseBlock();
  void closeLicenseBlock();
};

#endif // LICENSEVIEWMODEL_H
