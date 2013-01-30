#pragma once

#include <Features/GameDownloader/DownloadStats.h>

class DownloadStatsPub : public Features::GameDownloader::DownloadStats
{
public:
  explicit DownloadStatsPub(QObject *parent = 0);
  ~DownloadStatsPub();

public:
  using DownloadStats::_id;

  using DownloadStats::_initialized;
  using DownloadStats::_downloadTime;
  using DownloadStats::_applicationRestartCount;
  using DownloadStats::_applicationRestartCountChanged;
  using DownloadStats::_maxDownloadSpeed;
  using DownloadStats::_maxUploadSpeed;
  using DownloadStats::_totalSize;

  using DownloadStats::save;
};

