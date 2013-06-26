#pragma once

#include <QDeclarativeItem>
#include <QtCore/QScopedPointer>
#include <Phonon>
#include <Phonon/MediaObject>
#include <Phonon/AudioOutput>

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
  void stateChanged(Phonon::State newstate, Phonon::State oldstate);

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
  Phonon::MediaObject *_media;
  Phonon::AudioOutput *_output;
  QString _source;
  bool _autoPlay;
};