#include "gmock/gmock.h"
#include <curl/curl.h>
#include <fstream>
#include <iostream>
#include <jsoncpp/json/json.h>

using namespace std;
using namespace testing;

/*
// ={=========================================================================
https://curl.haxx.se/libcurl/c/curl_easy_perform.html
https://curl.haxx.se/libcurl/c/curl_easy_cleanup.html
https://curl.haxx.se/changes.html

sudo apt-get install libcurl4-openssl-dev


// -{-------------------------------------------------------------------------
Everything curl - the book
Everything curl is an extensive, detailed and totally free book, available in
multiple formats.

https://ec.haxx.se/libcurl/libcurl-drive/libcurl-drive-easy

       -o, --output <file>
              Write  output to <file> instead of stdout. 

Verbose

If your curl command does not execute or return what you expected it to, your
first gut reaction should always be to run the command with the -v / --verbose
option to get more information.

When verbose mode is enabled, curl gets more talkative and will explain and show
a lot more of its doings. It will add informational tests and prefix them with
'*'. For example, let's see what curl might say when trying a simple HTTP
example (saving the downloaded data in the file called 'saved'):

$ curl -v http://example.com -o saved
* Rebuilt URL to: http://example.com/

Ok so we invoked curl with a URL that it considers incomplete so it helps us and
it adds a trailing slash before it moves on.

    *   Trying 93.184.216.34...

This tells us curl now tries to connect to this IP address. It means the name
'example.com' has been resolved to one or more addresses and this is the first
(and possibly only) address curl will try to connect to.

    * Connected to example.com (93.184.216.34) port 80 (#0)

It worked! curl connected to the site and here it explains how the name maps to
the IP address and on which port it has connected to. The '(#0)' part is which
internal number curl has given this connection. If you try multiple URLs in the
same command line you can see it use more connections or reuse connections, so
the connection counter may increase or not increase depending on what curl
decides it needs to do.

If we use an HTTPS:// URL instead of an HTTP one, there will also be a whole
bunch of lines explaining how curl uses CA certs to verify the server's
certificate and some details from the server's certificate, etc. Including which
ciphers were selected and more TLS details.

In addition to the added information given from curl internals, the -v verbose
mode will also make curl show all headers it sends and receives. For protocols
without headers (like FTP, SMTP, POP3 and so on), we can consider commands and
responses as headers and they will thus also be shown with -v.

If we then continue the output seen from the command above (but ignore the
actual HTML response), curl will show:

    > GET / HTTP/1.1
    > Host: example.com
    > User-Agent: curl/7.45.0
    > Accept: *//*
    >

This is the full HTTP request to the site. This request is how it looks in a
default curl 7.45.0 installation and it may, of course, differ slightly between
different releases and in particular it will change if you add command line
options.

The last line of the HTTP request headers looks empty, and it is. It signals the
separation between the headers and the body, and in this request there is no
"body" to send.

Moving on and assuming everything goes according to plan, the sent request will
get a corresponding response from the server and that HTTP response will start
with a set of headers before the response body:

< HTTP/1.1 200 OK
< Accept-Ranges: bytes
< Cache-Control: max-age=604800
< Content-Type: text/html
< Date: Sat, 19 Dec 2015 22:01:03 GMT
< Etag: "359670651"
< Expires: Sat, 26 Dec 2015 22:01:03 GMT
< Last-Modified: Fri, 09 Aug 2013 23:54:35 GMT
< Server: ECS (ewr/15BD)
< Vary: Accept-Encoding
< X-Cache: HIT
< x-ec-custom-error: 1
< Content-Length: 1270
<

This may look mostly like mumbo jumbo to you, but this is normal set of HTTP
headers—metadata—about the response. The first line's "200" might be the most
important piece of information in there and means "everything is fine".

The last line of the received headers is, as you can see, empty, and that is the
marker used for the HTTP protocol to signal the end of the headers.

After the headers comes the actual response body, the data payload. The regular
-v verbose mode does not show that data but only displays

{ [1270 bytes data]

That 1270 bytes should then be in the 'saved' file. You can also see that there
was a header named Content-Length: in the response that contained the exact file
length (it will not always be present in responses).


HTTP/2 and HTTP/3

When doing file transfers using version two or three of the HTTP protocol, curl
sends and receives compressed headers. So to display outgoing and incoming
HTTP/2 and HTTP/3 headers in a readable and understandable way, curl will
actually show the uncompressed versions in a style similar to how they appear
with HTTP/1.1.

Silence

The opposite of verbose is, of course, to make curl more silent. With the -s (or
--silent) option you make curl switch off the progress meter and not output any
error messages for when errors occur. It gets mute. It will still output the
downloaded data you ask it to.

With silence activated, you can ask for it to still output the error message on
failures by adding -S or --show-error.


Trace options

There are times when -v is not enough. In particular, when you want to store the
complete stream including the actual transferred data.

For situations when curl does encrypted file transfers with protocols such as
HTTPS, FTPS or SFTP, other network monitoring tools (like Wireshark or tcpdump)
  will not be able to do this job as easily for you.

For this, curl offers two other options that you use instead of -v.

--trace [filename] will save a full trace in the given file name. You can also
use '-' (a single minus) instead of a file name to get it passed to stdout. You
would use it like this:

$ curl --trace dump http://example.com

When completed, there's a 'dump' file that can turn out pretty sizable. In this
case, the 15 first lines of the dump file looks like:


== Info: Rebuilt URL to: http://example.com/
== Info:   Trying 93.184.216.34...
== Info: Connected to example.com (93.184.216.34) port 80 (#0)
=> Send header, 75 bytes (0x4b)
0000: 47 45 54 20 2f 20 48 54 54 50 2f 31 2e 31 0d 0a GET / HTTP/1.1..
0010: 48 6f 73 74 3a 20 65 78 61 6d 70 6c 65 2e 63 6f Host: example.co
0020: 6d 0d 0a 55 73 65 72 2d 41 67 65 6e 74 3a 20 63 m..User-Agent: c
0030: 75 72 6c 2f 37 2e 34 35 2e 30 0d 0a 41 63 63 65 url/7.45.0..Acce
0040: 70 74 3a 20 2a 2f 2a 0d 0a 0d 0a                pt: *//*....
<= Recv header, 17 bytes (0x11)
0000: 48 54 54 50 2f 31 2e 31 20 32 30 30 20 4f 4b 0d HTTP/1.1 200 OK.
0010: 0a                                              .
<= Recv header, 22 bytes (0x16)
0000: 41 63 63 65 70 74 2d 52 61 6e 67 65 73 3a 20 62 Accept-Ranges: b
0010: 79 74 65 73 0d 0a                               ytes..

Every single sent and received byte get displayed individually in hexadecimal
numbers.

If you think the hexadecimals are not helping, you can try --trace-ascii
[filename] instead, also this accepting '-' for stdout and that makes the 15
first lines of tracing look like:

== Info: Rebuilt URL to: http://example.com/
== Info:   Trying 93.184.216.34...
== Info: Connected to example.com (93.184.216.34) port 80 (#0)
=> Send header, 75 bytes (0x4b)
0000: GET / HTTP/1.1
0010: Host: example.com
0023: User-Agent: curl/7.45.0
003c: Accept: *//*
0049:
<= Recv header, 17 bytes (0x11)
0000: HTTP/1.1 200 OK
<= Recv header, 22 bytes (0x16)
0000: Accept-Ranges: bytes
<= Recv header, 31 bytes (0x1f)
0000: Cache-Control: max-age=604800


--trace-time

This options prefixes all verbose/trace outputs with a high resolution timer for
when the line is printed. It works with the regular -v / --verbose option as
well as with --trace and --trace-ascii.

An example could look like this:

$ curl -v --trace-time http://example.com
23:38:56.837164 * Rebuilt URL to: http://example.com/
23:38:56.841456 *   Trying 93.184.216.34...
23:38:56.935155 * Connected to example.com (93.184.216.34) port 80 (#0)
23:38:56.935296 > GET / HTTP/1.1
23:38:56.935296 > Host: example.com
23:38:56.935296 > User-Agent: curl/7.45.0
23:38:56.935296 > Accept: *//*
23:38:56.935296 >
23:38:57.029570 < HTTP/1.1 200 OK
23:38:57.029699 < Accept-Ranges: bytes
23:38:57.029803 < Cache-Control: max-age=604800
23:38:57.029903 < Content-Type: text/html
---- snip ----

The lines are all the local time as hours:minutes:seconds and then number of
microseconds in that second.


HTTP response codes

An HTTP transfer gets a 3 digit response code back in the first response line.
The response code is the server's way of giving the client a hint about how the
request was handled.  It is important to note that curl does not consider it an
error even if the response code would indicate that the requested document could
not be delivered (or similar). curl considers a successful sending and receiving
of HTTP to be good.  The first digit of the HTTP response code is a kind of
"error class":

1xx: transient response, more is coming
2xx: success
3xx: a redirect
4xx: the client asked for something the server could not or would not deliver
5xx: there's problem in the server

Remember that you can use curl's --write-out option to extract the response
code. See the --write-out section.


// -{-------------------------------------------------------------------------
libcurl basics

Global initialization

Before you do anything libcurl related in your program, you should do a global
libcurl initialize call with curl_global_init(). This is necessary because some
underlying libraries that libcurl might be using need a call ahead to get setup
and initialized properly.

curl_global_init() is, unfortunately, not thread safe, so you must ensure that
you only do it once and never simultaneously with another call. It initializes
global state so you should only call it once, and once your program is
completely done using libcurl you can call curl_global_cleanup() to free and
clean up the associated global resources the init call allocated.

libcurl is built to handle the situation where you skip the curl_global_init()
call, but it does so by calling it itself instead (if you did not do it before
any actual file transfer starts) and it then uses its own defaults. But beware
that it is still not thread safe even then, so it might cause some "interesting"
side effects for you. It is much better to call curl_global_init() yourself in a
controlled manner.


Drive with easy

The name 'easy' was picked simply because this is really the easy way to use
libcurl, and with easy, of course, comes a few limitations. Like, for example,
that it can only do one transfer at a time and that it does the entire transfer
"in a single function call and returns once it is completed":

res = curl_easy_perform( easy_handle );

If the server is slow, if the transfer is large or if you have some unpleasant
timeouts in the network or similar, this function call can end up taking a long
time. You can, of course, set timeouts to not allow it to spend more than N
seconds, but it could still mean a substantial amount of time depending on the
particular conditions.

If you want your application to do something else while libcurl is transferring
with the easy interface, you need to use multiple threads. If you want to do
multiple simultaneous transfers when using the easy interface, you need to
perform each of the transfers in its own thread.


Callbacks

Lots of operations within libcurl are controlled with the use of callbacks. A
callback is a function pointer provided to libcurl that libcurl then calls at
some point to get a particular job done.

Each callback has its specific documented purpose and it requires that you write
it with the exact function prototype to accept the correct arguments and return
the documented return code and return value so that libcurl will perform the way
you want it to.

Each callback option also has a "companion option" that sets the associated
"user pointer". This user pointer is a pointer that libcurl does not touch or
care about, but just passes on as an argument to the callback. This allows you
to, for example, pass in pointers to local data all the way through to your
callback function.

Unless explicitly stated in a libcurl function documentation, it is not legal to
invoke libcurl functions from within a libcurl callback.


Write data

https://curl.haxx.se/libcurl/c/CURLOPT_WRITEFUNCTION.html

The write callback is set with CURLOPT_WRITEFUNCTION:

curl_easy_setopt(handle, CURLOPT_WRITEFUNCTION, write_callback);

The write_callback function must match this prototype:

size_t write_callback(char *ptr, size_t size, size_t nmemb, void *userdata);

This callback function gets called by libcurl as soon as there is data "received"
that needs to be saved. ptr points to the delivered data, and the size of that
data is size multiplied with nmemb.

If this callback is not set, libcurl instead uses 'fwrite' by default.

The write callback will be passed as much data as possible in all invokes, but
it must not make any assumptions. It may be one byte, it may be thousands. The
maximum amount of body data that will be passed to the write callback is defined
in the curl.h header file: CURL_MAX_WRITE_SIZE (the usual default is 16KB). If
CURLOPT_HEADER is enabled for this transfer, which makes header data get passed
to the write callback, you can get up to CURL_MAX_HTTP_HEADER bytes of header
data passed into it. This usually means 100KB.

This function may be called with zero bytes data if the transferred file is
empty.

The data passed to this function will not be zero terminated! You cannot, for
example, use printf's %s operator to display the contents nor strcpy to copy it.

This callback "should return the number of bytes" actually taken care of. If that
number differs from the number passed to your callback function, it will signal
an error condition to the library. This will cause the transfer to get aborted
and the libcurl function used will return CURLE_WRITE_ERROR.

The user pointer passed in to the callback in the userdata argument is set with
CURLOPT_WRITEDATA:

curl_easy_setopt(handle, CURLOPT_WRITEDATA, custom_pointer);


Cleanup

In previous sections we have discussed how to setup handles and how to drive the
transfers. All transfers will, of course, end up at some point, either
successfully or with a failure.

easy handle

When the easy handle is done serving its purpose, you can close it. If you
intend to do another transfer, you are however advised to rather reuse the
handle rather than to close it and create a new one.

If you do not intend to do another transfer with the easy handle, you simply ask
libcurl to cleanup:

curl_easy_cleanup( easy_handle );


--libcurl

We actively encourage users to first try out the transfer they want to do with
the curl command-line tool, and once it works roughly the way you want it to,
    you append the --libcurl [filename] option to the command line and run it
    again.

The --libcurl command-line option will create a C program in the provided file
name. That C program is an application that uses libcurl to run the transfer you
just had the curl command-line tool do. 

There are some exceptions and it is not always a 100% match, but you will find
that it can serve as an excellent inspiration source for what libcurl options
you want or can use and what additional arguments to provide to them.

If you specify the filename as a single dash, as in --libcurl - you will get the
program written to stdout instead of a file.

As an example, we run a command to just get http://example.com:

curl http://example.com --libcurl example.c

This creates example.c in the current directory, looking similar to this:


$ curl http://example.com --libcurl -
<!doctype html>
<html>
<head>
    <title>Example Domain</title>

    <meta charset="utf-8" />
    <meta http-equiv="Content-type" content="text/html; charset=utf-8" />
    <meta name="viewport" content="width=device-width, initial-scale=1" />
    <style type="text/css">
    body {
        background-color: #f0f0f2;
        margin: 0;
        padding: 0;
        font-family: -apple-system, system-ui, BlinkMacSystemFont, "Segoe UI", "Open Sans", "Helvetica Neue", Helvetica, Arial, sans-serif;

    }
    div {
        width: 600px;
        margin: 5em auto;
        padding: 2em;
        background-color: #fdfdff;
        border-radius: 0.5em;
        box-shadow: 2px 3px 7px 2px rgba(0,0,0,0.02);
    }
    a:link, a:visited {
        color: #38488f;
        text-decoration: none;
    }
    @media (max-width: 700px) {
        div {
            margin: 0 auto;
            width: auto;
        }
    }
    </style>
</head>

<body>
<div>
    <h1>Example Domain</h1>
    <p>This domain is for use in illustrative examples in documents. You may use this
    domain in literature without prior coordination or asking for permission.</p>
    <p><a href="https://www.iana.org/domains/example">More information...</a></p>
</div>
</body>
</html>
/--------- Sample code generated by the curl command line tool ----------
 * All curl_easy_setopt() options are documented at:
 * https://curl.haxx.se/libcurl/c/curl_easy_setopt.html
 ------------------------------------------------------------------------/
#include <curl/curl.h>

int main(int argc, char *argv[])
{
  CURLcode ret;
  CURL *hnd;

  hnd = curl_easy_init();
  curl_easy_setopt(hnd, CURLOPT_BUFFERSIZE, 102400L);
  curl_easy_setopt(hnd, CURLOPT_URL, "http://example.com");
  curl_easy_setopt(hnd, CURLOPT_NOPROGRESS, 1L);
  curl_easy_setopt(hnd, CURLOPT_USERAGENT, "curl/7.58.0");
  curl_easy_setopt(hnd, CURLOPT_MAXREDIRS, 50L);
  curl_easy_setopt(hnd, CURLOPT_HTTP_VERSION, (long)CURL_HTTP_VERSION_2TLS);
  curl_easy_setopt(hnd, CURLOPT_TCP_KEEPALIVE, 1L);

  // Here is a list of options the curl code used that cannot get generated
  //    as source easily. You may select to either not use them or implement
  //    them yourself.

  // CURLOPT_WRITEDATA set to a objectpointer
  // CURLOPT_INTERLEAVEDATA set to a objectpointer
  // CURLOPT_WRITEFUNCTION set to a functionpointer
  // CURLOPT_READDATA set to a objectpointer
  // CURLOPT_READFUNCTION set to a functionpointer
  // CURLOPT_SEEKDATA set to a objectpointer
  // CURLOPT_SEEKFUNCTION set to a functionpointer
  // CURLOPT_ERRORBUFFER set to a objectpointer
  // CURLOPT_STDERR set to a objectpointer
  // CURLOPT_HEADERFUNCTION set to a functionpointer
  // CURLOPT_HEADERDATA set to a objectpointer

  ret = curl_easy_perform(hnd);

  curl_easy_cleanup(hnd);
  hnd = NULL;

  return (int)ret;
}
/---- End of sample code ----/
$


Header files

There is only ever one header your libcurl using application needs to include:

#include <curl/curl.h>

That file in turn includes a few other public header files but you can pretend
they do not exist. (Historically speaking, we started out slightly different but
    over time we have stabilized around this form of only using a single one for
    includes.)


curl easy options

You set options in the easy handle to control how that transfer is going to be
done, or in some cases you can actually set options and modify the transfer's
behavior while it is in progress. You set options with curl_easy_setopt() and
you provide the handle, the option you want to set and the argument to the
option. All options take exactly one argument and you must always pass exactly
three parameters to the curl_easy_setopt() calls.

Since the curl_easy_setopt() call accepts several hundred different options and
the various options accept a variety of different types of arguments, it is
important to read up on the specifics and provide exactly the argument type the
specific option supports and expects. Passing in the wrong type can lead to
unexpected side-effects or hard to understand hiccups.

The perhaps most important option that every transfer needs, is the URL. libcurl
cannot perform a transfer without knowing which URL it concerns so you must tell
it. The URL option name is CURLOPT_URL as all options are prefixed with CURLOPT_
and then the descriptive name—all using uppercase letters. An example line
setting the URL to get the "http://example.com" HTTP contents could look like:

CURLcode ret = curl_easy_setopt(easy, CURLOPT_URL, "http://example.com");

Again: this only sets the option in the handle. It will not do the actual
transfer or anything. It will just tell libcurl to copy the string and if that
works it returns OK.

It is, of course, good form to check the return code to see that nothing went
wrong.


Setting numerical options

Since curl_easy_setopt() is a vararg function where the 3rd argument can use
different types depending on the situation, normal C language type conversion
cannot be done. So you must make sure that you truly pass a 'long' and not an
'int' if the documentation tells you so. On architectures where they are the
same size, you may not get any problems but not all work like that. Similarly,
     for options that accept a 'curl_off_t' type, it is crucial that you pass in
     an argument using that type and no other.

Enforce a long:

curl_easy_setopt(handle, CURLOPT_TIMEOUT, 5L); // 5 seconds timeout
Enforce a curl_off_t:

curl_off_t no_larger_than = 0x50000;
curl_easy_setopt(handle, CURLOPT_MAXFILE_LARGE, no_larger_than);

Get handle options

No, there's no general method to extract the same information you previously set
with curl_easy_setopt()! If you need to be able to extract the information again
that you set earlier, then we encourage you to keep track of that data yourself
in your application.


CURLcode return codes

Many libcurl functions return a CURLcode. That's a special libcurl typedefed
variable for error codes. It returns CURLE_OK (which has the value zero) if
everything is fine and dandy and it returns a non-zero number if a problem was
detected. There are almost one hundred CURLcode errors in use, and you can find
them all in the curl/curl.h header file and documented in the libcurl-errors man
page.

You can convert a CURLcode into a human readable string with the
curl_easy_strerror() function—but be aware that these errors are rarely phrased
in a way that is suitable for anyone to expose in a UI or to an end user:

const char *str = curl_easy_strerror( error );
printf("libcurl said %s\n", str);

Another way to get a slightly better error text in case of errors is to set the
CURLOPT_ERRORBUFFER option to point out a buffer in your program and then
libcurl will store a related error message there before it returns an error:

curl error[CURL_ERROR_SIZE]; // needs to be at least this big
CURLcode ret = curl_easy_setopt(handle, CURLOPT_ERRORBUFFER, error);


Verbose operations

Okay, we just showed how to get the error as a human readable text as that is an
excellent help to figure out what went wrong in a particular transfer and often
explains why it can be done like that or what the problem is for the moment.

The next lifesaver when writing libcurl applications that everyone needs to know
about and needs to use extensively, at least while developing libcurl
applications or debugging libcurl itself, is to enable "verbose mode" with
CURLOPT_VERBOSE:

CURLcode ret = curl_easy_setopt(handle, CURLOPT_VERBOSE, 1L);

When libcurl is told to be verbose it will mention transfer-related details and
information to "stderr" while the transfer is ongoing. This is awesome to figure
out why things fail and to learn exactly what libcurl does when you ask it
different things. 

You can redirect the output elsewhere by changing stderr with CURLOPT_STDERR or
you can get even more info in a fancier way with the debug callback (explained
    further in a later section).


Trace everything

Verbose is certainly fine, "but sometimes you need more." libcurl also offers a
trace callback that in addition to showing you all the stuff the verbose mode
does, it also passes on all data sent and received so that your application gets
a full trace of everything.

The sent and received data passed to the trace callback is given to the callback
in its unencrypted form, which can be handy when working with TLS or SSH based
protocols when capturing the data off the network for debugging is not
practical.

When you set the CURLOPT_DEBUGFUNCTION option, you still need to have
CURLOPT_VERBOSE enabled but with the trace callback set libcurl will use that
callback instead of its internal handling.

The trace callback should match a prototype like this:

int my_trace(CURL *handle, curl_infotype type, char *ptr, size_t size,
             void *userp);

handle is the easy handle it concerns, type describes the particular data passed
to the callback (data in/out, header in/out, TLS data in/out and "text"), ptr
points to the data being size number of bytes. userp is the custom pointer you
set with CURLOPT_DEBUGDATA.

The data pointed to by ptr will not be zero terminated, but will be exactly of
the size as told by the size argument.

The callback must return 0 or libcurl will consider it an error and abort the
transfer.

On the curl web site, we host an example called debug.c that includes a simple
trace function to get inspiration from.

There are also additional details in the CURLOPT_DEBUGFUNCTION man page.


for C++ programmers

TBD

strings are C strings, not C++ string objects


// -{-------------------------------------------------------------------------
https://curl.se/libcurl/c/example.html

https://curl.se/libcurl/c/getinfo.html


// -{-------------------------------------------------------------------------
Desktop version:

/usr/include/x86_64-linux-gnu/curl

keitee@kit-ubuntu:/usr/include/x86_64-linux-gnu/curl$ ls
curl.h  curlver.h  easy.h  mprintf.h  multi.h  stdcheaders.h  system.h  typecheck-gcc.h

keitee@kit-ubuntu:/usr/include/x86_64-linux-gnu/curl$ ag LIBCURL
curlver.h
29:#define LIBCURL_COPYRIGHT "1996 - 2017 Daniel Stenberg, <daniel@haxx.se>."
33:#define LIBCURL_VERSION "7.58.0"
37:#define LIBCURL_VERSION_MAJOR 7
38:#define LIBCURL_VERSION_MINOR 58
39:#define LIBCURL_VERSION_PATCH 0
42:   parsing and comparions by programs. The LIBCURL_VERSION_NUM define will
60:#define LIBCURL_VERSION_NUM 0x073a00
71:#define LIBCURL_TIMESTAMP "2018-01-24"
75:  (LIBCURL_VERSION_NUM >= CURL_VERSION_BITS(x, y, z))

*/

/*
// ={=========================================================================
https://curl.haxx.se/libcurl/c/simple.html
*/

TEST(CxxCurl, example_simple)
{
  CURL *curl;
  CURLcode res;

  curl = curl_easy_init();
  if(curl) {
    curl_easy_setopt(curl, CURLOPT_URL, "https://example.com");
    /* example.com is redirected, so we tell libcurl to follow redirection */
    curl_easy_setopt(curl, CURLOPT_FOLLOWLOCATION, 1L);

    /* Perform the request, res will get the return code */
    res = curl_easy_perform(curl);
    /* Check for errors */
    if(res != CURLE_OK)
      fprintf(stderr, "curl_easy_perform() failed: %s\n",
          curl_easy_strerror(res));

    /* always cleanup */
    curl_easy_cleanup(curl);
  }
}

/*
// ={=========================================================================
https://curl.haxx.se/libcurl/c/url2file.html
*/
namespace url2file
{
  // The function fwrite() writes nmemb items of data, each size bytes long, to
  // the stream pointed to by stream, obtaining them from the location given by
  // ptr.

  static size_t write_data(void *ptr, size_t size, size_t nmemb, void *stream)
  {
    printf("size {%ld}, nemeb{%ld}\n", size, nmemb);
    printf("%s", (char *)ptr);

    // size_t written = fwrite(ptr, size, nmemb, (FILE *)stream);
    // return written;

    return size;
  }
};

// with vervose as orignial code
TEST(CxxCurl, example_url2file_1)
{
  using namespace url2file;

  CURL *curl_handle;
  static const char *pagefilename = "page.out";
  FILE *pagefile;

  curl_global_init(CURL_GLOBAL_ALL);
 
  /* init the curl session */ 
  curl_handle = curl_easy_init();
 
  /* set URL to get here */ 
  // curl_easy_setopt(curl_handle, CURLOPT_URL, argv[1]);
  curl_easy_setopt(curl_handle, CURLOPT_URL, "https://example.com");

  /* Switch on full protocol/debug output while testing */ 
  curl_easy_setopt(curl_handle, CURLOPT_VERBOSE, 1L);
 
  /* disable progress meter, set to 0L to enable it */ 
  curl_easy_setopt(curl_handle, CURLOPT_NOPROGRESS, 1L);
 
  /* send all data to this function  */ 
  curl_easy_setopt(curl_handle, CURLOPT_WRITEFUNCTION, write_data);
 
  /* get it! */ 
  curl_easy_perform(curl_handle);

  /* cleanup curl stuff */ 
  curl_easy_cleanup(curl_handle);
 
  curl_global_cleanup();
}

// same but without vervose
TEST(CxxCurl, example_url2file_2)
{
  using namespace url2file;

  CURL *curl_handle;

  curl_global_init(CURL_GLOBAL_ALL);
 
  /* init the curl session */ 
  curl_handle = curl_easy_init();
 
  /* set URL to get here */ 
  // curl_easy_setopt(curl_handle, CURLOPT_URL, argv[1]);
  curl_easy_setopt(curl_handle, CURLOPT_URL, "https://example.com");

  /* Switch on full protocol/debug output while testing */
  // curl_easy_setopt(curl_handle, CURLOPT_VERBOSE, 1L);

  /* disable progress meter, set to 0L to enable it */ 
  curl_easy_setopt(curl_handle, CURLOPT_NOPROGRESS, 1L);
 
  /* send all data to this function  */ 
  curl_easy_setopt(curl_handle, CURLOPT_WRITEFUNCTION, write_data);
 
  /* get it! */ 
  curl_easy_perform(curl_handle);

  /* cleanup curl stuff */
  curl_easy_cleanup(curl_handle);
 
  curl_global_cleanup();
}

/*
// ={=========================================================================
// the case example
// to get file from online
//
// curl -H "X-SkyOTT-Provider: SKY" -H "X-SkyOTT-Proposition: SKYQ" -H "X-SkyOTT-Territory: GB" http://atlantis.epg.bskyb.com/as/services/4101/1
//
// use url2file example and when run, gets an error:
//
// {"errorCode":"400","developerMessage":"Missing header(s): X-SkyOTT-Proposition, X-SkyOTT-Provider, X-SkyOTT-Territory"}
*/

TEST(CxxCurl, case_1)
{
  using namespace url2file;

  CURL *curl_handle;

  curl_global_init(CURL_GLOBAL_ALL);
 
  /* init the curl session */ 
  curl_handle = curl_easy_init();
 
  /* set URL to get here */ 
  // curl_easy_setopt(curl_handle, CURLOPT_URL, argv[1]);
  curl_easy_setopt(curl_handle,
                   CURLOPT_URL,
                   "http://atlantis.epg.bskyb.com/as/services/4101/1");

  /* Switch on full protocol/debug output while testing */
  curl_easy_setopt(curl_handle, CURLOPT_VERBOSE, 1L);

  /* disable progress meter, set to 0L to enable it */ 
  curl_easy_setopt(curl_handle, CURLOPT_NOPROGRESS, 1L);
 
  /* send all data to this function  */ 
  curl_easy_setopt(curl_handle, CURLOPT_WRITEFUNCTION, write_data);
 
  /* get it! */ 
  curl_easy_perform(curl_handle);

  /* cleanup curl stuff */ 
  curl_easy_cleanup(curl_handle);
 
  curl_global_cleanup();
}

