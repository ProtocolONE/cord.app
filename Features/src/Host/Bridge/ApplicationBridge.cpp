#include <Host/Bridge/ApplicationBridge.h>

#include <Host/Application.h>
#include <Host/Thetta.h>
#include <Host/Translation.h>
#include <Host/AutoRunManager.h>

namespace GameNet {
  namespace Host {

    namespace Bridge {

      ApplicationBridge::ApplicationBridge(QObject *parent /* = 0 */)
        : QObject(parent)
        , _application(nullptr)
        , _thetta(nullptr)
        , _translation(nullptr)
        , _autoRunManager(nullptr)
      {
      }

      ApplicationBridge::~ApplicationBridge() 
      {
      }

      void ApplicationBridge::setApplication(Application* app) 
      {
        Q_ASSERT(app);
        this->_application = app;

        QObject::connect(this->_application, &Application::initCompleted,
                this, &ApplicationBridge::initCompleted);
        
        QObject::connect(this->_application, &Application::restartUIRequest,
          this, &ApplicationBridge::restartUIRequest);

        QObject::connect(this->_application, &Application::shutdownUIRequest,
          this, &ApplicationBridge::shutdownUIRequest);
      }

      void ApplicationBridge::setThetta(Thetta *value)
      {
        Q_ASSERT(value);
        this->_thetta = value;
      }

      void ApplicationBridge::setTranslation(Translation *value)
      {
        Q_ASSERT(value);
        this->_translation = value;

        QObject::connect(value, &Translation::languageChanged,
          this, &ApplicationBridge::languageChanged);
      }

      void ApplicationBridge::setAutoRunManager(AutoRunManager *value)
      {
        Q_ASSERT(value);
        this->_autoRunManager = value;
        QObject::connect(value, &AutoRunManager::autoStartModeChanged,
          this, &ApplicationBridge::autoStartModeChanged);
      }

      bool ApplicationBridge::isInitCompleted()
      {
        Q_ASSERT(this->_application);
        if (!this->_application) {
          return false;
        }
        
        return this->_application->isInitCompleted();
      }

      void ApplicationBridge::restartApplication(bool isMinimized)
      {
        Q_ASSERT(this->_application);
        this->_application->restartApplication(false, isMinimized);
      }

      void ApplicationBridge::shutdownUIResult()
      {
        Q_ASSERT(this->_application);
        this->_application->shutdownUIResult();
      }

      void ApplicationBridge::switchClientVersion()
      {
        Q_ASSERT(this->_application);
        this->_application->switchClientVersion();
      }

      void ApplicationBridge::openBrowser(const QString& url)
      {
        Q_ASSERT(this->_thetta);
        this->_thetta->openBrowser(url);
      }

      QString ApplicationBridge::language() const
      {
        Q_ASSERT(this->_translation);
        return this->_translation->language();
      }

      void ApplicationBridge::setLanguage(const QString& value)
      {
        Q_ASSERT(this->_translation);
        this->_translation->setLanguage(value);
      }

      void ApplicationBridge::setAutoStartMode(int value)
      {
        Q_ASSERT(this->_autoRunManager);
        this->_autoRunManager->setAutoStartMode(value);
      }

      int ApplicationBridge::autoStartMode() const
      {
        Q_ASSERT(this->_autoRunManager);
        return this->_autoRunManager->autoStartMode();
      }

    }
  }
}
