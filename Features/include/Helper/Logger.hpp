#pragma once

#include <QtCore/QString>

#include <Log4Qt/LogManager>
#include <Log4Qt/Level>
#include <Log4Qt/RollingFileAppender>
#include <Log4Qt/TTCCLayout>

namespace P1 {
  namespace Host {
    
    using namespace Log4Qt; 

    class LoggerHelper {
      TTCCLayout *layout;
      RollingFileAppender *appender;

    public:
      LoggerHelper(const QString& path) 
        : layout(nullptr)
        , appender(nullptr) 
      {
        auto q = LogManager::qtLogger()->logger();

        layout = new TTCCLayout(TTCCLayout::ISO8601);
        appender = new RollingFileAppender(layout, path, true);

        appender->setMaximumFileSize(1000000);
        appender->setMaxBackupIndex(1);
        appender->activateOptions();

        LogManager::qtLogger()->addAppender(appender);
        LogManager::setThreshold(Level::ALL_INT);

#ifndef _DEBUG
        LogManager::setHandleQtMessages(true);
#endif
      }

      LoggerHelper::~LoggerHelper() 
      {
        LogManager::qtLogger()->removeAllAppenders(); 
      }
    };

  }
}