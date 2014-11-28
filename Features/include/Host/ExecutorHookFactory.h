#pragma once
#include <Host/Factory.h>

#include <GameExecutor/HookInterface.h>

#include <QtCore/QObject>
#include <QtCore/QString>
#include <QtCore/QSet>
#include <QtCore/QDebug>

#include <QtWidgets/QWidget>

namespace Features {
  namespace Thetta {
    class SaveUserInfo;
  }
}

namespace GGS {
  namespace GameDownloader {
    class GameDownloadService;
  }

  namespace GameExecutor {
    class GameExecutorService;
  }
}

namespace GameNet {
  namespace Host {
    
    class HookFactory;
    class Thetta;

    class ExecutorHookFactory 
      : public QObject, public Factory<GGS::GameExecutor::HookInterface, QString>
    {
      Q_OBJECT
    public:
      explicit ExecutorHookFactory(QObject *parent = 0);
      virtual ~ExecutorHookFactory();

      void setDownloader(GGS::GameDownloader::GameDownloadService *value);
      void setExecutor(GGS::GameExecutor::GameExecutorService *value);
      void setDownloaderHookFactory(HookFactory *value);
      void setThetta(Thetta *value);

      void init();

      GGS::GameExecutor::HookInterface *create(const QString& id);

    private:
      GGS::GameDownloader::GameDownloadService *_downloader;
      // INFO основной запускатор. Нужен для закрытия СА из хука защиты.
      GGS::GameExecutor::GameExecutorService *_executor;
      HookFactory *_downloaderHookFactory;
      Thetta *_thetta;
      Features::Thetta::SaveUserInfo *_saveUserInfo;
      QWidget _window;

      // INFO кол выводит класс и его id при регистрации хуков
      //template<typename T>
      //void reg(std::function<void(T*)> init = nullptr) {
      //  qDebug() <<  typeid(T).name() << " " << T::id();
      //  this->Factory<GGS::GameExecutor::HookInterface, QString>::reg<T>();
      //}

    };

  }
}