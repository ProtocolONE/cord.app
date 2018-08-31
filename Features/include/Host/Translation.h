#pragma once

#include <QtCore/QObject>
#include <QtCore/QString>
#include <QtCore/QMap>

class QTranslator;

namespace P1 {
  namespace Host {

    class Translation : public QObject
    {
      Q_OBJECT
    public:
      explicit Translation(QObject *parent = 0);
      virtual ~Translation();

      void init();

      static void load(QMap<QString, QTranslator*> &translators, QObject* parent = 0);

      virtual QString language() const;
      virtual void setLanguage(const QString& value);
    
    signals:
      void languageChanged();

    private:
      void selectLanguage(const QString& value);
      void saveLanguage(const QString& value);

      QMap<QString, QTranslator*> _translators;
    };

  }
}