#include <gmock/gmock.h>
#include <gtest/gtest.h>

#include <Host/Factory.h>

#include <QtCore/QObject>

using namespace GameNet::Host;

class MookHookBase
{
public:
  MookHookBase()
    : value(0)
  {
  }

  virtual ~MookHookBase(){}

  int value;
};

class StringHook1 : public MookHookBase
{
public:
  StringHook1()
  {
  }

  virtual ~StringHook1() {}

  static QString id() { return "{47DCE7DA-DF34-4E7F-BBAC-66336DB8A588}"; }

  QString name;
};

class StringHook2 : public MookHookBase
{
public:
  StringHook2()
  {
  }

  virtual ~StringHook2() {}

  static QString id() { return "{0E3BF6D1-249A-40B1-9D50-82D8EFACD34A}"; }

  int temperature;
};

TEST(FactoryTest, create)
{
  Factory<MookHookBase, QString> factory;
  factory.reg<StringHook1>();
  factory.reg<StringHook2>();

  MookHookBase *hook1 = factory.create(StringHook1::id());
  ASSERT_NE(nullptr, hook1);
  ASSERT_NE(nullptr, dynamic_cast<StringHook1*>(hook1));
  delete hook1;

  MookHookBase *hook2 = factory.create(StringHook2::id());
  ASSERT_NE(nullptr, hook2);
  ASSERT_NE(nullptr, dynamic_cast<StringHook2*>(hook2));
  delete hook2;
}

TEST(FactoryTest, doubleCreate)
{
  Factory<MookHookBase, QString> factory;
  factory.reg<StringHook1>();
  factory.reg<StringHook1>();

  MookHookBase *hook1 = factory.create(StringHook1::id());
  ASSERT_NE(nullptr, hook1);
  ASSERT_NE(nullptr, dynamic_cast<StringHook1*>(hook1));
  delete hook1;
}

TEST(FactoryTest, initializer)
{
  Factory<MookHookBase, QString> factory;
  factory.reg<StringHook1>([](StringHook1 *hook) {
    hook->value = 42;
    hook->name = "StringHook1";
  });

  factory.reg<StringHook2>([](StringHook2 *hook) {
    hook->value = 24;
    hook->temperature = -273;
  });

  MookHookBase *hook1 = factory.create(StringHook1::id());
  ASSERT_NE(nullptr, hook1);
  ASSERT_EQ(42, hook1->value);

  StringHook1 *realHook1 = dynamic_cast<StringHook1*>(hook1);
  ASSERT_NE(nullptr, realHook1);
  ASSERT_EQ("StringHook1", realHook1->name);

  delete hook1;

  MookHookBase *hook2 = factory.create(StringHook2::id());
  ASSERT_NE(nullptr, hook2);
  ASSERT_EQ(24, hook2->value);

  StringHook2 *realHook2 = dynamic_cast<StringHook2*>(hook2);
  ASSERT_NE(nullptr, realHook2);
  ASSERT_EQ(-273, realHook2->temperature);

  delete hook2;
}

TEST(FactoryTest, unknownHook)
{
  Factory<MookHookBase, QString> factory;
  factory.reg<StringHook1>();

  MookHookBase *hook2 = factory.create(StringHook2::id());
  ASSERT_EQ(nullptr, hook2);
}