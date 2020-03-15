#include "geoserver.h"
#include "threadpool.h"
#include "time.h"
#include "user.h"
#include "gmock/gmock.h"

using namespace std;
using namespace testing;

class AGeoServer : public Test
{
public:
  GeoServer server;

  const string aUser{"auser"};
  const double LocationTolerance{0.005};
};

TEST_F(AGeoServer, TracksAUser)
{
  server.track(aUser);

  ASSERT_TRUE(server.isTracking(aUser));
}

TEST_F(AGeoServer, IsNotTrackingAUserNotTracked)
{
  ASSERT_FALSE(server.isTracking(aUser));
}

TEST_F(AGeoServer, TracksMultipleUsers)
{
  server.track(aUser);
  server.track("anotheruser");

  ASSERT_FALSE(server.isTracking("thirduser"));
  ASSERT_TRUE(server.isTracking(aUser));
  ASSERT_TRUE(server.isTracking("anotheruser"));
}

TEST_F(AGeoServer, IsTrackingAnswersFalseWhenUserNoLongerTracked)
{
  server.track(aUser);
  server.stopTracking(aUser);

  ASSERT_FALSE(server.isTracking(aUser));
}

TEST_F(AGeoServer, UpdatesLocationOfUser)
{
  server.track(aUser);
  server.updateLocation(aUser, Location{38, -104});

  auto location = server.locationOf(aUser);
  // DOUBLES_EQUAL(38, location.latitude(), LocationTolerance);
  ASSERT_THAT(location.latitude(), DoubleNear(38, LocationTolerance));
  // DOUBLES_EQUAL(-104, location.longitude(), LocationTolerance);
  ASSERT_THAT(location.longitude(), DoubleNear(-104, LocationTolerance));
}

TEST_F(AGeoServer, AnswersUnknownLocationForUserNotTracked)
{
  ASSERT_TRUE(server.locationOf("anAbUser").isUnknown());
}

TEST_F(AGeoServer, AnswersUnknownLocationForTrackedUserWithNoLocationUpdate)
{
  server.track(aUser);
  ASSERT_TRUE(server.locationOf(aUser).isUnknown());
}

TEST_F(AGeoServer, AnswersUnknownLocationForUserNoLongerTracked)
{
  server.track(aUser);
  server.updateLocation(aUser, Location(40, 100));
  server.stopTracking(aUser);
  ASSERT_TRUE(server.locationOf(aUser).isUnknown());
}

template <typename TFrom, typename TTo>
std::vector<TTo> Collect(const std::vector<TFrom> &source,
                         std::function<TTo(TFrom)> func)
{
  std::vector<TTo> results;
  std::transform(source.begin(),
                 source.end(),
                 std::back_inserter(results),
                 func);
  return results;
}

class AGeoServerUserInBox : public Test
{
public:
  GeoServer server;

  const double TenMeters{10};
  const double Width{2000 + TenMeters};
  const double Height{4000 + TenMeters};
  const string aUser{"auser"};
  const string bUser{"buser"};
  const string cUser{"cuser"};

  Location aUserLocation{38, -103};

  // moved to here to refactor
  class GeoServerUserTrackingListener : public GeoServerListener
  {
  public:
    void updated(const User &user) { users.push_back(user); }

    vector<User> users;
  } trackingListener;

  // The GeoServer tests AnswersUsersInSpecifiedRange and
  // AnswersOnlyUsersWithinSpecifiedRange must still work. But if we use a
  // ThreadPool, we’ll need to introduce waits in our tests, like the ones
  // we coded in ThreadPoolTest. Instead, we choose to introduce a test
  // double that reduces the ThreadPool to a single-threaded
  // implementation of the add function.
  //
  // So makt it synchronous.

  class SingleThreadedPool : public ThreadPool
  {
  public:
    virtual void add(Work work) override { work.execute(); }
  };

  shared_ptr<ThreadPool> pool;

  void SetUp() override
  {
    pool = make_shared<SingleThreadedPool>();
    server.useThreadPool(pool);

    server.track(aUser);
    server.track(bUser);
    server.track(cUser);

    server.updateLocation(aUser, aUserLocation);
  }

  vector<string> UserNames(const vector<User> &user)
  {
    return Collect<User, string>(user, [](User each) { return each.name(); });
  }
};

