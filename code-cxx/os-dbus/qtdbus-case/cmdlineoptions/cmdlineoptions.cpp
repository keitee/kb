#include <QDebug>

#include "cmdlineoptions.h"

/*
-{-----------------------------------------------------------------------------
QCommandLineParser

https://doc.qt.io/qt-5/qcommandlineparser.html#showHelp

void QCommandLineParser::setApplicationDescription(const QString &description)

Sets the application description shown by helpText().


QString QCommandLineParser::helpText() const

Returns a string containing the complete help information.


void QCommandLineParser::showHelp(int exitCode = 0)

Displays the help information, and exits the application. This is automatically
triggered by the --help option, but can also be used to display the help when
the user is not invoking the application correctly. The exit code is set to
exitCode. It should be set to 0 if the user requested to see the help, and to
any other value in case of an error.


QCommandLineOption QCommandLineParser::addHelpOption()

Adds the help option (-h, --help and -? on Windows) as well as an option
--help-all to include Qt-specific options in the output.

These options are handled automatically by QCommandLineParser.

Remember to use setApplicationDescription to set the application description,
which will be displayed when this option is used.

<ex>
keitee@kit-hdebi:~/git/kb/code-cxx/os-dbus/qtdbus-case/build$ ./qtdbus-case --help
Usage: ./qtdbus-case [options]
qtdus daemon case

Options:
  -h, --help  Displays this help.


bool	addOption(const QCommandLineOption &option)
bool	addOptions(const QList<QCommandLineOption> &options)


-{-----------------------------------------------------------------------------
QCommandLineOption

https://doc.qt.io/qt-5/qcommandlineoption.html#QCommandLineOption-3

QCommandLineOption::QCommandLineOption(
  const QStringList &names, 
  const QString &description, 
  const QString &valueName = QString(), 
  const QString &defaultValue = QString())

Constructs a command line option object with the given arguments.

This overload allows to set `multiple names for the option`, for instance o and
output.

The names of the option are set to names. The names can be either short or long.
Any name in the list that is one character in length is a short name. Option
names must not be empty, must not start with a dash or a slash character, must
not contain a = and cannot be repeated.

The description is set to description. It is customary to add a "." at the end
of the description.

In addition, the valueName needs to be set if the option expects a value. The
default value for the option is set to defaultValue.

In Qt versions before 5.4, this constructor was explicit. In Qt 5.4 and later,
   it no longer is and can be used for C++11-style uniform initialization:

QCommandLineParser parser;
parser.addOption({{"o", "output"}, "Write generated data into <file>.", "file"});

*/

CmdLineOptions::CmdLineOptions()
{
  m_parser.setApplicationDescription("qtdus daemon case");
  m_parser.addHelpOption();

  // BleRcuDaemon --noconsole --session --netns=3 --audio-fifo-dir=/tmp/blercu/
  // so add more options

#if 0
  // --k, --noconsole, -k works
  // TODO: to get `value` of option, have to set `valueName`. what's the use of
  // `valueName` since use name of option to get value??

  m_parser.addOption(QCommandLineOption({"k", "noconsole"},
                                        "disable console output on <false>",
                                        "xxx"));

  m_parser.addOption(QCommandLineOption("session", "use the session bus"));
#endif

  // NOTE: even if some handlers will not use handle, use `_1` since
  // OptionHandler typedef is a single type which expect an arg.

  QList<QPair<QCommandLineOption, OptionHandler>> options = {
    // so it is clear that not use value
    {QCommandLineOption({"k", "noconsole"},
                        "disable console output on <false>"),
     std::bind(&CmdLineOptions::closeConsole, this, std::placeholders::_1)},
    {QCommandLineOption("session", "use the session bus"),
     std::bind(&CmdLineOptions::setDBusSession, this, std::placeholders::_1)},

    {QCommandLineOption({"n", "netns"}, "The host network namespace <-1>"),
     std::bind(&CmdLineOptions::setNetworkNamespace,
               this,
               std::placeholders::_1)},

    // so it is clear that it will use value
    {QCommandLineOption({"f", "audio-fifo-dir"},
                        "Directory to use for audio fifos </tmp>",
                        "path"),
     std::bind(&CmdLineOptions::setAudioFifoDirectory,
               this,
               std::placeholders::_1)}};

  m_options.swap(options);

  for (const auto &e : m_options)
  {
    m_parser.addOption(e.first);
  }
}

void CmdLineOptions::process(const QCoreApplication &app)
{
  // let parser process the given options
  m_parser.process(app);

  // QStringList QCommandLineParser::optionNames() const

  auto options = m_parser.optionNames();

  for (const auto option : options)
  {
    // iterate the list to find the handler
    auto it = m_options.cbegin();

    for (it; it != m_options.cend(); ++it)
    {
      const auto &commandLine = it->first;
      const auto &handler = it->second;

      // QStringList QCommandLineOption::names() const
      // Returns the names set for this option.

      // bool QList::contains(const T &value) const
      // Returns true if the list contains an occurrence of value; otherwise
      // returns false.

      if (handler && commandLine.names().contains(option))
      {
        handler(m_parser.value(option));
        break;
      }
    }
  }
}

void CmdLineOptions::closeConsole(const QString)
{
  qDebug() << "handler: closeConsole called";
}

// setter.
// explicit say it do not use arg

void CmdLineOptions::setDBusSession(const QString) 
{
  qDebug() << "handler: setDBusSession called";

  // NOTE: when not use "enum class"
  // m_busType = SessionBus;

  m_busType = DBusType::SessionBus;
}

void CmdLineOptions::setNetworkNamespace(const QString)
{
  qDebug() << "handler: setNetworkNamespace called";
}

void CmdLineOptions::setAudioFifoDirectory(const QString)
{
  qDebug() << "handler: setAudioFifoDirectory called";
}

// debug. to see if options is set
bool CmdLineOptions::isSet(const QString &name) const
{
  return m_parser.isSet(name);
}

// debug. to see if options is set
QString CmdLineOptions::value(const QString &name) const
{
  return m_parser.value(name);
}

// getter. get dbus type

CmdLineOptions::DBusType CmdLineOptions::dbusType() const
{
  return m_busType;
}

QString CmdLineOptions::dbusAddress() const
{
  return m_busAddress;
}

QString CmdLineOptions::dbusService() const
{
  return m_serviceName;
}
