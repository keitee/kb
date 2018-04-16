#include "gmock/gmock.h"
#include "work.h"

using namespace std;
using namespace testing;

class AWorkObject : public Test
{
};

TEST_F(AWorkObject, DefaultsFunctionToNullObject) {
   Work work;
   try {
      work.execute();
   }
   catch(...) {
      cout << "unable to execute function";
   }
}

TEST_F(AWorkObject, DefaultsFunctionToNullObjectWhenConstructedWithId) {
   Work work(1);
   try {
      work.execute();
   }
   catch(...) {
      // FAIL("unable to execute function");
      cout << "unable to execute function";
   }
}

TEST_F(AWorkObject, CanBeConstructedWithAnId) {
   Work work(1);
   ASSERT_THAT(work.id(), Eq(1));
}

TEST_F(AWorkObject, DefaultsIdTo0) {
   Work work;
   ASSERT_THAT(work.id(), Eq(0));
}

TEST_F(AWorkObject, DefaultsIdTo0WhenFunctionSpecified) {
   Work work{[]{}};
   ASSERT_THAT(work.id(), Eq(0));
}

TEST_F(AWorkObject, CanBeConstructedWithAFunctionAndId) {
   Work work{[]{}, 1};
   ASSERT_THAT(work.id(), Eq(1));
}

TEST_F(AWorkObject, ExecutesFunctionStored) {
   bool wasExecuted{false};
   auto executeFunction = [&] { wasExecuted = true; };
   Work work(executeFunction);
   work.execute();
   ASSERT_TRUE(wasExecuted);
}

TEST_F(AWorkObject, CanExecuteOnDataCapturedWithFunction) {
   vector<string> data{"a", "b"};
   string result;

   auto callbackFunction = [&](string s) {
      result.append(s);
   };

   auto executeFunction = [&] {
      stringstream s;
      s << data[0] << data[1];
      callbackFunction(s.str());
   };

   Work work(executeFunction);
   work.execute();

   ASSERT_THAT(result, Eq("ab"));
}

