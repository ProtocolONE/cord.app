#pragma once

#include <Core/UI/Message.h>
#include <Core/UI/MessageAdapter.h>

#include <QSignalMapper>

using namespace P1::Core::UI;

class QmlMessageAdapter : public MessageAdapter
{
  Q_OBJECT
  Q_ENUMS(P1::Core::UI::Message::StandardButton)

public:
    QmlMessageAdapter(QObject *parent);
    ~QmlMessageAdapter();

public slots:
  void show(
    const QString& title,
    const QString& text,
    P1::Core::UI::Message::StandardButtons buttons,
    P1::Core::UI::Message::Icon icon,
    int messageId,
    QObject* reciever,
    const char* bindFunc);

  void buttonClicked(int messageId, int id);

  void callback(int messageId, int buttonId);

signals:
    // INFO Здесь передаем int buttons сразу в QML.
    // Потому что нет другой возможности получить доступ к унарным операциям внутри QML, если это не тип int
    void emitMessage(
      QString title,
      QString text,
      int buttons,
      P1::Core::UI::Message::Icon icon,
      int messageId);

private:
    QMap<int, QPair<QObject*, const char*>> _bindSlots;
};
