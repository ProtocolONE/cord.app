#include <QtCore/QTimer>
//#include <QtDeclarative/QDeclarativeContext>
#include <QtCore/QDebug>
#include <QmlMessageAdapter.h>

#define SIGNAL_CONNECT_CHECK(X){ bool result = X; Q_ASSERT_X(result, __FUNCTION__ , #X); }

using namespace GGS::Core::UI;

QmlMessageAdapter::QmlMessageAdapter(QObject *parent)
  : MessageAdapter(parent)
{
}

QmlMessageAdapter::~QmlMessageAdapter()
{
}

void QmlMessageAdapter::show(const QString& title, const QString& text, GGS::Core::UI::Message::StandardButtons buttons, GGS::Core::UI::Message::Icon icon, int messageId, QObject* reciever, const char* member)
{
  this->_bindSlots[messageId] = qMakePair(reciever, member);
  emit this->emitMessage(title, text, buttons, icon, messageId);
}

void QmlMessageAdapter::buttonClicked(int messageId, int id)
{
  this->_returnButtons[messageId] = static_cast<GGS::Core::UI::Message::StandardButton>(id);   
}

void QmlMessageAdapter::callback(int messageId, int buttonId)
{
    this->buttonClicked(messageId, buttonId);
    QTimer::singleShot(0, this->_bindSlots[messageId].first, this->_bindSlots[messageId].second);
}