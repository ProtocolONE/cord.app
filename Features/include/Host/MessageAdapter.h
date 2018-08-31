#pragma once

#include <Core/UI/Message.h>
#include <Core/UI/MessageAdapter.h>

#include <QtCore/QObject>
#include <QtCore/QString>

#include <functional>

namespace P1 {
  namespace Host {
    
    class MessageInfo 
    {
    public:
      MessageInfo();

      MessageInfo(QObject* receiver, const char* member);
      MessageInfo(const MessageInfo& c);
      MessageInfo& operator=(const MessageInfo &p);

      ~MessageInfo();

      QObject *receiver;
      QString member;
    };

    class MessageAdapter : public P1::Core::UI::MessageAdapter
    {
      Q_OBJECT
    public:
      explicit MessageAdapter(QObject *parent = 0);
      virtual ~MessageAdapter();

      /**
       * \fn  void :::setHasUiProcess(std::function<bool ()> value);
       *
       * \brief Задает функтор возвращающий true, если UI запущен, иначе false.
       *
       * \author  Ilya Tkachenko
       * \date  15.10.2014
       *
       * \param value функция проверки наличия UI.
       */

      void setHasUiProcess(std::function<bool ()> value);

      /**
       * \fn  void :::uiProcessClosed();
       *
       * \brief Обрабатывает ситуацию с закрытием UI при наличии не закрытых MessageBox.
       *
       * \author  Ilya Tkachenko
       * \date  15.10.2014
       */

      void uiProcessClosed();

      virtual void show(
        const QString& title, 
        const QString& text, 
        P1::Core::UI::Message::StandardButtons buttons, 
        P1::Core::UI::Message::Icon icon, 
        int messageId, 
        QObject* receiver, 
        const char* member) override;

      virtual void messageRequestResult(int messageId, int buttonId);

    signals:
      void messageRequest(
        int messageId,
        const QString& title,
        const QString& text,
        int buttons,
        int icon);

    private:
      void showDefaultMessage(const QString& title, 
        const QString& text,
        P1::Core::UI::Message::StandardButtons buttons,
        P1::Core::UI::Message::Icon icon,
        int messageId,
        QObject* receiver,
        const char* member);

      std::function<bool ()> _hasUiProcess;
      QMap<int, MessageInfo> _activeMessages;
      
    };

  }
}