/*
// ={=========================================================================

this gets error for some urls. is it url problem? 

curl_easy_perform() failed: {Error}
HTTP GET response is 200

added to get more error context

char error[CURL_ERROR_SIZE]; // needs to be at least this big
curl_easy_setopt(curl_handle, CURLOPT_ERRORBUFFER, error);

and gets:

curl_easy_perform() failed: {Error}
HTTP GET request http://atlantis.epgsky.com/as/services/4096/1 {Failed writing body (1434 != 497)} and response is 200


SO says:
https://stackoverflow.com/questions/9311782/failed-writing-body-in-libcurl
It means your write callback didn't return the same number of bytes as was passed into it!

so added debug on write_data:

Sep 28 20:30:48 skyxione com.sky.as.player[10990]: input {1, 5437} return {5437}
Sep 28 20:30:48 skyxione com.sky.as.player[10990]: input {1, 1434} return {1434}
Sep 28 20:30:48 skyxione com.sky.as.player[10990]: input {1, 1434} return {1434}
Sep 28 20:30:48 skyxione com.sky.as.player[10990]: input {1, 1434} return {1434}
Sep 28 20:30:48 skyxione com.sky.as.player[10990]: input {1, 2868} return {2868}
Sep 28 20:30:48 skyxione com.sky.as.player[10990]: input {1, 1434} return {1434}
Sep 28 20:30:48 skyxione com.sky.as.player[10990]: input {1, 1434} return {1434}
Sep 28 20:30:48 skyxione com.sky.as.player[10990]: input {1, 2868} return {2868}
Sep 28 20:30:48 skyxione com.sky.as.player[10990]: input {1, 2868} return {2868}
Sep 28 20:30:48 skyxione com.sky.as.player[10990]: input {1, 1434} return {1434}
Sep 28 20:30:48 skyxione com.sky.as.player[10990]: input {1, 1434} return {1434}
Sep 28 20:30:48 skyxione com.sky.as.player[10990]: input {1, 497} return {1434}
Sep 28 20:30:48 skyxione com.sky.as.player[10990]: curl_easy_perform() failed: {Error}
Sep 28 20:30:48 skyxione com.sky.as.player[10990]: HTTP GET request http://atlantis.epgsky.com/as/services/4096/1 {Failed writing body (1434 != 497)} and response is 200
 
*/

