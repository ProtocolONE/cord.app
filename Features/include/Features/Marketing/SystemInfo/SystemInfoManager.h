#pragma once

#include <RestApi/GameNetCredential.h>

#include <QtCore/QObject>
#include <QtCore/QString>
#include <QtCore/QXmlStreamWriter>
#include <QtCore/QDateTime>
#include <QtCore/QTimer>

namespace Features {
  namespace Marketing {
    namespace SystemInfo {

      class SystemInfoManager : public QObject
      {
        Q_OBJECT
      public:
        explicit SystemInfoManager(QObject *parent = 0);
        virtual ~SystemInfoManager();

        void init();
        void setCredential(const P1::RestApi::GameNetCredential& value);
        void setMid(const QString& value);

      private slots:
        void send();

      private:
        QString getSystemInfo(const QString& userId);

        void writeSoftwareVendors(QXmlStreamWriter &writer);
        void writeSystemInfo(QXmlStreamWriter &writer);

        bool canSendStatistic();
        void saveSendDate();
        QDateTime loadSendDate();

        P1::RestApi::GameNetCredential _credential;
        QString _mid;
        QTimer _timer;
      };
    }
  }
}