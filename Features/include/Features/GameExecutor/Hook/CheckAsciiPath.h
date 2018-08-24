#pragma once

#include <GameExecutor/HookInterface.h>
#include <Core/Service.h>

namespace Features {
  namespace GameExecutor {
    namespace Hook {

      class CheckAsciiPath : public P1::GameExecutor::HookInterface
      {
        Q_OBJECT
      public:
        explicit CheckAsciiPath(QObject *parent = 0);
        virtual ~CheckAsciiPath();

        static QString id();

        virtual void CanExecute(P1::Core::Service &service) override;

      };

    }
  }
}