#pragma once

#include <memory>

class IPollLoop;

// since it's interface class and will be polymorphic base and should be ABC
// otherwise, should have ctor/dtor. if not, see errors like:
//
// readline_poll.cpp.o: In function `IPollSource::~IPollSource()':
// readline_poll/ipoll.h:11: undefined reference to `vtable for IPollSource'
// readline_poll.cpp.o: In function `IPollSource::IPollSource()':
// readline_poll/ipoll.h:7: undefined reference to `vtable for IPollSource'
// readline_poll.cpp.o:(.data.rel.ro._ZTI8ReadLine[_ZTI8ReadLine]+0x18): undefined reference to `typeinfo for IPollSource'

class IPollSource
{
public:
  virtual ~IPollSource()                = default;
  virtual void process(const std::shared_ptr<IPollLoop> &poll,
                       uint32_t events) = 0;
};

class IPollLoop
{
public:
  virtual ~IPollLoop() = default;

public:
  virtual bool start(int priority = -1) = 0;
  virtual void stop()                   = 0;

  virtual bool addSource(const std::string &name,
                         const std::shared_ptr<IPollSource> &source,
                         int fd,
                         uint32_t events)                            = 0;
  virtual bool modSource(const std::shared_ptr<IPollSource> &source,
                         uint32_t events)                            = 0;
  virtual bool delSource(const std::shared_ptr<IPollSource> &source) = 0;
};