namespace curl_case
{
  // output from the command line run and use to check the result
  const std::string expected = R"JSON(
  {"documentId":"1600085171264","version":1,"services":[{"sid":"2002","c":"101","t":"BBC One Lon","xsg":3,"sg":3,"sf":"sd","dvbtriplet":"","sk":2002,"adult":false,"servicetype":"OTT","schedule":true,"startover":null,"startoverents":false,"timeshifted":false,"local":false,"servicetypes":["DSAT"]},{"sid":"2006","c":"102","t":"BBC Two","xsg":3,"sg":3,"sf":"sd","dvbtriplet":"","sk":2006,"adult":false,"servicetype":"OTT","schedule":true,"startover":null,"startoverents":false,"timeshifted":false,"local":false,"servicetypes":["DSAT"]},{"sid":"6000","c":"103","t":"ITV","xsg":3,"sg":3,"sf":"sd","dvbtriplet":"","sk":6000,"adult":false,"servicetype":"OTT","schedule":true,"startover":null,"startoverents":false,"timeshifted":false,"local":false,"servicetypes":["DSAT"]},{"sid":"1621","c":"104","t":"Channel 4","xsg":3,"sg":3,"sf":"sd","dvbtriplet":"","sk":1621,"adult":false,"servicetype":"OTT","schedule":true,"startover":null,"startoverents":false,"timeshifted":false,"local":false,"servicetypes":["DSAT"]},{"sid":"1800","c":"105","t":"Channel 5","xsg":3,"sg":3,"sf":"sd","dvbtriplet":"","sk":1800,"adult":false,"servicetype":"OTT","schedule":true,"startover":null,"startoverents":false,"timeshifted":false,"local":false,"servicetypes":["DSAT"]},{"sid":"4061","c":"106","t":"Sky One HD","xsg":3,"sg":3,"sf":"hd","dvbtriplet":"","sk":4061,"adult":false,"servicetype":"OTT","schedule":true,"startover":null,"startoverents":false,"timeshifted":false,"local":false,"servicetypes":["DSAT"]},{"sid":"4066","c":"107","t":"Sky Witness HD","xsg":3,"sg":3,"sf":"hd","dvbtriplet":"","sk":4066,"adult":false,"servicetype":"OTT","schedule":true,"startover":null,"startoverents":false,"timeshifted":false,"local":false,"servicetypes":["DSAT"]},{"sid":"4053","c":"108","t":"Sky Atlantic HD","xsg":3,"sg":3,"sf":"hd","dvbtriplet":"","sk":4053,"adult":false,"servicetype":"OTT","schedule":true,"startover":null,"startoverents":false,"timeshifted":false,"local":false,"servicetypes":["DSAT"]},{"sid":"3810","c":"109","t":"W HD","xsg":3,"sg":3,"sf":"hd","dvbtriplet":"","sk":3810,"adult":false,"servicetype":"OTT","schedule":true,"startover":null,"startoverents":false,"timeshifted":false,"local":false,"servicetypes":["DSAT"]},{"sid":"6513","c":"110","t":"GOLD HD","xsg":3,"sg":3,"sf":"hd","dvbtriplet":"","sk":6513,"adult":false,"servicetype":"OTT","schedule":true,"startover":null,"startoverents":false,"timeshifted":false,"local":false,"servicetypes":["DSAT"]},{"sid":"3809","c":"111","t":"Dave HD","xsg":3,"sg":3,"sf":"hd","dvbtriplet":"","sk":3809,"adult":false,"servicetype":"OTT","schedule":true,"startover":null,"startoverents":false,"timeshifted":false,"local":false,"servicetypes":["DSAT"]},{"sid":"4056","c":"112","t":"ComedyCentHD","xsg":3,"sg":3,"sf":"hd","dvbtriplet":"","sk":4056,"adult":false,"servicetype":"OTT","schedule":true,"startover":null,"startoverents":false,"timeshifted":false,"local":false,"servicetypes":["DSAT"]},{"sid":"1143","c":"113","t":"Sky Comedy HD","xsg":3,"sg":3,"sf":"hd","dvbtriplet":"","sk":1143,"adult":false,"servicetype":"OTT","schedule":true,"startover":null,"startoverents":false,"timeshifted":false,"local":false,"servicetypes":["DSAT"]},{"sid":"1136","c":"114","t":"SkyDocmntrsHD","xsg":11,"sg":11,"sf":"hd","dvbtriplet":"","sk":1136,"adult":false,"servicetype":"OTT","schedule":true,"startover":null,"startoverents":false,"timeshifted":false,"local":false,"servicetypes":["DSAT"]},{"sid":"2076","c":"115","t":"BBC One HD","xsg":3,"sg":3,"sf":"hd","dvbtriplet":"","sk":2076,"adult":false,"servicetype":"OTT","schedule":true,"startover":null,"startoverents":false,"timeshifted":false,"local":false,"servicetypes":["DSAT"]},{"sid":"2018","c":"116","t":"BBC Four","xsg":3,"sg":3,"sf":"sd","dvbtriplet":"","sk":2018,"adult":false,"servicetype":"OTT","schedule":true,"startover":null,"startoverents":false,"timeshifted":false,"local":false,"servicetypes":["DSAT"]},{"sid":"5090","c":"117","t":"London Live","xsg":3,"sg":3,"sf":"sd","dvbtriplet":"","sk":5090,"adult":false,"servicetype":"OTT","schedule":true,"startover":null,"startoverents":false,"timeshifted":false,"local":false,"servicetypes":["DSAT"]},{"sid":"6532","c":"118","t":"ITV2 HD","xsg":3,"sg":3,"sf":"hd","dvbtriplet":"","sk":6532,"adult":false,"servicetype":"OTT","schedule":true,"startover":null,"startoverents":false,"timeshifted":false,"local":false,"servicetypes":["DSAT"]},{"sid":"6533","c":"119","t":"ITV3 HD","xsg":3,"sg":3,"sf":"hd","dvbtriplet":"","sk":6533,"adult":false,"servicetype":"OTT","schedule":true,"startover":null,"startoverents":false,"timeshifted":false,"local":false,"servicetypes":["DSAT"]},{"sid":"6534","c":"120","t":"ITV4 HD","xsg":3,"sg":3,"sf":"hd","dvbtriplet":"","sk":6534,"adult":false,"servicetype":"OTT","schedule":true,"startover":null,"startoverents":false,"timeshifted":false,"local":false,"servicetypes":["DSAT"]},{"sid":"1212","c":"121","t":"Sky Crime HD","xsg":11,"sg":11,"sf":"hd","dvbtriplet":"","sk":1212,"adult":false,"servicetype":"OTT","schedule":true,"startover":null,"startoverents":false,"timeshifted":false,"local":false,"servicetypes":["DSAT"]},{"sid":"4063","c":"122","t":"Sky Arts HD","xsg":3,"sg":3,"sf":"hd","dvbtriplet":"","sk":4063,"adult":false,"servicetype":"OTT","schedule":true,"startover":null,"startoverents":false,"timeshifted":false,"local":false,"servicetypes":["DSAT"]},{"sid":"4086","c":"123","t":"Sky HistoryHD","xsg":11,"sg":11,"sf":"hd","dvbtriplet":"","sk":4086,"adult":false,"servicetype":"OTT","schedule":true,"startover":null,"startoverents":false,"timeshifted":false,"local":false,"servicetypes":["DSAT"]},{"sid":"4023","c":"124","t":"FOX HD","xsg":3,"sg":3,"sf":"hd","dvbtriplet":"","sk":4023,"adult":false,"servicetype":"OTT","schedule":true,"startover":null,"startoverents":false,"timeshifted":false,"local":false,"servicetypes":["DSAT"]},{"sid":"4003","c":"125","t":"Discovery HD","xsg":11,"sg":11,"sf":"hd","dvbtriplet":"","sk":4003,"adult":false,"servicetype":"OTT","schedule":true,"startover":null,"startoverents":false,"timeshifted":false,"local":false,"servicetypes":["DSAT"]},{"sid":"3831","c":"126","t":"MTV HD","xsg":3,"sg":3,"sf":"hd","dvbtriplet":"","sk":3831,"adult":false,"servicetype":"OTT","schedule":true,"startover":null,"startoverents":false,"timeshifted":false,"local":false,"servicetypes":["DSAT"]},{"sid":"1813","c":"127","t":"ComedyXtra","xsg":3,"sg":3,"sf":"sd","dvbtriplet":"","sk":1813,"adult":false,"servicetype":"OTT","schedule":true,"startover":null,"startoverents":false,"timeshifted":false,"local":false,"servicetypes":["DSAT"]},{"sid":"3023","c":"128","t":"5STAR","xsg":3,"sg":3,"sf":"sd","dvbtriplet":"","sk":3023,"adult":false,"servicetype":"OTT","schedule":true,"startover":null,"startoverents":false,"timeshifted":false,"local":false,"servicetypes":["DSAT"]},{"sid":"4031","c":"129","t":"Nat Geo HD","xsg":11,"sg":11,"sf":"hd","dvbtriplet":"","sk":4031,"adult":false,"servicetype":"OTT","schedule":true,"startover":null,"startoverents":false,"timeshifted":false,"local":false,"servicetypes":["DSAT"]},{"sid":"1165","c":"130","t":"Sky Nature HD","xsg":11,"sg":11,"sf":"hd","dvbtriplet":"","sk":1165,"adult":false,"servicetype":"OTT","schedule":true,"startover":null,"startoverents":false,"timeshifted":false,"local":false,"servicetypes":["DSAT"]},{"sid":"6508","c":"131","t":"ITVBe","xsg":3,"sg":3,"sf":"sd","dvbtriplet":"","sk":6508,"adult":false,"servicetype":"OTT","schedule":true,"startover":null,"startoverents":false,"timeshifted":false,"local":false,"servicetypes":["DSAT"]},{"sid":"3825","c":"132","t":"alibi HD","xsg":3,"sg":3,"sf":"hd","dvbtriplet":"","sk":3825,"adult":false,"servicetype":"OTT","schedule":true,"startover":null,"startoverents":false,"timeshifted":false,"local":false,"servicetypes":["DSAT"]},{"sid":"3812","c":"133","t":"TLC HD","xsg":3,"sg":3,"sf":"hd","dvbtriplet":"","sk":3812,"adult":false,"servicetype":"OTT","schedule":true,"startover":null,"startoverents":false,"timeshifted":false,"local":false,"servicetypes":["DSAT"]},{"sid":"5701","c":"134","t":"S4C","xsg":3,"sg":3,"sf":"sd","dvbtriplet":"","sk":5701,"adult":false,"servicetype":"OTT","schedule":true,"startover":null,"startoverents":false,"timeshifted":false,"local":false,"servicetypes":["DSAT"]},{"sid":"4076","c":"135","t":"E4 HD","xsg":3,"sg":3,"sf":"hd","dvbtriplet":"","sk":4076,"adult":false,"servicetype":"OTT","schedule":true,"startover":null,"startoverents":false,"timeshifted":false,"local":false,"servicetypes":["DSAT"]},{"sid":"4043","c":"136","t":"More4 HD","xsg":3,"sg":3,"sf":"hd","dvbtriplet":"","sk":4043,"adult":false,"servicetype":"OTT","schedule":true,"startover":null,"startoverents":false,"timeshifted":false,"local":false,"servicetypes":["DSAT"]},{"sid":"3150","c":"137","t":"4seven","xsg":3,"sg":3,"sf":"sd","dvbtriplet":"","sk":3150,"adult":false,"servicetype":"OTT","schedule":true,"startover":null,"startoverents":false,"timeshifted":false,"local":false,"servicetypes":["DSAT"]},{"sid":"1758","c":"139","t":"4Music","xsg":3,"sg":3,"sf":"sd","dvbtriplet":"","sk":1758,"adult":false,"servicetype":"OTT","schedule":true,"startover":null,"startoverents":false,"timeshifted":false,"local":false,"servicetypes":["DSAT"]},{"sid":"3590","c":"140","t":"Food Network","xsg":3,"sg":3,"sf":"sd","dvbtriplet":"","sk":3590,"adult":false,"servicetype":"OTT","schedule":true,"startover":null,"startoverents":false,"timeshifted":false,"local":false,"servicetypes":["DSAT"]},{"sid":"3022","c":"141","t":"5 USA","xsg":3,"sg":3,"sf":"sd","dvbtriplet":"","sk":3022,"adult":false,"servicetype":"OTT","schedule":true,"startover":null,"startoverents":false,"timeshifted":false,"local":false,"servicetypes":["DSAT"]},{"sid":"2325","c":"142","t":"Really","xsg":3,"sg":3,"sf":"sd","dvbtriplet":"","sk":2325,"adult":false,"servicetype":"OTT","schedule":true,"startover":null,"startoverents":false,"timeshifted":false,"local":false,"servicetypes":["DSAT"]},{"sid":"2612","c":"143","t":"Drama","xsg":3,"sg":3,"sf":"sd","dvbtriplet":"","sk":2612,"adult":false,"servicetype":"OTT","schedule":true,"startover":null,"startoverents":false,"timeshifted":false,"local":false,"servicetypes":["DSAT"]},{"sid":"6761","c":"144","t":"QUEST","xsg":3,"sg":3,"sf":"sd","dvbtriplet":"","sk":6761,"adult":false,"servicetype":"OTT","schedule":true,"startover":null,"startoverents":false,"timeshifted":false,"local":false,"servicetypes":["DSAT"]},{"sid":"1833","c":"145","t":"Sky Replay","xsg":3,"sg":3,"sf":"sd","dvbtriplet":"","sk":1833,"adult":false,"servicetype":"OTT","schedule":true,"startover":null,"startoverents":false,"timeshifted":false,"local":false,"servicetypes":["DSAT"]},{"sid":"3352","c":"146","t":"CBS Reality","xsg":3,"sg":3,"sf":"sd","dvbtriplet":"","sk":3352,"adult":false,"servicetype":"OTT","schedule":true,"startover":null,"startoverents":false,"timeshifted":false,"local":false,"servicetypes":["DSAT"]},{"sid":"3617","c":"147","t":"CBS Drama","xsg":3,"sg":3,"sf":"sd","dvbtriplet":"","sk":3617,"adult":false,"servicetype":"OTT","schedule":true,"startover":null,"startoverents":false,"timeshifted":false,"local":false,"servicetypes":["DSAT"]},{"sid":"4610","c":"148","t":"CBS Justice","xsg":3,"sg":3,"sf":"sd","dvbtriplet":"","sk":4610,"adult":false,"servicetype":"OTT","schedule":true,"startover":null,"startoverents":false,"timeshifted":false,"local":false,"servicetypes":["DSAT"]},{"sid":"2411","c":"149","t":"Quest Red","xsg":3,"sg":3,"sf":"sd","dvbtriplet":"","sk":2411,"adult":false,"servicetype":"OTT","schedule":true,"startover":null,"startoverents":false,"timeshifted":false,"local":false,"servicetypes":["DSAT"]},{"sid":"1036","c":"150","t":"Paramount","xsg":3,"sg":3,"sf":"sd","dvbtriplet":"","sk":1036,"adult":false,"servicetype":"OTT","schedule":true,"startover":null,"startoverents":false,"timeshifted":false,"local":false,"servicetypes":["DSAT"]},{"sid":"4028","c":"151","t":"E! HD","xsg":3,"sg":3,"sf":"hd","dvbtriplet":"","sk":4028,"adult":false,"servicetype":"OTT","schedule":true,"startover":null,"startoverents":false,"timeshifted":false,"local":false,"servicetypes":["DSAT"]},{"sid":"4074","c":"152","t":"SYFY HD","xsg":3,"sg":3,"sf":"hd","dvbtriplet":"","sk":4074,"adult":false,"servicetype":"OTT","schedule":true,"startover":null,"startoverents":false,"timeshifted":false,"local":false,"servicetypes":["DSAT"]},{"sid":"3028","c":"153","t":"5SELECT","xsg":3,"sg":3,"sf":"sd","dvbtriplet":"","sk":3028,"adult":false,"servicetype":"OTT","schedule":true,"startover":null,"startoverents":false,"timeshifted":false,"local":false,"servicetypes":["DSAT"]},{"sid":"2413","c":"154","t":"ID","xsg":11,"sg":11,"sf":"sd","dvbtriplet":"","sk":2413,"adult":false,"servicetype":"OTT","schedule":true,"startover":null,"startoverents":false,"timeshifted":false,"local":false,"servicetypes":["DSAT"]},{"sid":"2305","c":"155","t":"YESTERDAY","xsg":11,"sg":11,"sf":"sd","dvbtriplet":"","sk":2305,"adult":false,"servicetype":"OTT","schedule":true,"startover":null,"startoverents":false,"timeshifted":false,"local":false,"servicetypes":["DSAT"]},{"sid":"4088","c":"156","t":"Crime+Inv HD","xsg":11,"sg":11,"sf":"hd","dvbtriplet":"","sk":4088,"adult":false,"servicetype":"OTT","schedule":true,"startover":null,"startoverents":false,"timeshifted":false,"local":false,"servicetypes":["DSAT"]},{"sid":"4266","c":"157","t":"Sony Channel","xsg":3,"sg":3,"sf":"sd","dvbtriplet":"","sk":4266,"adult":false,"servicetype":"OTT","schedule":true,"startover":null,"startoverents":false,"timeshifted":false,"local":false,"servicetypes":["DSAT"]},{"sid":"2301","c":"158","t":"HGTV","xsg":3,"sg":3,"sf":"sd","dvbtriplet":"","sk":2301,"adult":false,"servicetype":"OTT","schedule":true,"startover":null,"startoverents":false,"timeshifted":false,"local":false,"servicetypes":["DSAT"]},{"sid":"1832","c":"159","t":"Pick","xsg":3,"sg":3,"sf":"sd","dvbtriplet":"","sk":1832,"adult":false,"servicetype":"OTT","schedule":true,"startover":null,"startoverents":false,"timeshifted":false,"local":false,"servicetypes":["DSAT"]},{"sid":"2202","c":"160","t":"Challenge","xsg":3,"sg":3,"sf":"sd","dvbtriplet":"","sk":2202,"adult":false,"servicetype":"OTT","schedule":true,"startover":null,"startoverents":false,"timeshifted":false,"local":false,"servicetypes":["DSAT"]},{"sid":"2409","c":"161","t":"Disc.Turbo","xsg":11,"sg":11,"sf":"sd","dvbtriplet":"","sk":2409,"adult":false,"servicetype":"OTT","schedule":true,"startover":null,"startoverents":false,"timeshifted":false,"local":false,"servicetypes":["DSAT"]},{"sid":"2402","c":"162","t":"Animal Planet","xsg":11,"sg":11,"sf":"sd","dvbtriplet":"","sk":2402,"adult":false,"servicetype":"OTT","schedule":true,"startover":null,"startoverents":false,"timeshifted":false,"local":false,"servicetypes":["DSAT"]},{"sid":"1195","c":"163","t":"SkyHistory2HD","xsg":11,"sg":11,"sf":"hd","dvbtriplet":"","sk":1195,"adult":false,"servicetype":"OTT","schedule":true,"startover":null,"startoverents":false,"timeshifted":false,"local":false,"servicetypes":["DSAT"]},{"sid":"1065","c":"164","t":"BLAZE","xsg":11,"sg":11,"sf":"sd","dvbtriplet":"","sk":1065,"adult":false,"servicetype":"OTT","schedule":true,"startover":null,"startoverents":false,"timeshifted":false,"local":false,"servicetypes":["DSAT"]},{"sid":"4025","c":"165","t":"NatGeoWild HD","xsg":11,"sg":11,"sf":"hd","dvbtriplet":"","sk":4025,"adult":false,"servicetype":"OTT","schedule":true,"startover":null,"startoverents":false,"timeshifted":false,"local":false,"servicetypes":["DSAT"]},{"sid":"2302","c":"166","t":"Eden","xsg":11,"sg":11,"sf":"sd","dvbtriplet":"","sk":2302,"adult":false,"servicetype":"OTT","schedule":true,"startover":null,"startoverents":false,"timeshifted":false,"local":false,"servicetypes":["DSAT"]},{"sid":"2406","c":"167","t":"Disc.Science","xsg":11,"sg":11,"sf":"sd","dvbtriplet":"","sk":2406,"adult":false,"servicetype":"OTT","schedule":true,"startover":null,"startoverents":false,"timeshifted":false,"local":false,"servicetypes":["DSAT"]},{"sid":"2012","c":"169","t":"BBC ALBA","xsg":3,"sg":3,"sf":"sd","dvbtriplet":"","sk":2012,"adult":false,"servicetype":"OTT","schedule":true,"startover":null,"startoverents":false,"timeshifted":false,"local":false,"servicetypes":["DSAT"]},{"sid":"1872","c":"170","t":"Together","xsg":11,"sg":11,"sf":"sd","dvbtriplet":"","sk":1872,"adult":false,"servicetype":"OTT","schedule":true,"startover":null,"startoverents":false,"timeshifted":false,"local":false,"servicetypes":["DSAT"]},{"sid":"1089","c":"171","t":"Smithsonian","xsg":11,"sg":11,"sf":"sd","dvbtriplet":"","sk":1089,"adult":false,"servicetype":"OTT","schedule":true,"startover":null,"startoverents":false,"timeshifted":false,"local":false,"servicetypes":["DSAT"]},{"sid":"3618","c":"172","t":"DMAX","xsg":3,"sg":3,"sf":"sd","dvbtriplet":"","sk":3618,"adult":false,"servicetype":"OTT","schedule":true,"startover":null,"startoverents":false,"timeshifted":false,"local":false,"servicetypes":["DSAT"]},{"sid":"3719","c":"173","t":"BET:BlackEntTv","xsg":3,"sg":3,"sf":"sd","dvbtriplet":"","sk":3719,"adult":false,"servicetype":"OTT","schedule":true,"startover":null,"startoverents":false,"timeshifted":false,"local":false,"servicetypes":["DSAT"]},{"sid":"5500","c":"174","t":"PBS America","xsg":11,"sg":11,"sf":"sd","dvbtriplet":"","sk":5500,"adult":false,"servicetype":"OTT","schedule":true,"startover":null,"startoverents":false,"timeshifted":false,"local":false,"servicetypes":["DSAT"]},{"sid":"1133","c":"175","t":"Live 360","xsg":3,"sg":3,"sf":"sd","dvbtriplet":"","sk":1133,"adult":false,"servicetype":"OTT","schedule":true,"startover":null,"startoverents":false,"timeshifted":false,"local":false,"servicetypes":["DSAT"]},{"sid":"2407","c":"177","t":"Disc.History","xsg":11,"sg":11,"sf":"sd","dvbtriplet":"","sk":2407,"adult":false,"servicetype":"OTT","schedule":true,"startover":null,"startoverents":false,"timeshifted":false,"local":false,"servicetypes":["DSAT"]},{"sid":"2408","c":"178","t":"Home & Health","xsg":3,"sg":3,"sf":"sd","dvbtriplet":"","sk":2408,"adult":false,"servicetype":"OTT","schedule":true,"startover":null,"startoverents":false,"timeshifted":false,"local":false,"servicetypes":["DSAT"]},{"sid":"1207","c":"179","t":"Court TV","xsg":3,"sg":3,"sf":"sd","dvbtriplet":"","sk":1207,"adult":false,"servicetype":"OTT","schedule":true,"startover":null,"startoverents":false,"timeshifted":false,"local":false,"servicetypes":["DSAT"]},{"sid":"5415","c":"181","t":"Forces TV","xsg":3,"sg":3,"sf":"sd","dvbtriplet":"","sk":5415,"adult":false,"servicetype":"OTT","schedule":true,"startover":null,"startoverents":false,"timeshifted":false,"local":false,"servicetypes":["DSAT"]},{"sid":"3687","c":"182","t":"Channel 7","xsg":3,"sg":3,"sf":"sd","dvbtriplet":"","sk":3687,"adult":false,"servicetype":"OTT","schedule":true,"startover":null,"startoverents":false,"timeshifted":false,"local":false,"servicetypes":["DSAT"]},{"sid":"1827","c":"183","t":"VICE","xsg":3,"sg":3,"sf":"sd","dvbtriplet":"","sk":1827,"adult":false,"servicetype":"OTT","schedule":true,"startover":null,"startoverents":false,"timeshifted":false,"local":false,"servicetypes":["DSAT"]},{"sid":"1102","c":"184","t":"Hi-Impact TV","xsg":3,"sg":3,"sf":"sd","dvbtriplet":"","sk":1102,"adult":false,"servicetype":"OTT","schedule":true,"startover":null,"startoverents":false,"timeshifted":false,"local":false,"servicetypes":["DSAT"]},{"sid":"1226","c":"185","t":"Travelxp","xsg":3,"sg":3,"sf":"sd","dvbtriplet":"","sk":1226,"adult":false,"servicetype":"OTT","schedule":true,"startover":null,"startoverents":false,"timeshifted":false,"local":false,"servicetypes":["DSAT"]},{"sid":"1058","c":"186","t":"YANGA!","xsg":3,"sg":3,"sf":"sd","dvbtriplet":"","sk":1058,"adult":false,"servicetype":"OTT","schedule":true,"startover":null,"startoverents":false,"timeshifted":false,"local":false,"servicetypes":["DSAT"]},{"sid":"4320","c":"187","t":"Lifetime","xsg":3,"sg":3,"sf":"sd","dvbtriplet":"","sk":4320,"adult":false,"servicetype":"OTT","schedule":true,"startover":null,"startoverents":false,"timeshifted":false,"local":false,"servicetypes":["DSAT"]},{"sid":"3731","c":"191","t":"mytv","xsg":3,"sg":3,"sf":"sd","dvbtriplet":"","sk":3731,"adult":false,"servicetype":"OTT","schedule":true,"startover":null,"startoverents":false,"timeshifted":false,"local":false,"servicetypes":["DSAT"]},{"sid":"3206","c":"192","t":"Showcase","xsg":3,"sg":3,"sf":"sd","dvbtriplet":"","sk":3206,"adult":false,"servicetype":"OTT","schedule":true,"startover":null,"startoverents":false,"timeshifted":false,"local":false,"servicetypes":["DSAT"]},{"sid":"3108","c":"193","t":"Vox Africa","xsg":3,"sg":3,"sf":"sd","dvbtriplet":"","sk":3108,"adult":false,"servicetype":"OTT","schedule":true,"startover":null,"startoverents":false,"timeshifted":false,"local":false,"servicetypes":["DSAT"]},{"sid":"4548","c":"197","t":"Discovery Shed","xsg":3,"sg":3,"sf":"sd","dvbtriplet":"","sk":4548,"adult":false,"servicetype":"OTT","schedule":true,"startover":null,"startoverents":false,"timeshifted":false,"local":false,"servicetypes":["DSAT"]},{"sid":"6155","c":"203","t":"ITV+1","xsg":3,"sg":3,"sf":"sd","dvbtriplet":"","sk":6155,"adult":false,"servicetype":"OTT","schedule":true,"startover":null,"startoverents":false,"timeshifted":false,"local":false,"servicetypes":["DSAT"]},{"sid":"1670","c":"204","t":"Channel 4+1","xsg":3,"sg":3,"sf":"sd","dvbtriplet":"","sk":1670,"adult":false,"servicetype":"OTT","schedule":true,"startover":null,"startoverents":false,"timeshifted":false,"local":false,"servicetypes":["DSAT"]},{"sid":"1839","c":"205","t":"Channel 5+1","xsg":3,"sg":3,"sf":"sd","dvbtriplet":"","sk":1839,"adult":false,"servicetype":"OTT","schedule":true,"startover":null,"startoverents":false,"timeshifted":false,"local":false,"servicetypes":["DSAT"]},{"sid":"1403","c":"206","t":"Sky One+1","xsg":3,"sg":3,"sf":"sd","dvbtriplet":"","sk":1403,"adult":false,"servicetype":"OTT","schedule":true,"startover":null,"startoverents":false,"timeshifted":false,"local":false,"servicetypes":["DSAT"]},{"sid":"2205","c":"207","t":"Sky Witness+1","xsg":3,"sg":3,"sf":"sd","dvbtriplet":"","sk":2205,"adult":false,"servicetype":"OTT","schedule":true,"startover":null,"startoverents":false,"timeshifted":false,"local":false,"servicetypes":["DSAT"]},{"sid":"1413","c":"208","t":"Sky Atlantic+1","xsg":3,"sg":3,"sf":"sd","dvbtriplet":"","sk":1413,"adult":false,"servicetype":"OTT","schedule":true,"startover":null,"startoverents":false,"timeshifted":false,"local":false,"servicetypes":["DSAT"]},{"sid":"2616","c":"209","t":"W+1","xsg":3,"sg":3,"sf":"sd","dvbtriplet":"","sk":2616,"adult":false,"servicetype":"OTT","schedule":true,"startover":null,"startoverents":false,"timeshifted":false,"local":false,"servicetypes":["DSAT"]},{"sid":"2703","c":"210","t":"GOLD+1","xsg":3,"sg":3,"sf":"sd","dvbtriplet":"","sk":2703,"adult":false,"servicetype":"OTT","schedule":true,"startover":null,"startoverents":false,"timeshifted":false,"local":false,"servicetypes":["DSAT"]},{"sid":"2320","c":"211","t":"Dave ja vu","xsg":3,"sg":3,"sf":"sd","dvbtriplet":"","sk":2320,"adult":false,"servicetype":"OTT","schedule":true,"startover":null,"startoverents":false,"timeshifted":false,"local":false,"servicetypes":["DSAT"]},{"sid":"3802","c":"212","t":"ComedyCent+1","xsg":3,"sg":3,"sf":"sd","dvbtriplet":"","sk":3802,"adult":false,"servicetype":"OTT","schedule":true,"startover":null,"startoverents":false,"timeshifted":false,"local":false,"servicetypes":["DSAT"]},{"sid":"6241","c":"218","t":"ITV2+1","xsg":3,"sg":3,"sf":"sd","dvbtriplet":"","sk":6241,"adult":false,"servicetype":"OTT","schedule":true,"startover":null,"startoverents":false,"timeshifted":false,"local":false,"servicetypes":["DSAT"]},{"sid":"6261","c":"219","t":"ITV3+1","xsg":3,"sg":3,"sf":"sd","dvbtriplet":"","sk":6261,"adult":false,"servicetype":"OTT","schedule":true,"startover":null,"startoverents":false,"timeshifted":false,"local":false,"servicetypes":["DSAT"]},{"sid":"6274","c":"220","t":"ITV4+1","xsg":3,"sg":3,"sf":"sd","dvbtriplet":"","sk":6274,"adult":false,"servicetype":"OTT","schedule":true,"startover":null,"startoverents":false,"timeshifted":false,"local":false,"servicetypes":["DSAT"]},{"sid":"4330","c":"221","t":"Sky Crime+1","xsg":11,"sg":11,"sf":"sd","dvbtriplet":"","sk":4330,"adult":false,"servicetype":"OTT","schedule":true,"startover":null,"startoverents":false,"timeshifted":false,"local":false,"servicetypes":["DSAT"]},{"sid":"1891","c":"223","t":"Sky History+1","xsg":11,"sg":11,"sf":"sd","dvbtriplet":"","sk":1891,"adult":false,"servicetype":"OTT","schedule":true,"startover":null,"startoverents":false,"timeshifted":false,"local":false,"servicetypes":["DSAT"]},{"sid":"1791","c":"224","t":"FOX+1","xsg":3,"sg":3,"sf":"sd","dvbtriplet":"","sk":1791,"adult":false,"servicetype":"OTT","schedule":true,"startover":null,"startoverents":false,"timeshifted":false,"local":false,"servicetypes":["DSAT"]},{"sid":"2404","c":"225","t":"Discovery+1","xsg":11,"sg":11,"sf":"sd","dvbtriplet":"","sk":2404,"adult":false,"servicetype":"OTT","schedule":true,"startover":null,"startoverents":false,"timeshifted":false,"local":false,"servicetypes":["DSAT"]},{"sid":"3024","c":"228","t":"5STAR+1","xsg":3,"sg":3,"sf":"sd","dvbtriplet":"","sk":3024,"adult":false,"servicetype":"OTT","schedule":true,"startover":null,"startoverents":false,"timeshifted":false,"local":false,"servicetypes":["DSAT"]},{"sid":"1822","c":"229","t":"Nat Geo+1","xsg":11,"sg":11,"sf":"sd","dvbtriplet":"","sk":1822,"adult":false,"servicetype":"OTT","schedule":true,"startover":null,"startoverents":false,"timeshifted":false,"local":false,"servicetypes":["DSAT"]},{"sid":"1014","c":"231","t":"ITVBe+1","xsg":3,"sg":3,"sf":"sd","dvbtriplet":"","sk":1014,"adult":false,"servicetype":"OTT","schedule":true,"startover":null,"startoverents":false,"timeshifted":false,"local":false,"servicetypes":["DSAT"]},{"sid":"2630","c":"232","t":"alibi+1","xsg":3,"sg":3,"sf":"sd","dvbtriplet":"","sk":2630,"adult":false,"servicetype":"OTT","schedule":true,"startover":null,"startoverents":false,"timeshifted":false,"local":false,"servicetypes":["DSAT"]},{"sid":"5451","c":"233","t":"TLC+1","xsg":3,"sg":3,"sf":"sd","dvbtriplet":"","sk":5451,"adult":false,"servicetype":"OTT","schedule":true,"startover":null,"startoverents":false,"timeshifted":false,"local":false,"servicetypes":["DSAT"]},{"sid":"3300","c":"235","t":"E4+1","xsg":3,"sg":3,"sf":"sd","dvbtriplet":"","sk":3300,"adult":false,"servicetype":"OTT","schedule":true,"startover":null,"startoverents":false,"timeshifted":false,"local":false,"servicetypes":["DSAT"]},{"sid":"3310","c":"236","t":"More4+1","xsg":3,"sg":3,"sf":"sd","dvbtriplet":"","sk":3310,"adult":false,"servicetype":"OTT","schedule":true,"startover":null,"startoverents":false,"timeshifted":false,"local":false,"servicetypes":["DSAT"]},{"sid":"3592","c":"240","t":"Food Netwrk+1","xsg":3,"sg":3,"sf":"sd","dvbtriplet":"","sk":3592,"adult":false,"servicetype":"OTT","schedule":true,"startover":null,"startoverents":false,"timeshifted":false,"local":false,"servicetypes":["DSAT"]},{"sid":"3027","c":"241","t":"5USA+1","xsg":3,"sg":3,"sf":"sd","dvbtriplet":"","sk":3027,"adult":false,"servicetype":"OTT","schedule":true,"startover":null,"startoverents":false,"timeshifted":false,"local":false,"servicetypes":["DSAT"]},{"sid":"1081","c":"243","t":"Drama+1","xsg":3,"sg":3,"sf":"sd","dvbtriplet":"","sk":1081,"adult":false,"servicetype":"OTT","schedule":true,"startover":null,"startoverents":false,"timeshifted":false,"local":false,"servicetypes":["DSAT"]},{"sid":"3621","c":"244","t":"QUEST+1","xsg":3,"sg":3,"sf":"sd","dvbtriplet":"","sk":3621,"adult":false,"servicetype":"OTT","schedule":true,"startover":null,"startoverents":false,"timeshifted":false,"local":false,"servicetypes":["DSAT"]},{"sid":"3602","c":"246","t":"CBS Reality+1","xsg":3,"sg":3,"sf":"sd","dvbtriplet":"","sk":3602,"adult":false,"servicetype":"OTT","schedule":true,"startover":null,"startoverents":false,"timeshifted":false,"local":false,"servicetypes":["DSAT"]},{"sid":"4547","c":"249","t":"Quest Red+1","xsg":3,"sg":3,"sf":"sd","dvbtriplet":"","sk":4547,"adult":false,"servicetype":"OTT","schedule":true,"startover":null,"startoverents":false,"timeshifted":false,"local":false,"servicetypes":["DSAT"]},{"sid":"2513","c":"252","t":"SYFY+1","xsg":3,"sg":3,"sf":"sd","dvbtriplet":"","sk":2513,"adult":false,"servicetype":"OTT","schedule":true,"startover":null,"startoverents":false,"timeshifted":false,"local":false,"servicetypes":["DSAT"]},{"sid":"5452","c":"254","t":"ID+1","xsg":11,"sg":11,"sf":"sd","dvbtriplet":"","sk":5452,"adult":false,"servicetype":"OTT","schedule":true,"startover":null,"startoverents":false,"timeshifted":false,"local":false,"servicetypes":["DSAT"]},{"sid":"2615","c":"255","t":"YESTERDAY+1","xsg":11,"sg":11,"sf":"sd","dvbtriplet":"","sk":2615,"adult":false,"servicetype":"OTT","schedule":true,"startover":null,"startoverents":false,"timeshifted":false,"local":false,"servicetypes":["DSAT"]},{"sid":"3620","c":"256","t":"Crime+Inv+1","xsg":11,"sg":11,"sf":"sd","dvbtriplet":"","sk":3620,"adult":false,"servicetype":"OTT","schedule":true,"startover":null,"startoverents":false,"timeshifted":false,"local":false,"servicetypes":["DSAT"]},{"sid":"5338","c":"257","t":"Sony Channel+1","xsg":3,"sg":3,"sf":"sd","dvbtriplet":"","sk":5338,"adult":false,"servicetype":"OTT","schedule":true,"startover":null,"startoverents":false,"timeshifted":false,"local":false,"servicetypes":["DSAT"]},{"sid":"2309","c":"258","t":"HGTV+1","xsg":3,"sg":3,"sf":"sd","dvbtriplet":"","sk":2309,"adult":false,"servicetype":"OTT","schedule":true,"startover":null,"startoverents":false,"timeshifted":false,"local":false,"servicetypes":["DSAT"]},{"sid":"1836","c":"259","t":"Pick+1","xsg":3,"sg":3,"sf":"sd","dvbtriplet":"","sk":1836,"adult":false,"servicetype":"OTT","schedule":true,"startover":null,"startoverents":false,"timeshifted":false,"local":false,"servicetypes":["DSAT"]},{"sid":"5245","c":"261","t":"Disc.Turbo+1","xsg":11,"sg":11,"sf":"sd","dvbtriplet":"","sk":5245,"adult":false,"servicetype":"OTT","schedule":true,"startover":null,"startoverents":false,"timeshifted":false,"local":false,"servicetypes":["DSAT"]},{"sid":"1352","c":"262","t":"Animal Plnt+1","xsg":11,"sg":11,"sf":"sd","dvbtriplet":"","sk":1352,"adult":false,"servicetype":"OTT","schedule":true,"startover":null,"startoverents":false,"timeshifted":false,"local":false,"servicetypes":["DSAT"]},{"sid":"2307","c":"266","t":"Eden+1","xsg":11,"sg":11,"sf":"sd","dvbtriplet":"","sk":2307,"adult":false,"servicetype":"OTT","schedule":true,"startover":null,"startoverents":false,"timeshifted":false,"local":false,"servicetypes":["DSAT"]},{"sid":"1351","c":"267","t":"Disc.Sci+1","xsg":11,"sg":11,"sf":"sd","dvbtriplet":"","sk":1351,"adult":false,"servicetype":"OTT","schedule":true,"startover":null,"startoverents":false,"timeshifted":false,"local":false,"servicetypes":["DSAT"]},{"sid":"1865","c":"272","t":"DMAX+1","xsg":3,"sg":3,"sf":"sd","dvbtriplet":"","sk":1865,"adult":false,"servicetype":"OTT","schedule":true,"startover":null,"startoverents":false,"timeshifted":false,"local":false,"servicetypes":["DSAT"]},{"sid":"3760","c":"277","t":"Disc.History+1","xsg":11,"sg":11,"sf":"sd","dvbtriplet":"","sk":3760,"adult":false,"servicetype":"OTT","schedule":true,"startover":null,"startoverents":false,"timeshifted":false,"local":false,"servicetypes":["DSAT"]},{"sid":"2410","c":"278","t":"HomeHealth+1","xsg":3,"sg":3,"sf":"sd","dvbtriplet":"","sk":2410,"adult":false,"servicetype":"OTT","schedule":true,"startover":null,"startoverents":false,"timeshifted":false,"local":false,"servicetypes":["DSAT"]},{"sid":"1409","c":"301","t":"Sky Premiere","xsg":6,"sg":6,"sf":"sd","dvbtriplet":"","sk":1409,"adult":false,"servicetype":"OTT","schedule":true,"startover":null,"startoverents":false,"timeshifted":false,"local":false,"servicetypes":["DSAT"]},{"sid":"4020","c":"302","t":"SkyMusicalsHD","xsg":6,"sg":6,"sf":"hd","dvbtriplet":"","sk":4020,"adult":false,"servicetype":"OTT","schedule":true,"startover":null,"startoverents":false,"timeshifted":false,"local":false,"servicetypes":["DSAT"]},{"sid":"4033","c":"303","t":"Sky Hits HD","xsg":6,"sg":6,"sf":"hd","dvbtriplet":"","sk":4033,"adult":false,"servicetype":"OTT","schedule":true,"startover":null,"startoverents":false,"timeshifted":false,"local":false,"servicetypes":["DSAT"]},{"sid":"1815","c":"304","t":"Sky Greats","xsg":6,"sg":6,"sf":"sd","dvbtriplet":"","sk":1815,"adult":false,"servicetype":"OTT","schedule":true,"startover":null,"startoverents":false,"timeshifted":false,"local":false,"servicetypes":["DSAT"]},{"sid":"1199","c":"305","t":"SkyAnimationHD","xsg":6,"sg":6,"sf":"hd","dvbtriplet":"","sk":1199,"adult":false,"servicetype":"OTT","schedule":true,"startover":null,"startoverents":false,"timeshifted":false,"local":false,"servicetypes":["DSAT"]},{"sid":"1808","c":"306","t":"Sky Family","xsg":6,"sg":6,"sf":"sd","dvbtriplet":"","sk":1808,"adult":false,"servicetype":"OTT","schedule":true,"startover":null,"startoverents":false,"timeshifted":false,"local":false,"servicetypes":["DSAT"]},{"sid":"1838","c":"307","t":"Sky Disney","xsg":6,"sg":6,"sf":"sd","dvbtriplet":"","sk":1838,"adult":false,"servicetype":"OTT","schedule":true,"startover":null,"startoverents":false,"timeshifted":false,"local":false,"servicetypes":["DSAT"]},{"sid":"1001","c":"308","t":"Sky Action","xsg":6,"sg":6,"sf":"sd","dvbtriplet":"","sk":1001,"adult":false,"servicetype":"OTT","schedule":true,"startover":null,"startoverents":false,"timeshifted":false,"local":false,"servicetypes":["DSAT"]},{"sid":"1002","c":"309","t":"Sky Comedy Cinema","xsg":6,"sg":6,"sf":"sd","dvbtriplet":"","sk":1002,"adult":false,"servicetype":"OTT","schedule":true,"startover":null,"startoverents":false,"timeshifted":false,"local":false,"servicetypes":["DSAT"]},{"sid":"1818","c":"310","t":"Sky Thriller","xsg":6,"sg":6,"sf":"sd","dvbtriplet":"","sk":1818,"adult":false,"servicetype":"OTT","schedule":true,"startover":null,"startoverents":false,"timeshifted":false,"local":false,"servicetypes":["DSAT"]},{"sid":"1816","c":"311","t":"Sky Drama","xsg":6,"sg":6,"sf":"sd","dvbtriplet":"","sk":1816,"adult":false,"servicetype":"OTT","schedule":true,"startover":null,"startoverents":false,"timeshifted":false,"local":false,"servicetypes":["DSAT"]},{"sid":"4017","c":"312","t":"Sky ScFi/HorHD","xsg":6,"sg":6,"sf":"hd","dvbtriplet":"","sk":4017,"adult":false,"servicetype":"OTT","schedule":true,"startover":null,"startoverents":false,"timeshifted":false,"local":false,"servicetypes":["DSAT"]},{"sid":"4044","c":"313","t":"Film4 HD","xsg":6,"sg":6,"sf":"hd","dvbtriplet":"","sk":4044,"adult":false,"servicetype":"OTT","schedule":true,"startover":null,"startoverents":false,"timeshifted":false,"local":false,"servicetypes":["DSAT"]},{"sid":"1629","c":"314","t":"Film4+1","xsg":6,"sg":6,"sf":"sd","dvbtriplet":"","sk":1629,"adult":false,"servicetype":"OTT","schedule":true,"startover":null,"startoverents":false,"timeshifted":false,"local":false,"servicetypes":["DSAT"]},{"sid":"5605","c":"315","t":"TCM Movies","xsg":6,"sg":6,"sf":"sd","dvbtriplet":"","sk":5605,"adult":false,"servicetype":"OTT","schedule":true,"startover":null,"startoverents":false,"timeshifted":false,"local":false,"servicetypes":["DSAT"]},{"sid":"5275","c":"316","t":"TCM Movies+1","xsg":6,"sg":6,"sf":"sd","dvbtriplet":"","sk":5275,"adult":false,"servicetype":"OTT","schedule":true,"startover":null,"startoverents":false,"timeshifted":false,"local":false,"servicetypes":["DSAT"]},{"sid":"3605","c":"317","t":"horror channel","xsg":6,"sg":6,"sf":"sd","dvbtriplet":"","sk":3605,"adult":false,"servicetype":"OTT","schedule":true,"startover":null,"startoverents":false,"timeshifted":false,"local":false,"servicetypes":["DSAT"]},{"sid":"4502","c":"318","t":"horror ch+1","xsg":6,"sg":6,"sf":"sd","dvbtriplet":"","sk":4502,"adult":false,"servicetype":"OTT","schedule":true,"startover":null,"startoverents":false,"timeshifted":false,"local":false,"servicetypes":["DSAT"]},{"sid":"3643","c":"319","t":"Sony Classic","xsg":6,"sg":6,"sf":"sd","dvbtriplet":"","sk":3643,"adult":false,"servicetype":"OTT","schedule":true,"startover":null,"startoverents":false,"timeshifted":false,"local":false,"servicetypes":["DSAT"]},{"sid":"3751","c":"320","t":"Sony Classic+1","xsg":6,"sg":6,"sf":"sd","dvbtriplet":"","sk":3751,"adult":false,"servicetype":"OTT","schedule":true,"startover":null,"startoverents":false,"timeshifted":false,"local":false,"servicetypes":["DSAT"]},{"sid":"3709","c":"321","t":"Sony Movies","xsg":6,"sg":6,"sf":"sd","dvbtriplet":"","sk":3709,"adult":false,"servicetype":"OTT","schedule":true,"startover":null,"startoverents":false,"timeshifted":false,"local":false,"servicetypes":["DSAT"]},{"sid":"3771","c":"322","t":"Sony Movies+1","xsg":6,"sg":6,"sf":"sd","dvbtriplet":"","sk":3771,"adult":false,"servicetype":"OTT","schedule":true,"startover":null,"startoverents":false,"timeshifted":false,"local":false,"servicetypes":["DSAT"]},{"sid":"3708","c":"323","t":"Sony Action","xsg":6,"sg":6,"sf":"sd","dvbtriplet":"","sk":3708,"adult":false,"servicetype":"OTT","schedule":true,"startover":null,"startoverents":false,"timeshifted":false,"local":false,"servicetypes":["DSAT"]},{"sid":"3721","c":"324","t":"Sony Action+1","xsg":6,"sg":6,"sf":"sd","dvbtriplet":"","sk":3721,"adult":false,"servicetype":"OTT","schedule":true,"startover":null,"startoverents":false,"timeshifted":false,"local":false,"servicetypes":["DSAT"]},{"sid":"4420","c":"325","t":"Movies 24","xsg":6,"sg":6,"sf":"sd","dvbtriplet":"","sk":4420,"adult":false,"servicetype":"OTT","schedule":true,"startover":null,"startoverents":false,"timeshifted":false,"local":false,"servicetypes":["DSAT"]},{"sid":"4421","c":"326","t":"Movies 24+","xsg":6,"sg":6,"sf":"sd","dvbtriplet":"","sk":4421,"adult":false,"servicetype":"OTT","schedule":true,"startover":null,"startoverents":false,"timeshifted":false,"local":false,"servicetypes":["DSAT"]},{"sid":"5252","c":"328","t":"TalkingPictures","xsg":6,"sg":6,"sf":"sd","dvbtriplet":"","sk":5252,"adult":false,"servicetype":"OTT","schedule":true,"startover":null,"startoverents":false,"timeshifted":false,"local":false,"servicetypes":["DSAT"]},{"sid":"3542","c":"329","t":"ROK","xsg":6,"sg":6,"sf":"sd","dvbtriplet":"","sk":3542,"adult":false,"servicetype":"OTT","schedule":true,"startover":null,"startoverents":false,"timeshifted":false,"local":false,"servicetypes":["DSAT"]},{"sid":"2512","c":"350","t":"MTV MUSIC","xsg":12,"sg":12,"sf":"sd","dvbtriplet":"","sk":2512,"adult":false,"servicetype":"OTT","schedule":true,"startover":null,"startoverents":false,"timeshifted":false,"local":false,"servicetypes":["DSAT"]},{"sid":"2506","c":"351","t":"MTV HITS","xsg":12,"sg":12,"sf":"sd","dvbtriplet":"","sk":2506,"adult":false,"servicetype":"OTT","schedule":true,"startover":null,"startoverents":false,"timeshifted":false,"local":false,"servicetypes":["DSAT"]},{"sid":"2508","c":"353","t":"MTV BASE","xsg":12,"sg":12,"sf":"sd","dvbtriplet":"","sk":2508,"adult":false,"servicetype":"OTT","schedule":true,"startover":null,"startoverents":false,"timeshifted":false,"local":false,"servicetypes":["DSAT"]},{"sid":"2509","c":"355","t":"MTV CLASSIC","xsg":12,"sg":12,"sf":"sd","dvbtriplet":"","sk":2509,"adult":false,"servicetype":"OTT","schedule":true,"startover":null,"startoverents":false,"timeshifted":false,"local":false,"servicetypes":["DSAT"]},{"sid":"1350","c":"358","t":"Box Hits","xsg":12,"sg":12,"sf":"sd","dvbtriplet":"","sk":1350,"adult":false,"servicetype":"OTT","schedule":true,"startover":null,"startoverents":false,"timeshifted":false,"local":false,"servicetypes":["DSAT"]},{"sid":"1802","c":"359","t":"The Box","xsg":12,"sg":12,"sf":"sd","dvbtriplet":"","sk":1802,"adult":false,"servicetype":"OTT","schedule":true,"startover":null,"startoverents":false,"timeshifted":false,"local":false,"servicetypes":["DSAT"]},{"sid":"1858","c":"360","t":"KISS","xsg":12,"sg":12,"sf":"sd","dvbtriplet":"","sk":1858,"adult":false,"servicetype":"OTT","schedule":true,"startover":null,"startoverents":false,"timeshifted":false,"local":false,"servicetypes":["DSAT"]},{"sid":"1877","c":"361","t":"Magic","xsg":12,"sg":12,"sf":"sd","dvbtriplet":"","sk":1877,"adult":false,"servicetype":"OTT","schedule":true,"startover":null,"startoverents":false,"timeshifted":false,"local":false,"servicetypes":["DSAT"]},{"sid":"1853","c":"362","t":"Kerrang!","xsg":12,"sg":12,"sf":"sd","dvbtriplet":"","sk":1853,"adult":false,"servicetype":"OTT","schedule":true,"startover":null,"startoverents":false,"timeshifted":false,"local":false,"servicetypes":["DSAT"]},{"sid":"3505","c":"364","t":"Trace Urban","xsg":12,"sg":12,"sf":"sd","dvbtriplet":"","sk":3505,"adult":false,"servicetype":"OTT","schedule":true,"startover":null,"startoverents":false,"timeshifted":false,"local":false,"servicetypes":["DSAT"]},{"sid":"3935","c":"365","t":"Trace Vault","xsg":12,"sg":12,"sf":"sd","dvbtriplet":"","sk":3935,"adult":false,"servicetype":"OTT","schedule":true,"startover":null,"startoverents":false,"timeshifted":false,"local":false,"servicetypes":["DSAT"]},{"sid":"3403","c":"368","t":"NOW 70s","xsg":12,"sg":12,"sf":"sd","dvbtriplet":"","sk":3403,"adult":false,"servicetype":"OTT","schedule":true,"startover":null,"startoverents":false,"timeshifted":false,"local":false,"servicetypes":["DSAT"]},{"sid":"4541","c":"371","t":"NOW 80s","xsg":12,"sg":12,"sf":"sd","dvbtriplet":"","sk":4541,"adult":false,"servicetype":"OTT","schedule":true,"startover":null,"startoverents":false,"timeshifted":false,"local":false,"servicetypes":["DSAT"]},{"sid":"3682","c":"372","t":"NOW 90s","xsg":12,"sg":12,"sf":"sd","dvbtriplet":"","sk":3682,"adult":false,"servicetype":"OTT","schedule":true,"startover":null,"startoverents":false,"timeshifted":false,"local":false,"servicetypes":["DSAT"]},{"sid":"4505","c":"373","t":"Clubland TV","xsg":12,"sg":12,"sf":"sd","dvbtriplet":"","sk":4505,"adult":false,"servicetype":"OTT","schedule":true,"startover":null,"startoverents":false,"timeshifted":false,"local":false,"servicetypes":["DSAT"]},{"sid":"4933","c":"376","t":"Spotlight TV","xsg":12,"sg":12,"sf":"sd","dvbtriplet":"","sk":4933,"adult":false,"servicetype":"OTT","schedule":true,"startover":null,"startoverents":false,"timeshifted":false,"local":false,"servicetypes":["DSAT"]},{"sid":"1301","c":"401","t":"SkySpMainEv","xsg":7,"sg":7,"sf":"sd","dvbtriplet":"","sk":1301,"adult":false,"servicetype":"OTT","schedule":true,"startover":null,"startoverents":false,"timeshifted":false,"local":false,"servicetypes":["DSAT"]},{"sid":"1303","c":"402","t":"SkySp PL","xsg":7,"sg":7,"sf":"sd","dvbtriplet":"","sk":1303,"adult":false,"servicetype":"OTT","schedule":true,"startover":null,"startoverents":false,"timeshifted":false,"local":false,"servicetypes":["DSAT"]},{"sid":"3838","c":"403","t":"SkySp F'ball","xsg":7,"sg":7,"sf":"sd","dvbtriplet":"","sk":3838,"adult":false,"servicetype":"OTT","schedule":true,"startover":null,"startoverents":false,"timeshifted":false,"local":false,"servicetypes":["DSAT"]},{"sid":"1302","c":"404","t":"SkySp Cricket","xsg":7,"sg":7,"sf":"sd","dvbtriplet":"","sk":1302,"adult":false,"servicetype":"OTT","schedule":true,"startover":null,"startoverents":false,"timeshifted":false,"local":false,"servicetypes":["DSAT"]},{"sid":"1322","c":"405","t":"Sky Sp Golf","xsg":7,"sg":7,"sf":"sd","dvbtriplet":"","sk":1322,"adult":false,"servicetype":"OTT","schedule":true,"startover":null,"startoverents":false,"timeshifted":false,"local":false,"servicetypes":["DSAT"]},{"sid":"1306","c":"406","t":"SkySp F1","xsg":7,"sg":7,"sf":"sd","dvbtriplet":"","sk":1306,"adult":false,"servicetype":"OTT","schedule":true,"startover":null,"startoverents":false,"timeshifted":false,"local":false,"servicetypes":["DSAT"]},{"sid":"1333","c":"407","t":"SkySp NFL","xsg":7,"sg":7,"sf":"sd","dvbtriplet":"","sk":1333,"adult":false,"servicetype":"OTT","schedule":true,"startover":null,"startoverents":false,"timeshifted":false,"local":false,"servicetypes":["DSAT"]},{"sid":"3839","c":"408","t":"SkySp Arena","xsg":7,"sg":7,"sf":"sd","dvbtriplet":"","sk":3839,"adult":false,"servicetype":"OTT","schedule":true,"startover":null,"startoverents":false,"timeshifted":false,"local":false,"servicetypes":["DSAT"]},{"sid":"4049","c":"409","t":"SkySp News HD","xsg":7,"sg":7,"sf":"hd","dvbtriplet":"","sk":4049,"adult":false,"servicetype":"OTT","schedule":true,"startover":null,"startoverents":false,"timeshifted":false,"local":false,"servicetypes":["DSAT"]},{"sid":"4004","c":"410","t":"Eurosport 1 HD","xsg":7,"sg":7,"sf":"hd","dvbtriplet":"","sk":4004,"adult":false,"servicetype":"OTT","schedule":true,"startover":null,"startoverents":false,"timeshifted":false,"local":false,"servicetypes":["DSAT"]},{"sid":"4009","c":"411","t":"Eurosport 2 HD","xsg":7,"sg":7,"sf":"hd","dvbtriplet":"","sk":4009,"adult":false,"servicetype":"OTT","schedule":true,"startover":null,"startoverents":false,"timeshifted":false,"local":false,"servicetypes":["DSAT"]},{"sid":"5153","c":"412","t":"Premier 1 HD","xsg":7,"sg":7,"sf":"hd","dvbtriplet":"","sk":5153,"adult":false,"servicetype":"OTT","schedule":true,"startover":null,"startoverents":false,"timeshifted":false,"local":false,"servicetypes":["DSAT"]},{"sid":"3661","c":"413","t":"BT Sport 1","xsg":7,"sg":7,"sf":"sd","dvbtriplet":"","sk":3661,"adult":false,"servicetype":"OTT","schedule":true,"startover":null,"startoverents":false,"timeshifted":false,"local":false,"servicetypes":["DSAT"]},{"sid":"3663","c":"414","t":"BT Sport 2","xsg":7,"sg":7,"sf":"sd","dvbtriplet":"","sk":3663,"adult":false,"servicetype":"OTT","schedule":true,"startover":null,"startoverents":false,"timeshifted":false,"local":false,"servicetypes":["DSAT"]},{"sid":"1354","c":"415","t":"SkySp Racing","xsg":7,"sg":7,"sf":"sd","dvbtriplet":"","sk":1354,"adult":false,"servicetype":"OTT","schedule":true,"startover":null,"startoverents":false,"timeshifted":false,"local":false,"servicetypes":["DSAT"]},{"sid":"4090","c":"416","t":"SkySp Mix HD","xsg":7,"sg":7,"sf":"hd","dvbtriplet":"","sk":4090,"adult":false,"servicetype":"OTT","schedule":true,"startover":null,"startoverents":false,"timeshifted":false,"local":false,"servicetypes":["DSAT"]},{"sid":"5032","c":"417","t":"BT Sport 3","xsg":7,"sg":7,"sf":"sd","dvbtriplet":"","sk":5032,"adult":false,"servicetype":"OTT","schedule":true,"startover":null,"startoverents":false,"timeshifted":false,"local":false,"servicetypes":["DSAT"]},{"sid":"1003","c":"418","t":"MUTV HD","xsg":7,"sg":7,"sf":"hd","dvbtriplet":"","sk":1003,"adult":false,"servicetype":"OTT","schedule":true,"startover":null,"startoverents":false,"timeshifted":false,"local":false,"servicetypes":["DSAT"]},{"sid":"1150","c":"422","t":"FreeSports HD","xsg":7,"sg":7,"sf":"hd","dvbtriplet":"","sk":1150,"adult":false,"servicetype":"OTT","schedule":true,"startover":null,"startoverents":false,"timeshifted":false,"local":false,"servicetypes":["DSAT"]},{"sid":"3141","c":"423","t":"BT Sport//ESPN","xsg":7,"sg":7,"sf":"sd","dvbtriplet":"","sk":3141,"adult":false,"servicetype":"OTT","schedule":true,"startover":null,"startoverents":false,"timeshifted":false,"local":false,"servicetypes":["DSAT"]},{"sid":"3845","c":"425","t":"LFCTV HD","xsg":7,"sg":7,"sf":"hd","dvbtriplet":"","sk":3845,"adult":false,"servicetype":"OTT","schedule":true,"startover":null,"startoverents":false,"timeshifted":false,"local":false,"servicetypes":["DSAT"]},{"sid":"3636","c":"426","t":"Racing TV","xsg":7,"sg":7,"sf":"sd","dvbtriplet":"","sk":3636,"adult":false,"servicetype":"OTT","schedule":true,"startover":null,"startoverents":false,"timeshifted":false,"local":false,"servicetypes":["DSAT"]},{"sid":"3417","c":"427","t":"BoxNation","xsg":7,"sg":7,"sf":"sd","dvbtriplet":"","sk":3417,"adult":false,"servicetype":"OTT","schedule":true,"startover":null,"startoverents":false,"timeshifted":false,"local":false,"servicetypes":["DSAT"]},{"sid":"1634","c":"429","t":"Premier 2 HD","xsg":7,"sg":7,"sf":"hd","dvbtriplet":"","sk":1634,"adult":false,"servicetype":"OTT","schedule":true,"startover":null,"startoverents":false,"timeshifted":false,"local":false,"servicetypes":["DSAT"]},{"sid":"1830","c":"433","t":"GINXeSportsTV","xsg":7,"sg":7,"sf":"sd","dvbtriplet":"","sk":1830,"adult":false,"servicetype":"OTT","schedule":true,"startover":null,"startoverents":false,"timeshifted":false,"local":false,"servicetypes":["DSAT"]},{"sid":"1015","c":"435","t":"LaLigaTV HD","xsg":7,"sg":7,"sf":"hd","dvbtriplet":"","sk":1015,"adult":false,"servicetype":"OTT","schedule":true,"startover":null,"startoverents":false,"timeshifted":false,"local":false,"servicetypes":["DSAT"]},{"sid":"1137","c":"457","t":"BBC Scotland","xsg":3,"sg":3,"sf":"sd","dvbtriplet":"","sk":1137,"adult":false,"servicetype":"OTT","schedule":true,"startover":null,"startoverents":false,"timeshifted":false,"local":false,"servicetypes":["DSAT"]},{"sid":"3208","c":"458","t":"BEN","xsg":3,"sg":3,"sf":"sd","dvbtriplet":"","sk":3208,"adult":false,"servicetype":"OTT","schedule":true,"startover":null,"startoverents":false,"timeshifted":false,"local":false,"servicetypes":["DSAT"]},{"sid":"6014","c":"490","t":"BTSptBoxOffHD","xsg":7,"sg":7,"sf":"hd","dvbtriplet":"","sk":6014,"adult":false,"servicetype":"OTT","schedule":true,"startover":null,"startoverents":false,"timeshifted":false,"local":false,"servicetypes":["DSAT"]},{"sid":"1034","c":"491","t":"SkySpBoxOff","xsg":7,"sg":7,"sf":"sd","dvbtriplet":"","sk":1034,"adult":false,"servicetype":"OTT","schedule":true,"startover":null,"startoverents":false,"timeshifted":false,"local":false,"servicetypes":["DSAT"]},{"sid":"1035","c":"492","t":"SkySpBoxOffHD","xsg":7,"sg":7,"sf":"hd","dvbtriplet":"","sk":1035,"adult":false,"servicetype":"OTT","schedule":true,"startover":null,"startoverents":false,"timeshifted":false,"local":false,"servicetypes":["DSAT"]},{"sid":"1218","c":"494","t":"BTSBoxOffWWE","xsg":7,"sg":7,"sf":"hd","dvbtriplet":"","sk":1218,"adult":false,"servicetype":"OTT","schedule":true,"startover":null,"startoverents":false,"timeshifted":false,"local":false,"servicetypes":["DSAT"]},{"sid":"4050","c":"501","t":"Sky News HD","xsg":5,"sg":5,"sf":"hd","dvbtriplet":"","sk":4050,"adult":false,"servicetype":"OTT","schedule":true,"startover":null,"startoverents":false,"timeshifted":false,"local":false,"servicetypes":["DSAT"]},{"sid":"1074","c":"502","t":"Bloomberg HD","xsg":5,"sg":5,"sf":"hd","dvbtriplet":"","sk":1074,"adult":false,"servicetype":"OTT","schedule":true,"startover":null,"startoverents":false,"timeshifted":false,"local":false,"servicetypes":["DSAT"]},{"sid":"2011","c":"503","t":"BBC NEWS","xsg":5,"sg":5,"sf":"sd","dvbtriplet":"","sk":2011,"adult":false,"servicetype":"OTT","schedule":true,"startover":null,"startoverents":false,"timeshifted":false,"local":false,"servicetypes":["DSAT"]},{"sid":"2072","c":"504","t":"BBC Parliament","xsg":5,"sg":5,"sf":"sd","dvbtriplet":"","sk":2072,"adult":false,"servicetype":"OTT","schedule":true,"startover":null,"startoverents":false,"timeshifted":false,"local":false,"servicetypes":["DSAT"]},{"sid":"3611","c":"505","t":"CNBC","xsg":5,"sg":5,"sf":"sd","dvbtriplet":"","sk":3611,"adult":false,"servicetype":"OTT","schedule":true,"startover":null,"startoverents":false,"timeshifted":false,"local":false,"servicetypes":["DSAT"]},{"sid":"5602","c":"506","t":"CNN","xsg":5,"sg":5,"sf":"sd","dvbtriplet":"","sk":5602,"adult":false,"servicetype":"OTT","schedule":true,"startover":null,"startoverents":false,"timeshifted":false,"local":false,"servicetypes":["DSAT"]},{"sid":"3147","c":"507","t":"NHK World HD","xsg":5,"sg":5,"sf":"hd","dvbtriplet":"","sk":3147,"adult":false,"servicetype":"OTT","schedule":true,"startover":null,"startoverents":false,"timeshifted":false,"local":false,"servicetypes":["DSAT"]},{"sid":"4604","c":"508","t":"Euronews","xsg":5,"sg":5,"sf":"sd","dvbtriplet":"","sk":4604,"adult":false,"servicetype":"OTT","schedule":true,"startover":null,"startoverents":false,"timeshifted":false,"local":false,"servicetypes":["DSAT"]},{"sid":"3211","c":"509","t":"CGTN","xsg":5,"sg":5,"sf":"sd","dvbtriplet":"","sk":3211,"adult":false,"servicetype":"OTT","schedule":true,"startover":null,"startoverents":false,"timeshifted":false,"local":false,"servicetypes":["DSAT"]},{"sid":"1821","c":"510","t":"NDTV 24x7","xsg":5,"sg":5,"sf":"sd","dvbtriplet":"","sk":1821,"adult":false,"servicetype":"OTT","schedule":true,"startover":null,"startoverents":false,"timeshifted":false,"local":false,"servicetypes":["DSAT"]},{"sid":"3213","c":"511","t":"RT","xsg":5,"sg":5,"sf":"sd","dvbtriplet":"","sk":3213,"adult":false,"servicetype":"OTT","schedule":true,"startover":null,"startoverents":false,"timeshifted":false,"local":false,"servicetypes":["DSAT"]},{"sid":"1121","c":"512","t":"FRANCE 24 HD","xsg":5,"sg":5,"sf":"hd","dvbtriplet":"","sk":1121,"adult":false,"servicetype":"OTT","schedule":true,"startover":null,"startoverents":false,"timeshifted":false,"local":false,"servicetypes":["DSAT"]},{"sid":"3107","c":"513","t":"Al Jazeera Eng","xsg":5,"sg":5,"sf":"sd","dvbtriplet":"","sk":3107,"adult":false,"servicetype":"OTT","schedule":true,"startover":null,"startoverents":false,"timeshifted":false,"local":false,"servicetypes":["DSAT"]},{"sid":"5882","c":"515","t":"TVC News","xsg":5,"sg":5,"sf":"sd","dvbtriplet":"","sk":5882,"adult":false,"servicetype":"OTT","schedule":true,"startover":null,"startoverents":false,"timeshifted":false,"local":false,"servicetypes":["DSAT"]},{"sid":"5071","c":"516","t":"TRT World","xsg":5,"sg":5,"sf":"sd","dvbtriplet":"","sk":5071,"adult":false,"servicetype":"OTT","schedule":true,"startover":null,"startoverents":false,"timeshifted":false,"local":false,"servicetypes":["DSAT"]},{"sid":"5520","c":"518","t":"Channels 24","xsg":5,"sg":5,"sf":"sd","dvbtriplet":"","sk":5520,"adult":false,"servicetype":"OTT","schedule":true,"startover":null,"startoverents":false,"timeshifted":false,"local":false,"servicetypes":["DSAT"]},{"sid":"5040","c":"519","t":"Arise News","xsg":5,"sg":5,"sf":"sd","dvbtriplet":"","sk":5040,"adult":false,"servicetype":"OTT","schedule":true,"startover":null,"startoverents":false,"timeshifted":false,"local":false,"servicetypes":["DSAT"]},{"sid":"1066","c":"522","t":"Arirang TV HD","xsg":5,"sg":5,"sf":"hd","dvbtriplet":"","sk":1066,"adult":false,"servicetype":"OTT","schedule":true,"startover":null,"startoverents":false,"timeshifted":false,"local":false,"servicetypes":["DSAT"]},{"sid":"3105","c":"580","t":"GOD Channel","xsg":13,"sg":13,"sf":"sd","dvbtriplet":"","sk":3105,"adult":false,"servicetype":"OTT","schedule":true,"startover":null,"startoverents":false,"timeshifted":false,"local":false,"servicetypes":["DSAT"]},{"sid":"3116","c":"581","t":"revelation","xsg":13,"sg":13,"sf":"sd","dvbtriplet":"","sk":3116,"adult":false,"servicetype":"OTT","schedule":true,"startover":null,"startoverents":false,"timeshifted":false,"local":false,"servicetypes":["DSAT"]},{"sid":"3109","c":"582","t":"TBN UK","xsg":13,"sg":13,"sf":"sd","dvbtriplet":"","sk":3109,"adult":false,"servicetype":"OTT","schedule":true,"startover":null,"startoverents":false,"timeshifted":false,"local":false,"servicetypes":["DSAT"]},{"sid":"5541","c":"583","t":"DAYSTAR HD","xsg":13,"sg":13,"sf":"hd","dvbtriplet":"","sk":5541,"adult":false,"servicetype":"OTT","schedule":true,"startover":null,"startoverents":false,"timeshifted":false,"local":false,"servicetypes":["DSAT"]},{"sid":"3219","c":"584","t":"Inspiration TV","xsg":13,"sg":13,"sf":"sd","dvbtriplet":"","sk":3219,"adult":false,"servicetype":"OTT","schedule":true,"startover":null,"startoverents":false,"timeshifted":false,"local":false,"servicetypes":["DSAT"]},{"sid":"1228","c":"585","t":"LoveWorld HD","xsg":13,"sg":13,"sf":"hd","dvbtriplet":"","sk":1228,"adult":false,"servicetype":"OTT","schedule":true,"startover":null,"startoverents":false,"timeshifted":false,"local":false,"servicetypes":["DSAT"]},{"sid":"3205","c":"587","t":"Word Network","xsg":13,"sg":13,"sf":"sd","dvbtriplet":"","sk":3205,"adult":false,"servicetype":"OTT","schedule":true,"startover":null,"startoverents":false,"timeshifted":false,"local":false,"servicetypes":["DSAT"]},{"sid":"3117","c":"588","t":"EWTN Catholic","xsg":13,"sg":13,"sf":"sd","dvbtriplet":"","sk":3117,"adult":false,"servicetype":"OTT","schedule":true,"startover":null,"startoverents":false,"timeshifted":false,"local":false,"servicetypes":["DSAT"]},{"sid":"4404","c":"589","t":"Faith World TV","xsg":13,"sg":13,"sf":"sd","dvbtriplet":"","sk":4404,"adult":false,"servicetype":"OTT","schedule":true,"startover":null,"startoverents":false,"timeshifted":false,"local":false,"servicetypes":["DSAT"]},{"sid":"3776","c":"590","t":"KICC TV","xsg":13,"sg":13,"sf":"sd","dvbtriplet":"","sk":3776,"adult":false,"servicetype":"OTT","schedule":true,"startover":null,"startoverents":false,"timeshifted":false,"local":false,"servicetypes":["DSAT"]},{"sid":"3392","c":"593","t":"SonLife","xsg":13,"sg":13,"sf":"sd","dvbtriplet":"","sk":3392,"adult":false,"servicetype":"OTT","schedule":true,"startover":null,"startoverents":false,"timeshifted":false,"local":false,"servicetypes":["DSAT"]},{"sid":"5354","c":"594","t":"Faith UK","xsg":13,"sg":13,"sf":"sd","dvbtriplet":"","sk":5354,"adult":false,"servicetype":"OTT","schedule":true,"startover":null,"startoverents":false,"timeshifted":false,"local":false,"servicetypes":["DSAT"]},{"sid":"1004","c":"595","t":"Hillsong","xsg":13,"sg":13,"sf":"sd","dvbtriplet":"","sk":1004,"adult":false,"servicetype":"OTT","schedule":true,"startover":null,"startoverents":false,"timeshifted":false,"local":false,"servicetypes":["DSAT"]},{"sid":"1188","c":"596","t":"Good News TV","xsg":13,"sg":13,"sf":"sd","dvbtriplet":"","sk":1188,"adult":false,"servicetype":"OTT","schedule":true,"startover":null,"startoverents":false,"timeshifted":false,"local":false,"servicetypes":["DSAT"]},{"sid":"1191","c":"597","t":"Dunamis TV","xsg":13,"sg":13,"sf":"sd","dvbtriplet":"","sk":1191,"adult":false,"servicetype":"OTT","schedule":true,"startover":null,"startoverents":false,"timeshifted":false,"local":false,"servicetypes":["DSAT"]},{"sid":"1206","c":"598","t":"Celebration TV","xsg":13,"sg":13,"sf":"sd","dvbtriplet":"","sk":1206,"adult":false,"servicetype":"OTT","schedule":true,"startover":null,"startoverents":false,"timeshifted":false,"local":false,"servicetypes":["DSAT"]},{"sid":"4077","c":"601","t":"Cartoon Net HD","xsg":2,"sg":2,"sf":"hd","dvbtriplet":"","sk":4077,"adult":false,"servicetype":"OTT","schedule":true,"startover":null,"startoverents":false,"timeshifted":false,"local":false,"servicetypes":["DSAT"]},{"sid":"5610","c":"602","t":"CN+1","xsg":2,"sg":2,"sf":"sd","dvbtriplet":"","sk":5610,"adult":false,"servicetype":"OTT","schedule":true,"startover":null,"startoverents":false,"timeshifted":false,"local":false,"servicetypes":["DSAT"]},{"sid":"4030","c":"603","t":"Boomerang HD","xsg":2,"sg":2,"sf":"hd","dvbtriplet":"","sk":4030,"adult":false,"servicetype":"OTT","schedule":true,"startover":null,"startoverents":false,"timeshifted":false,"local":false,"servicetypes":["DSAT"]},{"sid":"4069","c":"604","t":"NickelodeonHD","xsg":2,"sg":2,"sf":"hd","dvbtriplet":"","sk":4069,"adult":false,"servicetype":"OTT","schedule":true,"startover":null,"startoverents":false,"timeshifted":false,"local":false,"servicetypes":["DSAT"]},{"sid":"1845","c":"605","t":"Nickelodeon+1","xsg":2,"sg":2,"sf":"sd","dvbtriplet":"","sk":1845,"adult":false,"servicetype":"OTT","schedule":true,"startover":null,"startoverents":false,"timeshifted":false,"local":false,"servicetypes":["DSAT"]},{"sid":"1849","c":"606","t":"Nicktoons","xsg":2,"sg":2,"sf":"sd","dvbtriplet":"","sk":1849,"adult":false,"servicetype":"OTT","schedule":true,"startover":null,"startoverents":false,"timeshifted":false,"local":false,"servicetypes":["DSAT"]},{"sid":"4073","c":"607","t":"Disney Jnr HD","xsg":2,"sg":2,"sf":"hd","dvbtriplet":"","sk":4073,"adult":false,"servicetype":"OTT","schedule":true,"startover":null,"startoverents":false,"timeshifted":false,"local":false,"servicetypes":["DSAT"]},{"sid":"1890","c":"608","t":"Disney Junior+","xsg":2,"sg":2,"sf":"sd","dvbtriplet":"","sk":1890,"adult":false,"servicetype":"OTT","schedule":true,"startover":null,"startoverents":false,"timeshifted":false,"local":false,"servicetypes":["DSAT"]},{"sid":"4071","c":"609","t":"Disney Chnl HD","xsg":2,"sg":2,"sf":"hd","dvbtriplet":"","sk":4071,"adult":false,"servicetype":"OTT","schedule":true,"startover":null,"startoverents":false,"timeshifted":false,"local":false,"servicetypes":["DSAT"]},{"sid":"1882","c":"610","t":"Disney Chnl+1","xsg":2,"sg":2,"sf":"sd","dvbtriplet":"","sk":1882,"adult":false,"servicetype":"OTT","schedule":true,"startover":null,"startoverents":false,"timeshifted":false,"local":false,"servicetypes":["DSAT"]},{"sid":"4070","c":"611","t":"Disney XD HD","xsg":2,"sg":2,"sf":"hd","dvbtriplet":"","sk":4070,"adult":false,"servicetype":"OTT","schedule":true,"startover":null,"startoverents":false,"timeshifted":false,"local":false,"servicetypes":["DSAT"]},{"sid":"3645","c":"612","t":"BabyTV","xsg":2,"sg":2,"sf":"sd","dvbtriplet":"","sk":3645,"adult":false,"servicetype":"OTT","schedule":true,"startover":null,"startoverents":false,"timeshifted":false,"local":false,"servicetypes":["DSAT"]},{"sid":"2020","c":"613","t":"CBBC","xsg":2,"sg":2,"sf":"sd","dvbtriplet":"","sk":2020,"adult":false,"servicetype":"OTT","schedule":true,"startover":null,"startoverents":false,"timeshifted":false,"local":false,"servicetypes":["DSAT"]},{"sid":"2019","c":"614","t":"CBeebies","xsg":2,"sg":2,"sf":"sd","dvbtriplet":"","sk":2019,"adult":false,"servicetype":"OTT","schedule":true,"startover":null,"startoverents":false,"timeshifted":false,"local":false,"servicetypes":["DSAT"]},{"sid":"4039","c":"615","t":"Nick Jr. HD","xsg":2,"sg":2,"sf":"hd","dvbtriplet":"","sk":4039,"adult":false,"servicetype":"OTT","schedule":true,"startover":null,"startoverents":false,"timeshifted":false,"local":false,"servicetypes":["DSAT"]},{"sid":"3750","c":"616","t":"POP","xsg":2,"sg":2,"sf":"sd","dvbtriplet":"","sk":3750,"adult":false,"servicetype":"OTT","schedule":true,"startover":null,"startoverents":false,"timeshifted":false,"local":false,"servicetypes":["DSAT"]},{"sid":"3780","c":"617","t":"Tiny Pop","xsg":2,"sg":2,"sf":"sd","dvbtriplet":"","sk":3780,"adult":false,"servicetype":"OTT","schedule":true,"startover":null,"startoverents":false,"timeshifted":false,"local":false,"servicetypes":["DSAT"]},{"sid":"5603","c":"618","t":"Boomerang+1","xsg":2,"sg":2,"sf":"sd","dvbtriplet":"","sk":5603,"adult":false,"servicetype":"OTT","schedule":true,"startover":null,"startoverents":false,"timeshifted":false,"local":false,"servicetypes":["DSAT"]},{"sid":"1371","c":"619","t":"Cartoonito","xsg":2,"sg":2,"sf":"sd","dvbtriplet":"","sk":1371,"adult":false,"servicetype":"OTT","schedule":true,"startover":null,"startoverents":false,"timeshifted":false,"local":false,"servicetypes":["DSAT"]},{"sid":"4340","c":"620","t":"Nick Jr. Too","xsg":2,"sg":2,"sf":"sd","dvbtriplet":"","sk":4340,"adult":false,"servicetype":"OTT","schedule":true,"startover":null,"startoverents":false,"timeshifted":false,"local":false,"servicetypes":["DSAT"]},{"sid":"6273","c":"621","t":"CITV","xsg":2,"sg":2,"sf":"sd","dvbtriplet":"","sk":6273,"adult":false,"servicetype":"OTT","schedule":true,"startover":null,"startoverents":false,"timeshifted":false,"local":false,"servicetypes":["DSAT"]},{"sid":"4261","c":"624","t":"Tiny Pop+1","xsg":2,"sg":2,"sf":"sd","dvbtriplet":"","sk":4261,"adult":false,"servicetype":"OTT","schedule":true,"startover":null,"startoverents":false,"timeshifted":false,"local":false,"servicetypes":["DSAT"]},{"sid":"4216","c":"625","t":"POP+1","xsg":2,"sg":2,"sf":"sd","dvbtriplet":"","sk":4216,"adult":false,"servicetype":"OTT","schedule":true,"startover":null,"startoverents":false,"timeshifted":false,"local":false,"servicetypes":["DSAT"]},{"sid":"4262","c":"626","t":"POP Max","xsg":2,"sg":2,"sf":"sd","dvbtriplet":"","sk":4262,"adult":false,"servicetype":"OTT","schedule":true,"startover":null,"startoverents":false,"timeshifted":false,"local":false,"servicetypes":["DSAT"]},{"sid":"4263","c":"627","t":"POP Max+1","xsg":2,"sg":2,"sf":"sd","dvbtriplet":"","sk":4263,"adult":false,"servicetype":"OTT","schedule":true,"startover":null,"startoverents":false,"timeshifted":false,"local":false,"servicetypes":["DSAT"]},{"sid":"5089","c":"628","t":"Nick Jr+1","xsg":2,"sg":2,"sf":"sd","dvbtriplet":"","sk":5089,"adult":false,"servicetype":"OTT","schedule":true,"startover":null,"startoverents":false,"timeshifted":false,"local":false,"servicetypes":["DSAT"]},{"sid":"5601","c":"640","t":"Cartoon Netwrk","xsg":2,"sg":2,"sf":"sd","dvbtriplet":"","sk":5601,"adult":false,"servicetype":"OTT","schedule":true,"startover":null,"startoverents":false,"timeshifted":false,"local":false,"servicetypes":["DSAT"]},{"sid":"5609","c":"641","t":"Boomerang","xsg":2,"sg":2,"sf":"sd","dvbtriplet":"","sk":5609,"adult":false,"servicetype":"OTT","schedule":true,"startover":null,"startoverents":false,"timeshifted":false,"local":false,"servicetypes":["DSAT"]},{"sid":"1846","c":"642","t":"Nickelodeon","xsg":2,"sg":2,"sf":"sd","dvbtriplet":"","sk":1846,"adult":false,"servicetype":"OTT","schedule":true,"startover":null,"startoverents":false,"timeshifted":false,"local":false,"servicetypes":["DSAT"]},{"sid":"1884","c":"643","t":"Disney Junior","xsg":2,"sg":2,"sf":"sd","dvbtriplet":"","sk":1884,"adult":false,"servicetype":"OTT","schedule":true,"startover":null,"startoverents":false,"timeshifted":false,"local":false,"servicetypes":["DSAT"]},{"sid":"1881","c":"644","t":"Disney Chnl","xsg":2,"sg":2,"sf":"sd","dvbtriplet":"","sk":1881,"adult":false,"servicetype":"OTT","schedule":true,"startover":null,"startoverents":false,"timeshifted":false,"local":false,"servicetypes":["DSAT"]},{"sid":"1843","c":"645","t":"Disney XD","xsg":2,"sg":2,"sf":"sd","dvbtriplet":"","sk":1843,"adult":false,"servicetype":"OTT","schedule":true,"startover":null,"startoverents":false,"timeshifted":false,"local":false,"servicetypes":["DSAT"]},{"sid":"2078","c":"646","t":"CBBC HD","xsg":2,"sg":2,"sf":"hd","dvbtriplet":"","sk":2078,"adult":false,"servicetype":"OTT","schedule":true,"startover":null,"startoverents":false,"timeshifted":false,"local":false,"servicetypes":["DSAT"]},{"sid":"2088","c":"647","t":"CBeebies HD","xsg":2,"sg":2,"sf":"hd","dvbtriplet":"","sk":2088,"adult":false,"servicetype":"OTT","schedule":true,"startover":null,"startoverents":false,"timeshifted":false,"local":false,"servicetypes":["DSAT"]},{"sid":"1857","c":"648","t":"Nick Jr.","xsg":2,"sg":2,"sf":"sd","dvbtriplet":"","sk":1857,"adult":false,"servicetype":"OTT","schedule":true,"startover":null,"startoverents":false,"timeshifted":false,"local":false,"servicetypes":["DSAT"]},{"sid":"5907","c":"660","t":"QVC","xsg":1,"sg":1,"sf":"sd","dvbtriplet":"","sk":5907,"adult":false,"servicetype":"OTT","schedule":true,"startover":null,"startoverents":false,"timeshifted":false,"local":false,"servicetypes":["DSAT"]},{"sid":"5761","c":"661","t":"JML Direct","xsg":1,"sg":1,"sf":"sd","dvbtriplet":"","sk":5761,"adult":false,"servicetype":"OTT","schedule":true,"startover":null,"startoverents":false,"timeshifted":false,"local":false,"servicetypes":["DSAT"]},{"sid":"1033","c":"662","t":"TJC","xsg":1,"sg":1,"sf":"sd","dvbtriplet":"","sk":1033,"adult":false,"servicetype":"OTT","schedule":true,"startover":null,"startoverents":false,"timeshifted":false,"local":false,"servicetypes":["DSAT"]},{"sid":"4105","c":"663","t":"QVC Beauty","xsg":1,"sg":1,"sf":"sd","dvbtriplet":"","sk":4105,"adult":false,"servicetype":"OTT","schedule":true,"startover":null,"startoverents":false,"timeshifted":false,"local":false,"servicetypes":["DSAT"]},{"sid":"1244","c":"664","t":"Ideal World HD","xsg":1,"sg":1,"sf":"hd","dvbtriplet":"","sk":1244,"adult":false,"servicetype":"OTT","schedule":true,"startover":null,"startoverents":false,"timeshifted":false,"local":false,"servicetypes":["DSAT"]},{"sid":"3010","c":"665","t":"Gems TV","xsg":1,"sg":1,"sf":"sd","dvbtriplet":"","sk":3010,"adult":false,"servicetype":"OTT","schedule":true,"startover":null,"startoverents":false,"timeshifted":false,"local":false,"servicetypes":["DSAT"]},{"sid":"3580","c":"666","t":"High Street TV 1","xsg":1,"sg":1,"sf":"sd","dvbtriplet":"","sk":3580,"adult":false,"servicetype":"OTT","schedule":true,"startover":null,"startoverents":false,"timeshifted":false,"local":false,"servicetypes":["DSAT"]},{"sid":"5706","c":"667","t":"High Street TV 2","xsg":1,"sg":1,"sf":"sd","dvbtriplet":"","sk":5706,"adult":false,"servicetype":"OTT","schedule":true,"startover":null,"startoverents":false,"timeshifted":false,"local":false,"servicetypes":["DSAT"]},{"sid":"5212","c":"668","t":"High Street TV 3","xsg":1,"sg":1,"sf":"sd","dvbtriplet":"","sk":5212,"adult":false,"servicetype":"OTT","schedule":true,"startover":null,"startoverents":false,"timeshifted":false,"local":false,"servicetypes":["DSAT"]},{"sid":"4100","c":"669","t":"Best Direct","xsg":1,"sg":1,"sf":"sd","dvbtriplet":"","sk":4100,"adult":false,"servicetype":"OTT","schedule":true,"startover":null,"startoverents":false,"timeshifted":false,"local":false,"servicetypes":["DSAT"]},{"sid":"1129","c":"670","t":"Primal Living","xsg":1,"sg":1,"sf":"sd","dvbtriplet":"","sk":1129,"adult":false,"servicetype":"OTT","schedule":true,"startover":null,"startoverents":false,"timeshifted":false,"local":false,"servicetypes":["DSAT"]},{"sid":"5740","c":"671","t":"Ideal Extra","xsg":1,"sg":1,"sf":"sd","dvbtriplet":"","sk":5740,"adult":false,"servicetype":"OTT","schedule":true,"startover":null,"startoverents":false,"timeshifted":false,"local":false,"servicetypes":["DSAT"]},{"sid":"3404","c":"672","t":"High Street TV 4","xsg":1,"sg":1,"sf":"sd","dvbtriplet":"","sk":3404,"adult":false,"servicetype":"OTT","schedule":true,"startover":null,"startoverents":false,"timeshifted":false,"local":false,"servicetypes":["DSAT"]},{"sid":"3357","c":"673","t":"Hochanda","xsg":1,"sg":1,"sf":"sd","dvbtriplet":"","sk":3357,"adult":false,"servicetype":"OTT","schedule":true,"startover":null,"startoverents":false,"timeshifted":false,"local":false,"servicetypes":["DSAT"]},{"sid":"3354","c":"674","t":"JewelleryMaker","xsg":1,"sg":1,"sf":"sd","dvbtriplet":"","sk":3354,"adult":false,"servicetype":"OTT","schedule":true,"startover":null,"startoverents":false,"timeshifted":false,"local":false,"servicetypes":["DSAT"]},{"sid":"1130","c":"675","t":"High Street TV 5","xsg":1,"sg":1,"sf":"sd","dvbtriplet":"","sk":1130,"adult":false,"servicetype":"OTT","schedule":true,"startover":null,"startoverents":false,"timeshifted":false,"local":false,"servicetypes":["DSAT"]},{"sid":"3632","c":"676","t":"TV Warehouse","xsg":1,"sg":1,"sf":"sd","dvbtriplet":"","sk":3632,"adult":false,"servicetype":"OTT","schedule":true,"startover":null,"startoverents":false,"timeshifted":false,"local":false,"servicetypes":["DSAT"]},{"sid":"4410","c":"677","t":"QVC Style","xsg":1,"sg":1,"sf":"sd","dvbtriplet":"","sk":4410,"adult":false,"servicetype":"OTT","schedule":true,"startover":null,"startoverents":false,"timeshifted":false,"local":false,"servicetypes":["DSAT"]},{"sid":"3805","c":"679","t":"Thane","xsg":1,"sg":1,"sf":"sd","dvbtriplet":"","sk":3805,"adult":false,"servicetype":"OTT","schedule":true,"startover":null,"startoverents":false,"timeshifted":false,"local":false,"servicetypes":["DSAT"]},{"sid":"3415","c":"680","t":"Psychic Today","xsg":1,"sg":1,"sf":"sd","dvbtriplet":"","sk":3415,"adult":false,"servicetype":"OTT","schedule":true,"startover":null,"startoverents":false,"timeshifted":false,"local":false,"servicetypes":["DSAT"]},{"sid":"3359","c":"682","t":"QVC Extra","xsg":1,"sg":1,"sf":"sd","dvbtriplet":"","sk":3359,"adult":false,"servicetype":"OTT","schedule":true,"startover":null,"startoverents":false,"timeshifted":false,"local":false,"servicetypes":["DSAT"]},{"sid":"1245","c":"683","t":"Create&CraftHD","xsg":1,"sg":1,"sf":"hd","dvbtriplet":"","sk":1245,"adult":false,"servicetype":"OTT","schedule":true,"startover":null,"startoverents":false,"timeshifted":false,"local":false,"servicetypes":["DSAT"]},{"sid":"3012","c":"684","t":"Craft Extra","xsg":1,"sg":1,"sf":"sd","dvbtriplet":"","sk":3012,"adult":false,"servicetype":"OTT","schedule":true,"startover":null,"startoverents":false,"timeshifted":false,"local":false,"servicetypes":["DSAT"]},{"sid":"1018","c":"688","t":"Cruise1st.tv","xsg":1,"sg":1,"sf":"sd","dvbtriplet":"","sk":1018,"adult":false,"servicetype":"OTT","schedule":true,"startover":null,"startoverents":false,"timeshifted":false,"local":false,"servicetypes":["DSAT"]},{"sid":"5703","c":"701","t":"B4U Movies","xsg":14,"sg":14,"sf":"sd","dvbtriplet":"","sk":5703,"adult":false,"servicetype":"OTT","schedule":true,"startover":null,"startoverents":false,"timeshifted":false,"local":false,"servicetypes":["DSAT"]},{"sid":"5708","c":"702","t":"B4U Music","xsg":14,"sg":14,"sf":"sd","dvbtriplet":"","sk":5708,"adult":false,"servicetype":"OTT","schedule":true,"startover":null,"startoverents":false,"timeshifted":false,"local":false,"servicetypes":["DSAT"]},{"sid":"4045","c":"703","t":"SONY TV HD","xsg":14,"sg":14,"sf":"hd","dvbtriplet":"","sk":4045,"adult":false,"servicetype":"OTT","schedule":true,"startover":null,"startoverents":false,"timeshifted":false,"local":false,"servicetypes":["DSAT"]},{"sid":"3608","c":"704","t":"Star Bharat","xsg":14,"sg":14,"sf":"sd","dvbtriplet":"","sk":3608,"adult":false,"servicetype":"OTT","schedule":true,"startover":null,"startoverents":false,"timeshifted":false,"local":false,"servicetypes":["DSAT"]},{"sid":"5300","c":"705","t":"Star Plus HD","xsg":14,"sg":14,"sf":"hd","dvbtriplet":"","sk":5300,"adult":false,"servicetype":"OTT","schedule":true,"startover":null,"startoverents":false,"timeshifted":false,"local":false,"servicetypes":["DSAT"]},{"sid":"1372","c":"706","t":"COLORS HD","xsg":14,"sg":14,"sf":"hd","dvbtriplet":"","sk":1372,"adult":false,"servicetype":"OTT","schedule":true,"startover":null,"startoverents":false,"timeshifted":false,"local":false,"servicetypes":["DSAT"]},{"sid":"2608","c":"707","t":"Zee Cinema","xsg":14,"sg":14,"sf":"sd","dvbtriplet":"","sk":2608,"adult":false,"servicetype":"OTT","schedule":true,"startover":null,"startoverents":false,"timeshifted":false,"local":false,"servicetypes":["DSAT"]},{"sid":"1234","c":"708","t":"Republic Bharat","xsg":14,"sg":14,"sf":"sd","dvbtriplet":"","sk":1234,"adult":false,"servicetype":"OTT","schedule":true,"startover":null,"startoverents":false,"timeshifted":false,"local":false,"servicetypes":["DSAT"]},{"sid":"2607","c":"709","t":"Zee TV","xsg":14,"sg":14,"sf":"sd","dvbtriplet":"","sk":2607,"adult":false,"servicetype":"OTT","schedule":true,"startover":null,"startoverents":false,"timeshifted":false,"local":false,"servicetypes":["DSAT"]},{"sid":"3255","c":"710","t":"AAJ TAK","xsg":14,"sg":14,"sf":"sd","dvbtriplet":"","sk":3255,"adult":false,"servicetype":"OTT","schedule":true,"startover":null,"startoverents":false,"timeshifted":false,"local":false,"servicetypes":["DSAT"]},{"sid":"3401","c":"711","t":"MATV National","xsg":14,"sg":14,"sf":"sd","dvbtriplet":"","sk":3401,"adult":false,"servicetype":"OTT","schedule":true,"startover":null,"startoverents":false,"timeshifted":false,"local":false,"servicetypes":["DSAT"]},{"sid":"1227","c":"712","t":"Foodxp","xsg":14,"sg":14,"sf":"sd","dvbtriplet":"","sk":1227,"adult":false,"servicetype":"OTT","schedule":true,"startover":null,"startoverents":false,"timeshifted":false,"local":false,"servicetypes":["DSAT"]},{"sid":"3916","c":"713","t":"ColorsRishtey","xsg":14,"sg":14,"sf":"sd","dvbtriplet":"","sk":3916,"adult":false,"servicetype":"OTT","schedule":true,"startover":null,"startoverents":false,"timeshifted":false,"local":false,"servicetypes":["DSAT"]},{"sid":"1105","c":"714","t":"ColorsCineplex","xsg":14,"sg":14,"sf":"sd","dvbtriplet":"","sk":1105,"adult":false,"servicetype":"OTT","schedule":true,"startover":null,"startoverents":false,"timeshifted":false,"local":false,"servicetypes":["DSAT"]},{"sid":"4046","c":"715","t":"SONY MAX HD","xsg":14,"sg":14,"sf":"hd","dvbtriplet":"","sk":4046,"adult":false,"servicetype":"OTT","schedule":true,"startover":null,"startoverents":false,"timeshifted":false,"local":false,"servicetypes":["DSAT"]},{"sid":"3720","c":"716","t":"Venus TV","xsg":14,"sg":14,"sf":"sd","dvbtriplet":"","sk":3720,"adult":false,"servicetype":"OTT","schedule":true,"startover":null,"startoverents":false,"timeshifted":false,"local":false,"servicetypes":["DSAT"]},{"sid":"4047","c":"717","t":"Star GOLD HD","xsg":14,"sg":14,"sf":"hd","dvbtriplet":"","sk":4047,"adult":false,"servicetype":"OTT","schedule":true,"startover":null,"startoverents":false,"timeshifted":false,"local":false,"servicetypes":["DSAT"]},{"sid":"3631","c":"718","t":"SONY SAB","xsg":14,"sg":14,"sf":"sd","dvbtriplet":"","sk":3631,"adult":false,"servicetype":"OTT","schedule":true,"startover":null,"startoverents":false,"timeshifted":false,"local":false,"servicetypes":["DSAT"]},{"sid":"5431","c":"720","t":"SONY MAX 2","xsg":14,"sg":14,"sf":"sd","dvbtriplet":"","sk":5431,"adult":false,"servicetype":"OTT","schedule":true,"startover":null,"startoverents":false,"timeshifted":false,"local":false,"servicetypes":["DSAT"]},{"sid":"3594","c":"724","t":"Aastha","xsg":14,"sg":14,"sf":"sd","dvbtriplet":"","sk":3594,"adult":false,"servicetype":"OTT","schedule":true,"startover":null,"startoverents":false,"timeshifted":false,"local":false,"servicetypes":["DSAT"]},{"sid":"1100","c":"725","t":"Sanskar","xsg":14,"sg":14,"sf":"sd","dvbtriplet":"","sk":1100,"adult":false,"servicetype":"OTT","schedule":true,"startover":null,"startoverents":false,"timeshifted":false,"local":false,"servicetypes":["DSAT"]},{"sid":"3201","c":"731","t":"mta-muslim tv","xsg":14,"sg":14,"sf":"sd","dvbtriplet":"","sk":3201,"adult":false,"servicetype":"OTT","schedule":true,"startover":null,"startoverents":false,"timeshifted":false,"local":false,"servicetypes":["DSAT"]},{"sid":"1069","c":"732","t":"HUM Masala","xsg":14,"sg":14,"sf":"sd","dvbtriplet":"","sk":1069,"adult":false,"servicetype":"OTT","schedule":true,"startover":null,"startoverents":false,"timeshifted":false,"local":false,"servicetypes":["DSAT"]},{"sid":"4549","c":"733","t":"Hidayat TV","xsg":14,"sg":14,"sf":"sd","dvbtriplet":"","sk":4549,"adult":false,"servicetype":"OTT","schedule":true,"startover":null,"startoverents":false,"timeshifted":false,"local":false,"servicetypes":["DSAT"]},{"sid":"4407","c":"734","t":"GEO News","xsg":14,"sg":14,"sf":"sd","dvbtriplet":"","sk":4407,"adult":false,"servicetype":"OTT","schedule":true,"startover":null,"startoverents":false,"timeshifted":false,"local":false,"servicetypes":["DSAT"]},{"sid":"3646","c":"735","t":"PTV Global","xsg":14,"sg":14,"sf":"sd","dvbtriplet":"","sk":3646,"adult":false,"servicetype":"OTT","schedule":true,"startover":null,"startoverents":false,"timeshifted":false,"local":false,"servicetypes":["DSAT"]},{"sid":"1026","c":"736","t":"New Vision TV","xsg":14,"sg":14,"sf":"sd","dvbtriplet":"","sk":1026,"adult":false,"servicetype":"OTT","schedule":true,"startover":null,"startoverents":false,"timeshifted":false,"local":false,"servicetypes":["DSAT"]},{"sid":"3214","c":"737","t":"Islam Channel","xsg":14,"sg":14,"sf":"sd","dvbtriplet":"","sk":3214,"adult":false,"servicetype":"OTT","schedule":true,"startover":null,"startoverents":false,"timeshifted":false,"local":false,"servicetypes":["DSAT"]},{"sid":"4110","c":"738","t":"GEO TV","xsg":14,"sg":14,"sf":"sd","dvbtriplet":"","sk":4110,"adult":false,"servicetype":"OTT","schedule":true,"startover":null,"startoverents":false,"timeshifted":false,"local":false,"servicetypes":["DSAT"]},{"sid":"3715","c":"739","t":"Noor TV","xsg":14,"sg":14,"sf":"sd","dvbtriplet":"","sk":3715,"adult":false,"servicetype":"OTT","schedule":true,"startover":null,"startoverents":false,"timeshifted":false,"local":false,"servicetypes":["DSAT"]},{"sid":"6754","c":"742","t":"IQRA TV","xsg":14,"sg":14,"sf":"sd","dvbtriplet":"","sk":6754,"adult":false,"servicetype":"OTT","schedule":true,"startover":null,"startoverents":false,"timeshifted":false,"local":false,"servicetypes":["DSAT"]},{"sid":"3822","c":"743","t":"92 News","xsg":14,"sg":14,"sf":"sd","dvbtriplet":"","sk":3822,"adult":false,"servicetype":"OTT","schedule":true,"startover":null,"startoverents":false,"timeshifted":false,"local":false,"servicetypes":["DSAT"]},{"sid":"4403","c":"744","t":"Islam TV","xsg":14,"sg":14,"sf":"sd","dvbtriplet":"","sk":4403,"adult":false,"servicetype":"OTT","schedule":true,"startover":null,"startoverents":false,"timeshifted":false,"local":false,"servicetypes":["DSAT"]},{"sid":"6765","c":"745","t":"Ahlebait TV","xsg":14,"sg":14,"sf":"sd","dvbtriplet":"","sk":6765,"adult":false,"servicetype":"OTT","schedule":true,"startover":null,"startoverents":false,"timeshifted":false,"local":false,"servicetypes":["DSAT"]},{"sid":"4460","c":"746","t":"Madani Chnl","xsg":14,"sg":14,"sf":"sd","dvbtriplet":"","sk":4460,"adult":false,"servicetype":"OTT","schedule":true,"startover":null,"startoverents":false,"timeshifted":false,"local":false,"servicetypes":["DSAT"]},{"sid":"3722","c":"748","t":"Samaa","xsg":14,"sg":14,"sf":"sd","dvbtriplet":"","sk":3722,"adult":false,"servicetype":"OTT","schedule":true,"startover":null,"startoverents":false,"timeshifted":false,"local":false,"servicetypes":["DSAT"]},{"sid":"4408","c":"749","t":"Takbeer TV","xsg":14,"sg":14,"sf":"sd","dvbtriplet":"","sk":4408,"adult":false,"servicetype":"OTT","schedule":true,"startover":null,"startoverents":false,"timeshifted":false,"local":false,"servicetypes":["DSAT"]},{"sid":"5205","c":"751","t":"HUM EUROPE","xsg":14,"sg":14,"sf":"sd","dvbtriplet":"","sk":5205,"adult":false,"servicetype":"OTT","schedule":true,"startover":null,"startoverents":false,"timeshifted":false,"local":false,"servicetypes":["DSAT"]},{"sid":"5417","c":"752","t":"British Muslim","xsg":14,"sg":14,"sf":"sd","dvbtriplet":"","sk":5417,"adult":false,"servicetype":"OTT","schedule":true,"startover":null,"startoverents":false,"timeshifted":false,"local":false,"servicetypes":["DSAT"]},{"sid":"1038","c":"754","t":"Dunya News","xsg":14,"sg":14,"sf":"sd","dvbtriplet":"","sk":1038,"adult":false,"servicetype":"OTT","schedule":true,"startover":null,"startoverents":false,"timeshifted":false,"local":false,"servicetypes":["DSAT"]},{"sid":"5211","c":"755","t":"Islam Ch Urdu","xsg":14,"sg":14,"sf":"sd","dvbtriplet":"","sk":5211,"adult":false,"servicetype":"OTT","schedule":true,"startover":null,"startoverents":false,"timeshifted":false,"local":false,"servicetypes":["DSAT"]},{"sid":"5359","c":"756","t":"Channel 44","xsg":14,"sg":14,"sf":"sd","dvbtriplet":"","sk":5359,"adult":false,"servicetype":"OTT","schedule":true,"startover":null,"startoverents":false,"timeshifted":false,"local":false,"servicetypes":["DSAT"]},{"sid":"3111","c":"757","t":"Eman Channel","xsg":14,"sg":14,"sf":"sd","dvbtriplet":"","sk":3111,"adult":false,"servicetype":"OTT","schedule":true,"startover":null,"startoverents":false,"timeshifted":false,"local":false,"servicetypes":["DSAT"]},{"sid":"4265","c":"758","t":"ARY Family","xsg":14,"sg":14,"sf":"sd","dvbtriplet":"","sk":4265,"adult":false,"servicetype":"OTT","schedule":true,"startover":null,"startoverents":false,"timeshifted":false,"local":false,"servicetypes":["DSAT"]},{"sid":"1104","c":"759","t":"A1TV","xsg":14,"sg":14,"sf":"sd","dvbtriplet":"","sk":1104,"adult":false,"servicetype":"OTT","schedule":true,"startover":null,"startoverents":false,"timeshifted":false,"local":false,"servicetypes":["DSAT"]},{"sid":"1134","c":"760","t":"HUM News","xsg":14,"sg":14,"sf":"sd","dvbtriplet":"","sk":1134,"adult":false,"servicetype":"OTT","schedule":true,"startover":null,"startoverents":false,"timeshifted":false,"local":false,"servicetypes":["DSAT"]},{"sid":"5715","c":"766","t":"PTC PUNJABI","xsg":14,"sg":14,"sf":"sd","dvbtriplet":"","sk":5715,"adult":false,"servicetype":"OTT","schedule":true,"startover":null,"startoverents":false,"timeshifted":false,"local":false,"servicetypes":["DSAT"]},{"sid":"3653","c":"767","t":"Brit Asia TV","xsg":14,"sg":14,"sf":"sd","dvbtriplet":"","sk":3653,"adult":false,"servicetype":"OTT","schedule":true,"startover":null,"startoverents":false,"timeshifted":false,"local":false,"servicetypes":["DSAT"]},{"sid":"3650","c":"768","t":"Sikh Channel","xsg":14,"sg":14,"sf":"sd","dvbtriplet":"","sk":3650,"adult":false,"servicetype":"OTT","schedule":true,"startover":null,"startoverents":false,"timeshifted":false,"local":false,"servicetypes":["DSAT"]},{"sid":"3355","c":"769","t":"Sangat","xsg":14,"sg":14,"sf":"sd","dvbtriplet":"","sk":3355,"adult":false,"servicetype":"OTT","schedule":true,"startover":null,"startoverents":false,"timeshifted":false,"local":false,"servicetypes":["DSAT"]},{"sid":"5087","c":"770","t":"Akaal Channel","xsg":14,"sg":14,"sf":"sd","dvbtriplet":"","sk":5087,"adult":false,"servicetype":"OTT","schedule":true,"startover":null,"startoverents":false,"timeshifted":false,"local":false,"servicetypes":["DSAT"]},{"sid":"1056","c":"771","t":"KTV","xsg":14,"sg":14,"sf":"sd","dvbtriplet":"","sk":1056,"adult":false,"servicetype":"OTT","schedule":true,"startover":null,"startoverents":false,"timeshifted":false,"local":false,"servicetypes":["DSAT"]},{"sid":"3753","c":"772","t":"Kanshi TV","xsg":14,"sg":14,"sf":"sd","dvbtriplet":"","sk":3753,"adult":false,"servicetype":"OTT","schedule":true,"startover":null,"startoverents":false,"timeshifted":false,"local":false,"servicetypes":["DSAT"]},{"sid":"3815","c":"777","t":"CHSTV","xsg":14,"sg":14,"sf":"sd","dvbtriplet":"","sk":3815,"adult":false,"servicetype":"OTT","schedule":true,"startover":null,"startoverents":false,"timeshifted":false,"local":false,"servicetypes":["DSAT"]},{"sid":"3914","c":"778","t":"IQRA BANGLA","xsg":14,"sg":14,"sf":"sd","dvbtriplet":"","sk":3914,"adult":false,"servicetype":"OTT","schedule":true,"startover":null,"startoverents":false,"timeshifted":false,"local":false,"servicetypes":["DSAT"]},{"sid":"6753","c":"779","t":"ATN","xsg":14,"sg":14,"sf":"sd","dvbtriplet":"","sk":6753,"adult":false,"servicetype":"OTT","schedule":true,"startover":null,"startoverents":false,"timeshifted":false,"local":false,"servicetypes":["DSAT"]},{"sid":"4540","c":"780","t":"NTV","xsg":14,"sg":14,"sf":"sd","dvbtriplet":"","sk":4540,"adult":false,"servicetype":"OTT","schedule":true,"startover":null,"startoverents":false,"timeshifted":false,"local":false,"servicetypes":["DSAT"]},{"sid":"5426","c":"781","t":"TV One","xsg":14,"sg":14,"sf":"sd","dvbtriplet":"","sk":5426,"adult":false,"servicetype":"OTT","schedule":true,"startover":null,"startoverents":false,"timeshifted":false,"local":false,"servicetypes":["DSAT"]},{"sid":"4461","c":"782","t":"iON TV","xsg":14,"sg":14,"sf":"sd","dvbtriplet":"","sk":4461,"adult":false,"servicetype":"OTT","schedule":true,"startover":null,"startoverents":false,"timeshifted":false,"local":false,"servicetypes":["DSAT"]},{"sid":"3209","c":"786","t":"Abu Dhabi TV","xsg":14,"sg":14,"sf":"sd","dvbtriplet":"","sk":3209,"adult":false,"servicetype":"OTT","schedule":true,"startover":null,"startoverents":false,"timeshifted":false,"local":false,"servicetypes":["DSAT"]},{"sid":"3701","c":"787","t":"Ahlulbayt TV","xsg":14,"sg":14,"sf":"sd","dvbtriplet":"","sk":3701,"adult":false,"servicetype":"OTT","schedule":true,"startover":null,"startoverents":false,"timeshifted":false,"local":false,"servicetypes":["DSAT"]},{"sid":"4645","c":"788","t":"SkyNewsArabia","xsg":14,"sg":14,"sf":"sd","dvbtriplet":"","sk":4645,"adult":false,"servicetype":"OTT","schedule":true,"startover":null,"startoverents":false,"timeshifted":false,"local":false,"servicetypes":["DSAT"]},{"sid":"3258","c":"791","t":"PCNE Chinese","xsg":14,"sg":14,"sf":"sd","dvbtriplet":"","sk":3258,"adult":false,"servicetype":"OTT","schedule":true,"startover":null,"startoverents":false,"timeshifted":false,"local":false,"servicetypes":["DSAT"]},{"sid":"3414","c":"792","t":"Record TV HD","xsg":14,"sg":14,"sf":"hd","dvbtriplet":"","sk":3414,"adult":false,"servicetype":"OTT","schedule":true,"startover":null,"startoverents":false,"timeshifted":false,"local":false,"servicetypes":["DSAT"]},{"sid":"1114","c":"793","t":"Iran Int'l HD","xsg":14,"sg":14,"sf":"hd","dvbtriplet":"","sk":1114,"adult":false,"servicetype":"OTT","schedule":true,"startover":null,"startoverents":false,"timeshifted":false,"local":false,"servicetypes":["DSAT"]},{"sid":"2075","c":"802","t":"BBC Two HD","xsg":3,"sg":3,"sf":"hd","dvbtriplet":"","sk":2075,"adult":false,"servicetype":"OTT","schedule":true,"startover":null,"startoverents":false,"timeshifted":false,"local":false,"servicetypes":["DSAT"]},{"sid":"6504","c":"803","t":"ITV HD","xsg":3,"sg":3,"sf":"hd","dvbtriplet":"","sk":6504,"adult":false,"servicetype":"OTT","schedule":true,"startover":null,"startoverents":false,"timeshifted":false,"local":false,"servicetypes":["DSAT"]},{"sid":"4075","c":"804","t":"Channel 4 HD","xsg":3,"sg":3,"sf":"hd","dvbtriplet":"","sk":4075,"adult":false,"servicetype":"OTT","schedule":true,"startover":null,"startoverents":false,"timeshifted":false,"local":false,"servicetypes":["DSAT"]},{"sid":"4058","c":"805","t":"Channel 5 HD","xsg":3,"sg":3,"sf":"hd","dvbtriplet":"","sk":4058,"adult":false,"servicetype":"OTT","schedule":true,"startover":null,"startoverents":false,"timeshifted":false,"local":false,"servicetypes":["DSAT"]},{"sid":"1402","c":"806","t":"Sky One","xsg":3,"sg":3,"sf":"sd","dvbtriplet":"","sk":1402,"adult":false,"servicetype":"OTT","schedule":true,"startover":null,"startoverents":false,"timeshifted":false,"local":false,"servicetypes":["DSAT"]},{"sid":"2201","c":"807","t":"Sky Witness","xsg":3,"sg":3,"sf":"sd","dvbtriplet":"","sk":2201,"adult":false,"servicetype":"OTT","schedule":true,"startover":null,"startoverents":false,"timeshifted":false,"local":false,"servicetypes":["DSAT"]},{"sid":"1412","c":"808","t":"Sky Atlantic","xsg":3,"sg":3,"sf":"sd","dvbtriplet":"","sk":1412,"adult":false,"servicetype":"OTT","schedule":true,"startover":null,"startoverents":false,"timeshifted":false,"local":false,"servicetypes":["DSAT"]},{"sid":"2617","c":"809","t":"W","xsg":3,"sg":3,"sf":"sd","dvbtriplet":"","sk":2617,"adult":false,"servicetype":"OTT","schedule":true,"startover":null,"startoverents":false,"timeshifted":false,"local":false,"servicetypes":["DSAT"]},{"sid":"2304","c":"810","t":"GOLD","xsg":3,"sg":3,"sf":"sd","dvbtriplet":"","sk":2304,"adult":false,"servicetype":"OTT","schedule":true,"startover":null,"startoverents":false,"timeshifted":false,"local":false,"servicetypes":["DSAT"]},{"sid":"2306","c":"811","t":"Dave","xsg":3,"sg":3,"sf":"sd","dvbtriplet":"","sk":2306,"adult":false,"servicetype":"OTT","schedule":true,"startover":null,"startoverents":false,"timeshifted":false,"local":false,"servicetypes":["DSAT"]},{"sid":"2510","c":"812","t":"ComedyCentral","xsg":3,"sg":3,"sf":"sd","dvbtriplet":"","sk":2510,"adult":false,"servicetype":"OTT","schedule":true,"startover":null,"startoverents":false,"timeshifted":false,"local":false,"servicetypes":["DSAT"]},{"sid":"1177","c":"813","t":"Sky Comedy","xsg":3,"sg":3,"sf":"sd","dvbtriplet":"","sk":1177,"adult":false,"servicetype":"OTT","schedule":true,"startover":null,"startoverents":false,"timeshifted":false,"local":false,"servicetypes":["DSAT"]},{"sid":"1127","c":"814","t":"SkyDocumntrs","xsg":11,"sg":11,"sf":"sd","dvbtriplet":"","sk":1127,"adult":false,"servicetype":"OTT","schedule":true,"startover":null,"startoverents":false,"timeshifted":false,"local":false,"servicetypes":["DSAT"]},{"sid":"2086","c":"815","t":"BBC Four HD","xsg":3,"sg":3,"sf":"hd","dvbtriplet":"","sk":2086,"adult":false,"servicetype":"OTT","schedule":true,"startover":null,"startoverents":false,"timeshifted":false,"local":false,"servicetypes":["DSAT"]},{"sid":"6240","c":"816","t":"ITV2","xsg":3,"sg":3,"sf":"sd","dvbtriplet":"","sk":6240,"adult":false,"servicetype":"OTT","schedule":true,"startover":null,"startoverents":false,"timeshifted":false,"local":false,"servicetypes":["DSAT"]},{"sid":"6260","c":"817","t":"ITV3","xsg":3,"sg":3,"sf":"sd","dvbtriplet":"","sk":6260,"adult":false,"servicetype":"OTT","schedule":true,"startover":null,"startoverents":false,"timeshifted":false,"local":false,"servicetypes":["DSAT"]},{"sid":"6272","c":"818","t":"ITV4","xsg":3,"sg":3,"sf":"sd","dvbtriplet":"","sk":6272,"adult":false,"servicetype":"OTT","schedule":true,"startover":null,"startoverents":false,"timeshifted":false,"local":false,"servicetypes":["DSAT"]},{"sid":"2207","c":"819","t":"Sky Crime","xsg":11,"sg":11,"sf":"sd","dvbtriplet":"","sk":2207,"adult":false,"servicetype":"OTT","schedule":true,"startover":null,"startoverents":false,"timeshifted":false,"local":false,"servicetypes":["DSAT"]},{"sid":"1752","c":"820","t":"Sky Arts","xsg":3,"sg":3,"sf":"sd","dvbtriplet":"","sk":1752,"adult":false,"servicetype":"OTT","schedule":true,"startover":null,"startoverents":false,"timeshifted":false,"local":false,"servicetypes":["DSAT"]},{"sid":"1305","c":"821","t":"FOX","xsg":3,"sg":3,"sf":"sd","dvbtriplet":"","sk":1305,"adult":false,"servicetype":"OTT","schedule":true,"startover":null,"startoverents":false,"timeshifted":false,"local":false,"servicetypes":["DSAT"]},{"sid":"2401","c":"822","t":"Discovery","xsg":11,"sg":11,"sf":"sd","dvbtriplet":"","sk":2401,"adult":false,"servicetype":"OTT","schedule":true,"startover":null,"startoverents":false,"timeshifted":false,"local":false,"servicetypes":["DSAT"]},{"sid":"2501","c":"823","t":"MTV","xsg":3,"sg":3,"sf":"sd","dvbtriplet":"","sk":2501,"adult":false,"servicetype":"OTT","schedule":true,"startover":null,"startoverents":false,"timeshifted":false,"local":false,"servicetypes":["DSAT"]},{"sid":"1806","c":"824","t":"Nat Geo","xsg":11,"sg":11,"sf":"sd","dvbtriplet":"","sk":1806,"adult":false,"servicetype":"OTT","schedule":true,"startover":null,"startoverents":false,"timeshifted":false,"local":false,"servicetypes":["DSAT"]},{"sid":"1875","c":"825","t":"Sky History","xsg":11,"sg":11,"sf":"sd","dvbtriplet":"","sk":1875,"adult":false,"servicetype":"OTT","schedule":true,"startover":null,"startoverents":false,"timeshifted":false,"local":false,"servicetypes":["DSAT"]},{"sid":"2303","c":"826","t":"alibi","xsg":3,"sg":3,"sf":"sd","dvbtriplet":"","sk":2303,"adult":false,"servicetype":"OTT","schedule":true,"startover":null,"startoverents":false,"timeshifted":false,"local":false,"servicetypes":["DSAT"]},{"sid":"5450","c":"827","t":"TLC","xsg":3,"sg":3,"sf":"sd","dvbtriplet":"","sk":5450,"adult":false,"servicetype":"OTT","schedule":true,"startover":null,"startoverents":false,"timeshifted":false,"local":false,"servicetypes":["DSAT"]},{"sid":"2296","c":"828","t":"S4C HD","xsg":3,"sg":3,"sf":"hd","dvbtriplet":"","sk":2296,"adult":false,"servicetype":"OTT","schedule":true,"startover":null,"startoverents":false,"timeshifted":false,"local":false,"servicetypes":["DSAT"]},{"sid":"1628","c":"829","t":"E4","xsg":3,"sg":3,"sf":"sd","dvbtriplet":"","sk":1628,"adult":false,"servicetype":"OTT","schedule":true,"startover":null,"startoverents":false,"timeshifted":false,"local":false,"servicetypes":["DSAT"]},{"sid":"3340","c":"830","t":"More4","xsg":3,"sg":3,"sf":"sd","dvbtriplet":"","sk":3340,"adult":false,"servicetype":"OTT","schedule":true,"startover":null,"startoverents":false,"timeshifted":false,"local":false,"servicetypes":["DSAT"]},{"sid":"1128","c":"831","t":"QUEST HD","xsg":3,"sg":3,"sf":"hd","dvbtriplet":"","sk":1128,"adult":false,"servicetype":"OTT","schedule":true,"startover":null,"startoverents":false,"timeshifted":false,"local":false,"servicetypes":["DSAT"]},{"sid":"1360","c":"834","t":"E!","xsg":3,"sg":3,"sf":"sd","dvbtriplet":"","sk":1360,"adult":false,"servicetype":"OTT","schedule":true,"startover":null,"startoverents":false,"timeshifted":false,"local":false,"servicetypes":["DSAT"]},{"sid":"1448","c":"835","t":"Crime+Inv","xsg":11,"sg":11,"sf":"sd","dvbtriplet":"","sk":1448,"adult":false,"servicetype":"OTT","schedule":true,"startover":null,"startoverents":false,"timeshifted":false,"local":false,"servicetypes":["DSAT"]},{"sid":"1847","c":"839","t":"Nat Geo Wild","xsg":11,"sg":11,"sf":"sd","dvbtriplet":"","sk":1847,"adult":false,"servicetype":"OTT","schedule":true,"startover":null,"startoverents":false,"timeshifted":false,"local":false,"servicetypes":["DSAT"]},{"sid":"4021","c":"842","t":"SkyPremiereHD","xsg":6,"sg":6,"sf":"hd","dvbtriplet":"","sk":4021,"adult":false,"servicetype":"OTT","schedule":true,"startover":null,"startoverents":false,"timeshifted":false,"local":false,"servicetypes":["DSAT"]},{"sid":"4015","c":"845","t":"Sky Greats HD","xsg":6,"sg":6,"sf":"hd","dvbtriplet":"","sk":4015,"adult":false,"servicetype":"OTT","schedule":true,"startover":null,"startoverents":false,"timeshifted":false,"local":false,"servicetypes":["DSAT"]},{"sid":"4018","c":"846","t":"Sky Family HD","xsg":6,"sg":6,"sf":"hd","dvbtriplet":"","sk":4018,"adult":false,"servicetype":"OTT","schedule":true,"startover":null,"startoverents":false,"timeshifted":false,"local":false,"servicetypes":["DSAT"]},{"sid":"4013","c":"847","t":"Sky Disney HD","xsg":6,"sg":6,"sf":"hd","dvbtriplet":"","sk":4013,"adult":false,"servicetype":"OTT","schedule":true,"startover":null,"startoverents":false,"timeshifted":false,"local":false,"servicetypes":["DSAT"]},{"sid":"4014","c":"848","t":"Sky Action HD","xsg":6,"sg":6,"sf":"hd","dvbtriplet":"","sk":4014,"adult":false,"servicetype":"OTT","schedule":true,"startover":null,"startoverents":false,"timeshifted":false,"local":false,"servicetypes":["DSAT"]},{"sid":"4019","c":"849","t":"Sky Comedy HD","xsg":6,"sg":6,"sf":"hd","dvbtriplet":"","sk":4019,"adult":false,"servicetype":"OTT","schedule":true,"startover":null,"startoverents":false,"timeshifted":false,"local":false,"servicetypes":["DSAT"]},{"sid":"4062","c":"850","t":"Sky Thriller HD","xsg":6,"sg":6,"sf":"hd","dvbtriplet":"","sk":4062,"adult":false,"servicetype":"OTT","schedule":true,"startover":null,"startoverents":false,"timeshifted":false,"local":false,"servicetypes":["DSAT"]},{"sid":"4016","c":"851","t":"Sky Drama HD","xsg":6,"sg":6,"sf":"hd","dvbtriplet":"","sk":4016,"adult":false,"servicetype":"OTT","schedule":true,"startover":null,"startoverents":false,"timeshifted":false,"local":false,"servicetypes":["DSAT"]},{"sid":"1627","c":"853","t":"Film4","xsg":6,"sg":6,"sf":"sd","dvbtriplet":"","sk":1627,"adult":false,"servicetype":"OTT","schedule":true,"startover":null,"startoverents":false,"timeshifted":false,"local":false,"servicetypes":["DSAT"]},{"sid":"4002","c":"855","t":"SkySpMainEvHD","xsg":7,"sg":7,"sf":"hd","dvbtriplet":"","sk":4002,"adult":false,"servicetype":"OTT","schedule":true,"startover":null,"startoverents":false,"timeshifted":false,"local":false,"servicetypes":["DSAT"]},{"sid":"4010","c":"856","t":"SkySp PL HD","xsg":7,"sg":7,"sf":"hd","dvbtriplet":"","sk":4010,"adult":false,"servicetype":"OTT","schedule":true,"startover":null,"startoverents":false,"timeshifted":false,"local":false,"servicetypes":["DSAT"]},{"sid":"3939","c":"857","t":"SkySp F'ball HD","xsg":7,"sg":7,"sf":"hd","dvbtriplet":"","sk":3939,"adult":false,"servicetype":"OTT","schedule":true,"startover":null,"startoverents":false,"timeshifted":false,"local":false,"servicetypes":["DSAT"]},{"sid":"4081","c":"858","t":"SkySpCricketHD","xsg":7,"sg":7,"sf":"hd","dvbtriplet":"","sk":4081,"adult":false,"servicetype":"OTT","schedule":true,"startover":null,"startoverents":false,"timeshifted":false,"local":false,"servicetypes":["DSAT"]},{"sid":"4026","c":"859","t":"SkySp Golf HD","xsg":7,"sg":7,"sf":"hd","dvbtriplet":"","sk":4026,"adult":false,"servicetype":"OTT","schedule":true,"startover":null,"startoverents":false,"timeshifted":false,"local":false,"servicetypes":["DSAT"]},{"sid":"3835","c":"860","t":"SkySp F1 HD","xsg":7,"sg":7,"sf":"hd","dvbtriplet":"","sk":3835,"adult":false,"servicetype":"OTT","schedule":true,"startover":null,"startoverents":false,"timeshifted":false,"local":false,"servicetypes":["DSAT"]},{"sid":"4022","c":"861","t":"SkySp NFL HD","xsg":7,"sg":7,"sf":"hd","dvbtriplet":"","sk":4022,"adult":false,"servicetype":"OTT","schedule":true,"startover":null,"startoverents":false,"timeshifted":false,"local":false,"servicetypes":["DSAT"]},{"sid":"3940","c":"862","t":"SkySp ArenaHD","xsg":7,"sg":7,"sf":"hd","dvbtriplet":"","sk":3940,"adult":false,"servicetype":"OTT","schedule":true,"startover":null,"startoverents":false,"timeshifted":false,"local":false,"servicetypes":["DSAT"]},{"sid":"1314","c":"863","t":"SkySp News","xsg":7,"sg":7,"sf":"sd","dvbtriplet":"","sk":1314,"adult":false,"servicetype":"OTT","schedule":true,"startover":null,"startoverents":false,"timeshifted":false,"local":false,"servicetypes":["DSAT"]},{"sid":"1726","c":"864","t":"Eurosport 1","xsg":7,"sg":7,"sf":"sd","dvbtriplet":"","sk":1726,"adult":false,"servicetype":"OTT","schedule":true,"startover":null,"startoverents":false,"timeshifted":false,"local":false,"servicetypes":["DSAT"]},{"sid":"1841","c":"865","t":"Eurosport 2","xsg":7,"sg":7,"sf":"sd","dvbtriplet":"","sk":1841,"adult":false,"servicetype":"OTT","schedule":true,"startover":null,"startoverents":false,"timeshifted":false,"local":false,"servicetypes":["DSAT"]},{"sid":"3625","c":"867","t":"BT Sport 1 HD","xsg":7,"sg":7,"sf":"hd","dvbtriplet":"","sk":3625,"adult":false,"servicetype":"OTT","schedule":true,"startover":null,"startoverents":false,"timeshifted":false,"local":false,"servicetypes":["DSAT"]},{"sid":"3627","c":"868","t":"BT Sport 2 HD","xsg":7,"sg":7,"sf":"hd","dvbtriplet":"","sk":3627,"adult":false,"servicetype":"OTT","schedule":true,"startover":null,"startoverents":false,"timeshifted":false,"local":false,"servicetypes":["DSAT"]},{"sid":"3629","c":"869","t":"BT Sport 3 HD","xsg":7,"sg":7,"sf":"hd","dvbtriplet":"","sk":3629,"adult":false,"servicetype":"OTT","schedule":true,"startover":null,"startoverents":false,"timeshifted":false,"local":false,"servicetypes":["DSAT"]},{"sid":"1834","c":"870","t":"MUTV","xsg":7,"sg":7,"sf":"sd","dvbtriplet":"","sk":1834,"adult":false,"servicetype":"OTT","schedule":true,"startover":null,"startoverents":false,"timeshifted":false,"local":false,"servicetypes":["DSAT"]},{"sid":"4040","c":"872","t":"BTSpt//ESPNHD","xsg":7,"sg":7,"sf":"hd","dvbtriplet":"","sk":4040,"adult":false,"servicetype":"OTT","schedule":true,"startover":null,"startoverents":false,"timeshifted":false,"local":false,"servicetypes":["DSAT"]},{"sid":"5272","c":"874","t":"Racing TV HD","xsg":7,"sg":7,"sf":"hd","dvbtriplet":"","sk":5272,"adult":false,"servicetype":"OTT","schedule":true,"startover":null,"startoverents":false,"timeshifted":false,"local":false,"servicetypes":["DSAT"]},{"sid":"1404","c":"875","t":"Sky News","xsg":5,"sg":5,"sf":"sd","dvbtriplet":"","sk":1404,"adult":false,"servicetype":"OTT","schedule":true,"startover":null,"startoverents":false,"timeshifted":false,"local":false,"servicetypes":["DSAT"]},{"sid":"1138","c":"876","t":"BBCScotlandHD","xsg":3,"sg":3,"sf":"hd","dvbtriplet":"","sk":1138,"adult":false,"servicetype":"OTT","schedule":true,"startover":null,"startoverents":false,"timeshifted":false,"local":false,"servicetypes":["DSAT"]},{"sid":"2085","c":"877","t":"BBC NEWS HD","xsg":5,"sg":5,"sf":"hd","dvbtriplet":"","sk":2085,"adult":false,"servicetype":"OTT","schedule":true,"startover":null,"startoverents":false,"timeshifted":false,"local":false,"servicetypes":["DSAT"]},{"sid":"1019","c":"878","t":"CNN HD","xsg":5,"sg":5,"sf":"hd","dvbtriplet":"","sk":1019,"adult":false,"servicetype":"OTT","schedule":true,"startover":null,"startoverents":false,"timeshifted":false,"local":false,"servicetypes":["DSAT"]},{"sid":"5314","c":"879","t":"RT HD","xsg":5,"sg":5,"sf":"hd","dvbtriplet":"","sk":5314,"adult":false,"servicetype":"OTT","schedule":true,"startover":null,"startoverents":false,"timeshifted":false,"local":false,"servicetypes":["DSAT"]},{"sid":"1023","c":"880","t":"Al Jazeera HD","xsg":5,"sg":5,"sf":"hd","dvbtriplet":"","sk":1023,"adult":false,"servicetype":"OTT","schedule":true,"startover":null,"startoverents":false,"timeshifted":false,"local":false,"servicetypes":["DSAT"]},{"sid":"5237","c":"881","t":"TRT World HD","xsg":5,"sg":5,"sf":"hd","dvbtriplet":"","sk":5237,"adult":false,"servicetype":"OTT","schedule":true,"startover":null,"startoverents":false,"timeshifted":false,"local":false,"servicetypes":["DSAT"]},{"sid":"2601","c":"882","t":"SONY TV","xsg":14,"sg":14,"sf":"sd","dvbtriplet":"","sk":2601,"adult":false,"servicetype":"OTT","schedule":true,"startover":null,"startoverents":false,"timeshifted":false,"local":false,"servicetypes":["DSAT"]},{"sid":"1771","c":"883","t":"Star Plus","xsg":14,"sg":14,"sf":"sd","dvbtriplet":"","sk":1771,"adult":false,"servicetype":"OTT","schedule":true,"startover":null,"startoverents":false,"timeshifted":false,"local":false,"servicetypes":["DSAT"]},{"sid":"3714","c":"884","t":"COLORS","xsg":14,"sg":14,"sf":"sd","dvbtriplet":"","sk":3714,"adult":false,"servicetype":"OTT","schedule":true,"startover":null,"startoverents":false,"timeshifted":false,"local":false,"servicetypes":["DSAT"]},{"sid":"2603","c":"886","t":"SONY MAX","xsg":14,"sg":14,"sf":"sd","dvbtriplet":"","sk":2603,"adult":false,"servicetype":"OTT","schedule":true,"startover":null,"startoverents":false,"timeshifted":false,"local":false,"servicetypes":["DSAT"]},{"sid":"3613","c":"887","t":"Star GOLD","xsg":14,"sg":14,"sf":"sd","dvbtriplet":"","sk":3613,"adult":false,"servicetype":"OTT","schedule":true,"startover":null,"startoverents":false,"timeshifted":false,"local":false,"servicetypes":["DSAT"]},{"sid":"4032","c":"888","t":"SkySp Racing HD","xsg":7,"sg":7,"sf":"hd","dvbtriplet":"","sk":4032,"adult":false,"servicetype":"OTT","schedule":true,"startover":null,"startoverents":false,"timeshifted":false,"local":false,"servicetypes":["DSAT"]},{"sid":"5135","c":"889","t":"Smithsonian HD","xsg":11,"sg":11,"sf":"hd","dvbtriplet":"","sk":5135,"adult":false,"servicetype":"OTT","schedule":true,"startover":null,"startoverents":false,"timeshifted":false,"local":false,"servicetypes":["DSAT"]},{"sid":"1088","c":"890","t":"CNBC HD","xsg":5,"sg":5,"sf":"hd","dvbtriplet":"","sk":1088,"adult":false,"servicetype":"OTT","schedule":true,"startover":null,"startoverents":false,"timeshifted":false,"local":false,"servicetypes":["DSAT"]},{"sid":"1048","c":"891","t":"CGTN HD","xsg":5,"sg":5,"sf":"hd","dvbtriplet":"","sk":1048,"adult":false,"servicetype":"OTT","schedule":true,"startover":null,"startoverents":false,"timeshifted":false,"local":false,"servicetypes":["DSAT"]},{"sid":"2505","c":"892","t":"SYFY","xsg":3,"sg":3,"sf":"sd","dvbtriplet":"","sk":2505,"adult":false,"servicetype":"OTT","schedule":true,"startover":null,"startoverents":false,"timeshifted":false,"local":false,"servicetypes":["DSAT"]},{"sid":"1194","c":"893","t":"Sky Nature","xsg":11,"sg":11,"sf":"sd","dvbtriplet":"","sk":1194,"adult":false,"servicetype":"OTT","schedule":true,"startover":null,"startoverents":false,"timeshifted":false,"local":false,"servicetypes":["DSAT"]},{"sid":"1879","c":"894","t":"Sky History2","xsg":11,"sg":11,"sf":"sd","dvbtriplet":"","sk":1879,"adult":false,"servicetype":"OTT","schedule":true,"startover":null,"startoverents":false,"timeshifted":false,"local":false,"servicetypes":["DSAT"]},{"sid":"4091","c":"895","t":"SkySp Mix","xsg":7,"sg":7,"sf":"sd","dvbtriplet":"","sk":4091,"adult":false,"servicetype":"OTT","schedule":true,"startover":null,"startoverents":false,"timeshifted":false,"local":false,"servicetypes":["DSAT"]},{"sid":"1811","c":"896","t":"Sky Musicals","xsg":6,"sg":6,"sf":"sd","dvbtriplet":"","sk":1811,"adult":false,"servicetype":"OTT","schedule":true,"startover":null,"startoverents":false,"timeshifted":false,"local":false,"servicetypes":["DSAT"]},{"sid":"1814","c":"897","t":"Sky Hits","xsg":6,"sg":6,"sf":"sd","dvbtriplet":"","sk":1814,"adult":false,"servicetype":"OTT","schedule":true,"startover":null,"startoverents":false,"timeshifted":false,"local":false,"servicetypes":["DSAT"]},{"sid":"1807","c":"898","t":"Sky SciFi/Horror","xsg":6,"sg":6,"sf":"sd","dvbtriplet":"","sk":1807,"adult":false,"servicetype":"OTT","schedule":true,"startover":null,"startoverents":false,"timeshifted":false,"local":false,"servicetypes":["DSAT"]},{"sid":"1819","c":"899","t":"Sky Intro","xsg":0,"sg":0,"sf":"sd","dvbtriplet":"","sk":1819,"adult":false,"servicetype":"OTT","schedule":true,"startover":null,"startoverents":false,"timeshifted":false,"local":false,"servicetypes":["DSAT"]},{"sid":"1268","c":"900","t":"Television X","xsg":9,"sg":9,"sf":"sd","dvbtriplet":"","sk":1268,"adult":true,"servicetype":"OTT","schedule":true,"startover":null,"startoverents":false,"timeshifted":false,"local":false,"servicetypes":["DSAT"]},{"sid":"4195","c":"901","t":"Adult Channel","xsg":9,"sg":9,"sf":"sd","dvbtriplet":"","sk":4195,"adult":true,"servicetype":"OTT","schedule":false,"startover":null,"startoverents":false,"timeshifted":false,"local":false,"servicetypes":["DSAT"]},{"sid":"4191","c":"902","t":"Xpanded TV","xsg":9,"sg":9,"sf":"sd","dvbtriplet":"","sk":4191,"adult":true,"servicetype":"OTT","schedule":false,"startover":null,"startoverents":false,"timeshifted":false,"local":false,"servicetypes":["DSAT"]},{"sid":"3586","c":"903","t":"TVX Bangers","xsg":9,"sg":9,"sf":"sd","dvbtriplet":"","sk":3586,"adult":true,"servicetype":"OTT","schedule":false,"startover":null,"startoverents":false,"timeshifted":false,"local":false,"servicetypes":["DSAT"]},{"sid":"3587","c":"904","t":"TVX 40+","xsg":9,"sg":9,"sf":"sd","dvbtriplet":"","sk":3587,"adult":true,"servicetype":"OTT","schedule":false,"startover":null,"startoverents":false,"timeshifted":false,"local":false,"servicetypes":["DSAT"]},{"sid":"3400","c":"906","t":"Babenation","xsg":9,"sg":9,"sf":"sd","dvbtriplet":"","sk":3400,"adult":true,"servicetype":"OTT","schedule":false,"startover":null,"startoverents":false,"timeshifted":false,"local":false,"servicetypes":["DSAT"]},{"sid":"4538","c":"908","t":"Get Lucky TV","xsg":9,"sg":9,"sf":"sd","dvbtriplet":"","sk":4538,"adult":true,"servicetype":"OTT","schedule":false,"startover":null,"startoverents":false,"timeshifted":false,"local":false,"servicetypes":["DSAT"]},{"sid":"1030","c":"910","t":"XXXpublicpikups","xsg":9,"sg":9,"sf":"sd","dvbtriplet":"","sk":1030,"adult":true,"servicetype":"OTT","schedule":false,"startover":null,"startoverents":false,"timeshifted":false,"local":false,"servicetypes":["DSAT"]},{"sid":"4197","c":"913","t":"babes&brazzers","xsg":9,"sg":9,"sf":"sd","dvbtriplet":"","sk":4197,"adult":true,"servicetype":"OTT","schedule":false,"startover":null,"startoverents":false,"timeshifted":false,"local":false,"servicetypes":["DSAT"]},{"sid":"3421","c":"914","t":"Xpanded TV2","xsg":9,"sg":9,"sf":"sd","dvbtriplet":"","sk":3421,"adult":true,"servicetype":"OTT","schedule":false,"startover":null,"startoverents":false,"timeshifted":false,"local":false,"servicetypes":["DSAT"]},{"sid":"1031","c":"915","t":"XXX Big&Bouncy","xsg":9,"sg":9,"sf":"sd","dvbtriplet":"","sk":1031,"adult":true,"servicetype":"OTT","schedule":false,"startover":null,"startoverents":false,"timeshifted":false,"local":false,"servicetypes":["DSAT"]},{"sid":"1029","c":"916","t":"XXX Girl Girl","xsg":9,"sg":9,"sf":"sd","dvbtriplet":"","sk":1029,"adult":true,"servicetype":"OTT","schedule":false,"startover":null,"startoverents":false,"timeshifted":false,"local":false,"servicetypes":["DSAT"]},{"sid":"1028","c":"917","t":"XXX College","xsg":9,"sg":9,"sf":"sd","dvbtriplet":"","sk":1028,"adult":true,"servicetype":"OTT","schedule":false,"startover":null,"startoverents":false,"timeshifted":false,"local":false,"servicetypes":["DSAT"]},{"sid":"1754","c":"918","t":"XXX Mums","xsg":9,"sg":9,"sf":"sd","dvbtriplet":"","sk":1754,"adult":true,"servicetype":"OTT","schedule":false,"startover":null,"startoverents":false,"timeshifted":false,"local":false,"servicetypes":["DSAT"]},{"sid":"4192","c":"922","t":"Climax","xsg":9,"sg":9,"sf":"sd","dvbtriplet":"","sk":4192,"adult":true,"servicetype":"OTT","schedule":false,"startover":null,"startoverents":false,"timeshifted":false,"local":false,"servicetypes":["DSAT"]},{"sid":"3530","c":"924","t":"Studio 66 3","xsg":9,"sg":9,"sf":"sd","dvbtriplet":"","sk":3530,"adult":true,"servicetype":"OTT","schedule":false,"startover":null,"startoverents":false,"timeshifted":false,"local":false,"servicetypes":["DSAT"]},{"sid":"1852","c":"950","t":"Sky Intro","xsg":0,"sg":0,"sf":"sd","dvbtriplet":"","sk":1852,"adult":false,"servicetype":"OTT","schedule":true,"startover":null,"startoverents":false,"timeshifted":false,"local":false,"servicetypes":["DSAT"]},{"sid":"2004","c":"951","t":"BBC One Scot","xsg":3,"sg":3,"sf":"sd","dvbtriplet":"","sk":2004,"adult":false,"servicetype":"OTT","schedule":true,"startover":null,"startoverents":false,"timeshifted":false,"local":false,"servicetypes":["DSAT"]},{"sid":"2003","c":"952","t":"BBC One Wales","xsg":3,"sg":3,"sf":"sd","dvbtriplet":"","sk":2003,"adult":false,"servicetype":"OTT","schedule":true,"startover":null,"startoverents":false,"timeshifted":false,"local":false,"servicetypes":["DSAT"]},{"sid":"2005","c":"953","t":"BBC One NI","xsg":3,"sg":3,"sf":"sd","dvbtriplet":"","sk":2005,"adult":false,"servicetype":"OTT","schedule":true,"startover":null,"startoverents":false,"timeshifted":false,"local":false,"servicetypes":["DSAT"]},{"sid":"2155","c":"955","t":"BBC One NE&C","xsg":3,"sg":3,"sf":"sd","dvbtriplet":"","sk":2155,"adult":false,"servicetype":"OTT","schedule":true,"startover":null,"startoverents":false,"timeshifted":false,"local":false,"servicetypes":["DSAT"]},{"sid":"2104","c":"956","t":"BBC One Yorks","xsg":3,"sg":3,"sf":"sd","dvbtriplet":"","sk":2104,"adult":false,"servicetype":"OTT","schedule":true,"startover":null,"startoverents":false,"timeshifted":false,"local":false,"servicetypes":["DSAT"]},{"sid":"2103","c":"957","t":"BBC One Yk&Li","xsg":3,"sg":3,"sf":"sd","dvbtriplet":"","sk":2103,"adult":false,"servicetype":"OTT","schedule":true,"startover":null,"startoverents":false,"timeshifted":false,"local":false,"servicetypes":["DSAT"]},{"sid":"2102","c":"958","t":"BBC One N West","xsg":3,"sg":3,"sf":"sd","dvbtriplet":"","sk":2102,"adult":false,"servicetype":"OTT","schedule":true,"startover":null,"startoverents":false,"timeshifted":false,"local":false,"servicetypes":["DSAT"]},{"sid":"2101","c":"959","t":"BBC One W Mid","xsg":3,"sg":3,"sf":"sd","dvbtriplet":"","sk":2101,"adult":false,"servicetype":"OTT","schedule":true,"startover":null,"startoverents":false,"timeshifted":false,"local":false,"servicetypes":["DSAT"]},{"sid":"2105","c":"960","t":"BBC One E Mid","xsg":3,"sg":3,"sf":"sd","dvbtriplet":"","sk":2105,"adult":false,"servicetype":"OTT","schedule":true,"startover":null,"startoverents":false,"timeshifted":false,"local":false,"servicetypes":["DSAT"]},{"sid":"2106","c":"961","t":"BBC One East E","xsg":3,"sg":3,"sf":"sd","dvbtriplet":"","sk":2106,"adult":false,"servicetype":"OTT","schedule":true,"startover":null,"startoverents":false,"timeshifted":false,"local":false,"servicetypes":["DSAT"]},{"sid":"2073","c":"962","t":"BBC One East W","xsg":3,"sg":3,"sf":"sd","dvbtriplet":"","sk":2073,"adult":false,"servicetype":"OTT","schedule":true,"startover":null,"startoverents":false,"timeshifted":false,"local":false,"servicetypes":["DSAT"]},{"sid":"2152","c":"963","t":"BBC One S East","xsg":3,"sg":3,"sf":"sd","dvbtriplet":"","sk":2152,"adult":false,"servicetype":"OTT","schedule":true,"startover":null,"startoverents":false,"timeshifted":false,"local":false,"servicetypes":["DSAT"]},{"sid":"2153","c":"964","t":"BBC One South","xsg":3,"sg":3,"sf":"sd","dvbtriplet":"","sk":2153,"adult":false,"servicetype":"OTT","schedule":true,"startover":null,"startoverents":false,"timeshifted":false,"local":false,"servicetypes":["DSAT"]},{"sid":"2156","c":"965","t":"BBC One Oxford","xsg":3,"sg":3,"sf":"sd","dvbtriplet":"","sk":2156,"adult":false,"servicetype":"OTT","schedule":true,"startover":null,"startoverents":false,"timeshifted":false,"local":false,"servicetypes":["DSAT"]},{"sid":"2151","c":"966","t":"BBC One West","xsg":3,"sg":3,"sf":"sd","dvbtriplet":"","sk":2151,"adult":false,"servicetype":"OTT","schedule":true,"startover":null,"startoverents":false,"timeshifted":false,"local":false,"servicetypes":["DSAT"]},{"sid":"2154","c":"967","t":"BBC One S West","xsg":3,"sg":3,"sf":"sd","dvbtriplet":"","sk":2154,"adult":false,"servicetype":"OTT","schedule":true,"startover":null,"startoverents":false,"timeshifted":false,"local":false,"servicetypes":["DSAT"]},{"sid":"2074","c":"968","t":"BBC One CI","xsg":3,"sg":3,"sf":"sd","dvbtriplet":"","sk":2074,"adult":false,"servicetype":"OTT","schedule":true,"startover":null,"startoverents":false,"timeshifted":false,"local":false,"servicetypes":["DSAT"]},{"sid":"2015","c":"971","t":"BBC Two Wales","xsg":3,"sg":3,"sf":"sd","dvbtriplet":"","sk":2015,"adult":false,"servicetype":"OTT","schedule":true,"startover":null,"startoverents":false,"timeshifted":false,"local":false,"servicetypes":["DSAT"]},{"sid":"2017","c":"972","t":"BBC Two NI","xsg":3,"sg":3,"sf":"sd","dvbtriplet":"","sk":2017,"adult":false,"servicetype":"OTT","schedule":true,"startover":null,"startoverents":false,"timeshifted":false,"local":false,"servicetypes":["DSAT"]},{"sid":"2082","c":"977","t":"BBC One ScotHD","xsg":3,"sg":3,"sf":"hd","dvbtriplet":"","sk":2082,"adult":false,"servicetype":"OTT","schedule":true,"startover":null,"startoverents":false,"timeshifted":false,"local":false,"servicetypes":["DSAT"]},{"sid":"2083","c":"978","t":"BBC One Wal HD","xsg":3,"sg":3,"sf":"hd","dvbtriplet":"","sk":2083,"adult":false,"servicetype":"OTT","schedule":true,"startover":null,"startoverents":false,"timeshifted":false,"local":false,"servicetypes":["DSAT"]},{"sid":"2081","c":"979","t":"BBC One NI HD","xsg":3,"sg":3,"sf":"hd","dvbtriplet":"","sk":2081,"adult":false,"servicetype":"OTT","schedule":true,"startover":null,"startoverents":false,"timeshifted":false,"local":false,"servicetypes":["DSAT"]},{"sid":"3015","c":"980","t":"BBC Two Wal HD","xsg":3,"sg":3,"sf":"hd","dvbtriplet":"","sk":3015,"adult":false,"servicetype":"OTT","schedule":true,"startover":null,"startoverents":false,"timeshifted":false,"local":false,"servicetypes":["DSAT"]},{"sid":"2051","c":"981","t":"BBC RB 1","xsg":3,"sg":3,"sf":"sd","dvbtriplet":"","sk":2051,"adult":false,"servicetype":"OTT","schedule":true,"startover":null,"startoverents":false,"timeshifted":false,"local":false,"servicetypes":["DSAT"]},{"sid":"1850","c":"996","t":"Chl Line-up","xsg":0,"sg":0,"sf":"au","dvbtriplet":"","sk":1850,"adult":false,"servicetype":"OTT","schedule":false,"startover":null,"startoverents":false,"timeshifted":false,"local":false,"servicetypes":["DSAT"]},{"sid":"1022","c":"998","t":"Sky Intro","xsg":0,"sg":0,"sf":"hd","dvbtriplet":"","sk":1022,"adult":false,"servicetype":"OTT","schedule":true,"startover":null,"startoverents":false,"timeshifted":false,"local":false,"servicetypes":["DSAT"]},{"sid":"1810","c":"999","t":"Sky","xsg":0,"sg":0,"sf":"sd","dvbtriplet":"","sk":1810,"adult":false,"servicetype":"OTT","schedule":true,"startover":null,"startoverents":false,"timeshifted":false,"local":false,"servicetypes":["DSAT"]},{"sid":"8040","c":"2049","t":"My Sky","xsg":0,"sg":0,"sf":"sd","dvbtriplet":"","sk":8040,"adult":false,"servicetype":"OTT","schedule":false,"startover":null,"startoverents":false,"timeshifted":false,"local":false,"servicetypes":["DSAT"]},{"sid":"8025","c":"4901","t":"Browse Shop","xsg":5,"sg":5,"sf":"sd","dvbtriplet":"","sk":8025,"adult":false,"servicetype":"OTT","schedule":false,"startover":null,"startoverents":false,"timeshifted":false,"local":false,"servicetypes":["DSAT"]},{"sid":"8346","c":"4902","t":"Reset your PIN","xsg":0,"sg":0,"sf":"sd","dvbtriplet":"","sk":8346,"adult":false,"servicetype":"OTT","schedule":false,"startover":null,"startoverents":false,"timeshifted":false,"local":false,"servicetypes":["DSAT"]},{"sid":"8266","c":"4903","t":"My Account","xsg":0,"sg":0,"sf":"sd","dvbtriplet":"","sk":8266,"adult":false,"servicetype":"OTT","schedule":false,"startover":null,"startoverents":false,"timeshifted":false,"local":false,"servicetypes":["DSAT"]},{"sid":"8459","c":"4904","t":"Get Sky Products","xsg":0,"sg":0,"sf":"sd","dvbtriplet":"","sk":8459,"adult":false,"servicetype":"OTT","schedule":false,"startover":null,"startoverents":false,"timeshifted":false,"local":false,"servicetypes":["DSAT"]},{"sid":"8331","c":"4905","t":"Help and Support","xsg":0,"sg":0,"sf":"sd","dvbtriplet":"","sk":8331,"adult":false,"servicetype":"OTT","schedule":false,"startover":null,"startoverents":false,"timeshifted":false,"local":false,"servicetypes":["DSAT"]},{"sid":"8301","c":"4906","t":"Program Your Remote","xsg":0,"sg":0,"sf":"sd","dvbtriplet":"","sk":8301,"adult":false,"servicetype":"OTT","schedule":false,"startover":null,"startoverents":false,"timeshifted":false,"local":false,"servicetypes":["DSAT"]},{"sid":"2111","c":"0101","t":"BBC R1","xsg":4,"sg":4,"sf":"au","dvbtriplet":"","sk":2111,"adult":false,"servicetype":"OTT","schedule":true,"startover":null,"startoverents":false,"timeshifted":false,"local":false,"servicetypes":["DSAT"]},{"sid":"2112","c":"0102","t":"BBC R2","xsg":4,"sg":4,"sf":"au","dvbtriplet":"","sk":2112,"adult":false,"servicetype":"OTT","schedule":true,"startover":null,"startoverents":false,"timeshifted":false,"local":false,"servicetypes":["DSAT"]},{"sid":"2113","c":"0103","t":"BBC R3","xsg":4,"sg":4,"sf":"au","dvbtriplet":"","sk":2113,"adult":false,"servicetype":"OTT","schedule":true,"startover":null,"startoverents":false,"timeshifted":false,"local":false,"servicetypes":["DSAT"]},{"sid":"2114","c":"0104","t":"BBC R4 FM","xsg":4,"sg":4,"sf":"au","dvbtriplet":"","sk":2114,"adult":false,"servicetype":"OTT","schedule":true,"startover":null,"startoverents":false,"timeshifted":false,"local":false,"servicetypes":["DSAT"]},{"sid":"2026","c":"0105","t":"BBC R5L","xsg":4,"sg":4,"sf":"au","dvbtriplet":"","sk":2026,"adult":false,"servicetype":"OTT","schedule":true,"startover":null,"startoverents":false,"timeshifted":false,"local":false,"servicetypes":["DSAT"]},{"sid":"3470","c":"0106","t":"Classic FM","xsg":4,"sg":4,"sf":"au","dvbtriplet":"","sk":3470,"adult":false,"servicetype":"OTT","schedule":true,"startover":null,"startoverents":false,"timeshifted":false,"local":false,"servicetypes":["DSAT"]},{"sid":"2901","c":"0107","t":"Absolute","xsg":4,"sg":4,"sf":"au","dvbtriplet":"","sk":2901,"adult":false,"servicetype":"OTT","schedule":true,"startover":null,"startoverents":false,"timeshifted":false,"local":false,"servicetypes":["DSAT"]},{"sid":"2902","c":"0108","t":"talkSPORT","xsg":4,"sg":4,"sf":"au","dvbtriplet":"","sk":2902,"adult":false,"servicetype":"OTT","schedule":true,"startover":null,"startoverents":false,"timeshifted":false,"local":false,"servicetypes":["DSAT"]},{"sid":"2908","c":"0109","t":"Capital","xsg":4,"sg":4,"sf":"au","dvbtriplet":"","sk":2908,"adult":false,"servicetype":"OTT","schedule":true,"startover":null,"startoverents":false,"timeshifted":false,"local":false,"servicetypes":["DSAT"]},{"sid":"3475","c":"0110","t":"Planet Rock","xsg":4,"sg":4,"sf":"au","dvbtriplet":"","sk":3475,"adult":false,"servicetype":"OTT","schedule":true,"startover":null,"startoverents":false,"timeshifted":false,"local":false,"servicetypes":["DSAT"]},{"sid":"2909","c":"0111","t":"Heart","xsg":4,"sg":4,"sf":"au","dvbtriplet":"","sk":2909,"adult":false,"servicetype":"OTT","schedule":true,"startover":null,"startoverents":false,"timeshifted":false,"local":false,"servicetypes":["DSAT"]},{"sid":"2928","c":"0112","t":"Heart 80s","xsg":4,"sg":4,"sf":"au","dvbtriplet":"","sk":2928,"adult":false,"servicetype":"OTT","schedule":true,"startover":null,"startoverents":false,"timeshifted":false,"local":false,"servicetypes":["DSAT"]},{"sid":"2904","c":"0113","t":"Radio X","xsg":4,"sg":4,"sf":"au","dvbtriplet":"","sk":2904,"adult":false,"servicetype":"OTT","schedule":true,"startover":null,"startoverents":false,"timeshifted":false,"local":false,"servicetypes":["DSAT"]},{"sid":"2971","c":"0114","t":"Capital XTRA","xsg":4,"sg":4,"sf":"au","dvbtriplet":"","sk":2971,"adult":false,"servicetype":"OTT","schedule":true,"startover":null,"startoverents":false,"timeshifted":false,"local":false,"servicetypes":["DSAT"]},{"sid":"2122","c":"0115","t":"BBC WS","xsg":4,"sg":4,"sf":"au","dvbtriplet":"","sk":2122,"adult":false,"servicetype":"OTT","schedule":true,"startover":null,"startoverents":false,"timeshifted":false,"local":false,"servicetypes":["DSAT"]},{"sid":"2117","c":"0116","t":"BBC R Scot.","xsg":4,"sg":4,"sf":"au","dvbtriplet":"","sk":2117,"adult":false,"servicetype":"OTT","schedule":true,"startover":null,"startoverents":false,"timeshifted":false,"local":false,"servicetypes":["DSAT"]},{"sid":"2118","c":"0117","t":"BBC R Wales","xsg":4,"sg":4,"sf":"au","dvbtriplet":"","sk":2118,"adult":false,"servicetype":"OTT","schedule":true,"startover":null,"startoverents":false,"timeshifted":false,"local":false,"servicetypes":["DSAT"]},{"sid":"2119","c":"0118","t":"BBC R Ulster","xsg":4,"sg":4,"sf":"au","dvbtriplet":"","sk":2119,"adult":false,"servicetype":"OTT","schedule":true,"startover":null,"startoverents":false,"timeshifted":false,"local":false,"servicetypes":["DSAT"]},{"sid":"2120","c":"0119","t":"BBC Asian","xsg":4,"sg":4,"sf":"au","dvbtriplet":"","sk":2120,"adult":false,"servicetype":"OTT","schedule":true,"startover":null,"startoverents":false,"timeshifted":false,"local":false,"servicetypes":["DSAT"]},{"sid":"2127","c":"0120","t":"BBC 6 Music","xsg":4,"sg":4,"sf":"au","dvbtriplet":"","sk":2127,"adult":false,"servicetype":"OTT","schedule":true,"startover":null,"startoverents":false,"timeshifted":false,"local":false,"servicetypes":["DSAT"]},{"sid":"2903","c":"0121","t":"Gold","xsg":4,"sg":4,"sf":"au","dvbtriplet":"","sk":2903,"adult":false,"servicetype":"OTT","schedule":true,"startover":null,"startoverents":false,"timeshifted":false,"local":false,"servicetypes":["DSAT"]},{"sid":"4158","c":"0122","t":"WRN Europe","xsg":4,"sg":4,"sf":"au","dvbtriplet":"","sk":4158,"adult":false,"servicetype":"OTT","schedule":true,"startover":null,"startoverents":false,"timeshifted":false,"local":false,"servicetypes":["DSAT"]},{"sid":"2926","c":"0124","t":"LBC","xsg":4,"sg":4,"sf":"au","dvbtriplet":"","sk":2926,"adult":false,"servicetype":"OTT","schedule":true,"startover":null,"startoverents":false,"timeshifted":false,"local":false,"servicetypes":["DSAT"]},{"sid":"2917","c":"0128","t":"Smooth","xsg":4,"sg":4,"sf":"au","dvbtriplet":"","sk":2917,"adult":false,"servicetype":"OTT","schedule":true,"startover":null,"startoverents":false,"timeshifted":false,"local":false,"servicetypes":["DSAT"]},{"sid":"3363","c":"0130","t":"Panjab Radio","xsg":4,"sg":4,"sf":"au","dvbtriplet":"","sk":3363,"adult":false,"servicetype":"OTT","schedule":true,"startover":null,"startoverents":false,"timeshifted":false,"local":false,"servicetypes":["DSAT"]},{"sid":"2128","c":"0131","t":"BBC R4 Ex","xsg":4,"sg":4,"sf":"au","dvbtriplet":"","sk":2128,"adult":false,"servicetype":"OTT","schedule":true,"startover":null,"startoverents":false,"timeshifted":false,"local":false,"servicetypes":["DSAT"]},{"sid":"2126","c":"0137","t":"BBC R1X","xsg":4,"sg":4,"sf":"au","dvbtriplet":"","sk":2126,"adult":false,"servicetype":"OTT","schedule":true,"startover":null,"startoverents":false,"timeshifted":false,"local":false,"servicetypes":["DSAT"]},{"sid":"4155","c":"0138","t":"TWR","xsg":4,"sg":4,"sf":"au","dvbtriplet":"","sk":4155,"adult":false,"servicetype":"OTT","schedule":true,"startover":null,"startoverents":false,"timeshifted":false,"local":false,"servicetypes":["DSAT"]},{"sid":"2129","c":"0139","t":"BBC R n Gael","xsg":4,"sg":4,"sf":"au","dvbtriplet":"","sk":2129,"adult":false,"servicetype":"OTT","schedule":true,"startover":null,"startoverents":false,"timeshifted":false,"local":false,"servicetypes":["DSAT"]},{"sid":"2115","c":"0143","t":"BBC R4 LW","xsg":4,"sg":4,"sf":"au","dvbtriplet":"","sk":2115,"adult":false,"servicetype":"OTT","schedule":true,"startover":null,"startoverents":false,"timeshifted":false,"local":false,"servicetypes":["DSAT"]},{"sid":"2034","c":"0144","t":"BBC R5SX","xsg":4,"sg":4,"sf":"au","dvbtriplet":"","sk":2034,"adult":false,"servicetype":"OTT","schedule":true,"startover":null,"startoverents":false,"timeshifted":false,"local":false,"servicetypes":["DSAT"]},{"sid":"2920","c":"0146","t":"Heart Dance","xsg":4,"sg":4,"sf":"au","dvbtriplet":"","sk":2920,"adult":false,"servicetype":"OTT","schedule":true,"startover":null,"startoverents":false,"timeshifted":false,"local":false,"servicetypes":["DSAT"]},{"sid":"4163","c":"0147","t":"EWTN Catholic","xsg":4,"sg":4,"sf":"au","dvbtriplet":"","sk":4163,"adult":false,"servicetype":"OTT","schedule":true,"startover":null,"startoverents":false,"timeshifted":false,"local":false,"servicetypes":["DSAT"]},{"sid":"3368","c":"0150","t":"Sukh Sagar","xsg":4,"sg":4,"sf":"au","dvbtriplet":"","sk":3368,"adult":false,"servicetype":"OTT","schedule":true,"startover":null,"startoverents":false,"timeshifted":false,"local":false,"servicetypes":["DSAT"]},{"sid":"3383","c":"0151","t":"Khushkhabri","xsg":4,"sg":4,"sf":"au","dvbtriplet":"","sk":3383,"adult":false,"servicetype":"OTT","schedule":true,"startover":null,"startoverents":false,"timeshifted":false,"local":false,"servicetypes":["DSAT"]},{"sid":"2130","c":"0152","t":"BBC RLondon","xsg":4,"sg":4,"sf":"au","dvbtriplet":"","sk":2130,"adult":false,"servicetype":"OTT","schedule":true,"startover":null,"startoverents":false,"timeshifted":false,"local":false,"servicetypes":["DSAT"]},{"sid":"2123","c":"0154","t":"BBC R Cymru","xsg":4,"sg":4,"sf":"au","dvbtriplet":"","sk":2123,"adult":false,"servicetype":"OTT","schedule":true,"startover":null,"startoverents":false,"timeshifted":false,"local":false,"servicetypes":["DSAT"]},{"sid":"1261","c":"0160","t":"RTÉ Radio 1","xsg":4,"sg":4,"sf":"au","dvbtriplet":"","sk":1261,"adult":false,"servicetype":"OTT","schedule":true,"startover":null,"startoverents":false,"timeshifted":false,"local":false,"servicetypes":["DSAT"]},{"sid":"1262","c":"0164","t":"RTÉ 2FM","xsg":4,"sg":4,"sf":"au","dvbtriplet":"","sk":1262,"adult":false,"servicetype":"OTT","schedule":true,"startover":null,"startoverents":false,"timeshifted":false,"local":false,"servicetypes":["DSAT"]},{"sid":"1264","c":"0165","t":"RTÉ Lyric fm","xsg":4,"sg":4,"sf":"au","dvbtriplet":"","sk":1264,"adult":false,"servicetype":"OTT","schedule":true,"startover":null,"startoverents":false,"timeshifted":false,"local":false,"servicetypes":["DSAT"]},{"sid":"1263","c":"0166","t":"RTÉ R na G","xsg":4,"sg":4,"sf":"au","dvbtriplet":"","sk":1263,"adult":false,"servicetype":"OTT","schedule":true,"startover":null,"startoverents":false,"timeshifted":false,"local":false,"servicetypes":["DSAT"]},{"sid":"4152","c":"0169","t":"Desi Radio","xsg":4,"sg":4,"sf":"au","dvbtriplet":"","sk":4152,"adult":false,"servicetype":"OTT","schedule":true,"startover":null,"startoverents":false,"timeshifted":false,"local":false,"servicetypes":["DSAT"]},{"sid":"3656","c":"0178","t":"Kiss","xsg":4,"sg":4,"sf":"au","dvbtriplet":"","sk":3656,"adult":false,"servicetype":"OTT","schedule":true,"startover":null,"startoverents":false,"timeshifted":false,"local":false,"servicetypes":["DSAT"]},{"sid":"3660","c":"0180","t":"Magic","xsg":4,"sg":4,"sf":"au","dvbtriplet":"","sk":3660,"adult":false,"servicetype":"OTT","schedule":true,"startover":null,"startoverents":false,"timeshifted":false,"local":false,"servicetypes":["DSAT"]},{"sid":"5562","c":"0188","t":"Hits Radio","xsg":4,"sg":4,"sf":"au","dvbtriplet":"","sk":5562,"adult":false,"servicetype":"OTT","schedule":true,"startover":null,"startoverents":false,"timeshifted":false,"local":false,"servicetypes":["DSAT"]},{"sid":"4373","c":"0200","t":"Absolute 80s","xsg":4,"sg":4,"sf":"au","dvbtriplet":"","sk":4373,"adult":false,"servicetype":"OTT","schedule":true,"startover":null,"startoverents":false,"timeshifted":false,"local":false,"servicetypes":["DSAT"]},{"sid":"4372","c":"0201","t":"Absolute 90s","xsg":4,"sg":4,"sf":"au","dvbtriplet":"","sk":4372,"adult":false,"servicetype":"OTT","schedule":true,"startover":null,"startoverents":false,"timeshifted":false,"local":false,"servicetypes":["DSAT"]},{"sid":"4376","c":"0202","t":"Jazz FM","xsg":4,"sg":4,"sf":"au","dvbtriplet":"","sk":4376,"adult":false,"servicetype":"OTT","schedule":true,"startover":null,"startoverents":false,"timeshifted":false,"local":false,"servicetypes":["DSAT"]},{"sid":"4371","c":"0203","t":"Classic Rock","xsg":4,"sg":4,"sf":"au","dvbtriplet":"","sk":4371,"adult":false,"servicetype":"OTT","schedule":true,"startover":null,"startoverents":false,"timeshifted":false,"local":false,"servicetypes":["DSAT"]},{"sid":"3616","c":"0210","t":"Newstalk","xsg":4,"sg":4,"sf":"au","dvbtriplet":"","sk":3616,"adult":false,"servicetype":"OTT","schedule":true,"startover":null,"startoverents":false,"timeshifted":false,"local":false,"servicetypes":["DSAT"]},{"sid":"3371","c":"0211","t":"BFBS Radio","xsg":4,"sg":4,"sf":"au","dvbtriplet":"","sk":3371,"adult":false,"servicetype":"OTT","schedule":true,"startover":null,"startoverents":false,"timeshifted":false,"local":false,"servicetypes":["DSAT"]},{"sid":"2862","c":"0214","t":"UCB Ireland","xsg":4,"sg":4,"sf":"au","dvbtriplet":"","sk":2862,"adult":false,"servicetype":"OTT","schedule":true,"startover":null,"startoverents":false,"timeshifted":false,"local":false,"servicetypes":["DSAT"]},{"sid":"1148","c":"0215","t":"Virgin Radio","xsg":4,"sg":4,"sf":"au","dvbtriplet":"","sk":1148,"adult":false,"servicetype":"OTT","schedule":true,"startover":null,"startoverents":false,"timeshifted":false,"local":false,"servicetypes":["DSAT"]},{"sid":"1101","c":"0216","t":"Scala Radio","xsg":4,"sg":4,"sf":"au","dvbtriplet":"","sk":1101,"adult":false,"servicetype":"OTT","schedule":true,"startover":null,"startoverents":false,"timeshifted":false,"local":false,"servicetypes":["DSAT"]},{"sid":"1083","c":"0217","t":"Heart 90s","xsg":4,"sg":4,"sf":"au","dvbtriplet":"","sk":1083,"adult":false,"servicetype":"OTT","schedule":true,"startover":null,"startoverents":false,"timeshifted":false,"local":false,"servicetypes":["DSAT"]},{"sid":"2021","c":"0219","t":"BBC Cymru 2","xsg":4,"sg":4,"sf":"au","dvbtriplet":"","sk":2021,"adult":false,"servicetype":"OTT","schedule":true,"startover":null,"startoverents":false,"timeshifted":false,"local":false,"servicetypes":["DSAT"]},{"sid":"2124","c":"0220","t":"BBC R Foyle","xsg":4,"sg":4,"sf":"au","dvbtriplet":"","sk":2124,"adult":false,"servicetype":"OTT","schedule":true,"startover":null,"startoverents":false,"timeshifted":false,"local":false,"servicetypes":["DSAT"]},{"sid":"1205","c":"0221","t":"LBC News","xsg":4,"sg":4,"sf":"au","dvbtriplet":"","sk":1205,"adult":false,"servicetype":"OTT","schedule":true,"startover":null,"startoverents":false,"timeshifted":false,"local":false,"servicetypes":["DSAT"]},{"sid":"5030","t":"BT Sport Mosaic","xsg":7,"sg":7,"sf":"sd","dvbtriplet":"","sk":5030,"adult":false,"servicetype":"OTT","schedule":false,"startover":null,"startoverents":false,"timeshifted":false,"local":false,"servicetypes":["DSAT"]},{"sid":"1297","t":"Sports Active","xsg":7,"sg":7,"sf":"sd","dvbtriplet":"","sk":1297,"adult":false,"servicetype":"OTT","schedule":false,"startover":null,"startoverents":false,"timeshifted":false,"local":false,"servicetypes":["DSAT"]},{"sid":"1396","t":"Sports Active","xsg":7,"sg":7,"sf":"sd","dvbtriplet":"","sk":1396,"adult":false,"servicetype":"OTT","schedule":false,"startover":null,"startoverents":false,"timeshifted":false,"local":false,"servicetypes":["DSAT"]},{"sid":"1296","t":"Sports Active","xsg":7,"sg":7,"sf":"sd","dvbtriplet":"","sk":1296,"adult":false,"servicetype":"OTT","schedule":false,"startover":null,"startoverents":false,"timeshifted":false,"local":false,"servicetypes":["DSAT"]},{"sid":"1395","t":"Sports Active","xsg":7,"sg":7,"sf":"sd","dvbtriplet":"","sk":1395,"adult":false,"servicetype":"OTT","schedule":false,"startover":null,"startoverents":false,"timeshifted":false,"local":false,"servicetypes":["DSAT"]},{"sid":"1299","t":"Sports Active","xsg":7,"sg":7,"sf":"sd","dvbtriplet":"","sk":1299,"adult":false,"servicetype":"OTT","schedule":false,"startover":null,"startoverents":false,"timeshifted":false,"local":false,"servicetypes":["DSAT"]},{"sid":"1398","t":"Sports Active","xsg":7,"sg":7,"sf":"sd","dvbtriplet":"","sk":1398,"adult":false,"servicetype":"OTT","schedule":false,"startover":null,"startoverents":false,"timeshifted":false,"local":false,"servicetypes":["DSAT"]},{"sid":"5031","t":"BT Sport Extra 1","xsg":7,"sg":7,"sf":"sd","dvbtriplet":"","sk":5031,"adult":false,"servicetype":"OTT","schedule":true,"startover":null,"startoverents":false,"timeshifted":false,"local":false,"servicetypes":["DSAT"]},{"sid":"1397","t":"Sports Active","xsg":7,"sg":7,"sf":"sd","dvbtriplet":"","sk":1397,"adult":false,"servicetype":"OTT","schedule":false,"startover":null,"startoverents":false,"timeshifted":false,"local":false,"servicetypes":["DSAT"]},{"sid":"1298","t":"Sports Active","xsg":7,"sg":7,"sf":"sd","dvbtriplet":"","sk":1298,"adult":false,"servicetype":"OTT","schedule":false,"startover":null,"startoverents":false,"timeshifted":false,"local":false,"servicetypes":["DSAT"]},{"sid":"1399","t":"Sports Active","xsg":7,"sg":7,"sf":"sd","dvbtriplet":"","sk":1399,"adult":false,"servicetype":"OTT","schedule":false,"startover":null,"startoverents":false,"timeshifted":false,"local":false,"servicetypes":["DSAT"]},{"sid":"9000","t":"Ultra HD","xsg":7,"sg":7,"sf":"uhd","dvbtriplet":"","sk":9000,"adult":false,"servicetype":"OTT","schedule":true,"startover":null,"startoverents":false,"timeshifted":false,"local":false,"servicetypes":["DSAT"]},{"sid":"1171","t":"BEM","xsg":0,"sg":0,"sf":"sd","dvbtriplet":"","sk":1171,"adult":false,"servicetype":"OTT","schedule":false,"startover":null,"startoverents":false,"timeshifted":false,"local":false,"servicetypes":["DSAT"]},{"sid":"1140","t":"Anytime","xsg":7,"sg":7,"sf":"sd","dvbtriplet":"","sk":1140,"adult":false,"servicetype":"OTT","schedule":false,"startover":null,"startoverents":false,"timeshifted":false,"local":false,"servicetypes":["DSAT"]},{"sid":"8376","t":"SOM Test Bert/Ernie","xsg":0,"sg":0,"sf":"sd","dvbtriplet":"","sk":8376,"adult":false,"servicetype":"OTT","schedule":false,"startover":null,"startoverents":false,"timeshifted":false,"local":false,"servicetypes":["DSAT"]},{"sid":"1472","t":"Sports Active","xsg":7,"sg":7,"sf":"sd","dvbtriplet":"","sk":1472,"adult":false,"servicetype":"OTT","schedule":false,"startover":null,"startoverents":false,"timeshifted":false,"local":false,"servicetypes":["DSAT"]},{"sid":"9001","t":"Ultra HD","xsg":7,"sg":7,"sf":"uhd","dvbtriplet":"","sk":9001,"adult":false,"servicetype":"OTT","schedule":false,"startover":null,"startoverents":false,"timeshifted":false,"local":false,"servicetypes":["DSAT"]},{"sid":"1108","t":"XXXFirst Timers","xsg":9,"sg":9,"sf":"sd","dvbtriplet":"","sk":1108,"adult":true,"servicetype":"OTT","schedule":false,"startover":null,"startoverents":false,"timeshifted":false,"local":false,"servicetypes":["DSAT"]},{"sid":"1471","t":"Sports Active","xsg":7,"sg":7,"sf":"sd","dvbtriplet":"","sk":1471,"adult":false,"servicetype":"OTT","schedule":false,"startover":null,"startoverents":false,"timeshifted":false,"local":false,"servicetypes":["DSAT"]},{"sid":"9002","t":"Ultra HD","xsg":7,"sg":7,"sf":"uhd","dvbtriplet":"","sk":9002,"adult":false,"servicetype":"OTT","schedule":false,"startover":null,"startoverents":false,"timeshifted":false,"local":false,"servicetypes":["DSAT"]},{"sid":"1111","t":"Playboy Lovers","xsg":9,"sg":9,"sf":"sd","dvbtriplet":"","sk":1111,"adult":true,"servicetype":"OTT","schedule":false,"startover":null,"startoverents":false,"timeshifted":false,"local":false,"servicetypes":["DSAT"]},{"sid":"8016","t":"launchpad","xsg":3,"sg":3,"sf":"sd","dvbtriplet":"","sk":8016,"adult":false,"servicetype":"OTT","schedule":false,"startover":null,"startoverents":false,"timeshifted":false,"local":false,"servicetypes":["DSAT"]},{"sid":"1474","t":"Sports Active","xsg":7,"sg":7,"sf":"sd","dvbtriplet":"","sk":1474,"adult":false,"servicetype":"OTT","schedule":false,"startover":null,"startoverents":false,"timeshifted":false,"local":false,"servicetypes":["DSAT"]},{"sid":"1110","t":"Sky Box Office","xsg":6,"sg":6,"sf":"sd","dvbtriplet":"","sk":1110,"adult":false,"servicetype":"OTT","schedule":false,"startover":null,"startoverents":false,"timeshifted":false,"local":false,"servicetypes":["DSAT"]},{"sid":"1242","t":"50666","xsg":7,"sg":7,"sf":"sd","dvbtriplet":"","sk":1242,"adult":false,"servicetype":"OTT","schedule":true,"startover":null,"startoverents":false,"timeshifted":false,"local":false,"servicetypes":["DSAT"]},{"sid":"1473","t":"Sports Active","xsg":7,"sg":7,"sf":"sd","dvbtriplet":"","sk":1473,"adult":false,"servicetype":"OTT","schedule":false,"startover":null,"startoverents":false,"timeshifted":false,"local":false,"servicetypes":["DSAT"]},{"sid":"1476","t":"Sports Active","xsg":7,"sg":7,"sf":"sd","dvbtriplet":"","sk":1476,"adult":false,"servicetype":"OTT","schedule":false,"startover":null,"startoverents":false,"timeshifted":false,"local":false,"servicetypes":["DSAT"]},{"sid":"1112","t":"XXX Mums","xsg":9,"sg":9,"sf":"sd","dvbtriplet":"","sk":1112,"adult":true,"servicetype":"OTT","schedule":false,"startover":null,"startoverents":false,"timeshifted":false,"local":false,"servicetypes":["DSAT"]},{"sid":"1475","t":"Sports Active","xsg":7,"sg":7,"sf":"sd","dvbtriplet":"","sk":1475,"adult":false,"servicetype":"OTT","schedule":false,"startover":null,"startoverents":false,"timeshifted":false,"local":false,"servicetypes":["DSAT"]},{"sid":"1479","t":"Sports Active","xsg":7,"sg":7,"sf":"sd","dvbtriplet":"","sk":1479,"adult":false,"servicetype":"OTT","schedule":false,"startover":null,"startoverents":false,"timeshifted":false,"local":false,"servicetypes":["DSAT"]},{"sid":"1478","t":"Sports Active","xsg":7,"sg":7,"sf":"sd","dvbtriplet":"","sk":1478,"adult":false,"servicetype":"OTT","schedule":false,"startover":null,"startoverents":false,"timeshifted":false,"local":false,"servicetypes":["DSAT"]},{"sid":"9012","t":"Ultra HD","xsg":7,"sg":7,"sf":"uhd","dvbtriplet":"","sk":9012,"adult":false,"servicetype":"OTT","schedule":true,"startover":null,"startoverents":false,"timeshifted":false,"local":false,"servicetypes":["DSAT"]},{"sid":"1477","t":"Sports Active","xsg":7,"sg":7,"sf":"sd","dvbtriplet":"","sk":1477,"adult":false,"servicetype":"OTT","schedule":false,"startover":null,"startoverents":false,"timeshifted":false,"local":false,"servicetypes":["DSAT"]},{"sid":"8612","t":"Pending Downgrade","xsg":0,"sg":0,"sf":"sd","dvbtriplet":"","sk":8612,"adult":false,"servicetype":"OTT","schedule":false,"startover":null,"startoverents":false,"timeshifted":false,"local":false,"servicetypes":["DSAT"]},{"sid":"1117","t":"Viewers' Wives","xsg":9,"sg":9,"sf":"sd","dvbtriplet":"","sk":1117,"adult":true,"servicetype":"OTT","schedule":false,"startover":null,"startoverents":false,"timeshifted":false,"local":false,"servicetypes":["DSAT"]},{"sid":"1051","t":"1051","xsg":0,"sg":0,"sf":"sd","dvbtriplet":"","sk":1051,"adult":false,"servicetype":"OTT","schedule":false,"startover":null,"startoverents":false,"timeshifted":false,"local":false,"servicetypes":["DSAT"]},{"sid":"1381","t":"Sports Active","xsg":7,"sg":7,"sf":"sd","dvbtriplet":"","sk":1381,"adult":false,"servicetype":"OTT","schedule":false,"startover":null,"startoverents":false,"timeshifted":false,"local":false,"servicetypes":["DSAT"]},{"sid":"1116","t":"UK Swingers","xsg":9,"sg":9,"sf":"sd","dvbtriplet":"","sk":1116,"adult":true,"servicetype":"OTT","schedule":false,"startover":null,"startoverents":false,"timeshifted":false,"local":false,"servicetypes":["DSAT"]},{"sid":"6170","t":"ITVi Stream 44","xsg":0,"sg":0,"sf":"sd","dvbtriplet":"","sk":6170,"adult":false,"servicetype":"OTT","schedule":false,"startover":null,"startoverents":false,"timeshifted":false,"local":false,"servicetypes":["DSAT"]},{"sid":"1050","t":"BDL","xsg":0,"sg":0,"sf":"sd","dvbtriplet":"","sk":1050,"adult":false,"servicetype":"OTT","schedule":false,"startover":null,"startoverents":false,"timeshifted":false,"local":false,"servicetypes":["DSAT"]},{"sid":"1053","t":"ETBG","xsg":0,"sg":0,"sf":"sd","dvbtriplet":"","sk":1053,"adult":false,"servicetype":"OTT","schedule":false,"startover":null,"startoverents":false,"timeshifted":false,"local":false,"servicetypes":["DSAT"]},{"sid":"8619","t":"Pending Cancel","xsg":0,"sg":0,"sf":"sd","dvbtriplet":"","sk":8619,"adult":false,"servicetype":"OTT","schedule":false,"startover":null,"startoverents":false,"timeshifted":false,"local":false,"servicetypes":["DSAT"]},{"sid":"1119","t":"Xplicit Nightly","xsg":9,"sg":9,"sf":"sd","dvbtriplet":"","sk":1119,"adult":true,"servicetype":"OTT","schedule":false,"startover":null,"startoverents":false,"timeshifted":false,"local":false,"servicetypes":["DSAT"]},{"sid":"1052","t":"TMS","xsg":0,"sg":0,"sf":"sd","dvbtriplet":"","sk":1052,"adult":false,"servicetype":"OTT","schedule":false,"startover":null,"startoverents":false,"timeshifted":false,"local":false,"servicetypes":["DSAT"]},{"sid":"1151","t":"IEPG data 1","xsg":0,"sg":0,"sf":"sd","dvbtriplet":"","sk":1151,"adult":false,"servicetype":"OTT","schedule":false,"startover":null,"startoverents":false,"timeshifted":false,"local":false,"servicetypes":["DSAT"]},{"sid":"5383","t":"BT Sport Extra 3","xsg":7,"sg":7,"sf":"sd","dvbtriplet":"","sk":5383,"adult":false,"servicetype":"OTT","schedule":true,"startover":null,"startoverents":false,"timeshifted":false,"local":false,"servicetypes":["DSAT"]},{"sid":"2013","t":"BBC ALBA","xsg":3,"sg":3,"sf":"sd","dvbtriplet":"","sk":2013,"adult":false,"servicetype":"OTT","schedule":false,"startover":null,"startoverents":false,"timeshifted":false,"local":false,"servicetypes":["DSAT"]},{"sid":"5382","t":"BT Sport Extra 2","xsg":7,"sg":7,"sf":"sd","dvbtriplet":"","sk":5382,"adult":false,"servicetype":"OTT","schedule":true,"startover":null,"startoverents":false,"timeshifted":false,"local":false,"servicetypes":["DSAT"]},{"sid":"8023","t":"eSales Test","xsg":0,"sg":0,"sf":"sd","dvbtriplet":"","sk":8023,"adult":false,"servicetype":"OTT","schedule":false,"startover":null,"startoverents":false,"timeshifted":false,"local":false,"servicetypes":["DSAT"]},{"sid":"1054","t":"ETFG","xsg":0,"sg":0,"sf":"sd","dvbtriplet":"","sk":1054,"adult":false,"servicetype":"OTT","schedule":false,"startover":null,"startoverents":false,"timeshifted":false,"local":false,"servicetypes":["DSAT"]},{"sid":"5381","t":"BT Sport Mosaic 2","xsg":7,"sg":7,"sf":"sd","dvbtriplet":"","sk":5381,"adult":false,"servicetype":"OTT","schedule":false,"startover":null,"startoverents":false,"timeshifted":false,"local":false,"servicetypes":["DSAT"]},{"sid":"5385","t":"BT Sport Extra 5","xsg":7,"sg":7,"sf":"sd","dvbtriplet":"","sk":5385,"adult":false,"servicetype":"OTT","schedule":false,"startover":null,"startoverents":false,"timeshifted":false,"local":false,"servicetypes":["DSAT"]},{"sid":"7500","t":"PSL","xsg":0,"sg":0,"sf":"sd","dvbtriplet":"","sk":7500,"adult":false,"servicetype":"OTT","schedule":false,"startover":null,"startoverents":false,"timeshifted":false,"local":false,"servicetypes":["DSAT"]},{"sid":"4096","t":"Anytime","xsg":6,"sg":6,"sf":"sd","dvbtriplet":"","sk":4096,"adult":false,"servicetype":"OTT","schedule":false,"startover":null,"startoverents":false,"timeshifted":false,"local":false,"servicetypes":["DSAT"]},{"sid":"1123","t":"Blue Tube","xsg":9,"sg":9,"sf":"sd","dvbtriplet":"","sk":1123,"adult":true,"servicetype":"OTT","schedule":false,"startover":null,"startoverents":false,"timeshifted":false,"local":false,"servicetypes":["DSAT"]},{"sid":"5384","t":"BT Sport Extra 4","xsg":7,"sg":7,"sf":"sd","dvbtriplet":"","sk":5384,"adult":false,"servicetype":"OTT","schedule":false,"startover":null,"startoverents":false,"timeshifted":false,"local":false,"servicetypes":["DSAT"]},{"sid":"4095","t":"Anytime","xsg":6,"sg":6,"sf":"sd","dvbtriplet":"","sk":4095,"adult":false,"servicetype":"OTT","schedule":true,"startover":null,"startoverents":false,"timeshifted":false,"local":false,"servicetypes":["DSAT"]},{"sid":"1122","t":"XXX Gay","xsg":9,"sg":9,"sf":"sd","dvbtriplet":"","sk":1122,"adult":true,"servicetype":"OTT","schedule":false,"startover":null,"startoverents":false,"timeshifted":false,"local":false,"servicetypes":["DSAT"]},{"sid":"1291","t":"Sports Active","xsg":7,"sg":7,"sf":"sd","dvbtriplet":"","sk":1291,"adult":false,"servicetype":"OTT","schedule":false,"startover":null,"startoverents":false,"timeshifted":false,"local":false,"servicetypes":["DSAT"]},{"sid":"4098","t":"Anytime","xsg":6,"sg":6,"sf":"sd","dvbtriplet":"","sk":4098,"adult":false,"servicetype":"OTT","schedule":false,"startover":null,"startoverents":false,"timeshifted":false,"local":false,"servicetypes":["DSAT"]},{"sid":"5386","t":"BT Sport Extra 6","xsg":7,"sg":7,"sf":"sd","dvbtriplet":"","sk":5386,"adult":false,"servicetype":"OTT","schedule":false,"startover":null,"startoverents":false,"timeshifted":false,"local":false,"servicetypes":["DSAT"]},{"sid":"8060","t":"STB Dev App","xsg":0,"sg":0,"sf":"sd","dvbtriplet":"","sk":8060,"adult":false,"servicetype":"OTT","schedule":false,"startover":null,"startoverents":false,"timeshifted":false,"local":false,"servicetypes":["DSAT"]},{"sid":"8424","t":"Sports Generic Apps","xsg":0,"sg":0,"sf":"sd","dvbtriplet":"","sk":8424,"adult":false,"servicetype":"OTT","schedule":false,"startover":null,"startoverents":false,"timeshifted":false,"local":false,"servicetypes":["DSAT"]},{"sid":"1293","t":"Sports Active","xsg":7,"sg":7,"sf":"sd","dvbtriplet":"","sk":1293,"adult":false,"servicetype":"OTT","schedule":false,"startover":null,"startoverents":false,"timeshifted":false,"local":false,"servicetypes":["DSAT"]},{"sid":"1392","t":"Sports Active","xsg":7,"sg":7,"sf":"sd","dvbtriplet":"","sk":1392,"adult":false,"servicetype":"OTT","schedule":false,"startover":null,"startoverents":false,"timeshifted":false,"local":false,"servicetypes":["DSAT"]},{"sid":"2052","t":"6391","xsg":3,"sg":3,"sf":"sd","dvbtriplet":"","sk":2052,"adult":false,"servicetype":"OTT","schedule":false,"startover":null,"startoverents":false,"timeshifted":false,"local":false,"servicetypes":["DSAT"]},{"sid":"1292","t":"Sports Active","xsg":7,"sg":7,"sf":"sd","dvbtriplet":"","sk":1292,"adult":false,"servicetype":"OTT","schedule":false,"startover":null,"startoverents":false,"timeshifted":false,"local":false,"servicetypes":["DSAT"]},{"sid":"1391","t":"Sports Active","xsg":7,"sg":7,"sf":"sd","dvbtriplet":"","sk":1391,"adult":false,"servicetype":"OTT","schedule":false,"startover":null,"startoverents":false,"timeshifted":false,"local":false,"servicetypes":["DSAT"]},{"sid":"1295","t":"Sports Active","xsg":7,"sg":7,"sf":"sd","dvbtriplet":"","sk":1295,"adult":false,"servicetype":"OTT","schedule":false,"startover":null,"startoverents":false,"timeshifted":false,"local":false,"servicetypes":["DSAT"]},{"sid":"1394","t":"Sports Active","xsg":7,"sg":7,"sf":"sd","dvbtriplet":"","sk":1394,"adult":false,"servicetype":"OTT","schedule":false,"startover":null,"startoverents":false,"timeshifted":false,"local":false,"servicetypes":["DSAT"]},{"sid":"1393","t":"Sports Active","xsg":7,"sg":7,"sf":"sd","dvbtriplet":"","sk":1393,"adult":false,"servicetype":"OTT","schedule":false,"startover":null,"startoverents":false,"timeshifted":false,"local":false,"servicetypes":["DSAT"]},{"sid":"1294","t":"Sports Active","xsg":7,"sg":7,"sf":"sd","dvbtriplet":"","sk":1294,"adult":false,"servicetype":"OTT","schedule":false,"startover":null,"startoverents":false,"timeshifted":false,"local":false,"servicetypes":["DSAT"]}]}
  )JSON";

  std::string response{};

  // so gets called many times to give the full data transferred.
  static size_t write_data(void *ptr, size_t size, size_t nmemb, void *stream)
  {
    std::cout << "response size: " << response.size() << ", size: " << size
              << ", nemeb: " << nmemb << std::endl;

    std::string coll{(char *)ptr};

    response.append(coll);

    std::cout << "response size: " << response.size() << ", coll: " << coll.size() << std::endl;

    return coll.size();
  }

  // this can be simplified to:
  //
  // static size_t writeData_(void *ptr, size_t size, size_t nmemb, void *stream)
  // {
  //   response.append((char *)ptr, (size * nmemb));
  //   return size * nmemb;
  // }
};

