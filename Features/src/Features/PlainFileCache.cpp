#include <Features/PlainFileCache.h>
#include <QtCore/QUrl>
#include <QtCore/QUrlQuery>
#include <QtCore/QCoreApplication>
#include <QtCore/QStringList>
#include <QtCore/QFile>
#include <QtCore/QDir>

namespace Features {

  PlainFileCache::PlainFileCache(QObject* parent /*= 0*/)
    : QObject(parent)
  {
  }

  PlainFileCache::~PlainFileCache()
  {

  }

  bool PlainFileCache::tryGet(const QString& key, QString& value)
  {
    QString cachePath = QString("%1/Cache").arg(QCoreApplication::applicationDirPath());
    QDir cacheDir(cachePath);
    QFileInfoList files = cacheDir.entryInfoList(QStringList() << "*", QDir::Files | QDir::NoSymLinks);

    QUrl url(key);
    QUrlQuery query(url);
    QList<QPair<QString, int> > cacheList;

    for (const auto& fi : files) {
      const QString& fileName(fi.fileName());
      QString request = QString("http://a.a/?%1").arg(fileName);
      QUrl fileUrl(request);
      QUrlQuery fileQuery(fileUrl);
      bool hit = true;
      int count = 0;
      for (const auto& pair : fileQuery.queryItems()) {
        QString v = query.queryItemValue(pair.first);
        if (v != pair.second) {
          hit = false;
          break;
        }

        ++count;
      }

      if (hit)
        cacheList << qMakePair(fi.absoluteFilePath(), count);
    }

    std::sort(cacheList.begin(), cacheList.end(), [](const QPair<QString, int> &a, const QPair<QString, int> &b) -> bool {
      return b.second < a.second;
    });

    if (!cacheList.isEmpty()) {
      QFile file(cacheList.at(0).first);
      
      if (file.open(QFile::ReadOnly | QFile::Unbuffered)) {
        QByteArray data = file.readAll();
        value = QString::fromUtf8(data);
        return true;
      }
    }

    return false;
  }

  void PlainFileCache::put(const QString& key, const QString& value, int timeInSec)
  {
  }

}
