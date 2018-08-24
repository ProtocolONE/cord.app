#pragma once

#include <QtCore/QString>

namespace Features {
  void terminateProcessByName(const QString &procName);
  void terminateProcessByName(const QString &procName, bool trySoft, int timeout);
}