#include <Host/Bridge/ApplicationBridge.h>

#include <Host/Application.h>
#include <Host/Thetta.h>

namespace GameNet {
  namespace Host {

    namespace Bridge {

      ApplicationBridge::ApplicationBridge(QObject *parent /* = 0 */)
        : QObject(parent)
        , _application(nullptr)
        , _thetta(nullptr)
      {

      }

      ApplicationBridge::~ApplicationBridge() 
      {
     
      }

      void ApplicationBridge::setApplcation(Application* app) 
      {
        Q_ASSERT(app);
        this->_application = app;

        connect(this->_application, &Application::initCompleted,
                this, &ApplicationBridge::initCompleted);
      }

      void ApplicationBridge::setThetta(Thetta *value)
      {
        Q_ASSERT(value);
        this->_thetta = value;
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

    }
  }
}