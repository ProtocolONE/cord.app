#include <HostMessageAdapter.h>

#include <Host/DBus/DbusConnection.h>
#include <Host/Dbus/MessageAdapterBridgeProxy.h>

#include <Core/UI/Message.h>

using GameNet::Host::DBus::DBusConnection;
using P1::Core::UI::Message;

HostMessageAdapter::HostMessageAdapter(QObject *parent /*= 0*/)
  : QObject(parent)
{
  QString dbusService("com.gamenet.dbus");
  this->_messageAdapter = new MessageAdapterBridgeProxy(
    dbusService, "/messageAdapter", DBusConnection::bus(), this);

  QObject::connect(this->_messageAdapter, &MessageAdapterBridgeProxy::messageRequest,
    this, &HostMessageAdapter::onMessageRequest);
}

HostMessageAdapter::~HostMessageAdapter()
{
}

void HostMessageAdapter::onMessageRequest(
  int messageId,
  const QString& title,
  const QString& text,
  int buttons,
  int icon)
{
  Message msgbox;
  msgbox.setTitle(title);
  msgbox.setText(text);
  msgbox.addButton(Message::StandardButton(buttons));
  msgbox.setIcon(static_cast<Message::Icon>(icon));

  int result = msgbox.exec();

  this->_messageAdapter->messageRequestResult(messageId, result);
}
