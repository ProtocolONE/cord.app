#include <Host/Bridge/ApplicationBridge.h>

#include <Host/Application.h>
#include <Host/Thetta.h>
#include <Host/Translation.h>

namespace GameNet {
  namespace Host {

    namespace Bridge {

      ApplicationBridge::ApplicationBridge(QObject *parent /* = 0 */)
        : QObject(parent)
        , _application(nullptr)
        , _thetta(nullptr)
        , _translation(nullptr)
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
        this->_application->restartApplication(true, isMinimized);
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

    }
  }
}