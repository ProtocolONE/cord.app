#include <Host/Updater.h>
#include <Host/Bridge/UpdateManagerBridge.h>

namespace GameNet {
  namespace Host {

    namespace Bridge {

      UpdateManagerBridge::UpdateManagerBridge(QObject *parent /* = 0 */) 
        : QObject(parent)
        , _updater(0)
      {

      }

      UpdateManagerBridge::~UpdateManagerBridge() 
      {

      }

      int UpdateManagerBridge::updateState()
      {
        Q_ASSERT(this->_updater);
        return this->_updater->updateState();
      }

      void UpdateManagerBridge::setUpdateManager(Updater* updater) {
        Q_ASSERT(updater);
        this->_updater = updater;

        QObject::connect(this->_updater, &Updater::downloadUpdateProgress, 
          this, &UpdateManagerBridge::downloadUpdateProgress);

        QObject::connect(this->_updater, &Updater::updatesFound, 
          this, &UpdateManagerBridge::updatesFound);

        QObject::connect(this->_updater, &Updater::noUpdatesFound, 
          this, &UpdateManagerBridge::noUpdatesFound);

        QObject::connect(this->_updater, &Updater::downloadRetryNumber, 
          this, &UpdateManagerBridge::downloadRetryNumber);

        QObject::connect(this->_updater, &Updater::allCompleted, 
          this, &UpdateManagerBridge::allCompleted);

        QObject::connect(this->_updater, &Updater::updateStateChanged, 
          this, &UpdateManagerBridge::updateStateChanged);

        QObject::connect(this->_updater, &Updater::updateError, 
          this, &UpdateManagerBridge::updateError);
      }
    }
  }
}