#pragma once
#include <QtCore/QObject>

class ApplicationStatisticBridgeProxy;

class ApplicationStatisticViewModel : public QObject
{
  Q_OBJECT
public:
  explicit ApplicationStatisticViewModel(QObject *parent = 0);
  virtual ~ApplicationStatisticViewModel();

public slots:
  bool isGameInstalled(const QString& serviceId) const;
  quint64 gameInstallDate(const QString& serviceId) const;
  quint64 gameLastExecutionTime(const QString& serviceId) const;

  int executeGameSuccessCount(const QString& serviceId) const;
  int executeGameFailedCount(const QString& serviceId) const;
  int executeGameTotalCount(const QString& serviceId) const;

  QString installWithService() const;
  quint64 installDate() const;

private:
  ApplicationStatisticBridgeProxy *_applicationStatisticProxy;
};

