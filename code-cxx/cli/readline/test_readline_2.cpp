/*
={=========================================================================
https://tiswww.cwru.edu/php/chet/readline/rltop.html

A snapshot of the current development sources (generally updated monthly) is
also available from the GNU git readline devel branch.

git clone git://git.savannah.gnu.org/readline.git


={=========================================================================
The GNU Readline Library

Documentation

The documentation for the Readline and History libraries appears in the `doc'
subdirectory. There are three texinfo files and two Unix-style manual pages
describing the facilities available in the Readline and History libraries. The
texinfo files include both user and programmer's manuals. The current manuals
are:

The GNU Readline Library

https://tiswww.case.edu/php/chet/readline/readline.html


={=========================================================================
2. Programming with GNU Readline

This chapter describes the interface between the GNU Readline Library and other
programs. If you are a programmer, and you wish to include the features found in
GNU Readline such as completion, line editing, and interactive history
manipulation in your own programs, this section is for you.


={=========================================================================
2.1 Basic Behavior

Many programs provide a command line interface, such as mail, ftp, and sh. For
such programs, the default behaviour of Readline is sufficient. This section
describes how to use Readline in the simplest way possible, perhaps to replace
calls in your code to gets() or fgets().


The function readline() prints a prompt prompt and then reads and returns a
single line of text from the user. If prompt is NULL or the empty string, no
prompt is displayed. The line readline returns is allocated with malloc(); the
caller should free() the line when it has finished with it. The declaration for
readline in ANSI C is


char *readline (const char *prompt);

So, one might say

char *line = readline ("Enter a line: ");

in order to read a line of text from the user. The line returned has the final
newline removed, so only the text remains.


If readline encounters an EOF while reading the line, and the line is empty at
that point, then (char *)NULL is returned. Otherwise, the line is ended just as
if a newline had been typed.

Readline performs some expansion on the prompt before it is displayed on the
screen. See the description of rl_expand_prompt (see section 2.4.6 Redisplay)
for additional details, especially if prompt will contain characters that do not
consume physical screen space when displayed.

If you want the user to be able to get at the line later, (with C-p for
example), you must call add_history() to save the line away in a history list of
such lines.

add_history (line);

For full details on the GNU History Library, see the associated manual.

It is preferable to avoid saving empty lines on the history list, since users
rarely have a burning need to reuse a blank line. Here is a function which
usefully replaces the standard gets() library function, and has the advantage of
no static buffer to overflow:


// A static variable for holding the line.
static char *line_read = (char *)NULL;

// Read a string, and return a pointer to it. Returns NULL on EOF.
char *
rl_gets ()
{
  // If the buffer has already been allocated,
  // return the memory to the free pool.
  if (line_read)
    {
      free (line_read);
      line_read = (char *)NULL;
    }

  // Get a line from the user.
  line_read = readline ("");

  // If the line has any text in it,
  // save it on the history.
  if (line_read && *line_read)
    add_history (line_read);

  return (line_read);
}

This function gives the user the default behaviour of TAB completion: completion
on file names. If you do not want Readline to complete on filenames, you can
change the binding of the TAB key with rl_bind_key().

int rl_bind_key (int key, rl_command_func_t *function);

rl_bind_key() takes two arguments: key is the character that you want to bind,
  and function is the address of the function to call when key is pressed.
  Binding TAB to rl_insert() makes TAB insert itself. rl_bind_key() returns
  non-zero if key is not a valid ASCII character code (between 0 and 255).

Thus, to disable the default TAB behavior, the following suffices:

rl_bind_key ('\t', rl_insert);

// By default readline does filename completion. With -d, we disable this
// by asking readline to just insert the TAB character itself.

This code should be executed once at the start of your program; you might write
a function called initialize_readline() which performs this and other desired
initializations, such as installing custom completers (see section 2.6 Custom
    Completers).


={=========================================================================
2.4 Readline Convenience Functions

2.4.12 Alternate Interface

An alternate interface is available to plain readline(). Some applications need
to interleave keyboard I/O with file, device, or window system I/O, typically by
using a main loop to select() on various file descriptors. To accommodate this
need, readline can also be invoked as a `callback' function "from an event loop."
There are functions available to make this easy.


Function: void rl_callback_handler_install (const char *prompt, rl_vcpfunc_t *lhandler)

Set up the terminal for readline I/O and display the initial expanded value of
prompt. Save the value of lhandler to use as a handler function to call when a
complete line of input has been entered. The handler function receives the text
of the line as an argument. As with readline(), the handler function should free
the line when it it finished with it.


={=========================================================================
2.6 Custom Completers

Typically, a program that reads commands from the user has a way of
disambiguating commands and data. If your program is one of these, then it can
provide completion for commands, data, or both. The following sections describe
how your program and Readline cooperate to provide this service.


2.6.1 How Completing Works

In order to complete some text, the full list of possible completions must be
available. That is, it is not possible to accurately expand a partial word
without knowing all of the possible words which make sense in that context. The
Readline library provides the user interface to completion, and two of the most
common completion functions: filename and username. For completing other types
of text, you must write your own completion function. This section describes
exactly what such functions must do, and provides an example.

There are three major functions used to perform completion:

1. The user-interface function rl_complete(). This function is called with the
same arguments as other bindable Readline functions: count and invoking_key. It
isolates the word to be completed and "calls rl_completion_matches()" to
generate a list of possible completions. It then either lists the possible
completions, inserts the possible completions, or actually performs the
completion, depending on which behavior is desired.

2. The internal function rl_completion_matches() uses an application-supplied
generator function to generate the list of possible matches, and then returns
"the array of these matches." The caller should place the address of its
generator function in rl_completion_entry_function.

3. The generator function is called repeatedly from rl_completion_matches(),
  returning a string each time. The arguments to the generator function are text
  and state. "text is the partial word to be completed." state is zero the first
  time the function is called, allowing the generator to perform any necessary
  initialization, and a positive non-zero integer for each subsequent call. The
  generator function returns (char *)NULL to inform rl_completion_matches() that
  there are no more possibilities left. Usually the generator function computes
  the list of possible completions when state is zero, and returns them one at a
  time on subsequent calls. Each string the generator function returns as a
  match must be allocated with malloc(); Readline frees the strings when it has
  finished with them. Such a generator function is referred to as an
  application-specific completion function.


Function: int rl_complete (int ignore, int invoking_key)

Complete the word at or before point. You have supplied the function that does
the initial simple matching selection algorithm (see rl_completion_matches()).
The default is to do filename completion.

Variable: rl_compentry_func_t * rl_completion_entry_function

This is a pointer to the generator function for rl_completion_matches(). If the
value of rl_completion_entry_function is NULL then the default filename
generator function, rl_filename_completion_function(), is used. 

An application-specific completion function is a function whose address is
assigned to rl_completion_entry_function and whose return values are used to
generate possible completions.


2.6.2 Completion Functions

Function: 
char ** rl_completion_matches (const char *text, rl_compentry_func_t *entry_func)

Returns "an array of strings" which is a list of completions for text. If there
are no completions, returns NULL. The first entry in the returned array is the
substitution for text. The remaining entries are the possible completions. The
array is terminated with a NULL pointer.

entry_func is a function of two args, and returns a char *. The first argument
is text. The second is a state argument; it is zero on the first call, and
non-zero on subsequent calls. entry_func returns a NULL pointer to the caller
when there are no more matches.


2.6.3 Completion Variables

Variable: 
rl_completion_func_t * rl_attempted_completion_function

A pointer to an alternative function to create matches. The function is called
with text, start, and end. start and end are indices in rl_line_buffer defining
the boundaries of text, which is a character string. If this function exists and
returns NULL, or if this variable is set to NULL, then rl_complete() will call
the value of rl_completion_entry_function to generate matches, otherwise the
array of strings returned will be used. 

If this function sets the "rl_attempted_completion_over" variable to a non-zero
value, Readline will not perform its default completion even if this function
returns no matches.


Variable: 
int rl_attempted_completion_over

If an application-specific completion function assigned to
rl_attempted_completion_function sets this variable to a non-zero value,
Readline will not perform its default filename completion even if the
  application's completion function returns no matches. It should be set only by
  an application's completion function.


2.6.4 A Short Completion Example

Here is a small application demonstrating the use of the GNU Readline library.
It is called fileman, and the source code resides in `examples/fileman.c'. This
sample application provides completion of command names, line editing features,
and access to the history list.

*/