// use the following command to code output:
//
// curl -H "X-SkyOTT-Provider: SKY" -H "X-SkyOTT-Proposition: SKYQ" -H
// "X-SkyOTT-Territory: GB" http://atlantis.epg.bskyb.com/as/services/4101/1
// --libcurl -
//
// curl 
// -H "X-SkyOTT-Provider: SKY" 
// -H "X-SkyOTT-Proposition: SKYQ" 
// -H "X-SkyOTT-Territory: GB" 
// -H "X-SkyOTT-Device: IPSETTOPBOX" 
// -H "X-SkyOTT-Platform: SKYQ-RDK" http://atlantis.epg.bskyb.com/as/services/4096/1

TEST(CxxCurl, case_2)
{
  using namespace curl_case;

  CURL *curl_handle;

  curl_global_init(CURL_GLOBAL_ALL);

  // https://curl.haxx.se/libcurl/c/curl_slist_append.html
  // curl_slist_append - add a string to an slist

  struct curl_slist *slist1;

  slist1 = NULL;
  slist1 = curl_slist_append(slist1, "X-SkyOTT-Provider: SKY");
  slist1 = curl_slist_append(slist1, "X-SkyOTT-Proposition: SKYQ");
  slist1 = curl_slist_append(slist1, "X-SkyOTT-Territory: GB");
  slist1 = curl_slist_append(slist1, "X-SkyOTT-Device: MOBILE");
  slist1 = curl_slist_append(slist1, "X-SkyOTT-Platform: ANDROID");
 
  /* init the curl session */ 
  curl_handle = curl_easy_init();
 
  /* set URL to get here */ 
  // curl_easy_setopt(curl_handle, CURLOPT_URL, argv[1]);

  // curl_easy_setopt(curl_handle,
  //                  CURLOPT_URL,
  //                  "http://atlantis.epg.bskyb.com/as/services/4101/1");

  curl_easy_setopt(curl_handle,
                   CURLOPT_URL,
                   "http://atlantis.epg.bskyb.com/as/services/4096/1");

  /* disable progress meter, set to 0L to enable it */ 
  curl_easy_setopt(curl_handle, CURLOPT_NOPROGRESS, 1L);

  curl_easy_setopt(curl_handle, CURLOPT_HTTPHEADER, slist1);
  curl_easy_setopt(curl_handle, CURLOPT_BUFFERSIZE, 102400L);
  curl_easy_setopt(curl_handle, CURLOPT_USERAGENT, "curl/7.58.0");
  curl_easy_setopt(curl_handle, CURLOPT_MAXREDIRS, 50L);
  curl_easy_setopt(curl_handle, CURLOPT_HTTP_VERSION, (long)CURL_HTTP_VERSION_2TLS);
  curl_easy_setopt(curl_handle, CURLOPT_TCP_KEEPALIVE, 1L);

  /* send all data to this function  */
  curl_easy_setopt(curl_handle, CURLOPT_WRITEFUNCTION, write_data);
 
  /* get it! */ 
  curl_easy_perform(curl_handle);

  Json::Reader reader;
  Json::Value root;

  // parse the json
  if (!reader.parse(response, root))
  {
    std::cout << "failed to parse config json" << std::endl;
  }

  // to print out
  // std::cout << root << std::endl;

  const Json::Value services = root["services"];
  std::cout << "number of services: "<< services.size() << std::endl;

  /* cleanup curl stuff */ 
  curl_easy_cleanup(curl_handle);
 
  curl_global_cleanup();
}

