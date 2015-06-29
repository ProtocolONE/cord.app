#include <Helper/FileUtils.h>
#include <QtCore/QDir>
#include <QtCore/QDirIterator>
#include <QtCore/QFileInfo>

namespace Features {

  bool FileUtils::createDirectory(const QString &target)
  {
    QDir dir;
    return dir.mkpath(target);
  }

  QString FileUtils::createDirectoryForFile(const QString &targetFile)
  {
    QFileInfo info(targetFile);
    QDir targetPath = info.dir();
    QString path = info.absolutePath();
    if (!targetPath.exists())
      targetPath.mkpath(path);

    return path;
  }

  bool FileUtils::recurseCopyDiretory(const QString& source, const QString& target)
  {
    bool result = true;
    int len = source.length();
    QDir dir(source);

    QDirIterator it(dir, QDirIterator::Subdirectories);
    while (it.hasNext()) {
      QString file = it.next();
      if (!it.fileInfo().isDir()) {
        QString name = file.right(file.length() - len);
        QString targetFile = QString("%1/%2").arg(target, name);
        FileUtils::createDirectoryForFile(targetFile);
        result &= QFile::copy(file, targetFile);
      }
    }

    return result;
  }

  bool FileUtils::removeRecursively(const QString &target)
  {
    QDir dir(target);
    return dir.removeRecursively();
  }

  FileUtils::FileUtils()
  {
  }


  FileUtils::~FileUtils()
  {
  }

}