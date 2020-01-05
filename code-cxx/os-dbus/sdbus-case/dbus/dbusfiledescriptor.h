#ifndef DBUSFILEDESCRIPTOR_H
#define DBUSFILEDESCRIPTOR_H

class DBusFileDescriptor
{
  private:
    int m_fd;

  public:
    DBusFileDescriptor();
    explicit DBusFileDescriptor(int fd);
    ~DBusFileDescriptor();

    bool isValid() const;
    int fd() const;

    // since clear() is the same as reset() but is added to follow c++ naming
    // convention, then use clear() only as reset() is for smart pointers.
    // void reset();

    void clear();
};

#endif // DBUSFILEDESCRIPTOR_H
