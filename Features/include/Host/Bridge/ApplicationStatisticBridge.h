#pragma once

#include <QtCore/QObject>

namespace GameNet {
  namespace Host {

    class ApplicationStatistic;

    namespace Bridge {

      class ApplicationStatisticBridge : public QObject
      {
        Q_OBJECT
        Q_CLASSINFO("Version", "1.0.0.0")
        Q_CLASSINFO("D-Bus Interface", "com.gamenet.dbus.ApplicationStatistic")
      public:
        explicit ApplicationStatisticBridge(QObject *parent = 0);
        virtual ~ApplicationStatisticBridge();

        void setApplicationStatistic(ApplicationStatistic *value);

      public slots:
        bool isGameInstalled(const QString& serviceId) const;
        quint64 gameInstallDate(const QString& serviceId) const;
        quint64 gameLastExecutionTime(const QString& serviceId) const;

        int executeGameSuccessCount(const QString& serviceId) const;
        int executeGameFailedCount(const QString& serviceId) const;
        int executeGameTotalCount(const QString& serviceId) const;

        QString installWithService() const;
        quint64 installDate() const;

      private:
        ApplicationStatistic *_applicationStatistic;
      };

    }
  }
}
