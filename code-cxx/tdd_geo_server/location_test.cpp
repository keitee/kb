#include "geoserver.h"
#include "location.h"
#include "gmock/gmock.h"

using namespace std;
using namespace testing;

class ALocation : public Test
{
public:
  const double Tolerance{0.005};
  const Location ArbitraryLocation{38.2, -104.5};
};

TEST_F(ALocation, AnswersLatitudeAndLongitude)
{
  Location location{10, 20};

  ASSERT_THAT(location.latitude(), Eq(10));
  ASSERT_THAT(location.longitude(), Eq(20));
}

TEST_F(ALocation, IsNotUnknownWhenLatitudeAndLongitudeProvided)
{
  Location location{1, 1};

  ASSERT_FALSE(location.isUnknown());
}

TEST_F(ALocation, IsUnknownWhenLatitudeAndLongitudeNotProvided)
{
  Location location;

  ASSERT_TRUE(location.isUnknown());
}

TEST_F(ALocation, AnswersDistanceFromAnotherInMeters)
{
  Location point1{38.017, -104.84};
  Location point2{38.025, -104.99};

  // verified at www.ig.utexas.edu/outreach/googleearth/latlong.html
  // DOUBLES_EQUAL(13170, point1.distanceInMeters(point2), 5);
  ASSERT_THAT(point1.distanceInMeters(point2), DoubleNear(13170, 5));
}

TEST_F(ALocation, IsNotEqualToAnotherWhenLatDiffers)
{
  Location point1{10, 11};
  Location point2{11, 11};

  ASSERT_TRUE(point1 != point2);
}

TEST_F(ALocation, IsNotEqualToAnotherWhenLongDiffers)
{
  Location point1{10, 11};
  Location point2{10, 12};

  ASSERT_TRUE(point1 != point2);
}

TEST_F(ALocation, IsNotEqualToAnotherWhenLatAndLongMatch)
{
  Location point1{10, 11};
  Location point2{10, 11};

  ASSERT_TRUE(point1 == point2);
}

TEST_F(ALocation, AnswersNewLocationGivenDistanceAndBearing)
{
  Location start{0, 0};

  auto newLocation = start.go(MetersPerDegreeAtEquator, East);

  Location expectedEnd{0, 1};
  // DOUBLES_EQUAL(1, newLocation.longitude(), Tolerance);
  ASSERT_THAT(newLocation.longitude(), DoubleNear(1, Tolerance));
  // DOUBLES_EQUAL(0, newLocation.latitude(), Tolerance);
  ASSERT_THAT(newLocation.latitude(), DoubleNear(0, Tolerance));
}

TEST_F(ALocation, AnswersNewLocationGivenDistanceAndBearingVerifiedByHaversine)
{
  double distance{100};
  Location start{38, -78};

  auto end = start.go(distance, 35);

  // DOUBLES_EQUAL(distance, start.distanceInMeters(end), Tolerance);
  ASSERT_THAT(start.distanceInMeters(end), DoubleNear(distance, Tolerance));
}

TEST_F(ALocation, CanBeAPole)
{
  Location start{90, 0};

  auto end = start.go(MetersPerDegreeAtEquator, South);

  // DOUBLES_EQUAL(0, end.longitude(), Tolerance);
  ASSERT_THAT(end.longitude(), DoubleNear(0, Tolerance));
  // DOUBLES_EQUAL(89, end.latitude(), Tolerance);
  ASSERT_THAT(end.latitude(), DoubleNear(89, Tolerance));
}

TEST_F(ALocation, IsVeryCloseToAnotherWhenSmallDistanceApart)
{
  Location threeMetersAway{ArbitraryLocation.go(3, South)};

  ASSERT_TRUE(ArbitraryLocation.isVeryCloseTo(threeMetersAway));
}

TEST_F(ALocation, IsNotVeryCloseToAnotherWhenNotSmallDistanceApart)
{
  Location fourMetersAway{ArbitraryLocation.go(4, South)};

  ASSERT_FALSE(ArbitraryLocation.isVeryCloseTo(fourMetersAway));
}

TEST_F(ALocation, ProvidesPrintableRepresentation)
{
  Location location{-32, -105};
  stringstream s;

  s << location;

  ASSERT_THAT(s.str(), StrEq("(-32,-105)"));
}

// int main(int argc, char** argv)
// {
//     testing::InitGoogleMock(&argc, argv);
//     return RUN_ALL_TESTS();
// }
