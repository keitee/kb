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
#include "threadpool.h"

class GeoServerListener
{
    public:
        virtual void updated(const User &user) = 0;
};


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

        // std::vector<User> usersInBox(
        void usersInBox(
                const std::string& user, double widthInMeters, 
                double heightInMeters,
                GeoServerListener *listener=nullptr) const;

        // c9/15/GeoServer.h
        void useThreadPool(std::shared_ptr<ThreadPool> pool);

    private:
        std::unordered_map<std::string, Location> locations_;
        std::unordered_map<std::string, Location>::const_iterator
            find(const std::string &user) const;

        std::shared_ptr<ThreadPool> pool_;
};

#endif // __GEOSERVER_H__
