#pragma once

#include <Host/ServiceLoader.h>

class ServiceLoaderFixture : public GameNet::Host::ServiceLoader
{
  Q_OBJECT
public:
  explicit ServiceLoaderFixture(QObject *parent = 0){};
  ~ServiceLoaderFixture(){};
};