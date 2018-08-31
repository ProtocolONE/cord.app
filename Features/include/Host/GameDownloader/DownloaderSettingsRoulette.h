#pragma once
#include <QtCore/QString>

namespace P1 {
  namespace Host {
    namespace GameDownloader {

      class DownloaderSettingsRoulette
      {
      public:
        DownloaderSettingsRoulette() = default;
        ~DownloaderSettingsRoulette() = default;

        bool hasTestInfo() const;

        QString testName() const;
        QString groupName() const;

        /*!
        \fn void DownloaderSettingsRoulette::chooseYourDestiny() const;
        
        \brief  Choose and save downloader settings preset for current A/B test.
        
        \author "Ilya Tkachenko"
        \date 07.06.2017
         */
        void chooseYourDestiny();

      private:
        void saveTestGroup(const QString& name, const QString& group);

        void setGroupA();
        void setGroupB();
        void setGroupC();
        void setGroupD();
      };

    }
  }
}