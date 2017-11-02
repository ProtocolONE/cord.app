#include <Helper/GetDirectoryDialog.h>

#include <Core/UI/Message>

#include <QtCore/QDir>
#include <QtCore/QDebug>
#include <QtCore/QDirIterator>
#include <QtCore/QStorageInfo>
#include <QtCore/QSettings>
#include <QtCore/QCoreApplication>

#include <QtWidgets/QFileDialog>

#include <Windows.h>
#include <Shlobj.h>

using GGS::Core::UI::Message;

GetDirectoryDialog::GetDirectoryDialog(QWidget *parent)
  : QObject(parent)
{
}

GetDirectoryDialog::~GetDirectoryDialog()
{
}

QString GetDirectoryDialog::removeNotExisting(const QString & inputPath)
{
  QString resultPath = inputPath;
  QDir dir(inputPath);

  while (!dir.exists()) {
    QString tmpPath = dir.path();
    int slashPos = tmpPath.lastIndexOf("/");

    if (slashPos == -1) {
      //Returning default
      resultPath = inputPath; 
      break;
    }

    resultPath = tmpPath.remove(slashPos, tmpPath.length() - slashPos);
    dir.setPath(resultPath);
  }
  return resultPath;
}

void GetDirectoryDialog::getDirectory(const QString& serviceName, const int size, const QString& defaultDir)
{
  QFileDialog *dialog = new QFileDialog(qobject_cast<QWidget*>(this->parent()), tr("CAPTION_OPEN_DIR"), this->removeNotExisting(defaultDir));
  dialog->setOptions(QFileDialog::ShowDirsOnly | QFileDialog::DontResolveSymlinks);
  dialog->setFileMode(QFileDialog::Directory);

  /*
    QGNA-919
    Диалог выбора папки в стандартном режиме (QFileDialog::Directory), игнорирует эту опцию опцию в Windows XP, и все равно 
    ожидает выделения файла.
    После замены вида диалога на DirectoryOnly, все работает как надо
  */
  if (QSysInfo::windowsVersion() == QSysInfo::WV_XP) {
    dialog->setFileMode(QFileDialog::DirectoryOnly);
  }

  QObject::connect(dialog, &QDialog::finished, [=](int result) {
    dialog->deleteLater();
        
    if (!result) {
      this->directoryEntered(QString());
      return;
    }

    QString newDirectory = dialog->selectedFiles().first();
    if (newDirectory.isEmpty()) {
      this->directoryEntered(QString());
      return;
    }

    newDirectory = QDir::toNativeSeparators(newDirectory);

    QString tmpDir;
    if (this->isDeniedFolder(newDirectory, &tmpDir)) {
      Message::information(
        tr("DIRECTORY_NOT_ACCEPTED_TO_INSTALL_INFO"),
        tr("DIRECTORY_NOT_ACCEPTED_TO_INSTALL_BODY_INFORMATION").arg(tmpDir));

      this->directoryEntered(QString());
      return;
    }

    if (!this->isAcceptedFolder(newDirectory, &tmpDir)) {
        Message::information(
          tr("DIRECTORY_NOT_ACCEPTED_TO_INSTALL_INFO"), 
          tr("DIRECTORY_NOT_ACCEPTED_TO_INSTALL_BODY_INFORMATION").arg(tmpDir));

      this->directoryEntered(QString());
      return;
    }

    if (!this->checkFreeSpace(newDirectory, serviceName, size))  {
        this->directoryEntered(QString());
        return;
    } 

    if (this->isRootFolder(newDirectory)) {
      this->directoryEntered(newDirectory + serviceName);
      return;
    }

    bool dontUseFolder = false;

    QDir path = QCoreApplication::applicationDirPath();
    QDir newPath = newDirectory;
    if (newPath.absolutePath().toCaseFolded().indexOf(path.absolutePath().toCaseFolded()) == 0) {
      dontUseFolder = Message::Cancel == Message::question(
        tr("DIRECTORY_NOT_EMPTY_INFO"),
        tr("DIRECTORY_QGNA_QUESTION"),
        static_cast<Message::StandardButton>(Message::Ok | GGS::Core::UI::Message::Cancel));

    } else {
      if (!this->isEmptyFolder(newDirectory)) {
        dontUseFolder = Message::Cancel == Message::question(
          tr("DIRECTORY_NOT_EMPTY_INFO"),
          tr("DIRECTORY_NOT_EMPTY_QUESTION"),
          static_cast<Message::StandardButton>(Message::Ok | GGS::Core::UI::Message::Cancel));
      }
    }

    if (dontUseFolder) {
      this->directoryEntered(QString());
      return;
    }

    this->directoryEntered(newDirectory);
  });

  dialog->open(); 
} 