/*
https://eli.thegreenplace.net/2016/basics-of-using-the-readline-library/

Did it ever happen to you to find a program that provides a shell / REPL-like
interface that doesn't support basic features like line editing, command history
and completion? I personally find it pretty annoying. And there's really no
justification for it these days, at least on Unix-y systems, since a portable
library to provide this service has existed for decades. I'm talking about
readline.

This brief post shows some basic examples of using readline in both C++ and
Python. Admittedly, it doesn't have a lot of content. Rather, its main goal is
to present the accompanying code samples, which I think folks will find more
useful.

Let's start with a very basic readline usage example that records command
history and lets us search and scroll through it:


#include <stdio.h>
#include <stdlib.h>
#include <string>

#include <readline/history.h>
#include <readline/readline.h>

int main(int argc, char** argv) {
  printf("Welcome! You can exit by pressing Ctrl+C at any time...\n");

  if (argc > 1 && std::string(argv[1]) == "-d") {
    // By default readline does filename completion. With -d, we disable this
    // by asking readline to just insert the TAB character itself.
    rl_bind_key('\t', rl_insert);
  }

  char* buf;
  while ((buf = readline(">> ")) != nullptr) {
    if (strlen(buf) > 0) {
      add_history(buf);
    }

    printf("[%s]\n", buf);

    // readline malloc's a new buffer every time.
    free(buf);
  }

  return 0;
}

The main thing in this code sample is using the readline function instead of
standard language APIs for user input (such as fgets in C or std::getline in
    C++). This already gives us many of readline's features like line editing:
having typed a word, we can actually go back and fix some part of it (using the
    left arrow key or Ctrl-B), jump to input end (Ctrl+E) and so on - all the
editing facilities we're so used to from the standard Linux terminal.

The add_history calls go further: they add every command typed into the history
buffer. With this done, we can now scroll through command history with up/down
arrows, and even do history seaches with Ctrl+R.

Note also that readline automatically enables tab completion. The default
completion functionality auto-completes file names in the current directory,
           which isn't something we necessarily want. In this sample, tab
           completion is optionally disabled by binding the tab key to
           rl_insert, which just sends the actual key code to the terminal
           rather than doing anything special like completion.


Simple completion

Implementing custom completion with readline is fairly simple. Here is a sample
that will tab-complete words from a certain vocabulary. The main function
remains as before, with a small difference - registering our completion function
with readline.

rl_attempted_completion_function = completer;

Configuring readline happens through "global" variables it exports. These
variables are all documented. (see 2.6.3 Completion Variables)

While we could use rl_completion_entry_function to make our code slightly
shorter, for extra fun let's instead use rl_attempted_completion_function - it
lets us customize things a bit more. The default rl_completion_entry_function
performs filename completion in the current directory. We can disable it in our
own "attempted" completion function:

char** completer(const char* text, int start, int end) {
  // Don't do filename completion even if our generator finds no matches.
  rl_attempted_completion_over = 1;

  // Note: returning nullptr here will make readline use the default filename
  // completer.
  return rl_completion_matches(text, completion_generator);
}

Otherwise, it's all the same. We have to implement a "completion generator" and
pass it to the helper rl_completion_matches to generate the actual matches. Our
completion generator auto-completes from a global vocabulary of words:

char* completion_generator(const char* text, int state) {

  // This function is called with state=0 the first time; subsequent calls are
  // with a nonzero state. state=0 can be used to perform one-time
  // initialization for this completion session.
  static std::vector<std::string> matches;
  static size_t match_index = 0;

  if (state == 0) {
    // During initialization, compute the actual matches for 'text' and keep
    // them in a static vector.
    matches.clear();
    match_index = 0;

    // Collect a vector of matches: vocabulary words that begin with text.
    std::string textstr = std::string(text);
    for (auto word : vocabulary) {
      if (word.size() >= textstr.size() &&
          word.compare(0, textstr.size(), textstr) == 0) {
        matches.push_back(word);
      }
    }
  }

  if (match_index >= matches.size()) {
    // We return nullptr to notify the caller no more matches are available.
    return nullptr;
  } else {
    // Return a malloc'd char* for the match. The caller frees it.
    return strdup(matches[match_index++].c_str());
  }
}

You can read more details about how the completion works on this page. The
samples respository contains several additional variations on this theme,
        including a more sophisticated program that provides hierarchical
        completion of sub-commands, where the first token determines the
        autocompletion vocabulary for subsequent tokens.


Python

The Python standard library comes with a readline module that provides an
intereface to the underlying C library. In fact, it can also use libedit under
the hood. libedit is the BSD implementation of the readline interface, and can
be used on some platforms. In Python you don't have to care about this though.

A basic completion example in Python using readline is as simple as:

import readline

def make_completer(vocabulary):
    def custom_complete(text, state):
        # None is returned for the end of the completion session.
        results = [x for x in vocabulary if x.startswith(text)] + [None]
        # A space is added to the completion since the Python readline doesn't
        # do this on its own. When a word is fully completed we want to mimic
        # the default readline library behavior of adding a space after it.
        return results[state] + " "
    return custom_complete

def main():
    vocabulary = {'cat', 'dog', 'canary', 'cow', 'hamster'}
    readline.parse_and_bind('tab: complete')
    readline.set_completer(make_completer(vocabulary))

    try:
        while True:
            s = input('>> ').strip()
            print('[{0}]'.format(s))
    except (EOFError, KeyboardInterrupt) as e:
        print('\nShutting down...')

if __name__ == '__main__':
    main()

It's fairly obvious that the Python API is a thin veneer around the underlying C
API - the completion implements state in the same way. That said, Python's
built-in features like first order functions and lexical closures make writing
more sophisticated completion code a much simpler task. See the included code
samples for more examples.


Other libraries and modules

Frustrated by the complexity of readline, Salvatore Sanfilippo (of Redis fame)
  created a simple line-completion library named linenoise. It has a prety
  simple interface and is very small, so I think it's good for inclusion into
  projects that want to minimize system dependencies (like figuring out how to
      link with readline on different platforms).

On the Python side, there are a couple of other related modules in the standard
library I'd like to mention. One is rlcompleter; think of it as readline
pre-configured with completions for Python functions and identifiers. If you
want to build Python interpreters or shells, this is the tool for you.

Yet another Python module is cmd; it's a higher-level abstraction around
readline, allowing one to encapsulate commands into class methods. If you want
to write a simple command interpreter (think Logo), using cmd will result in
less code.

IMHO while having readline in the standard Python library is wonderful , both
rlcompleter and cmd are signs that the Python developers sometimes go too far
with the "batteries included" philosophy. But YMMV.

*/

/*

o To link command completion and command execution, readline.cpp maintains
  command vector and an iterface to run the input command.

o To support global function and member function of any class:

  - have a base class and derive class from it which has command handler
  function implementation. And have base class type in handler function type
  definition. However, it's limited to that classing tree.

  - to avoid this, use std::function<> out of any class so that can be used
  globally and use std::bind for any classes member function. so can have
  flexibility. see sample commands

*/

#include "console.h"
#include <iostream>

// standard readline include files
#include <readline/history.h>
#include <readline/readline.h>

bool testCommand1(const std::string &command, void *)
{
  std::cout << "command " << command << " runs\n";
}

int main(int argc, char **argv)
{
  char *buf{nullptr};

  Console console;

  console.addCommand("test1",
                     "command for global function",
                     testCommand1,
                     nullptr);

  while ((buf = readline(">> ")) != nullptr)
  {
    if (strlen(buf) > 0)
    {
      add_history(buf);
    }

    // to see what's entered.
    // printf("[%s]\n", buf);

    console.runCommand(buf);

    // readline mallocs a new buffer every time so have to free it.
    free(buf);
  }

  return EXIT_SUCCESS;
}
