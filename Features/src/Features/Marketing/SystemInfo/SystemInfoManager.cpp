#include <Features/Marketing/SystemInfo/SystemInfoManager.h>

#include <Features/Marketing/SystemInfo/SoftVendor/FourGame.h>
#include <Features/Marketing/SystemInfo/SoftVendor/Steam.h>
#include <Features/Marketing/SystemInfo/SoftVendor/Ubisoft.h>
#include <Features/Marketing/SystemInfo/SoftVendor/Origin.h>
#include <Features/Marketing/SystemInfo/SoftVendor/MailRu.h>
#include <Features/Marketing/SystemInfo/SoftVendor/ZZima.h>
#include <Features/Marketing/SystemInfo/SoftVendor/Nikita.h>
#include <Features/Marketing/SystemInfo/SoftVendor/Antivirus.h>

#include <Features/Marketing/SystemInfo/Hardware/CpuInfo.h>
#include <Features/Marketing/SystemInfo/Hardware/Ram.h>
#include <Features/Marketing/SystemInfo/Hardware/Hdd.h>
#include <Features/Marketing/SystemInfo/Hardware/Video.h>
#include <Features/Marketing/SystemInfo/Hardware/Screen.h>
#include <Features/Marketing/SystemInfo/Hardware/OsInfo.h>

#include <RestApi/Commands/Marketing/SetGnaInstallStep>

#include <Settings/Settings.h>
#include <Core/System/HardwareId.h>

using GGS::RestApi::Commands::Marketing::SetGnaInstallStep;
using GGS::RestApi::CommandBase;
using GGS::RestApi::GameNetCredential;
using GGS::RestApi::RestApiManager;
using GGS::Settings::Settings;


namespace Features {
  namespace Marketing {
    namespace SystemInfo {

      template <typename T>
      void writeStatNode(QXmlStreamWriter *writer) 
      {
        T t;
        t.write(writer);
      }

      SystemInfoManager::SystemInfoManager(QObject *parent /*= 0*/)
        : QObject(parent)
      {
      }

      SystemInfoManager::~SystemInfoManager()
      {
      }

      void SystemInfoManager::init()
      {
        QObject::connect(&this->_timer, &QTimer::timeout, this, &SystemInfoManager::send);
        this->_timer.start(3600000);
        QTimer::singleShot(300000, this, SLOT(send()));
      }

      void SystemInfoManager::setCredential(const GGS::RestApi::GameNetCredential& value)
      {
        this->_credential = value;
      }

      void SystemInfoManager::setMid(const QString& value)
      {
        this->_mid = value;
      }

      void SystemInfoManager::send()
      {
        if (!this->canSendStatistic())
          return;

        QString xml = this->getSystemInfo(this->_credential.userId());

        SetGnaInstallStep *cmd = new SetGnaInstallStep();
        cmd->setUserId(this->_credential.userId());
        cmd->setAppKey(this->_credential.appKey());
        cmd->setHwid(GGS::Core::System::HardwareId::value());
        cmd->setServiceId(QLatin1String("1"));
        cmd->setMarketingTarget(2761);
        cmd->setMarketingId(this->_mid);
        cmd->appendParameter("params", xml);

        QObject::connect(cmd, &SetGnaInstallStep::result, [this, cmd](CommandBase::CommandResults result) {
          cmd->deleteLater();
          if (result == GGS::RestApi::CommandBase::NoError)
            this->saveSendDate();
        });

        cmd->execute();
      }

      QString SystemInfoManager::getSystemInfo(const QString& userId)
      {
        QByteArray resultBuffer;
        QXmlStreamWriter writer(&resultBuffer);
        writer.setAutoFormatting(false);
        writer.setCodec("UTF-8");
        writer.writeStartElement("root");

        if (!userId.isEmpty()) {
          writer.writeStartElement("gamenet");
          writer.writeTextElement("user", userId);
          writer.writeEndElement();
        }

        this->writeSystemInfo(writer);
        this->writeSoftwareVendors(writer);

        writer.writeEndDocument();

        return QString(resultBuffer);
      }

      void SystemInfoManager::writeSoftwareVendors(QXmlStreamWriter &writer)
      {
        using namespace Features::Marketing::SystemInfo::SoftVendor;
        writer.writeStartElement("vendors");

        writeStatNode<FourGame>(&writer);
        writeStatNode<Steam>(&writer);
        writeStatNode<Ubisoft>(&writer);
        writeStatNode<Origin>(&writer);
        writeStatNode<MailRu>(&writer);
        writeStatNode<ZZima>(&writer);
        writeStatNode<Nikita>(&writer);
        writeStatNode<Antivirus>(&writer);

        writer.writeEndElement(); // vendors
      }

      void SystemInfoManager::writeSystemInfo(QXmlStreamWriter &writer)
      {
        using namespace Features::Marketing::SystemInfo::Hardware;

        writer.writeStartElement("system");

        writeStatNode<CpuInfo>(&writer);
        writeStatNode<Ram>(&writer);
        writeStatNode<Hdd>(&writer);
        writeStatNode<Video>(&writer);
        writeStatNode<Screen>(&writer);
        writeStatNode<OsInfo>(&writer);

        writer.writeEndElement(); // system
      }

      bool SystemInfoManager::canSendStatistic()
      {
        QDateTime sendDate = this->loadSendDate();
        return !sendDate.isValid() || (sendDate < QDateTime::currentDateTime().addDays(-7));
      }

      void SystemInfoManager::saveSendDate()
      {
        ::Settings settings;
        settings.beginGroup("Features");
        settings.beginGroup("Marketing");
        settings.beginGroup("SystemInfo");
        settings.setValue("sendDate", QDateTime::currentDateTime());
      }

      QDateTime SystemInfoManager::loadSendDate()
      {
        ::Settings settings;
        settings.beginGroup("Features");
        settings.beginGroup("Marketing");
        settings.beginGroup("SystemInfo");
        return settings.value("sendDate", QDateTime()).toDateTime();
      }

    }
  }
}