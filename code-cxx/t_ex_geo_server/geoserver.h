//
// Modern C++ Programming with Test-Driven Development
// c9/1/GeoServerTest.cpp
//

#ifndef __GEOSERVER_H__
#define __GEOSERVER_H__

#include <string>
#include <vector>
#include <unordered_map>
#include "location.h"
#include "area.h"
#include "user.h"

// class User {
// public:
//    User(const std::string& name, Location location) 
//       : name_(name), location_(location) {}

//    std::string name() { return name_; }
//    Location location() { return location_; }

// private:
//    std::string name_;
//    Location location_;
// };

class GeoServer
{
    public:
        void track(const std::string &user);
        void stopTracking(const std::string &user);
        void updateLocation(const std::string &user, const Location &location);
        bool isTracking(const std::string &user) const;
        Location locationOf(const std::string &user) const;

        bool isDifferentUserInBounds(
         const std::pair<std::string, Location>& each,
         const std::string& user,
         const Area& box) const;

        std::vector<User> usersInBox(
                const std::string& user, double widthInMeters, double heightInMeters) const;

    private:
        std::unordered_map<std::string, Location> locations_;
        std::unordered_map<std::string, Location>::const_iterator
            find(const std::string &user) const;
};

#endif // __GEOSERVER_H__
