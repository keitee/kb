cpp-handlers
============

Demo project showing off C++ terminate and signal handlers.

Read the article at http://www.tiwoc.de/blog/2013/12/globally-handling-uncaught-exceptions-and-signals-in-c/ for details!

Daniel Seither  Freelance Software Engineer, co-founder of Kullo

Free Software Publications Impressum Blog 

Globally handling uncaught exceptions and signals in C++

If you have a sementation fault in your C++ program, the program crashes and
this is printed to standard error:

Segmentation fault (core dumped)

It tells you that a core dump has been created if your system was configured to
do so. You can use it to further debug the crash, but when you have a quick look
at the error message, you learn near to nothing about the cause of the crash.

Similarly, when there is an uncaught exception in your code, the following is
printed (again to standard error):

terminate called after throwing an instance of 'std::runtime_error'
what():  Hello, world!
Aborted (core dumped)

Again, you can use the core dump, but why not let the program print a stack
trace to help you find the location where the segfault or exception occurred and
the call chain that lead to it, just as many other languages do?

Environment

The following was tested with gcc 4.8.2 running under Fedora 19 and I’m not sure
how portable this all is. Just try it on the platform(s) of your choice 😉

Printing a stack trace

gcc/glibc provides us with backtrace(), backtrace_symbols(), and
backtrace_symbols_fd() from execinfo.h, which can be used to generate a binary
stack trace (backtrace()) and convert it to a nicely formatted array of strings
(backtrace_symbols()) or directly print it to a file descriptor, e.g. stderr
(backtrace_symbols_fd()).

This is how you do it in code, with a maximum number of 20 entries in your stack
trace:

void *array[20];
size_t size = backtrace(array, sizeof(array) / sizeof(array[0]));
backtrace_symbols_fd(array, size, STDERR_FILENO);
The result looks something like that:

./handlertest(_Z15printStacktracev+0x14)[0x401374]
./handlertest(_Z13signalHandleri+0x1e)[0x4013ae]
/lib64/libc.so.6[0x369c435a60]
./handlertest(main+0x36)[0x4011c6]
/lib64/libc.so.6(__libc_start_main+0xf5)[0x369c421b45]
./handlertest[0x401291]

You can see that the method names are mangled, but at least you can approximate
the location of the error.

For this to work properly, you need to call g++ with "-rdynamic", so that the
linker exports all symbols. But how do we print the stack trace when our program
crashes? Enter signal and terminate handlers…

Adding a signal handler

To catch segmentation faults, you can register a signal handler that is called
when your programm receives a SIGSEGV signal (segmentation violation, code 11).
This is done through signal() from signal.h:

void signalHandler(int sig) { /* print stacktrace */ }
...
signal(SIGSEGV, signalHandler);

Adding a terminate handler

When an exception in a C++ program is not caught by try-catch, std::terminate()
  is called before the program aborts. In fact, the default implementation
  simply calls abort(), but you can do something meaningful beforehand. This is
  how you register a terminate handler with std::set_terminate as defined by the
  exception header:

void terminateHandler() { /* print stacktrace */ }
...
std::set_terminate(terminateHandler);

Accessing the uncaught exception from the terminate handler

Now you get a chance to print a stack trace on termination, but you probably
also want to print the exception that caused the crash. This caused some
headache, but finally I got the following:

std::exception_ptr exptr = std::current_exception();
try {
    std::rethrow_exception(exptr);
}
catch (std::exception &ex) {
    std::fprintf(stderr, "Terminated due to exception: %s\n", ex.what());
}

The solution uses the std::current_exception() method introduced by C++11
(compile with -std=c++0x or -std=c++11). It returns an exception_ptr object that
has only one meaningful use: It can be rethrown, which we do to finally catch
and process it.

Putting it all together

I built a small demo project that first installs a signal handler and a
terminate handler and then crashes the program randomly either by dereferencing
a null pointer to cause a segfault, or by throwing an uncaught exception.



Jun 19 20:08:52 skyxione asrdkplayer[22498]: [AAMP-PLAYER]ConfigurePlayerModeSettings:9698 Player Mode :: JS Player
Jun 19 20:08:52 skyxione asrdkplayer[22498]: Terminated due to exception: basic_string::_M_construct null not valid
Jun 19 20:08:52 skyxione asrdkplayer[22498]: /usr/sbin/asrdkplayer[0x12a28]
Jun 19 20:08:52 skyxione asrdkplayer[22498]: /usr/sbin/asrdkplayer[0x12b40]
Jun 19 20:08:52 skyxione asrdkplayer[22498]: /usr/lib/libstdc++.so.6(+0x74f14)[0xb65f6f14]
Jun 19 20:08:52 skyxione asrdkplayer[22498]: [AAMP-PLAYER]PrivateInstanceAAMP::SetLiveOffset(), liveoffset 30

