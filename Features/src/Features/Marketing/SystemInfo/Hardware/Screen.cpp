#include <Features/Marketing/SystemInfo/Hardware/Screen.h>
#include <QtWidgets/QDesktopWidget>

namespace Features {
  namespace Marketing {
    namespace SystemInfo {
      namespace Hardware {

        Screen::Screen(QObject *parent /*= 0*/)
          : QObject(parent)
        {
        }

        Screen::~Screen()
        {
        }

        void Screen::write(QXmlStreamWriter *writer)
        {
          writer->writeStartElement("desktop");

          QDesktopWidget desktop;
          int monitorCount = desktop.screenCount();
          QRect primaryScreen = desktop.screenGeometry();

          writer->writeTextElement("primaryScreenWidth", QString::number(primaryScreen.width()));
          writer->writeTextElement("primaryScreenHeight", QString::number(primaryScreen.height()));

          writer->writeStartElement("monitors");
          for (int i = 0; i < desktop.screenCount(); i++) {
            QRect screen = desktop.screenGeometry(i);

            writer->writeStartElement("monitor");
            writer->writeTextElement("width", QString::number(screen.width()));
            writer->writeTextElement("height", QString::number(screen.height()));
            writer->writeEndElement();
          }
          writer->writeEndElement();

          writer->writeEndElement();
        }

      }
    }
  }
}