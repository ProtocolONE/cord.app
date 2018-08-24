#pragma once

#include <QtCore/QObject>
#include <QtCore/QString>
#include <QtCore/QMetaType>

namespace GameNet {
  namespace Host {

    class MessageAdapter;

    namespace Bridge {

      class MessageAdapterBridge : public QObject
      {
        Q_OBJECT
      public:
        explicit MessageAdapterBridge(QObject *parent = 0);
        virtual ~MessageAdapterBridge();

        void setAdapter(MessageAdapter *value);

      public slots:
        void messageRequestResult(int messageId, int buttonId);

      signals:
        void messageRequest(
          int messageId,
          const QString& title,
          const QString& text,
          int buttons,
          int icon);

      private:
        MessageAdapter *_adapter;

      };

    }
  }
}