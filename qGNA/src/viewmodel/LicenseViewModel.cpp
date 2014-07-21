#include "viewmodel/LicenseViewModel.h"
#include "Helper/GetDirectoryDialog.h"

#include <QtCore/QDebug>
#include <QtCore/QDir>
#include <QtWidgets/QFileDialog>
  
LicenseViewModel::LicenseViewModel(QObject *parent)
  : QObject(parent)
{
  this->_isShurtCutInDesktop = true; 
  this->_isShurtCutInStart = true;
  this->_isLicenseAccepted = false;
  this->_pathToInstall = QString(); 
}

LicenseViewModel::~LicenseViewModel()
{

}

void LicenseViewModel::openLicense(GGS::Core::Service* service, const QString& hash)
{
  this->_serviceId = service->id();
  this->_hash = hash;
  this->_service = service;
  emit this->openLicenseBlock(); 
}

void LicenseViewModel::closeLicense() { 
  emit this->closeLicenseBlock();
}

void LicenseViewModel::setShurtCutInDesktop(const bool shurtCutInDesktop)
{
  if (shurtCutInDesktop != this->_isShurtCutInDesktop) {
    this->_isShurtCutInDesktop = shurtCutInDesktop;
    emit this->shurtCutInDesktopChanged();
  }
  qDebug() << "[LicenseViewModel] shurtCutInDesktop." << shurtCutInDesktop;
}

void LicenseViewModel::setLicenseAccepted(const bool licenseAccepted)
{
  if (licenseAccepted != this->_isLicenseAccepted) {
    this->_isLicenseAccepted = licenseAccepted;
    emit this->licenseAcceptedChanged();
  }
  qDebug() << "[LicenseViewModel] licenseAccepted." << licenseAccepted;
}

void LicenseViewModel::setShurtCutInStart(const bool shurtCutInStart)
{
  if (shurtCutInStart != this->_isShurtCutInStart) {
    this->_isShurtCutInStart = shurtCutInStart;
    emit this->shurtCutInStartChanged();
  }
  qDebug() << "[LicenseViewModel] shurtCutInStart." << shurtCutInStart;
}

void LicenseViewModel::setPathToInstall(const QString& pathToInstall)
{
  if (pathToInstall != this->_pathToInstall) {
    this->_pathToInstall = pathToInstall;
    emit this->pathToInstallChanged();
  }
  qDebug() << "[LicenseViewModel] pathToInstall." << pathToInstall;
}

void LicenseViewModel::okPressed() {
  emit this->result();
  qDebug() << "[LicenseViewModel] okPressed.";
}

void LicenseViewModel::searchPressed()
{
  GetDirectoryDialog *dialog = new GetDirectoryDialog(qobject_cast<QWidget*>(this->parent()));
  dialog->getDirectory(this->_service->name(), this->_service->installPath());

  QObject::connect(dialog, &GetDirectoryDialog::directoryEntered, [dialog, this](const QString& newDirectory) {
    dialog->deleteLater();

    if (!newDirectory.isEmpty()) 
      this->setPathToInstall(newDirectory);
  });
}

void LicenseViewModel::setLicense(const QString& license)
{
  if (this->_license != license) {
    this->_license = license;
    emit this->licenseChanged();
  }
}
