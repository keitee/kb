#include "gmock/gmock.h"
#include "geoserver.h"
#include "user.h"
#include "time.h"

using namespace std;
using namespace testing;

class AGeoServer : public Test
{
    public:
        GeoServer server;

        const string aUser{"auser"};
        const double LocationTolerance{0.005};
};

TEST_F(AGeoServer, TracksAUser) {
   server.track(aUser);

   ASSERT_TRUE(server.isTracking(aUser));
}

TEST_F(AGeoServer, IsNotTrackingAUserNotTracked) {
   ASSERT_FALSE(server.isTracking(aUser));
}

TEST_F(AGeoServer, TracksMultipleUsers) {
   server.track(aUser);
   server.track("anotheruser");

   ASSERT_FALSE(server.isTracking("thirduser"));
   ASSERT_TRUE(server.isTracking(aUser));
   ASSERT_TRUE(server.isTracking("anotheruser"));
}

TEST_F(AGeoServer, IsTrackingAnswersFalseWhenUserNoLongerTracked) {
   server.track(aUser);
   server.stopTracking(aUser);

   ASSERT_FALSE(server.isTracking(aUser));
}

TEST_F(AGeoServer, UpdatesLocationOfUser) {
   server.track(aUser);
   server.updateLocation(aUser, Location{38, -104});

   auto location = server.locationOf(aUser);
   // DOUBLES_EQUAL(38, location.latitude(), LocationTolerance);
   ASSERT_THAT(location.latitude(), DoubleNear(38, LocationTolerance));
   // DOUBLES_EQUAL(-104, location.longitude(), LocationTolerance);
   ASSERT_THAT(location.longitude(), DoubleNear(-104, LocationTolerance));
}

TEST_F(AGeoServer, AnswersUnknownLocationForUserNotTracked) {
   ASSERT_TRUE(server.locationOf("anAbUser").isUnknown());
}

TEST_F(AGeoServer, AnswersUnknownLocationForTrackedUserWithNoLocationUpdate) {
   server.track(aUser);
   ASSERT_TRUE(server.locationOf(aUser).isUnknown());
}

TEST_F(AGeoServer, AnswersUnknownLocationForUserNoLongerTracked) {
   server.track(aUser);
   server.updateLocation(aUser, Location(40, 100));
   server.stopTracking(aUser);
   ASSERT_TRUE(server.locationOf(aUser).isUnknown());
}


template<typename TFrom, typename TTo> std::vector<TTo> Collect(
      const std::vector<TFrom>& source,
      std::function<TTo(TFrom)> func) {
   std::vector<TTo> results;
   std::transform(source.begin(), source.end(), std::back_inserter(results), func);
   return results;
}

class AGeoServerUserInBox: public Test
{
    public:
        GeoServer server;

        const double TenMeters { 10 };
        const double Width { 2000 + TenMeters };
        const double Height { 4000 + TenMeters};
        const string aUser { "auser" };
        const string bUser { "buser" };
        const string cUser { "cuser" };

        Location aUserLocation { 38, -103 };

        void SetUp() override
        {
            server.track(aUser);
            server.track(bUser);
            server.track(cUser);

            server.updateLocation(aUser, aUserLocation);
        }

        vector<string> UserNames(const vector<User> &user)
        {
            return Collect<User, string>(user, 
                    [](User each) {return each.name(); });
        }
};


// TEST_F(AGeoServerUserInBox, AnswersUsersInSpecifiedRange) {
//    server.updateLocation(
//       bUser, Location{aUserLocation.go(Width / 2 - TenMeters, East)}); 
// 
//    auto users = server.usersInBox(aUser, Width, Height);
// 
//    ASSERT_THAT(UserNames(users), ElementsAre(bUser));
// }

TEST_F(AGeoServerUserInBox, AnswersUsersInSpecifiedRange) 
{
    class GeoServerUserTrackingListener: public GeoServerListener
    {
        public:
            void updated(const User &user)
            { users.push_back(user); }

            vector<User> users;
    } trackingListener;

    server.updateLocation(
            bUser, Location{aUserLocation.go(Width / 2 - TenMeters, East)}); 

    // auto users = server.usersInBox(aUser, Width, Height, &trackingListener);
    server.usersInBox(aUser, Width, Height, &trackingListener);

    ASSERT_THAT(UserNames(trackingListener.users), ElementsAre(bUser));
}

TEST_F(AGeoServerUserInBox, AnswersOnlyUsersWithinSpecifiedRange) 
{
    class GeoServerUserTrackingListener: public GeoServerListener
    {
        public:
            void updated(const User &user)
            { users.push_back(user); }

            vector<User> users;
    } trackingListener;

    server.updateLocation(
            bUser, Location{aUserLocation.go(Width / 2 + TenMeters, East)}); 
    server.updateLocation(
            cUser, Location{aUserLocation.go(Width / 2 - TenMeters, East)}); 

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

TEST_F(AGeoServerUserInBox, DISABLED_HandlesLargeNumbersOfUsers) {

    const unsigned int lots {500000};

    // (two map look up and assign)*500000 times 
    {
        TestTimer timer("UpdateLocation");

        Location anotherLocation{aUserLocation.go(10, West)};
        for (unsigned int i{0}; i < lots; i++) {
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


int main(int argc, char** argv)
{
    testing::InitGoogleMock(&argc, argv);
    return RUN_ALL_TESTS();
}

