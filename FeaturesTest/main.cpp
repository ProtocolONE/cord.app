#include <Features/Thetta/TlsInitializer.h>
#include <Features/Thetta/Protector.h>

#include <Settings/Settings.h>
#include <Settings/InitializeHelper.h>

#include <gtest/gtest.h>
#include <gmock/gmock.h>

#include <QtWidgets/QApplication>

#pragma optimize("", off)
void checkProtector()
{
  MemoryProtector_CheckFunction;
  MemoryProtector_CheckFunction2(4788, 28426, 26374, 35950);
  MemoryProtector_CheckFunction3(0x0A18C78A, 0x171D48F4, 0x0D860760, 0x37a14d6a);
  MemoryProtector_CheckFunction3(0x0A08C78A, 0x171DD8F4, 0x0DD60760, 0x37e12d6a);
  MemoryProtector_CheckFunction4(0x4FEE102, 0x588FFDB0, 0x6CC7D53, 0x57d01d20);

  MemoryProtector_CheckFunction1(0x90d7a5bd, 0x8b448e59, 0xb72c5e02, 0xD3489218);
  MemoryProtector_CheckFunction2(0x16a85745, 0xf9c46c0f, 0xf8a41a8d, 0x25F4C917);
}

#pragma optimize("", on)

bool initDatabase()
{
  GGS::Settings::InitializeHelper helper;
  helper.setFileName(QString("%1/featureTest.sql").arg(QCoreApplication::applicationDirPath()));
  if (!helper.init())
    return false;

  GGS::Settings::Settings::setCacheEnabled(true);
  return true;
}

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    QStringList plugins;
    QString path = QCoreApplication::applicationDirPath();

    plugins << path + "/plugins";
    a.setLibraryPaths(plugins);

    ::testing::InitGoogleMock(&argc, argv);
    ::testing::InitGoogleTest(&argc, argv);

    checkProtector();
    initDatabase();

    int result = RUN_ALL_TESTS();
    return result;
}