/*
when there is no internet connection that is out of vpn since the url used is in
coporate vpn, easy_perform returns error when run on PC but not on a embedded
box. Returns CURLE_OK (0) and has 27 bytes response.

[ RUN      ] CxxCurl.case_2_2
before curl_easy_perform(), response size 0
response size: 0, size: 1, nemeb: 27
response size: 27, coll: 27
curl_easy_perform() failed Unknown error
return
after curl_easy_perform(), response size 27, response: {Content-Type: text/html

}
[       OK ] CxxCurl.case_2_2 (7 ms)

Or 

[ RUN      ] CxxCurl.case_2_2
before curl_easy_perform(), response size 0
curl_easy_perform() failed Unknown error
return
We received response code: 0
after curl_easy_perform(), response size 0, response: {}
[       OK ] CxxCurl.case_2_2 (29 ms)

*/

TEST(CxxCurl, case_2_2)
{
  using namespace curl_case;

  CURL *curl_handle;

  curl_global_init(CURL_GLOBAL_ALL);
  response.clear();

  // https://curl.haxx.se/libcurl/c/curl_slist_append.html
  // curl_slist_append - add a string to an slist

  struct curl_slist *slist1;

  slist1 = NULL;
  slist1 = curl_slist_append(slist1, "X-SkyOTT-Provider: SKY");
  slist1 = curl_slist_append(slist1, "X-SkyOTT-Proposition: SKYQ");
  slist1 = curl_slist_append(slist1, "X-SkyOTT-Territory: GB");
  slist1 = curl_slist_append(slist1, "X-SkyOTT-Device: MOBILE");
  slist1 = curl_slist_append(slist1, "X-SkyOTT-Platform: ANDROID");
 
  /* init the curl session */ 
  curl_handle = curl_easy_init();
 
  /* set URL to get here */ 
  // curl_easy_setopt(curl_handle, CURLOPT_URL, argv[1]);

  // curl_easy_setopt(curl_handle,
  //                  CURLOPT_URL,
  //                  "http://atlantis.epg.bskyb.com/as/services/4101/1");

  curl_easy_setopt(curl_handle,
                   CURLOPT_URL,
                   "http://atlantis.epg.bskyb.com/as/services/4096/1");

  /* disable progress meter, set to 0L to enable it */ 
  curl_easy_setopt(curl_handle, CURLOPT_NOPROGRESS, 1L);

  curl_easy_setopt(curl_handle, CURLOPT_HTTPHEADER, slist1);
  curl_easy_setopt(curl_handle, CURLOPT_BUFFERSIZE, 102400L);
  curl_easy_setopt(curl_handle, CURLOPT_USERAGENT, "curl/7.58.0");
  curl_easy_setopt(curl_handle, CURLOPT_MAXREDIRS, 50L);
  curl_easy_setopt(curl_handle, CURLOPT_HTTP_VERSION, (long)CURL_HTTP_VERSION_2TLS);
  curl_easy_setopt(curl_handle, CURLOPT_TCP_KEEPALIVE, 1L);

  /* send all data to this function  */
  curl_easy_setopt(curl_handle, CURLOPT_WRITEFUNCTION, write_data);
 
  std::cout << "before curl_easy_perform(), response size " << response.size() << std::endl;

  /* get it! */
  CURLcode res;
  curl_easy_perform(curl_handle);
  if (res != CURLE_OK)
  {
    std::cout << "curl_easy_perform() failed " << curl_easy_strerror(res) << std::endl;
    std::cout << "return" << std::endl;
  }

  {
    long response_code;
    res = curl_easy_getinfo(curl_handle, CURLINFO_RESPONSE_CODE, &response_code);
    if (CURLE_OK == res)
      std::cout << "We received response code: " << response_code << std::endl;
  }

  std::cout << "after curl_easy_perform(), response size " << response.size()
            << ", response: {" << response << "}" << std::endl;

  /* cleanup curl stuff */ 
  curl_easy_cleanup(curl_handle);
 
  curl_global_cleanup();
}

