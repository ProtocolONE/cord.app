#pragma once

#include <QtCore/QObject>
#include <QtCore/QString>

namespace Features {
  namespace Marketing {

    class AbMarketing : public QObject
    {
    public:
      explicit AbMarketing(QObject *parent = 0);
      ~AbMarketing();

      QString getTestValue();

    private:
      void dropCoin();
      bool isValueSet();
      void saveResult();

      QString _testValue;
    };

  }
}
