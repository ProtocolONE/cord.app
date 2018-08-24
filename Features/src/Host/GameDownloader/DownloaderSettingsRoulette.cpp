#include <Host/GameDownloader/DownloaderSettingsRoulette.h>

#include <Settings/Settings.h>

#include <Helper/DebugLog.h>

#include <chrono>
#include <random>
#include <cstdint>

using P1::Settings::Settings;

namespace GameNet {
  namespace Host {
    namespace GameDownloader {

      enum class TorrentProfile : uint32_t {
        DEFAULT_PROFILE = 0,
        HIGH_PERFORMANCE_SEED = 1,
        MIN_MEMORY_USAGE = 2
      };

      void setDownloadProfile(TorrentProfile value)
      {
        P1::Settings::Settings settings;
        settings.setValue("qGNA/torrentProfile", QString::number(static_cast<uint32_t>(value)));
      }

      void setMaxConnection(uint32_t value)
      {
        // INFO we shouldn't set less than 10 max connection.
        if (value < 10)
          value = 10;

        P1::Settings::Settings settings;
        settings.setValue("qGNA/numConnections", QString::number(value));
      }

      bool DownloaderSettingsRoulette::hasTestInfo() const
      {
        return !this->testName().isEmpty() && !this->groupName().isEmpty();
      }

      QString DownloaderSettingsRoulette::testName() const
      {
        Settings settings;
        settings.beginGroup("qGNA");
        settings.beginGroup("Features");
        settings.beginGroup("DownloaderSettingsRoulette");
        return settings.value("TestName", QString()).toString();
      }

      QString DownloaderSettingsRoulette::groupName() const
      {
        Settings settings;
        settings.beginGroup("qGNA");
        settings.beginGroup("Features");
        settings.beginGroup("DownloaderSettingsRoulette");
        return settings.value("GroupName", QString()).toString();
      }

      void DownloaderSettingsRoulette::chooseYourDestiny()
      {
        if (this->hasTestInfo()) {
          WARNING_LOG << "Test already has been initialized. TestName"
            << this->testName() << "GroupName" << this->groupName();
          return;
        }

        uint64_t seed = std::chrono::system_clock::now().time_since_epoch().count();
        std::default_random_engine engine(seed);
        std::uniform_int_distribution<uint32_t> distribution(0, 99);

        uint32_t roll = distribution(engine);

        if (roll < 40)
          this->setGroupA();
        else if (roll < 60)
          this->setGroupB();
        else if (roll < 80)
          this->setGroupC();
        else if (roll < 100)
          this->setGroupD();
      }

      void DownloaderSettingsRoulette::saveTestGroup(const QString& name, const QString& group)
      {
        Settings settings;
        settings.beginGroup("qGNA");
        settings.beginGroup("Features");
        settings.beginGroup("DownloaderSettingsRoulette");
        settings.setValue("TestName", name);
        settings.setValue("GroupName", group);

        DEBUG_LOG << "Downloader settings TestName" << name << "GroupName" << group;
      }

      void DownloaderSettingsRoulette::setGroupA()
      {
        // Default settings: HIGH_PERFORMANCE_SEED and 200 connection
        setDownloadProfile(TorrentProfile::HIGH_PERFORMANCE_SEED);
        setMaxConnection(200);
        this->saveTestGroup("QGNA1667", "A");
      }

      void DownloaderSettingsRoulette::setGroupB()
      {
        // MIN_MEMORY_USAGE and 200 connection
        setDownloadProfile(TorrentProfile::MIN_MEMORY_USAGE);
        setMaxConnection(200);
        this->saveTestGroup("QGNA1667", "B");
      }

      void DownloaderSettingsRoulette::setGroupC()
      {
        // HIGH_PERFORMANCE_SEED and 20 connection
        setDownloadProfile(TorrentProfile::HIGH_PERFORMANCE_SEED);
        setMaxConnection(20);
        this->saveTestGroup("QGNA1667", "C");
      }

      void DownloaderSettingsRoulette::setGroupD()
      {
        // MIN_MEMORY_USAGE and 20 connection
        setDownloadProfile(TorrentProfile::MIN_MEMORY_USAGE);
        setMaxConnection(20);
        this->saveTestGroup("QGNA1667", "D");
      }

    }
  }
}
