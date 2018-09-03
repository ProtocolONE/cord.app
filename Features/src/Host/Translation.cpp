#include <Host/Translation.h>

#include <Settings/Settings.h>

#include <QtCore/QTranslator>
#include <QtCore/QDir>
#include <QtCore/QCoreApplication>

namespace P1 {
  namespace Host {

    Translation::Translation(QObject *parent /*= 0*/)
      : QObject(parent)
    {
    }

    Translation::~Translation()
    {
    }

    void Translation::init()
    {
      Translation::load(this->_translators, this);
      this->selectLanguage(this->language());
    }

    QString Translation::language() const
    {
      // HACK
      return "en";

      P1::Settings::Settings settings;
      return settings.value("launcher/language").toString(); 
    }

    void Translation::setLanguage(const QString& value)
    {
      if (value == this->language())
        return;

      this->saveLanguage(value);
      this->selectLanguage(value);

      emit this->languageChanged();
    }

    void Translation::selectLanguage(const QString& value)
    {
      if (this->_translators.contains(value)) {
        QCoreApplication::installTranslator(this->_translators[value]);
        return;
      }

      QString defaultLanguage("ru");
      if (this->_translators.contains(defaultLanguage))
        QCoreApplication::installTranslator(this->_translators[defaultLanguage]);

      this->saveLanguage(defaultLanguage);
    }

    void Translation::saveLanguage(const QString& value)
    {
      P1::Settings::Settings settings;
      settings.setValue("launcher/language", value);
    }

    void Translation::load(QMap<QString, QTranslator*> &translators, QObject* parent)
    {
      QString filenamePrefix("launcher_");

      QDir dir(QCoreApplication::applicationDirPath() + "/Languages/");
      dir.setFilter(QDir::Files | QDir::Hidden | QDir::NoSymLinks);
      dir.setSorting(QDir::Size | QDir::Reversed);

      QFileInfoList list = dir.entryInfoList();

      for (int i = 0; i < list.size(); ++i) { // TODO Немного хардкод, обсудить это
        QFileInfo fileInfo = list.at(i);

        QString fileName = fileInfo.fileName();
        if (!fileName.startsWith(filenamePrefix, Qt::CaseInsensitive))
          continue;

        fileName.remove(filenamePrefix);
        fileName.remove(".qm");

        if (fileName.size() == 2) {  
          QTranslator *translator = new QTranslator(parent);
          // TODO да да, знаю, сделаю красиво позже.
          translator->load(fileInfo.fileName(), QCoreApplication::applicationDirPath() + "/Languages/");

          translators[fileName] = translator;
        }
      }
    }

  }
}