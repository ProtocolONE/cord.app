#pragma once

#include <RestApi/ProtocolOneCredential.h>

#include <QtCore/QObject>
#include <QtCore/QString>

namespace P1 {
  namespace Core {
    class Service;
  }
}

namespace P1 {
  namespace Host {
    
    class MarketingStatistic : public QObject
    {
    public:
      explicit MarketingStatistic(QObject *parent = 0);
      virtual ~MarketingStatistic();

      void onServiceStarted(const QString& serviceId);
      void onServiceFinished(const QString& serviceId, int finishState);

      void onGameDownloadStarted(const P1::Core::Service *service);
      void onGameTorrentDownloadFinished(const P1::Core::Service *service);
      void onGameDownloadFinished(const P1::Core::Service *service);

    private:
      void setCredential(QVariantMap &params, const P1::RestApi::ProtocolOneCredential &credetial);
    };

  }
}
