#pragma  once

#include <QtCore/QObject>
#include <QtCore/QString>
#include <QtCore/QStringList>

#include <windows.h>

namespace GameNet {
  namespace Host {

    class UIProcessPrivate;

    class UIProcess: public QObject 
    {
      Q_OBJECT
      friend class UIProcessPrivate;
    public:
      explicit UIProcess::UIProcess(QObject* parent = 0);
      virtual UIProcess::~UIProcess();

      /**
       * \fn  void UIProcess::setDirectory(const QString& value);
       *
       * \brief Задает полный путь до директории с UI процессом.
       *
       * \author  Ilya Tkachenko
       * \date  08.10.2014
       *
       * \param value Полный путь до папки с UI.
       */
      void setDirectory(const QString& value);

      /**
       * \fn  void UIProcess::setFileName(const QString& value);
       *
       * \brief Задает имя исполняемого файла UI процесса.
       *
       * \author  Ilya Tkachenko
       * \date  08.10.2014
       *
       * \param value Имя исполняемого файла UI процесса.
       */

      void setFileName(const QString& value);

      /**
       * \fn  bool UIProcess::isRunning();
       *
       * \brief Возвращает true, если запушен процесс UI.
       *
       * \author  Ilya Tkachenko
       * \date  08.10.2014
       *
       * \return  true если запущен, иначе false.
       */
      bool isRunning();

      /**
       * \fn  bool UIProcess::start(const QStringList& args);
       *
       * \brief Запускает процесс UI. Этот метод запускает процесс в любом случаи запущен процесс или нет.
       *        Если процесс уже был запущен, то завершение его не мониториться. Предполагается, что запуск
       *        второго процесса UI необходимо только для передечи комманды существующей копии.
       *
       * \author  Ilya Tkachenko
       * \date  08.10.2014
       *
       * \param args  Аргументы коммандной строки для запуска процесса UI.
       *
       * \return  true if it succeeds, false if it fails.
       */
      bool start(const QStringList& args);

      /**
       * \fn  void UIProcess::sendCommand(const QString& name, const QStringList& args);
       *
       * \brief На основе комманды и аргументов формирует строку для запуска UI процесса и запускает его.
       *        Все комманды преобразуются по формату uri:gamenet://name/args[0]/args[1]/...
       *
       * \author  Ilya Tkachenko
       * \date  08.10.2014
       *
       * \param name  Имя комманды.
       * \param args  Список аргументов комманды.
       */
      void sendCommand(const QString& name, const QStringList& args); 

    private:
      void processFinished();

      UIProcessPrivate *_d;
    };

  }
}