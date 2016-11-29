#include <Features/GameExecutor/Hook/CheckAsciiPath.h>

#include <QtCore/QRegularExpression>
#include <Core/UI/Message.h>

using GGS::GameExecutor::HookInterface;
using GGS::Core::Service;

using namespace GGS::Core::UI;

namespace Features {
  namespace GameExecutor {
    namespace Hook {

      CheckAsciiPath::CheckAsciiPath(QObject *parent)
        : HookInterface(parent)
      {
      }

      CheckAsciiPath::~CheckAsciiPath()
      {
      }

      QString CheckAsciiPath::id()
      {
        return "DD9C78EE-A95A-4326-A913-F02F75C942EF";
      }

      void CheckAsciiPath::CanExecute(Service &service)
      {
        QString path = service.installPath();
        QRegularExpression re("^[-0-9a-zA-Z:_()\\\\\\/\\. ]+$");
        QRegularExpressionMatch match = re.match(path);
        bool hasMatch = match.hasMatch(); // true
        if (hasMatch) {
          emit this->canExecuteCompleted(service, GGS::GameExecutor::Success);
        } else {
          QString message = tr("CHECK_ASCII_PATH_MESSAGE");
          Message::StandardButton button = Message::critical(tr("CHECK_ASCII_PATH_ERROR_TITLE"),
            message,
            Message::Close);

          emit this->canExecuteCompleted(service, GGS::GameExecutor::CanExecutionHookBreak);
        }
      }

    }
  }
}

