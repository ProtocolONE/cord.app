#include <Settings/Settings.h>

#include <Features/Marketing/AbMarketing.h>

#include <random>
#include <chrono>

namespace Features {
  namespace Marketing {

    AbMarketing::AbMarketing(QObject *parent /*= 0*/)
    {
      if (!this->isValueSet()) {
        this->dropCoin();
        this->saveResult();
      }
    }

    AbMarketing::~AbMarketing()
    {

    }

    QString AbMarketing::getTestValue()
    {
      return this->_testValue;
    }

    void AbMarketing::dropCoin()
    {
      // Generate random (0,1)
      // Drop coin
      unsigned seed = std::chrono::system_clock::now().time_since_epoch().count();
      std::mt19937_64 generator(seed);
      std::uniform_int_distribution<int> distribution(0, 1);

      this->_testValue = distribution(generator) == 1 ? "A" : "B";
    }

    bool AbMarketing::isValueSet()
    {
      GGS::Settings::Settings settings;
      bool abInitFlag = true;

      settings.beginGroup("qGNA");
      settings.beginGroup("settings");
      settings.beginGroup("ABTestGroup");
      this->_testValue = settings.value("QGNA1681", QString()).toString();

      if (this->_testValue.isEmpty())
        abInitFlag = false;

      return abInitFlag;
    }

    void AbMarketing::saveResult()
    {
      GGS::Settings::Settings settings;

      // Remember value
      settings.beginGroup("qGNA");
      settings.beginGroup("settings");
      settings.beginGroup("ABTestGroup");
      settings.setValue("QGNA1681", this->_testValue);
    }

  }
}
