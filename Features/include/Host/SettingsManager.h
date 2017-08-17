#pragma once

#include <QtCore/QObject>
#include <QtCore/QString>

namespace GameNet {
  namespace Host {

    class SettingsManager : public QObject
    {
    public:
      explicit SettingsManager(QObject * parent = 0);
      ~SettingsManager();

      virtual void setValue(const QString& path, const QString& key, const QString& data);
      virtual QString value(const QString& path, const QString& key, const QString& defValue);
    };

  }
}
