#pragma once

#include <QtDeclarative/QDeclarativeItem>

#include <QtMultimedia/QAudioOutput>
#include <QtMultimedia/QMediaPlayer>

class Player: public QDeclarativeItem
{
  Q_OBJECT
  Q_PROPERTY(QString source READ source WRITE setSource NOTIFY sourceChanged) 
  Q_PROPERTY(qreal volume READ volume WRITE setVolume NOTIFY volumeChanged) 
  Q_PROPERTY(bool autoPlay READ autoPlay WRITE setAutoPlay NOTIFY autoPlayChanged) 

public:
  explicit Player(QDeclarativeItem *parent = 0);
  virtual ~Player();

  QString source() const;
  void setSource(const QString &value);
  bool autoPlay() const;
  void setAutoPlay(bool value);
  qreal volume() const;
  void setVolume(qreal value);

public slots:
  void play();
  void pause();
  void stop();

private slots:
  void stateChanged(QMediaPlayer::State state);
  void mediaStatusChanged(QMediaPlayer::MediaStatus status);

signals:
  void sourceChanged();
  void autoPlayChanged();
  void volumeChanged();
  void playing();
  void error();
  void stopped();
  void finished();
  void paused();

private:
  QMediaPlayer *_media;
  QString _source;
  bool _autoPlay;
};