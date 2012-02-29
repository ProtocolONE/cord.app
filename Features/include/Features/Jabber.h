#ifndef _GGS_FEATURES_JABBER_H_
#define _GGS_FEATURES_JABBER_H_

#include <QObject>
#include <QtCore/QString>

class QXmppClient;

namespace Features {

  class Jabber : public QObject
  {
    Q_OBJECT
  public:
    Jabber(QObject *parent = 0);
    ~Jabber();

  public slots:
    void authSuccess(const QString& userId, const QString& appKey);
    void logout();
  
  private:
    QXmppClient *_client;
  };
}

#endif // _GGS_FEATURES_JABBER_H_