#if 0
TEST_F(AGeoServerUserInBox, AnswersUsersInSpecifiedRange) {
   server.updateLocation(
      bUser, Location{aUserLocation.go(Width / 2 - TenMeters, East)}); 
 
   auto users = server.usersInBox(aUser, Width, Height);
 
   ASSERT_THAT(UserNames(users), ElementsAre(bUser));
}
#endif

// c9/13/GeoServerTest.cpp

TEST_F(AGeoServerUserInBox, AnswersUsersInSpecifiedRange)
{
  // class GeoServerUserTrackingListener: public GeoServerListener
  // {
  //     public:
  //         void updated(const User &user)
  //         { users.push_back(user); }

  //         vector<User> users;
  // } trackingListener;

  pool->start(0);

  server.updateLocation(
    bUser,
    Location{aUserLocation.go(Width / 2 - TenMeters, East)});

  // auto users = server.usersInBox(aUser, Width, Height, &trackingListener);
  server.usersInBox(aUser, Width, Height, &trackingListener);

  ASSERT_THAT(UserNames(trackingListener.users), ElementsAre(bUser));
}

TEST_F(AGeoServerUserInBox, AnswersOnlyUsersWithinSpecifiedRange)
{
  // class GeoServerUserTrackingListener: public GeoServerListener
  // {
  //     public:
  //         void updated(const User &user)
  //         { users.push_back(user); }

  //         vector<User> users;
  // } trackingListener;

  pool->start(0);

  server.updateLocation(
    bUser,
    Location{aUserLocation.go(Width / 2 + TenMeters, East)});
  server.updateLocation(
    cUser,
    Location{aUserLocation.go(Width / 2 - TenMeters, East)});

  // auto users = server.usersInBox(aUser, Width, Height);
  server.usersInBox(aUser, Width, Height, &trackingListener);

  ASSERT_THAT(UserNames(trackingListener.users), ElementsAre(cUser));
}

// [ RUN      ] AGeoServerUserInBox.HandlesLargeNumbersOfUsers
// time taken for UpdateLocation is 468076
// time taken for UpdateLocation is 468
// time taken for UpdateLocation is 468.076
// time taken for UpdateLocation is 468
// time taken for UsersInBox is 127232
// time taken for UsersInBox is 127
// time taken for UsersInBox is 127.232
// time taken for UsersInBox is 127
// [       OK ] AGeoServerUserInBox.HandlesLargeNumbersOfUsers (671 ms)

TEST_F(AGeoServerUserInBox, DISABLED_HandlesLargeNumbersOfUsers)
{

  const unsigned int lots{500000};

  // (two map look up and assign)*500000 times
  {
    TestTimer timer("UpdateLocation");

    Location anotherLocation{aUserLocation.go(10, West)};
    for (unsigned int i{0}; i < lots; i++)
    {
      string user{"user" + to_string(i)};
      server.track(user);
      server.updateLocation(user, anotherLocation);
    }
  }

  // one look up and loops on 500000 map.
  {
    TestTimer timer("UsersInBox");
    // auto users = server.usersInBox(aUser, Width, Height);
    // ASSERT_THAT(lots, Eq(users.size()));
  }
}

#if 0

// c9/17/GeoServerTest.cpp
// TEST_GROUP_BASE(AGeoServer_ScaleTests, GeoServerUsersInBoxTests) {

class AGeoServer_ScaleTests : public Test
{
   class GeoServerCountingListener: public GeoServerListener {
   public:
      void updated(const User& user) override {
         unique_lock<std::mutex> lock(mutex_);
         Count++;
         wasExecuted_.notify_all();
      }

      void waitForCountAndFailOnTimeout(unsigned int expectedCount, 
            const milliseconds& time=milliseconds(10000)) {
         unique_lock<mutex> lock(mutex_);
         CHECK_TRUE(wasExecuted_.wait_for(lock, time, [&] 
                  { return expectedCount == Count; }));
      }
      condition_variable wasExecuted_;
      unsigned int Count{0};
      mutex mutex_;
   };
   GeoServerCountingListener countingListener;
   shared_ptr<thread> t;

   void setup() override {
      pool = make_shared<ThreadPool>();
      GeoServerUsersInBoxTests::setup();
   }

   void teardown() override {
      t->join();
   }
};
#endif

int main(int argc, char **argv)
{
  testing::InitGoogleMock(&argc, argv);
  return RUN_ALL_TESTS();
}
