#pragma once

#include <QtCore/QString>

namespace GameNet {
  namespace Host {
    namespace Installer {
      
      class UninstallInfo
      {
      public:
        UninstallInfo(const QString &serviceId = QString());
        virtual ~UninstallInfo();

        void save();
        void remove();

        void setDisplayName(const QString &value);
        QString displayName() const;

        void setDisplayVersion(const QString &value);
        QString displayVersion() const;

        void setHelpLink(const QString &value);
        QString helpLink() const;

        void setInstallDate(const QString &value);
        QString installDate() const;

        void setMajorVersion(int value);
        int majorVersion() const;

        void setMinorVersion(int value);
        int minorVersion() const;

        void setNoModify(int value);
        int noModify() const;

        void setNoRepair(int value);
        int noRepair() const;
        
        void setPublisher(const QString &value);
        QString publisher() const;

        void setUrlInfoAbout(const QString &value);
        QString urlInfoAbout() const;

        void setUrlUpdateInfo(const QString &value);
        QString urlUpdateInfo() const;

      private:
        QString _serviceId;

        QString _displayName;
        QString _displayVersion;

        QString _helpLink;
        QString _installDate;

        int _majorVersion;
        int _minorVersion;

        int _noModify;
        int _noRepair;

        QString _publisher;
        QString _urlInfoAbout;
        QString _urlUpdateInfo;
      };

    }
  }
}