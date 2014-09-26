#include <Fixtures/GameDownloadServiceFixture.h>

#include <Core\Service.h>

GameDownloadServiceFixture::GameDownloadServiceFixture()
  : _service(0)
{

}

GameDownloadServiceFixture::~GameDownloadServiceFixture()
{

}

void GameDownloadServiceFixture::setService(GGS::Core::Service *service)
{
  this->_service = service;
}

void GameDownloadServiceFixture::emitStarted(GGS::GameDownloader::StartType startType)
{
  emit started(this->_service, startType);
}

void GameDownloadServiceFixture::emitFinished()
{
  emit finished(this->_service);
}

void GameDownloadServiceFixture::emitStopped()
{
  emit stopped(this->_service);
}

void GameDownloadServiceFixture::emitStopping()
{
  emit stopping(this->_service);
}

void GameDownloadServiceFixture::emitFailed()
{
  emit failed(this->_service);
}

void GameDownloadServiceFixture::emitShutdownCompleted()
{
  emit shutdownCompleted();
}

void GameDownloadServiceFixture::emitServiceInstalled()
{
  emit serviceInstalled(this->_service);
}

void GameDownloadServiceFixture::emitServiceUpdated()
{
  emit serviceUpdated(this->_service);
}

void GameDownloadServiceFixture::emitStatusMessageChanged(const QString &statusMsg)
{
  emit statusMessageChanged(this->_service, statusMsg);
}

void GameDownloadServiceFixture::emitProgressChanged(quint8 progress)
{
  emit progressChanged(this->_service->id(), progress);
}

void GameDownloadServiceFixture::emitProgressDownloadChanged(qint8 progress, GGS::Libtorrent::EventArgs::ProgressEventArgs args)
{
  emit progressDownloadChanged(this->_service->id(), progress, args);
}

void GameDownloadServiceFixture::emitProgressExtractionChanged(qint8 progress, qint64 current, qint64 total)
{
  emit progressExtractionChanged(this->_service->id(), progress, current, total);
}

void GameDownloadServiceFixture::emitTotalProgressChanged(quint8 progress)
{
  emit totalProgressChanged(this->_service, progress);
}

void GameDownloadServiceFixture::emitDownloadProgressChanged(quint8 progress, GGS::Libtorrent::EventArgs::ProgressEventArgs args)
{
   emit downloadProgressChanged(this->_service, progress, args);
}

void GameDownloadServiceFixture::emitListeningPortChanged(unsigned short port)
{
  emit listeningPortChanged(port);
}
