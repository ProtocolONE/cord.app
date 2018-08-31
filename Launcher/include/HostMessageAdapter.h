#pragma once

#include <QtCore/QObject>
#include <QtCore/QString>

class MessageAdapterBridgeProxy;

class HostMessageAdapter : public QObject
{
  Q_OBJECT
public:
  explicit HostMessageAdapter(QObject *parent = 0);
  virtual ~HostMessageAdapter();

private:
  MessageAdapterBridgeProxy *_messageAdapter;

  void onMessageRequest(
    int messageId,
    const QString& title,
    const QString& text,
    int buttons,
    int icon);
};

