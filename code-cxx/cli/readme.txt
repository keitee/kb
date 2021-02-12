readline
=============================================================================
simple/
simple cli that provides simple mapping between <command string, function>

readline_qt/
use gnu readline library to implement CLI under Qt and this is a reference
code.

readline/
same as readline_qt but do it without Qt

readline_poll/
use gnu readline library to implement CLI and use epoll

the reference are:
AppInfrastructure/Common/lib/source/PollLoop.cpp
AppInfrastructure/ReadLine/source/ReadLine.cpp
