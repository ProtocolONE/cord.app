#pragma once

#include <QtCore/QObject>

namespace P1 {
  namespace Host {

    class ApplicationStatistic;

    namespace Bridge {

      class ApplicationStatisticBridge : public QObject
      {
        Q_OBJECT
        Q_CLASSINFO("Version", "1.0.0.0")
        Q_CLASSINFO("D-Bus Interface", "com.protocolone.launcher.dbus.ApplicationStatistic")
      public:
        explicit ApplicationStatisticBridge(QObject *parent = 0);
        virtual ~ApplicationStatisticBridge();

        /// \cond
        void setApplicationStatistic(ApplicationStatistic *value);
        /// \endcond

      public slots:

        /**
         * \fn  bool ApplicationStatisticBridge::isGameInstalled(const QString& serviceId) const;
         *
         * \brief Query if Game with 'serviceId' is installed.
         *
         * \author  Ilya Tkachenko
         * \date  01.10.2014
         *
         * \param serviceId Identifier for the service.
         *
         * \return  true if game installed, false if not.
         */
        bool isGameInstalled(const QString& serviceId) const;

        /**
         * \fn  quint64 ApplicationStatisticBridge::gameInstallDate(const QString& serviceId) const;
         *
         * \brief Game install date as the number of milliseconds that have passed since 1970-01-01T00:00:00.000.
         *
         * \author  Ilya Tkachenko
         * \date  01.10.2014
         *
         * \param serviceId Identifier for the service.
         *
         * \return Game install date.
         */

        quint64 gameInstallDate(const QString& serviceId) const;

        /**
         * \fn  quint64 ApplicationStatisticBridge::gameLastExecutionTime(const QString& serviceId) const;
         *
         * \brief Game last execution time as the number of milliseconds that 
         *        have passed since 1970-01-01T00:00:00.000.
         *
         * \author  Ilya Tkachenko
         * \date  01.10.2014
         *
         * \param serviceId Identifier for the service.
         *
         * \return Game last execution time.
         */

        quint64 gameLastExecutionTime(const QString& serviceId) const;

        /**
         * \fn  int ApplicationStatisticBridge::executeGameSuccessCount(const QString& serviceId) const;
         *
         * \brief The number of successful launches of game.
         *
         * \author  Ilya Tkachenko
         * \date  01.10.2014
         *
         * \param serviceId Identifier for the service.
         *
         * \return number of successful launches of game.
         */

        int executeGameSuccessCount(const QString& serviceId) const;

        /**
         * \fn  int ApplicationStatisticBridge::executeGameFailedCount(const QString& serviceId) const;
         *
         * \brief The number of unsuccessful launches of game.
         *
         * \author  Ilya Tkachenko
         * \date  01.10.2014
         *
         * \param serviceId Identifier for the service.
         *
         * \return number of unsuccessful launches of game.
         */

        int executeGameFailedCount(const QString& serviceId) const;

        /**
         * \fn  int ApplicationStatisticBridge::executeGameTotalCount(const QString& serviceId) const;
         *
         * \brief Total number of launches game.
         *
         * \author  Ilya Tkachenko
         * \date  01.10.2014
         *
         * \param serviceId Identifier for the service.
         *
         * \return total number of launches game.
         */

        int executeGameTotalCount(const QString& serviceId) const;

        /**
         * \fn  QString ApplicationStatisticBridge::installWithService() const;
         *
         * \brief ProtocolOne Launcher application installed the with game 'serviceId'.
         *
         * \author  Ilya Tkachenko
         * \date  01.10.2014
         *
         * \return identifier for the game.
         */

        QString installWithService() const;

        /**
         * \fn  quint64 ApplicationStatisticBridge::installDate() const;
         *
         * \brief Query ProtocolOne Launcher application install date as the number of seconds that 
         *        have passed since 1970-01-01T00:00:00.000. 
         *
         * \author  Ilya Tkachenko
         * \date  01.10.2014
         *
         * \return ProtocolOne Launcher application install date.
         */

        quint64 installDate() const;

        /*!
        \fn virtual bool ApplicationStatistic::isFirstStart() const;

        \brief  Query if application first time launched.

        \author "Ilya Tkachenko"
        \date 07.06.2017

        \return true if first launch, false if not.
        */

        bool isFirstStart() const;

      private:
        ApplicationStatistic *_applicationStatistic;
      };

    }
  }
}
