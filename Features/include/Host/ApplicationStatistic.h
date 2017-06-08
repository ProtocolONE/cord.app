#pragma once

#include <QtCore/QObject>
#include <QtCore/QString>

namespace GGS {
  namespace GameDownloader {
    class GameDownloadService;
  }
}

namespace GameNet {
  namespace Host {

    class ApplicationStatistic : public QObject
    {
      Q_OBJECT
    public:
      explicit ApplicationStatistic(QObject *parent = 0);
      virtual ~ApplicationStatistic();

      void setDownloader(GGS::GameDownloader::GameDownloadService *value);
      void setStartingGame(const QString& serviceId);

      void init();
      void applcationStarted();

    public slots:

      /**
       * \fn  virtual bool ApplicationStatistic::isGameInstalled(const QString& serviceId) const;
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

      virtual bool isGameInstalled(const QString& serviceId) const;

      /**
       * \fn  virtual quint64 ApplicationStatistic::gameInstallDate(const QString& serviceId) const;
       *
       * \brief  Game install date as the number of milliseconds that have passed since 1970-01-01T00:00:00.000.
       *
       * \author  Ilya Tkachenko
       * \date  01.10.2014
       *
       * \param serviceId Identifier for the service.
       *
       * \return  Game install date.
       */

      virtual quint64 gameInstallDate(const QString& serviceId) const;

      /**
       * \fn  virtual quint64 ApplicationStatistic::gameLastExecutionTime(const QString& serviceId) const;
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

      virtual quint64 gameLastExecutionTime(const QString& serviceId) const;

      /**
       * \fn  virtual int ApplicationStatistic::executeGameSuccessCount(const QString& serviceId) const;
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

      virtual int executeGameSuccessCount(const QString& serviceId) const;

      /**
       * \fn  virtual int ApplicationStatistic::executeGameFailedCount(const QString& serviceId) const;
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

      virtual int executeGameFailedCount(const QString& serviceId) const;

      /**
       * \fn  virtual int ApplicationStatistic::executeGameTotalCount(const QString& serviceId) const;
       *
       * \brief Total number of launches game.
       *
       * \author  Ilya Tkachenko
       * \date  01.10.2014
       *
       * \param serviceId Identifier for the service.
       *
       * \return identifier for the game.
       */

      virtual int executeGameTotalCount(const QString& serviceId) const;

      /**
       * \fn  virtual QString ApplicationStatistic::installWithService() const;
       *
       * \brief GameNet application installed the with game 'serviceId'.
       *
       * \author  Ilya Tkachenko
       * \date  01.10.2014
       *
       * \return identifier for the game.
       */

      virtual QString installWithService() const;

      /**
       * \fn  virtual quint64 ApplicationStatistic::installDate() const;
       *
       * \brief Query GameNet application install date as the number of seconds that 
       *        have passed since 1970-01-01T00:00:00.000. 
       *
       * \author  Ilya Tkachenko
       * \date  01.10.2014
       *
       * \return GameNet application install date.
       */
      virtual quint64 installDate() const;

      /*!
      \fn virtual bool ApplicationStatistic::isFirstStart() const;
      
      \brief  Query if application first time launched.
      
      \author "Ilya Tkachenko"
      \date 07.06.2017
      
      \return true if first launch, false if not.
       */

      virtual bool isFirstStart() const;

    private:
      void setInstallDate();
      bool hasStartingService();
      
      QString _startingServiceId;
      GGS::GameDownloader::GameDownloadService *_downloader = nullptr;
      bool _firstStart = false;
    };

  }
}