#ifndef _GGS_FEATURES_REMEMBERGAMEDOWNLOADING_H_
#define _GGS_FEATURES_REMEMBERGAMEDOWNLOADING_H_

#include <Core/Service>

#include <QtCore/QObject>
#include <QtCore/QString>
#include <QtCore/QStringList>

namespace Features {

  class RememberGameDownloading : public QObject
  {
    Q_OBJECT

  public:
    RememberGameDownloading(QObject *parent = 0);
    virtual ~RememberGameDownloading();

  public slots:
    void started(const GGS::Core::Service *service);
    void finished(const GGS::Core::Service *service);
    void commandRecieved(QString name, QStringList arguments);       

  signals:
    void startGameRequest(QString serviceId);
  };
}
#endif // _GGS_FEATURES_REMEMBERGAMEDOWNLOADING_H_
