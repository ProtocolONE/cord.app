#pragma once


#include <QtCore/QObject>
#include <QtCore/QString>
#include <QtCore/QStringList>
namespace Features {

  class RemouteStartGame : public QObject
  {
    Q_OBJECT
  public:
    RemouteStartGame(QObject *parent = 0);
    ~RemouteStartGame();

  public slots:
    void commandRecieved(QString name, QStringList arguments);

  signals:
    void startGameRequest(QString serviceId);
  };

}
