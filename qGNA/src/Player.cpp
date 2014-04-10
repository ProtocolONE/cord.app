#include <Player.h>
#include <QDebug>

#ifdef _DEBUG
//#define DISABLE_PHONONE 1
#endif
Player::Player(QDeclarativeItem *parent) 
  : QDeclarativeItem(parent),
    _autoPlay(true)   
{
#ifndef DISABLE_PHONONE
  this->_media = new Phonon::MediaObject(parent);
  this->_output = new Phonon::AudioOutput(Phonon::MusicCategory, parent);

  Phonon::createPath(this->_media, this->_output);
  QObject::connect(this->_media, SIGNAL(finished()), this, SIGNAL(finished()));
  QObject::connect(this->_media, SIGNAL(stateChanged(Phonon::State, Phonon::State)),
    this, SLOT(stateChanged(Phonon::State, Phonon::State)));
#endif
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
    emit this->error();
    break;
  }
}

QString Player::source() const
{
  return this->_source;
}

void Player::setSource(const QString &value)
{
#ifdef DISABLE_PHONONE
  return;
#endif

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
#ifdef DISABLE_PHONONE
  return 0;
#endif

  return this->_output->volume();
}

void Player::setVolume(qreal value)
{
#ifdef DISABLE_PHONONE
  return;
#endif

  this->_output->setVolume(value);
  emit this->volumeChanged();
}
