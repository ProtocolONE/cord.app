#include <QtCore/QTimer>
#include <QtCore/QDebug>
#include <QmlMessageAdapter.h>

#define SIGNAL_CONNECT_CHECK(X){ bool result = X; Q_ASSERT_X(result, __FUNCTION__ , #X); }

using namespace P1::Core::UI;

QmlMessageAdapter::QmlMessageAdapter(QObject *parent)
  : MessageAdapter(parent)
{
}

QmlMessageAdapter::~QmlMessageAdapter()
{
}

void QmlMessageAdapter::show(
  const QString& title,
  const QString& text,
  P1::Core::UI::Message::StandardButtons buttons,
  P1::Core::UI::Message::Icon icon,
  int messageId,
  QObject* reciever,
  const char* member)
{
  this->_bindSlots[messageId] = qMakePair(reciever, member);
  emit this->emitMessage(title, text, buttons, icon, messageId);
}

void QmlMessageAdapter::buttonClicked(int messageId, int id)
{
  this->_returnButtons[messageId] = static_cast<P1::Core::UI::Message::StandardButton>(id);
}

void QmlMessageAdapter::callback(int messageId, int buttonId)
{
    this->buttonClicked(messageId, buttonId);
    QTimer::singleShot(0, this->_bindSlots[messageId].first, this->_bindSlots[messageId].second);
}