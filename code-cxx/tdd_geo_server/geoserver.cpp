//
// Modern C++ Programming with Test-Driven Development
// c9/1/GeoServerTest.cpp
//
#include "geoserver.h"
#include "user.h"

void GeoServer::track(const std::string &user)
{
    // Q?
    locations_[user] = Location();
}

void GeoServer::stopTracking(const std::string &user)
{
    locations_.erase(user);
}

bool GeoServer::isTracking(const std::string &user) const 
{
    return locations_.find(user) != locations_.end();
}

void GeoServer::updateLocation(const std::string &user, const Location &location)
{
    locations_[user] = location;
}

Location GeoServer::locationOf(const std::string &user) const
{
    if (!isTracking(user))
        return Location{};

    return find(user)->second;
}

std::unordered_map<std::string, Location>::const_iterator
GeoServer::find(const std::string &user) const
{
    return locations_.find(user);
}

bool GeoServer::isDifferentUserInBounds(
        const std::pair<std::string, Location> &each,
        const std::string &user,
        const Area &box) const
{
    if( each.first == user )
        return false;

    return box.inBounds(each.second);
}


// std::vector<User> GeoServer::usersInBox(
void GeoServer::usersInBox(
        const std::string &user, double widthInMeters, 
        double heightInMeters,
        GeoServerListener *listener) const
{
    auto location = locations_.find(user)->second;

    Area box{location, widthInMeters, heightInMeters};

    // std::vector<User> users;

    // for( auto &each : locations_ )
    //     if( isDifferentUserInBounds(each, user, box) )
    //     {
    //         // users.push_back( User{each.first, each.second} );
    //         if (listener)
    //             listener->updated(User{each.first, each.second});
    //     }

    // c9/15/GeoServer.cpp
    for( auto &each : locations_ )
    {
        Work work{ [&] 
            {
                if( isDifferentUserInBounds(each, user, box) )
                {
                    // users.push_back( User{each.first, each.second} );
                    if (listener)
                        listener->updated(User{each.first, each.second});
                }
            }};
        pool_->add(work);
    }

    // NOTE:
    // As always, we seek incremental change, leaving in place the logic that
    // directly returns a vector of users. This allows us to prove our idea
    // before wasting a lot of time applying a similar implementation to other
    // tests.

    // return users;
}

// c9/15/GeoServer.cpp

void GeoServer::useThreadPool(std::shared_ptr<ThreadPool> pool)
{
    pool_ = pool;
}
