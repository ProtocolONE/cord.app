#pragma once

#include <QtCore/QStringList>

class Uninstall
{
  Uninstall();

public:
  ~Uninstall();

  static void run(const QStringList& arguments);

};
