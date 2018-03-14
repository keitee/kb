#include <iostream>
#include <set>
#include <memory>
#include <exception>

#include "gtest/gtest.h"
#include "gmock/gmock.h"

using namespace std;

class WeatherException : public exception
{
public:
        const char *what() const throw()
        {
            return "weather exception";
        }
};

class WeatherStation
{
public:
    virtual ~WeatherStation(){};
 
    // typedef enum
    // {
    //     North, South, East, West
    // } Direction;
 
    // typedef enum
    // {
    //     Optimistic, Pessimistic
    // } Outlook;

    enum Direction
    {
        North, South, East, West
    };
 
    enum Outlook
    {
        Optimistic, Pessimistic
    };
 
    // NB Semantics on wind deliberately ugly to show a neat feature in gmock
    virtual void wind( Direction* pDirection, double* strength ) const = 0;
    virtual double rainfall() const = 0;
    virtual std::string prediction( Outlook outlook ) const = 0;
    virtual std::string snow( Outlook outlook ) const = 0;
};
 
class UserInterface
{
public:
    UserInterface( const std::shared_ptr<WeatherStation>& weather_station ) :
        weather_station_( weather_station )
    {
    }
 
    typedef enum
    {
        Heavy, Medium, Light
    } Range;
 
    Range rain()
    {
        auto rainfall = weather_station_->rainfall();
        if ( 0.0 <= rainfall && rainfall < 2.0 ) return Light;
        else if ( 2.0 <= rainfall && rainfall < 4.0 ) return Medium;
        else return Heavy;
    }
 
    Range wind()
    {
        WeatherStation::Direction direction;
        double strength;
        weather_station_->wind( &direction, &strength );
 
        if ( 0.0 <= strength && strength < 5.0 ) return Light;
        else if ( 5.0 <= strength && strength < 10.0 ) return Medium;
        else return Heavy;
    }
 
    std::pair<std::string, std::string> predict_range()
    {
        return std::make_pair( 
            weather_station_->prediction( WeatherStation::Optimistic ),
            weather_station_->prediction( WeatherStation::Pessimistic ) );
    }

    std::string snow()
    {
        try {
            return weather_station_->snow(WeatherStation::Pessimistic);
            // return weather_station_->snow(WeatherStation::Optimistic);
        } catch(WeatherException &e)
        {
            cout << "snow: exception: " << e.what() << endl;
            return "snow exception";
        }
    }
 
private:
    std::shared_ptr<WeatherStation> weather_station_;
};
 
using namespace testing;
 
class MockWeatherStation : public WeatherStation
{
public:
    MOCK_CONST_METHOD0( rainfall, double() );
    MOCK_CONST_METHOD2( wind, void(WeatherStation::Direction*, double*) );
    MOCK_CONST_METHOD1( prediction, std::string( WeatherStation::Outlook ) );
    MOCK_CONST_METHOD1( snow, std::string( WeatherStation::Outlook ) );
};
 
TEST( WeatherStationUserInterface, rain_should_be_heavy )
{
    auto weather_station = std::make_shared<MockWeatherStation>();
    // GMock: specify a simple return value using Return(x)
    EXPECT_CALL( *weather_station, rainfall() )
        .WillRepeatedly( Return(5.0) );
    UserInterface ui( weather_station );
    EXPECT_EQ( UserInterface::Heavy, ui.rain() );
}
 
TEST( WeatherStationUserInterface, wind_should_be_light )
{
    auto weather_station = std::make_shared<MockWeatherStation>();
    // GMock: specify out parameter values using SetArgPointee
    EXPECT_CALL( *weather_station, wind(_,_) )
        .WillRepeatedly( DoAll( SetArgPointee<0>( WeatherStation::North ),
                          SetArgPointee<1>( 0.5 )) );
    UserInterface ui( weather_station );
    EXPECT_EQ( UserInterface::Light, ui.wind() );
}
 
TEST( WeatherStationUserInterface, predictions_are_displayed )
{
    auto weather_station = std::make_shared<MockWeatherStation>();

    // GMock: inject more complex logic using C++11 lambdas,
    // and pattern match on the input value
    EXPECT_CALL( *weather_station, prediction(WeatherStation::Optimistic) )
        .WillRepeatedly( Invoke( []( WeatherStation::Outlook x) -> std::string
            {
                return "Sunny";
            }) );

    EXPECT_CALL( *weather_station, prediction(WeatherStation::Pessimistic) )
        .WillRepeatedly( Invoke( []( WeatherStation::Outlook x) -> std::string
            {
                return "Overcast";
            }) );
 
    UserInterface ui( weather_station );
    auto predicted_range = ui.predict_range();
    EXPECT_EQ( "Sunny", predicted_range.first );
    EXPECT_EQ( "Overcast", predicted_range.second );
}

TEST( WeatherStationUserInterface, DISABLED_snow_exception )
{
    auto weather_station = std::make_shared<MockWeatherStation>();

    // GMock: inject more complex logic using C++11 lambdas,
    // and pattern match on the input value
    EXPECT_CALL( *weather_station, snow(_) )
        .WillRepeatedly(Throw(WeatherException()));
 
    UserInterface ui( weather_station );
    EXPECT_EQ( "snow exception", ui.snow() );
}

// ={=========================================================================
// test MATCHER_P

class RetweetCollection
{
    public:
        RetweetCollection() {}

        bool isEmpty() const
        {
            return 0 == size();
        }

        unsigned int size() const 
        {
            return 0;
        }
};

class ARetweetCollection : public Test 
{
    public:
        RetweetCollection collection;
};

MATCHER_P(HasSize, expected, "")
{
    return arg.size() == expected 
        && arg.isEmpty() == (0 == expected); 
}

TEST_F(ARetweetCollection, MatcherWithSingleArgument)
{
    EXPECT_THAT(collection, HasSize(0u));
}


// ={=========================================================================
typedef std::pair<std::string, bool> PairReturn;

ostream &operator<<(ostream &os, const PairReturn &p)
{
    os << "out" << endl;
}

bool ComparePair(const PairReturn &m, const PairReturn &n) 
{
    return m == n ? true : false;

    // if ( (get<0>(m) == get<0>(n)) && (get<1>(m) == get<1>(n)))
    //     return true;

    // return (n%m == 0) ? true : false;
}

TEST(AssertTest, AssertPredicate)
{
    // PairReturn p1 = make_pair({"KIDS", "NEWS"}, false);
    // PairReturn p2 = make_pair({"KIDS", "NEWS"}, false);
    // PairReturn p3 = make_pair({"KIDS", "NEWS"}, true);

    PairReturn p1 = std::pair<std::string, bool>({"KIDS", "NEWS"}, false);
    PairReturn p2 = PairReturn({"KIDS", "NEWS"}, false);
    PairReturn p3 = PairReturn({"KIDS", "NEWS"}, true);

    // auto weather_station = std::make_shared<MockWeatherStation>();

    // // GMock: inject more complex logic using C++11 lambdas,
    // // and pattern match on the input value
    // EXPECT_CALL( *weather_station, snow(_) )
    //     .WillRepeatedly(Throw(WeatherException()));

    EXPECT_PRED2(ComparePair, p1, p2);
    // EXPECT_PRED2(ComparePair, p1, p3);
}


int main(int argc, char **argv)
{
    std::cout << "Running main() from gmock_main.cc\n";

    // Since Google Mock depends on Google Test, InitGoogleMock() is
    // also responsible for initializing Google Test.  Therefore there's
    // no need for calling testing::InitGoogleTest() separately.
    testing::InitGoogleMock(&argc, argv);
    return RUN_ALL_TESTS();
}
