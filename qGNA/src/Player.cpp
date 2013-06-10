#include "Player.h"
#include <QDebug>

Player::Player(QDeclarativeItem *parent) 
  : QDeclarativeItem(parent),
    _autoPlay(true),
    _media(new Phonon::MediaObject(this)),
    _output(new Phonon::AudioOutput(Phonon::MusicCategory, this))
{
  Phonon::createPath(this->_media, this->_output);
  QObject::connect(this->_media, SIGNAL(finished()), this, SIGNAL(finished()));
  QObject::connect(this->_media, SIGNAL(stateChanged(Phonon::State, Phonon::State)),
    this, SLOT(stateChanged(Phonon::State, Phonon::State)));
}

Player::~Player()
{
}

void Player::stateChanged(Phonon::State newstate, Phonon::State oldstate)
{ 
  switch(newstate) {
  case Phonon::PlayingState:
    emit this->playing();
    break;
  case Phonon::StoppedState:
    emit this->stopped();
    break;
  case Phonon::PausedState:
    emit this->paused();
    break;
  case Phonon::ErrorState:
    qDebug() << this->_media->errorString();
    emit this->error();
    break;
  }
}

QString Player::source() const
{
  return this->_source;
}

void Player::setSource(QString value)
{
  if (value != this->_source) {
    this->_media->setCurrentSource(Phonon::MediaSource(value));

    this->_source = value;
    emit this->sourceChanged();

    if (this->_autoPlay)
      this->play();
  }
}

void Player::play()
{
  this->_media->play();
}

void Player::stop()
{
  this->_media->stop();
}

void Player::pause()
{
  this->_media->pause();
}

bool Player::autoPlay() const
{
  return this->_autoPlay;
}

void Player::setAutoPlay(bool value)
{
  this->_autoPlay = value;
}

qreal Player::volume() const
{
  return this->_output->volume();
}

void Player::setVolume(qreal value)
{
  this->_output->setVolume(value);
  emit this->volumeChanged();
}