/*
Again, when use invalid url, easy_perform returns error when run on 
PC but not on a embedded box. Returns CURLE_OK (0) and has developer error message.

This is PC version and see curl_easy_perform() fails.
[ RUN      ] CxxCurl.case_2_3
before curl_easy_perform(), response size 0
response size: 0, size: 1, nemeb: 83
response size: 83, coll: 83
curl_easy_perform() failed Unknown error
return
after curl_easy_perform(), response size 83, response: {{"errorCode":"404","developerMessage":"Invalid bouquet 9999 or/and subbouquet 999"}}
[       OK ] CxxCurl.case_2_3 (64 ms)


NOTE:
thought that this comes from libcurl version differences. turns out that the
libcurl on PC version is older than one on the box. The newer version returns ok
since requset is transmitted.

https://stackoverflow.com/questions/20487162/i-cant-get-http-code-404-with-libcurl

libcurl returns CURLE_OK when the transfer went fine. Getting a 404 from a HTTP
server is considered a fine transfer. You can make >=4xx HTTP response codes
cause a libcurl error by setting the CURLOPT_FAILONERROR option.

Alternatively, and this may be the nicer way, you extract the HTTP response code
after the transfer, with for example curl_easy_getinfo() to figure out the HTTP
response code to see what the HTTP server thought about the resource you
requested.

When use "response code":

[ RUN      ] CxxCurl.case_2_3
before curl_easy_perform(), response size 0
response size: 0, size: 1, nemeb: 83
response size: 83, coll: 83
curl_easy_perform() failed Unknown error
return
We received response code: 404
after curl_easy_perform(), response size 83, response: {{"errorCode":"404","developerMessage":"Invalid bouquet 9999 or/and subbouquet 999"}}
[       OK ] CxxCurl.case_2_3 (53 ms)


So can cover this case when use this option on the box to cover this case.

*/

