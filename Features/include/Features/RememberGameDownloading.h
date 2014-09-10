#pragma once

#include <QtCore/QObject>
#include <QtCore/QString>

namespace Features {

  class RememberGameDownloading : public QObject
  {
    Q_OBJECT

  public:
    RememberGameDownloading(QObject *parent = 0);
    virtual ~RememberGameDownloading();

  public slots:
    void started(const QString &serviceId);
    void finished(const QString &serviceId);
    void update();       

  signals:
    void startGameRequest(QString serviceId);
  };
}
