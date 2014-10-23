#include <gmock/gmock.h>
#include <gtest/gtest.h>

#include <Host/Bridge/ServiceHandleBridge.h>
#include <Host/Proxy/ServiceHandleProxy.h>

using GameNet::Host::Bridge::ServiceHandleBridge;
using GameNet::Host::Proxy::ServiceHandleProxy;

using ::testing::Return;


class ServiceHandleMock : public ServiceHandleProxy
{
public:
  MOCK_METHOD1(lock, bool(const QString&));
  MOCK_METHOD1(unlock, void(const QString&));

  // mock for signals
  MOCK_METHOD1(onLocked, void(const QString&));
  MOCK_METHOD1(onUnlocked, void(const QString&));
};

class ServiceHandleBridgeTest : public ::testing::Test
{
public:

  ServiceHandleBridgeTest()
    : serviceId("1233455677891234")
  {
    bridge.setServiceHandle(&serviceHandle);

    QObject::connect(&bridge, &ServiceHandleBridge::locked,
      &serviceHandle, &ServiceHandleMock::onLocked);

    QObject::connect(&bridge, &ServiceHandleBridge::unlocked,
      &serviceHandle, &ServiceHandleMock::onUnlocked);
  }

  ServiceHandleBridge bridge;
  ServiceHandleMock serviceHandle;
  QString serviceId;
};

TEST_F(ServiceHandleBridgeTest, lock)
{
  EXPECT_CALL(serviceHandle, lock(serviceId))
    .Times(1);

  bridge.lock(serviceId);
}

TEST_F(ServiceHandleBridgeTest, unlock)
{
  EXPECT_CALL(serviceHandle, unlock(serviceId))
    .Times(1);

  bridge.unlock(serviceId);
}

TEST_F(ServiceHandleBridgeTest, locked)
{
  EXPECT_CALL(serviceHandle, onLocked(serviceId))
    .Times(1);

  serviceHandle.locked(serviceId);
}

TEST_F(ServiceHandleBridgeTest, unlocked)
{
  EXPECT_CALL(serviceHandle, onUnlocked(serviceId))
    .Times(1);

  serviceHandle.unlocked(serviceId);
}