#include <Player.h>
#include <QDebug>

Player::Player(QObject *parent) 
  : QObject(parent)
  , _autoPlay(true)   
{
#ifndef DISABLE_PHONONE
  this->_media = new Phonon::MediaObject(parent);
  this->_output = new Phonon::AudioOutput(Phonon::MusicCategory, parent);

  this->_media = new QMediaPlayer(parent);

  QObject::connect(this->_media, &QMediaPlayer::stateChanged,
    this, &Player::stateChanged);

  QObject::connect(this->_media, SIGNAL(error(QMediaPlayer::Error)), this, SIGNAL(error()));

  QObject::connect(this->_media, &QMediaPlayer::mediaStatusChanged, 
    this, &Player::mediaStatusChanged);
}

Player::~Player()
{
}

void Player::mediaStatusChanged(QMediaPlayer::MediaStatus status)
{
  if (status == QMediaPlayer::EndOfMedia)
    emit this->finished();
}

void Player::stateChanged(QMediaPlayer::State state)
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
