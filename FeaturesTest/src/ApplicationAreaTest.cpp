#include <gmock/gmock.h>
#include <gtest/gtest.h>

#include <Helper/ApplicationArea.hpp>

#include <QtCore/QObject>

using GGS::ApplicationArea;

TEST(ApplicationAreaTest, isValid)
{
  ApplicationArea defaultArea;
  ASSERT_EQ(ApplicationArea::SupportedArea::Live, defaultArea);
}

TEST(ApplicationAreaTest, isTestVersion)
{
  ApplicationArea areaLive(ApplicationArea::SupportedArea::Live);
  ApplicationArea areaPts(ApplicationArea::SupportedArea::Pts);
  ApplicationArea areaTst(ApplicationArea::SupportedArea::Tst);

  ASSERT_FALSE(areaLive.isTestVersion());
  ASSERT_FALSE(areaPts.isTestVersion());
  ASSERT_TRUE(areaTst.isTestVersion());
}

TEST(ApplicationAreaTest, initFromSupportedArea)
{
  ApplicationArea area(ApplicationArea::SupportedArea::Pts);
  ASSERT_EQ(1, area);
}

TEST(ApplicationAreaTest, stringCast)
{
  ApplicationArea areaLive(ApplicationArea::SupportedArea::Live);
  ASSERT_TRUE(std::string("live") == std::string(areaLive));

  ApplicationArea areaPts(ApplicationArea::SupportedArea::Pts);
  ASSERT_TRUE(std::string("pts") == std::string(areaPts));

  ApplicationArea areaTst(ApplicationArea::SupportedArea::Tst);
  ASSERT_TRUE(std::string("tst") == std::string(areaTst));

  ApplicationArea areaTst2(ApplicationArea::SupportedArea::Tst2);
  ASSERT_TRUE(std::string("tst2") == std::string(areaTst2));

  ApplicationArea areaTst3(ApplicationArea::SupportedArea::Tst3);
  ASSERT_TRUE(std::string("tst3") == std::string(areaTst3));
}

