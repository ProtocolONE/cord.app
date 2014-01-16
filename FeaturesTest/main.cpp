#include <Features/Thetta/TlsInitializer.h>
#include <Features/Thetta/Protector.h>

#include <Settings/Settings>

#include <gtest/gtest.h>

#include <QtCore/QCoreApplication>

int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);
    testing::InitGoogleTest(&argc, argv);

    MemoryProtector_CheckFunction;
    MemoryProtector_CheckFunction2(4788, 28426, 26374, 35950);
    MemoryProtector_CheckFunction3(0x0A18C78A, 0x171D48F4, 0x0D860760, 0x37a14d6a);
    MemoryProtector_CheckFunction3(0x0A08C78A, 0x171DD8F4, 0x0DD60760, 0x37e12d6a);
    MemoryProtector_CheckFunction4(0x4FEE102, 0x588FFDB0, 0x6CC7D53, 0x57d01d20);

    int result = RUN_ALL_TESTS();
    return result;
}
