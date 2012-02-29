#ifndef QmlMessageAdapter_H
#define QmlMessageAdapter_H

#include <Core/UI/Message.h>
#include <Core/UI/MessageAdapter.h>

#include <QtDeclarative/QDeclarativeView>
#include <QSignalMapper>

using namespace GGS::Core::UI;

class QmlMessageAdapter : public MessageAdapter
{
  Q_OBJECT
  Q_ENUMS(GGS::Core::UI::Message::StandardButton)

public:
    QmlMessageAdapter(QObject *parent);
    ~QmlMessageAdapter();

public slots:
  void show(const QString& title, const QString& text, GGS::Core::UI::Message::StandardButtons buttons, GGS::Core::UI::Message::Icon icon, int messageId, QObject* reciever, const char* bindFunc);   
  void bindSlot(QObject* sender, int messageId); 
  void buttonClicked(int messageId, int id);

signals:
    void emitMessage(QString title, QString text, int buttons, GGS::Core::UI::Message::Icon icon, int messageId);    
    // Здесь передаем int buttons сразу в QML. Потому что нет другой возможности получить доступ к унарным операциям внутри QML, если это не тип int

private:
    QMap<int, QPair<QObject*, const char*>> _bindSlots;
};

#endif // QmlMessageAdapter_H 