QString GetDirectoryDialog::getFolderName(int type) {
  wchar_t tmp[MAX_PATH];
  if (SHGetSpecialFolderPath(0, tmp, type, false))
    return QString::fromWCharArray(tmp);

  return QString();
}

bool GetDirectoryDialog::isDeniedFolder(const QString &newDirectory, QString *folder)
{
  int deniedFolders[] = { CSIDL_PROGRAM_FILES,
    CSIDL_PROGRAM_FILES_COMMON,
    CSIDL_SYSTEM,
    CSIDL_WINDOWS,
    -1 };

  for (int i = 0; i < sizeof(deniedFolders) / sizeof(int); ++i) {
    QString folderName;

    if (deniedFolders[i] == -1) {
      WCHAR szNativeProgramFilesFolder[MAX_PATH];
      ExpandEnvironmentStrings(L"%ProgramW6432%", szNativeProgramFilesFolder, ARRAYSIZE(szNativeProgramFilesFolder));
      folderName = QString::fromWCharArray(szNativeProgramFilesFolder);
    } else {
      folderName = QDir::toNativeSeparators(this->getFolderName(deniedFolders[i]));
    }

    if (newDirectory.compare(folderName, Qt::CaseInsensitive) == 0) {
      if (folder)
        *folder = folderName;

      return true; 
    }
  }

  return false;
}

bool GetDirectoryDialog::isRootFolder(const QString &newDirectory)
{
  QDir dir(newDirectory);
  dir.setFilter(QDir::NoDotAndDotDot);

  return dir.isRoot();
}

bool GetDirectoryDialog::isEmptyFolder(const QString &newDirectory)
{
  QDirIterator it(newDirectory, QDirIterator::Subdirectories);
  for (int count = 0; it.hasNext(); ++count, it.next()) {
    if (count > 2) {
      return false;
    }
  }

  return true;
}

bool GetDirectoryDialog::isAcceptedFolder(const QString &newDirectory, QString *folder)
{
  QString winDir = QDir::toNativeSeparators(this->getFolderName(CSIDL_WINDOWS));
  if (newDirectory.startsWith(winDir, Qt::CaseInsensitive)) {
    if (folder) 
      *folder = winDir;
    return false;
  }

  return true;
}

QString GetDirectoryDialog::formatSize(quint64 value)
{
  if (value < 1024) {
    return QObject::tr("%1 B").arg(value);
  }

  if (value < 1024 * 1024) {
    return QObject::tr("%1 KB").arg(value >> 10);
  }

  if (value < 1024 * 1024 * 1024) {
    return QObject::tr("%1 MB").arg(value >> 20);
  }

  if (value < 1024ull * 1024ull * 1024ull * 1024ull) {
    return QObject::tr("%1 GB").arg(value >> 30);
  }

  return QObject::tr("%1 TB").arg(value >> 40);
}

bool GetDirectoryDialog::checkFreeSpace(const QString &newDirectory, const QString &serviceName, const int size)
{
  using GGS::Core::UI::Message;

  QStorageInfo info = QStorageInfo(newDirectory);

  quint64 freeBytes = info.bytesAvailable();
  quint64 needBytes = (quint64)size << 20;

  if (needBytes < freeBytes)
    return true;

  QString needSize = formatSize(needBytes);
  QString freeSize = formatSize(freeBytes);

  QString title = QObject::tr("Недостаточно места на диске \"%1\"").arg(info.rootPath());
  QString message = QObject::tr("Для установки \"%1\" требуется не менее <b>%2</b>. На выбранном вами диске доступно <b>%3</b>. Выберите другой каталог или освободите место на диске.")
    .arg(serviceName)
    .arg(needSize)
    .arg(freeSize);

  return Message::Ignore == Message::warning(title, message, static_cast<Message::StandardButton>(Message::Ignore | Message::Cancel));
}
