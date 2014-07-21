#include <Helper/GetDirectoryDialog.h>

#include <Core/UI/Message>

#include <QtCore/QDir>
#include <QtCore/QDebug>
#include <QtCore/QDirIterator>

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

void GetDirectoryDialog::getDirectory(const QString& serviceName, const QString& defaultDir)
{
  QFileDialog *dialog = new QFileDialog(qobject_cast<QWidget*>(this->parent()), tr("CAPTION_OPEN_DIR"), defaultDir);
  dialog->setOptions(QFileDialog::ShowDirsOnly | QFileDialog::DontResolveSymlinks);
  dialog->setFileMode(QFileDialog::Directory);

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

    if (this->isRootFolder(newDirectory)) {
      this->directoryEntered(newDirectory + serviceName);
      return;
    }

    if (!this->isEmptyFolder(newDirectory)) {
      bool dontUseFolder = Message::Cancel == Message::question(
        tr("DIRECTORY_NOT_EMPTY_INFO"), 
        tr("DIRECTORY_NOT_EMPTY_QUESTION"), 
        static_cast<Message::StandardButton>(Message::Ok | GGS::Core::UI::Message::Cancel));

      if (dontUseFolder) {
        this->directoryEntered(QString());
        return;
      }
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
