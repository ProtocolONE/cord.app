#pragma once

#include <QtCore/QObject>

class ServiceSettingsBridgeProxy;

class BestInstallPath : public QObject
{
  Q_OBJECT
public:
  explicit BestInstallPath(QObject *parent = 0);
  virtual ~BestInstallPath();

  void setServiceSettings(ServiceSettingsBridgeProxy* value);

  /**
   * \fn  QString BestInstallPath::expectedPath(const QString& serviceId) const;
   *
   * \brief Возвращает путь на диске для установки игры. Выбирается лучший диск с максимальным
   *        доступным свободным местом.
   *
   * \author  Ilya Tkachenko
   * \date  19.09.2014
   *
   * \param serviceId Identifier for the service.
   *
   * \return Лучший путь для установки игры.
   */
  QString expectedPath(const QString& serviceId) const;

  QString bestInstallPath(const QString& serviceId) const;

private:
  QString getBestDrive(const QString& serviceId) const;
  int getDiskFreeSpaceInMb(const wchar_t* drive) const;
  bool hasEnoughSpace(const QString& serviceId, int free) const;

  ServiceSettingsBridgeProxy *_serviceSettings;
};

