#include "dbusfiledescriptor.h"
#include "rlog.h"

#include <errno.h>
#include <fcntl.h>
#include <unistd.h> // close()

/* ={--------------------------------------------------------------------------
 @brief :
  DBusFileDescriptor

  Light wrapper around a file descriptor so it can be used safely with dbus.

  Why do we need this? Because we want to safely pass a file descriptor around
  using the DBusMessage class.

  Why not just use an integer? Because although it's obviously fine to pass an
  integer around, the life time of the file descriptor can get lost.  This class
  uses dup(2) to ensure that if the object was created with a valid file
  descriptor in the first place then it and all copy constructed objects will
  have a valid file descriptor.

*/

DBusFileDescriptor::DBusFileDescriptor()
    : m_fd(-1)
{}

DBusFileDescriptor::DBusFileDescriptor(int fd)
    : m_fd(-1)
{
  if (fd >= 0)
  {
    m_fd = fcntl(fd, F_DUPFD_CLOEXEC, 3);
    if (m_fd < 0)
      logSysError(errno, "failed to dup the supplied fd");
  }
}

DBusFileDescriptor::~DBusFileDescriptor()
{
  clear();
}

bool DBusFileDescriptor::isValid() const
{
  return (m_fd >= 0);
}

// Returns the Unix file descriptor contained by this DBusFileDescriptor object.
// An invalid file descriptor is represented by the value -1.
//
// Note that the file descriptor returned by this function is owned by the
// DBusFileDescriptor object and must not be stored past the lifetime of this
// object. It is ok to use it while this object is valid, but if one wants to
// store it for longer use, the file descriptor should be cloned using the Unix
// dup(2), dup2(2) or dup3(2) functions.

int DBusFileDescriptor::fd() const
{
  return m_fd;
}

// close the file descriptor contained by this object. after this isValid() will
// return flase and fd() will return -1

void DBusFileDescriptor::clear()
{
  if ((m_fd >= 0) && (0 != close(m_fd)))
    logSysError(errno, "failed to close fd");

  m_fd = -1;
}