TEST(CxxCurl, case_2_3)
{
  using namespace curl_case;

  CURL *curl_handle;

  curl_global_init(CURL_GLOBAL_ALL);
  response.clear();

  // https://curl.haxx.se/libcurl/c/curl_slist_append.html
  // curl_slist_append - add a string to an slist

  struct curl_slist *slist1;

  slist1 = NULL;
  slist1 = curl_slist_append(slist1, "X-SkyOTT-Provider: SKY");
  slist1 = curl_slist_append(slist1, "X-SkyOTT-Proposition: SKYQ");
  slist1 = curl_slist_append(slist1, "X-SkyOTT-Territory: GB");
  slist1 = curl_slist_append(slist1, "X-SkyOTT-Device: MOBILE");
  slist1 = curl_slist_append(slist1, "X-SkyOTT-Platform: ANDROID");
 
  /* init the curl session */ 
  curl_handle = curl_easy_init();
 
  /* set URL to get here */ 
  // curl_easy_setopt(curl_handle, CURLOPT_URL, argv[1]);

  curl_easy_setopt(curl_handle,
                   CURLOPT_URL,
                   "http://atlantis.epg.bskyb.com/as/services/9999/999");

  /* disable progress meter, set to 0L to enable it */ 
  curl_easy_setopt(curl_handle, CURLOPT_NOPROGRESS, 1L);

  curl_easy_setopt(curl_handle, CURLOPT_HTTPHEADER, slist1);
  curl_easy_setopt(curl_handle, CURLOPT_BUFFERSIZE, 102400L);
  curl_easy_setopt(curl_handle, CURLOPT_USERAGENT, "curl/7.58.0");
  curl_easy_setopt(curl_handle, CURLOPT_MAXREDIRS, 50L);
  curl_easy_setopt(curl_handle, CURLOPT_HTTP_VERSION, (long)CURL_HTTP_VERSION_2TLS);
  curl_easy_setopt(curl_handle, CURLOPT_TCP_KEEPALIVE, 1L);

  /* send all data to this function  */
  curl_easy_setopt(curl_handle, CURLOPT_WRITEFUNCTION, write_data);
 
  std::cout << "before curl_easy_perform(), response size " << response.size() << std::endl;

  /* get it! */
  CURLcode res;
  curl_easy_perform(curl_handle);
  if (res != CURLE_OK)
  {
    std::cout << "curl_easy_perform() failed " << curl_easy_strerror(res) << std::endl;
    std::cout << "return" << std::endl;
  }

  {
    // https://curl.se/libcurl/c/curl_easy_getinfo.html
    // ask for the content-type
    // char *ct;
    // res = curl_easy_getinfo(curl_handle, CURLINFO_CONTENT_TYPE, &ct);
    // if((CURLE_OK == res) && ct)
    //   std::cout << "We received Content-Type: " << ct << std::endl;

    long response_code;
    res = curl_easy_getinfo(curl_handle, CURLINFO_RESPONSE_CODE, &response_code);
    if (CURLE_OK == res)
      std::cout << "We received response code: " << response_code << std::endl;
  }

  std::cout << "after curl_easy_perform(), response size " << response.size()
            << ", response: {" << response << "}" << std::endl;

  /* cleanup curl stuff */ 
  curl_easy_cleanup(curl_handle);
 
  curl_global_cleanup();
}

