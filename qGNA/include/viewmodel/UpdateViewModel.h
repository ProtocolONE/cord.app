#pragma once

#include <QtCore/QObject>

class UpdateManagerBridgeProxy;
class UpdateViewModel : public QObject
{
  Q_OBJECT
  Q_PROPERTY(int updateState READ updateState)

public:
  explicit UpdateViewModel(QObject *parent = 0);
  virtual ~UpdateViewModel();

  public slots:
    int updateState();

  signals:
    void downloadUpdateProgress(quint64 currentSize, quint64 totalSize);
    void updatesFound();
    void noUpdatesFound();
    void downloadRetryNumber(int retryNumber);
    void allCompleted(bool isNeedRestart);
    void updateStateChanged(int updateState);
    void updateError(int errorCode);

private:
  UpdateManagerBridgeProxy* _updaterProxy;

};
