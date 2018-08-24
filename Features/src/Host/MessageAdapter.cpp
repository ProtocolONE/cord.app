#include <Host/MessageAdapter.h>

#include <QtCore/QTimer>

#include <QtWidgets/QMessageBox>

namespace GameNet {
  namespace Host {

    MessageAdapter::MessageAdapter(QObject *parent)
      : P1::Core::UI::MessageAdapter(parent)
    {
    }

    MessageAdapter::~MessageAdapter()
    {
    }

    void MessageAdapter::setHasUiProcess(std::function<bool ()> value)
    {
      Q_ASSERT(value);
      this->_hasUiProcess = value;
    }

    void MessageAdapter::show(
      const QString& title,
      const QString& text,
      P1::Core::UI::Message::StandardButtons buttons,
      P1::Core::UI::Message::Icon icon,
      int messageId,
      QObject* receiver,
      const char* member)
    {
      Q_ASSERT(this->_hasUiProcess);

      if (this->_hasUiProcess()) {
        this->_activeMessages[messageId] = MessageInfo(receiver, member);
        emit this->messageRequest(messageId, title, text, buttons, icon);
        return;
      }
      
      this->showDefaultMessage(title, text, buttons, icon, messageId, receiver, member);
    }

    void MessageAdapter::messageRequestResult(int messageId, int buttonId)
    {
      if (!this->_activeMessages.contains(messageId))
        return;

      MessageInfo info = this->_activeMessages.take(messageId);
      this->_returnButtons[messageId] = static_cast<P1::Core::UI::Message::StandardButton>(buttonId);
      QTimer::singleShot(
        0, 
        info.receiver,
        (const char*)info.member.toLatin1().data());
    }

    void MessageAdapter::uiProcessClosed()
    {
      QMap<int, MessageInfo> messages;
      this->_activeMessages.swap(messages);

      Q_FOREACH(int messageId, messages.keys()) {
        MessageInfo info = messages[messageId];
        this->_returnButtons[messageId] = static_cast<P1::Core::UI::Message::StandardButton>(0);
        QTimer::singleShot(
          0, 
          info.receiver,
          (const char*)info.member.toLatin1().data());
      }
    }

    void MessageAdapter::showDefaultMessage(
      const QString& title,
      const QString& text,
      P1::Core::UI::Message::StandardButtons buttons,
      P1::Core::UI::Message::Icon icon,
      int messageId,
      QObject* receiver,
      const char* member)
    {
      QMessageBox *msgbox = new QMessageBox();
      msgbox->setWindowTitle(title);
      msgbox->setText(text);
      msgbox->setStandardButtons(QMessageBox::StandardButtons(buttons.operator int()));
      msgbox->setIcon(static_cast<QMessageBox::Icon>(icon));
      int result = msgbox->exec();
      this->_returnButtons[messageId] = static_cast<P1::Core::UI::Message::StandardButton>(result);
      QTimer::singleShot(0, receiver, member);
    }

    MessageInfo::MessageInfo()
      : receiver(nullptr)
    {
    }

    MessageInfo::~MessageInfo()
    {
    }

    MessageInfo::MessageInfo(const MessageInfo& c)
    {
      this->operator=(c);
    }

    MessageInfo::MessageInfo(
      QObject* receiver,
      const char* member)
      : receiver(receiver)
      , member(QString(member))
    {
    }

    MessageInfo& MessageInfo::operator=(const MessageInfo &p)
    {
      if (this == &p)
        return *this; // return on same object 

      this->receiver = p.receiver;
      this->member = p.member;
      return *this;
    }


  }
}