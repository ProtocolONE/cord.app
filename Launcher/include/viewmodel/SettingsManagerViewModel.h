#pragma once

#include <QtCore/QObject>
#include <QString>

class SettingsBridgeProxy;
class SettingsManagerViewModel : public QObject
{
  Q_OBJECT
public:
  explicit SettingsManagerViewModel(QObject *parent = 0);
  virtual ~SettingsManagerViewModel();

public slots:
  void setValue(const QString& path, const QString& key, const QString& data);
  QString value(const QString& path, const QString& key, const QString& defValue);


private:
  SettingsBridgeProxy* _settingsProxy;
};