/*
when use the CURLOPT_FAILONERROR option.

https://curl.se/libcurl/c/CURLOPT_FAILONERROR.html

A long parameter set to 1 tells the library to fail the request if the HTTP code
returned is equal to or larger than 400. The default action would be to return
the page normally, ignoring that code.

[ RUN      ] CxxCurl.case_2_4
before curl_easy_perform(), response size 0
curl_easy_perform() failed Unknown error
return
We received response code: 0
after curl_easy_perform(), response size 0, response: {}
[       OK ] CxxCurl.case_2_4 (17 ms)

Now, no error on response code

*/

TEST(CxxCurl, case_2_4)
{
  using namespace curl_case;

  CURL *curl_handle;

  curl_global_init(CURL_GLOBAL_ALL);
  response.clear();

  // https://curl.haxx.se/libcurl/c/curl_slist_append.html
  // curl_slist_append - add a string to an slist

  struct curl_slist *slist1;

  slist1 = NULL;
  slist1 = curl_slist_append(slist1, "X-SkyOTT-Provider: SKY");
  slist1 = curl_slist_append(slist1, "X-SkyOTT-Proposition: SKYQ");
  slist1 = curl_slist_append(slist1, "X-SkyOTT-Territory: GB");
  slist1 = curl_slist_append(slist1, "X-SkyOTT-Device: MOBILE");
  slist1 = curl_slist_append(slist1, "X-SkyOTT-Platform: ANDROID");
 
  /* init the curl session */ 
  curl_handle = curl_easy_init();
 
  /* set URL to get here */
  // curl_easy_setopt(curl_handle, CURLOPT_URL, argv[1]);

  curl_easy_setopt(curl_handle,
                   CURLOPT_URL,
                   "http://atlantis.epg.bskyb.com/as/services/9999/999");

  /* disable progress meter, set to 0L to enable it */ 
  curl_easy_setopt(curl_handle, CURLOPT_NOPROGRESS, 1L);

  curl_easy_setopt(curl_handle, CURLOPT_HTTPHEADER, slist1);
  curl_easy_setopt(curl_handle, CURLOPT_BUFFERSIZE, 102400L);
  curl_easy_setopt(curl_handle, CURLOPT_USERAGENT, "curl/7.58.0");
  curl_easy_setopt(curl_handle, CURLOPT_MAXREDIRS, 50L);
  curl_easy_setopt(curl_handle, CURLOPT_HTTP_VERSION, (long)CURL_HTTP_VERSION_2TLS);
  curl_easy_setopt(curl_handle, CURLOPT_TCP_KEEPALIVE, 1L);
  curl_easy_setopt(curl_handle, CURLOPT_FAILONERROR, 1L);

  /* send all data to this function  */
  curl_easy_setopt(curl_handle, CURLOPT_WRITEFUNCTION, write_data);
 
  std::cout << "before curl_easy_perform(), response size " << response.size() << std::endl;

  /* get it! */
  CURLcode res;
  curl_easy_perform(curl_handle);
  if (res != CURLE_OK)
  {
    std::cout << "curl_easy_perform() failed " << curl_easy_strerror(res) << std::endl;
    std::cout << "return" << std::endl;
  }

  {
    long response_code;
    res = curl_easy_getinfo(curl_handle, CURLINFO_RESPONSE_CODE, &response_code);
    if (CURLE_OK == res)
      std::cout << "We received response code: " << response_code << std::endl;
  }

  std::cout << "after curl_easy_perform(), response size " << response.size()
            << ", response: {" << response << "}" << std::endl;

  /* cleanup curl stuff */ 
  curl_easy_cleanup(curl_handle);
 
  curl_global_cleanup();
}


/*
// ={=========================================================================
to check the result and the expected.
keitee@kit-ubuntu:~/git/kb/code-cxx/cxx_libcurl/build$ diff out1.json out2.json
--- out1.json   2020-09-14 13:28:36.465379126 +0100
+++ out2.json   2020-09-14 13:28:40.953449037 +0100
@@ -1,5 +1,5 @@
 {
-       "documentId" : "1600086425811",
+       "documentId" : "1600085171264",
        "services" :
        [
                {
keitee@kit-ubuntu:~/git/kb/code-cxx/cxx_libcurl/build$

*/

TEST(CxxCurl, case_3)
{
  using namespace curl_case;

  Json::Reader reader;
  Json::Value root;

  // parse the json
  if (!reader.parse(expected, root))
  {
    std::cout << "failed to parse config json" << std::endl;
  }

  // std::cout << root << std::endl;
}

// ={=========================================================================
int main(int argc, char **argv)
{
  testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}
