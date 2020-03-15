#include "work.h"
#include <gmock/gmock.h>
// #include <sstream>

using namespace testing;

// keep test naming like TEST(WorkTest, DefaultsFunctionToNullObject) as the
// original text
//
// How about "CanXXX" naming?

TEST(WorkTest, DefaultsFunctionToNullObject)
{
  Work work;
  work.execute();
}

TEST(WorkTest, DefaultsFunctionToNullObjectWhenConstructedWithId)
{
  Work work(1);
  work.execute();
}

TEST(WorkTest, CanBeConstructedWithAnId)
{
  Work work{10};
  EXPECT_THAT(work.id(), 10);
}

TEST(WorkTest, DefaultsIdTo0)
{
  Work work;
  EXPECT_THAT(work.id(), 0);
}

TEST(WorkTest, DefaultsIdTo0WhenFunctionSpecified)
{
  Work work{[] {}};
  EXPECT_THAT(work.id(), 0);
}

TEST(WorkTest, CanBeConstructedWithAFunctionAndId)
{
  Work work{[] {}, 10};
  EXPECT_THAT(work.id(), 10);
}

TEST(WorkTest, ExecutesFunctionStored)
{
  bool isExecuted{false};
  Work work{[&] { isExecuted = true; }};
  work.execute();
  EXPECT_THAT(isExecuted, true);
}

TEST(WorkTest, CanExecuteOnDataCapturedWithFunction)
{
  std::vector<std::string> data{"a", "b"};
  std::string result{};

  auto callback = [&](std::string s) { result.append(s); };

  auto execute = [&]() {
    std::stringstream ss;
    ss << data[0] << data[1];
    callback(ss.str());
  };

  Work work(execute);
  work.execute();

  EXPECT_THAT(result, "ab");
}
