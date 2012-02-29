#include <Features/RemouteStartGame.h>

namespace Features {
  RemouteStartGame::RemouteStartGame(QObject *parent)
    : QObject(parent)
  {
  }

  RemouteStartGame::~RemouteStartGame()
  {
  }

  void RemouteStartGame::commandRecieved(QString name, QStringList arguments)
  {
    if (name != "startservice")
      return;

    if (arguments.size() < 1)
      return;

    emit this->startGameRequest(arguments.at(0));
  }

}
