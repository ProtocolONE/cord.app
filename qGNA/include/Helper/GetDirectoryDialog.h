#pragma once

#include <functional>

#include <QtCore/QObject>
#include <QtCore/QString>
#include <QtCore/QStringList>

/*!
  \class GetDirectoryDialog

  \brief Диалог для получения дирректории для установки игры с проверками на разрешенные дирректории.
*/
class GetDirectoryDialog : public QObject
{
  Q_OBJECT
public:
  explicit GetDirectoryDialog(QWidget *parent);
  ~GetDirectoryDialog();

  void getDirectory(const QString& serviceName, const int size, const QString& defaultDir);

signals:
 void directoryEntered(const QString & directory);

private:
  QString getFolderName(int type);
  QString removeNotExisting(const QString &);

  bool isDeniedFolder(const QString &newDirectory, QString *folder = 0);
  bool isAcceptedFolder(const QString &newDirectory, QString *folder = 0);
  bool isRootFolder(const QString &newDirectory);
  bool isEmptyFolder(const QString &newDirectory);
  bool checkFreeSpace(const QString &newDirectory, const QString &serviceName, const int size);
  QString formatSize(quint64 value);
};