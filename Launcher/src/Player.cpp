#include <Player.h>
#include <QDebug>

Player::Player(QObject *parent) 
  : QObject(parent)
  , _autoPlay(true)   
{

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
  switch(state) {
  case QMediaPlayer::PlayingState:
    emit this->playing();
    break;
  case QMediaPlayer::StoppedState:
    emit this->stopped();
    break;
  case QMediaPlayer::PausedState:
    emit this->paused();
    break;
  }
}

QString Player::source() const
{
  return this->_source;
}

void Player::setSource(const QString &value)
{
  if (value != this->_source) {
    this->_media->setMedia(QUrl(value));

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
  return this->_media->volume();
}

void Player::setVolume(qreal value)
{
  emit this->_media->setVolume(value);
  emit this->volumeChanged();
}
