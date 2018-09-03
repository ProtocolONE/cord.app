#pragma once
#include <Core/RegisterTypes.h>
#include <GameDownloader/RegisterTypes.h>
#include <GameExecutor/RegisterTypes.h>
#include <RestApi/RegisterTypes.h>
#include <UpdateSystem/RegisterTypes.h>

void registerDependenicesTypes()
{
  P1::Core::registerTypes();
  P1::GameDownloader::registerTypes();
  P1::GameExecutor::registerTypes();
  P1::RestApi::registerTypes();
  P1::UpdateSystem::registerTypes();
}