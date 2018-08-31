#pragma once
#include <Host/Factory.h>

#include <GameExecutor/HookInterface.h>

#include <QtCore/QObject>
#include <QtCore/QString>
#include <QtCore/QSet>
#include <QtCore/QDebug>

#include <QtWidgets/QWidget>

namespace Features {
  namespace WorkStationLock {
    class RegisterSessionNotificationFilter;
  }
}

namespace P1 {
  namespace GameDownloader {
    class GameDownloadService;
  }

  namespace GameExecutor {
    class GameExecutorService;
  }
}

namespace P1 {
  namespace Host {
    
    class HookFactory;
    class Application;

    class ExecutorHookFactory 
      : public QObject, public Factory<P1::GameExecutor::HookInterface, QString>
    {
      Q_OBJECT
    public:
      explicit ExecutorHookFactory(QObject *parent = 0);
      virtual ~ExecutorHookFactory();

      void setDownloader(P1::GameDownloader::GameDownloadService *value);
      void setExecutor(P1::GameExecutor::GameExecutorService *value);
      void setDownloaderHookFactory(HookFactory *value);
      void setApplication(Application *value);

      void init();

      P1::GameExecutor::HookInterface *create(const QString& id);

    private:
      P1::GameDownloader::GameDownloadService *_downloader;
      // INFO основной запускатор. Нужен для закрытия СА из хука защиты.
      P1::GameExecutor::GameExecutorService *_executor;
      HookFactory *_downloaderHookFactory;
      Features::WorkStationLock::RegisterSessionNotificationFilter *_filter;
      QWidget _window;

      // INFO кол выводит класс и его id при регистрации хуков
      //template<typename T>
      //void reg(std::function<void(T*)> init = nullptr) {
      //  qDebug() <<  typeid(T).name() << " " << T::id();
      //  this->Factory<P1::GameExecutor::HookInterface, QString>::reg<T>();
      //}

    };

  }
}