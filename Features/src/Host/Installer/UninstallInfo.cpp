#include <Host/Installer/UninstallInfo.h>

#include <QtCore/QCoreApplication>
#include <QtCore/QDir>
#include <QtCore/QFile>
#include <QtCore/QSettings>

namespace GameNet {
  namespace Host {
    namespace Installer {

      //  INFO: разделы в Uninstall для каждой игры именуются 
      //  {GUID}.serviceId, где
      //    GUID = 4BD65630-3A19-4fc2-8AD8-2CF729DB6608
      static const QString gameNetUninstallerGUID = "{4BD65630-3A19-4fc2-8AD8-2CF729DB6608}";

      UninstallInfo::UninstallInfo(const QString &serviceId)
        : _serviceId(serviceId)
        , _displayVersion("1.0")
        , _helpLink("https://support.gamenet.ru")
        , _majorVersion(1)
        , _minorVersion(0)
        , _noModify(1)
        , _noRepair(1)
        , _publisher("Global Gamers Solutions Ltd. ©")
        , _urlInfoAbout("https://gamenet.ru/")
        , _urlUpdateInfo("https://gamenet.ru/")
      {

      }      

      UninstallInfo::~UninstallInfo()
      {

      }

      void UninstallInfo::save()
      {
        if (this->_serviceId.isEmpty())
          return;

        QString keyName = gameNetUninstallerGUID + "." + this->_serviceId;

        QString applicationPath = QString("%1\\%2").arg(QDir::toNativeSeparators(QCoreApplication::applicationDirPath()))
          .arg("Uninstaller.exe");

        QString iconPath = QString("%1\\Assets\\Images\\icons\\%2.ico")
          .arg(applicationPath)
          .arg(this->_serviceId);
        iconPath = QFile::exists(iconPath) ? iconPath : QDir::toNativeSeparators(QCoreApplication::applicationFilePath());

        QString uninstallString = QString("\"%1\" /uninstall %2")
          .arg(applicationPath)
          .arg(this->_serviceId);

        QSettings settings("HKEY_LOCAL_MACHINE\\Software\\Microsoft\\Windows\\CurrentVersion\\Uninstall", QSettings::NativeFormat);
        settings.beginGroup(keyName);
        
        settings.setValue("DisplayIcon", iconPath);
        
        if (!this->_displayName.isEmpty())
          settings.setValue("DisplayName", this->_displayName);
        if (!this->_displayVersion.isEmpty())
          settings.setValue("DisplayVersion", this->_displayVersion);
        // EstimatedSize
        if (!this->_helpLink.isEmpty())
          settings.setValue("HelpLink", this->_helpLink);

        if (!this->_installDate.isEmpty())
          settings.setValue("InstallDate", this->_installDate);
        // InstallLocation
        settings.setValue("MajorVersion", this->_majorVersion);
        settings.setValue("MinorVersion", this->_minorVersion);
        settings.setValue("NoModify", this->_noModify);
        settings.setValue("NoRepair", this->_noRepair);
        if (!this->_publisher.isEmpty())
          settings.setValue("Publisher", this->_publisher);
        // INFO: QuietUninstallString not implemented
        settings.setValue("UninstallString", uninstallString);
        if (!this->_urlInfoAbout.isEmpty())
          settings.setValue("URLInfoAbout", this->_urlInfoAbout);
        if (!this->_urlUpdateInfo.isEmpty())
            settings.setValue("URLUpdateInfo", this->_urlUpdateInfo);
        settings.endGroup();
      }
      
      void UninstallInfo::remove()
      {
        if (this->_serviceId.isEmpty())
          return;

        QString keyName = gameNetUninstallerGUID + "." + this->_serviceId;
        QSettings uninstallSection("HKEY_LOCAL_MACHINE\\Software\\Microsoft\\Windows\\CurrentVersion\\Uninstall", QSettings::NativeFormat);

        uninstallSection.beginGroup(keyName);
        uninstallSection.remove("");
        uninstallSection.endGroup();
        
      }

      void UninstallInfo::setDisplayName(const QString &value)
      {
        this->_displayName = value;
      }

      QString UninstallInfo::displayName() const
      {
        return this->_displayName;
      }

      void UninstallInfo::setDisplayVersion(const QString &value)
      {
        this->_displayVersion = value;
      }

      QString UninstallInfo::displayVersion() const
      {
        return this->_displayVersion;
      }

      void UninstallInfo::setHelpLink(const QString &value)
      {
        this->_helpLink = value;
      }

      QString UninstallInfo::helpLink() const
      {
        return this->_helpLink;
      }

      void UninstallInfo::setInstallDate(const QString &value)

      {
        this->_installDate = value;
      }

      QString UninstallInfo::installDate() const
      {
        return this->_installDate;
      }

      void UninstallInfo::setMajorVersion(int value)
      {
        this->_majorVersion = value;
      }

      int UninstallInfo::majorVersion() const
      {
        return this->_majorVersion;
      }

      void UninstallInfo::setMinorVersion(int value)
      {
        this->_minorVersion = value;
      }

      int UninstallInfo::minorVersion() const
      {
        return this->_minorVersion;
      }

      void UninstallInfo::setNoModify(int value)
      {
        this->_noModify = value;
      }

      int UninstallInfo::noModify() const
      {
        return this->_noModify;
      }

      void UninstallInfo::setNoRepair(int value)
      {
        this->_noRepair = value;
      }
      
      int UninstallInfo::noRepair() const
      {
        return this->_noRepair;
      }

      void UninstallInfo::setPublisher(const QString &value)
      {
        this->_publisher = value;
      }

      QString UninstallInfo::publisher() const
      {
        return this->_publisher;
      }

      void UninstallInfo::setUrlInfoAbout(const QString &value)
      {
        this->_urlInfoAbout = value;
      }

      QString UninstallInfo::urlInfoAbout() const
      {
        return this->_urlInfoAbout;
      }

      void UninstallInfo::setUrlUpdateInfo(const QString &value)
      {
        this->_urlUpdateInfo = value;
      }

      QString UninstallInfo::urlUpdateInfo() const
      {
        return this->_urlUpdateInfo;
      }
     
    }
  }
}