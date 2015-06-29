#pragma once
#include <QtCore/QString>

namespace Features {

  class FileUtils
  {
  public:
    static bool createDirectory(const QString &target);
    static QString createDirectoryForFile(const QString &targetFile);
    static bool recurseCopyDiretory(const QString& source, const QString& target);
    static bool removeRecursively(const QString &target);

  private:
    FileUtils();
    ~FileUtils();
  };

}