#pragma once

#include <RestApi/GameNetCredential.h>

#include <QtCore/QObject>
#include <QtCore/QString>

namespace Features {
  namespace Marketing {
    class AbMarketing;
  }
}

namespace GGS {
  namespace Core {
    class Service;
  }
}

namespace GameNet {
  namespace Host {
    
    class MarketingStatistic : public QObject
    {
    public:
      explicit MarketingStatistic(QObject *parent = 0);
      virtual ~MarketingStatistic();

      void onServiceStarted(const QString& serviceId);
      void onServiceFinished(const QString& serviceId, int finishState);
      void onSecondServiceStarted(const QString& serviceId);
      void onSecondServiceFinished(const QString& serviceId, int finishState);

      void onGameDownloadStarted(const GGS::Core::Service *service);
      void onGameTorrentDownloadFinished(const GGS::Core::Service *service);
      void onGameDownloadFinished(const GGS::Core::Service *service);

    private:
      void setCredential(QVariantMap &params, const GGS::RestApi::GameNetCredential &credetial);
      Features::Marketing::AbMarketing* _abMarketing;
    };

  }
}
