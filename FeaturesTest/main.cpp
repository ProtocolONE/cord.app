#include <Settings/Settings.h>
#include <Settings/InitializeHelper.h>

#include <gtest/gtest.h>
#include <gmock/gmock.h>

#include <QtWidgets/QApplication>


bool initDatabase()
{
  P1::Settings::InitializeHelper helper;
  helper.setFileName(QString("%1/featureTest.sql").arg(QCoreApplication::applicationDirPath()));
  if (!helper.init())
    return false;

  P1::Settings::Settings::setCacheEnabled(true);
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

    initDatabase();

    int result = RUN_ALL_TESTS();
    return result;
}
