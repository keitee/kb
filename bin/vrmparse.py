#!/usr/bin/python

# Issues
#  - Need to know the endstate of all vrm state machines to end columns correctly
#  - performance/mem usage, possibly only read one line from file at a time, not whole file
#

import sys
import optparse
import re
import cgi

version = "0.02"

class jobcolumn:
  jobname = ""
  xpos = 0
  lineid = ""
  oldy = 0

  def __init__(self, jobname, xpos):
    self.jobname = jobname
    self.xpos = xpos

#############################################################################
# HTML Header

def htmlheader():
  return """<!DOCTYPE html PUBLIC "-//W3C//DTD XHTML 1.0 Strict//EN" "http://www.w3.org/TR/xhtml1/DTD/xhtml1-strict.dtd">
<html xmlns="http://www.w3.org/1999/xhtml">
<head>
<title>****title****</title>
<meta http-equiv="Content-Type" content="text/html; charset=utf-8"/>
<style type="text/css">

body {
 padding:0;
 margin:0;
 font-family: sans-serif;
 font-size: 0.6em;
}

.fsmbox {
 background-color: #CCCCCC;
 border-color: #000000;
 position:absolute;
 overflow:hidden;
 border-style: solid;
 border-width: 1px;
 box-sizing: border-box;
 -moz-box-sizing: border-box;
 -webkit-box-sizing: border-box;
 width: ****COLWIDTH****px;
}

.headerbox {
 position:absolute;
 overflow:hidden;
 border-style: solid;
 border-width: 1px;
 border-color: #000000;
 background-color: #AAAAAA;
 font-size: 1em;
 box-sizing: border-box;
 -moz-box-sizing: border-box;
 -webkit-box-sizing: border-box;
 font-weight: bold;
 width: ****COLWIDTH****px;
 height: ****ITEMHEIGHT****px;
}

.lineitem {
 position:absolute;
 overflow:hidden;
 border-style: solid;
 border-width: 1px;
 box-sizing: border-box;
 -moz-box-sizing: border-box;
 -webkit-box-sizing: border-box;
 opacity: 0.5;
 z-index: 1;
}

.MIL {
 background-color: #CCCCFF;
 border-color: #000000;
}

.ERROR {
 background-color: #FFCCCC;
 border-color: #000000;
}

.FATAL {
 background-color: #FFCCFF;
 border-color: #000000;
}

.WARN {
 background-color: #FFDFCC;
 border-color: #000000;
}

.linebox{
 position:absolute;
 overflow:hidden;
 background-color: #FFFFFF;
 border-color: #999999;
 border-style: solid;
 border-width: 1px;
 box-sizing: border-box;
 -moz-box-sizing: border-box;
 -webkit-box-sizing: border-box;
 width: ****LINENUMBOXWIDTH****px;
 height: ****ITEMHEIGHT****px;
}

.popup {
 position: fixed;
 font-size: 2em;
 top:10px;
 right: 10px;
 background-color: #CCCCCC;
 border-color: #000000;
 border-width:1px;
 border-style: solid;
 visibility: hidden;
 width: 50%;
 z-index: 4;
}

.grid {
 position:absolute;
 left: 0;
 height: 1px;
 background-color: rgba(0,0,0,0.1);
 z-index: 2;
}

.headerbar {
 margin-left: 0px;
 position: fixed;
 top: 0px;
 width: 100%;
 z-index: 3;
}

.linebar {
 margin-top: 0px;
 position: fixed;
 left: 0px;
 top: 0px;
 height: 100%;
 z-index: 3;
}

</style>
<script type="text/javascript">
<!--
function popupmouseover(job, logline, time, prev, input, newstate, thread, module, line)
{
  var e = document.getElementById("popup");
  e.innerHTML = "<b>" + job + "<\/b><br/>"
    + "log line: " + logline + "<br/>"
    + "Time: " + time + "<br/>"
    + "PrevState: " + prev + "<br/>"
    + "InputtoFSM: " + input + "<br/>"
    + "NewState: " + newstate + "<br/>"
    + "Thread: " + thread + "<br/>"
    + module + ":" + line + "<br/>";

  e.style.visibility = "visible";
}

function popupmouseoverlinemess(level, logline, time, message, thread, module, line)
{
  var e = document.getElementById("popup");
/*  e.innerText = "test";*/

  message = message.replace(/&/g, "&amp;").replace(/>/g, "&gt;").replace(/</g, "&lt;").replace(/"/g, "&quot;");

  e.innerHTML = "<b>" + level + ":" + message + "<\/b><br/>"
    + "log line: " + logline + "<br/>"
    + "Time: " + time + "<br/>"
    + "Thread: " + thread + "<br/>"
    + module + ":" + line + "<br/>";

  e.style.visibility = "visible";
}

function popupmouseout()
{
  var e = document.getElementById("popup");
  e.style.visibility = "hidden";
}

window.onscroll = function(oEvent)
{
  var e = document.getElementById("headerbar");
  e.style.marginLeft = 0 - window.pageXOffset + "px";
  e = document.getElementById("linebar");
  e.style.marginTop = 0 - window.pageYOffset + "px";
}

-->
</script>
</head>
<body>
<div id="popup" class="popup">popup</div>
<p style="color: red">WARNING requires modern web browser (not IE 8) - no autodetecting of required features yet</p>
"""

#############################################################################
# main()

# commandline parse
usage = "usage: vrmparse [options] filename"
parser = optparse.OptionParser(usage)
parser.add_option("-v", "--version", help="Output program version information", action="store_true", dest="version", default=False)
(options, args) = parser.parse_args()

if options.version == True:
  print "vrmparse " + version + " Contact phowkins@cisco.com"
  sys.exit(0)

# extract filename from commandline
if len(args) != 1:
  print usage
  sys.exit(1)
filename = args[0]

try:
  f = open(filename, "rb")
except:
  sys.stderr.write("Error opening file '" + filename + "'\n")
  sys.exit(1)

colwidth = 200
itemheight = 30
linenumboxwidth = 100

globalx = linenumboxwidth
globaly = itemheight
jobtypes = []

header = htmlheader()
header = header.replace("****title****", filename + " : VRM analysis")
header = header.replace("****COLWIDTH****", str(colwidth))
header = header.replace("****ITEMHEIGHT****", str(itemheight))
header = header.replace("****LINENUMBOXWIDTH****", str(linenumboxwidth))

outputl = []              # List of output lines
outputl.append(header)
outputreplacemap = dict() # index of str replaces that need to be done in outputl
outputlongrowlist = []    # List of lines that have a LONGROW and need a replace

headerbarlines = []       # List of lines used in scrollable top bar of job titles
linebarlines = []         # List of lines used for log line numbers

# Compile up the two regexes we use
## Detect a FSM change line
fsmregex = re.compile('.*?([0-9\.]+) !MIL.+-VRM.+t:([^ ]+).+?M:([^ ]+).+?L:([0-9]+).+VRM-FSM: (.+): (.+) -> \((.+)\) -> (.+)$')
## Detect a VRMS debug line
loglineregex = re.compile('^.*?NDS: \^\[?.+?\]?([0-9]+\.[0-9]+) !(.+) +-VRM.+t:(.+) .+M:(.+) .+L:([0-9]+) > (.+)$')

linenum = 0
for line in f:
  linenum = linenum + 1
  line = line.strip()
  # Search for VRM FSM lines
  if "VRM-FSM:" in line:
    m = fsmregex.search(line)
    if m == None:
      sys.stderr.write("failed to parse line \"" + line + "\" as VRM-FSM line, need to update regex\n")
      sys.exit(1)
    time = m.group(1).strip()
    thread = m.group(2).strip()
    module = m.group(3).strip()
    line = m.group(4).strip()
    job = m.group(5).strip()
    prev = m.group(6).strip()
    input = m.group(7).strip()
    newstate = m.group(8).strip()

    # Find existing job?
    found = False
    for jobtype in jobtypes:
      if jobtype.jobname == job:
        found = True
        break

    if found == False:
      # New job type found
      globaly += itemheight # Hack, make the job headers not overlap any previous lineitems

      jobtype = jobcolumn(job, globalx)
      jobtypes.append(jobtype)
      globalx += colwidth
      # Job column header
      outputl.append("<div class=\"headerbox\" style=\"left:" + str(jobtype.xpos))
      outputl.append("px; top:" + str(globaly - itemheight) + "px;")
      outputl.append("\">" + jobtype.jobname + "</div>\n")
      # Also copy into the top bar
      headerbarlines.append("<div class=\"headerbox\" style=\"left:" + str(jobtype.xpos))
      headerbarlines.append("px;\">" + jobtype.jobname + "</div>\n")
    else:
      # existing jobtype found, update column of previous state
      if jobtype.lineid != "":
        outputl[outputreplacemap[jobtype.lineid]] = outputl[outputreplacemap[jobtype.lineid]].replace("****" + jobtype.lineid + "****", str(globaly - jobtype.oldy))
        del outputreplacemap[jobtype.lineid]

    assert(jobtype)

    # Output line number and time div
    linediv = """<div class="linebox" style="top: %spx;">line: %s<br/>%s</div>"""
    linebarlines.append(linediv % (str(globaly), str(linenum), time) + "\n")
    
    # FSM change div, start a new box for the new state
    outputl.append("<div class=\"fsmbox\" style=\"left:" + str(jobtype.xpos))
    outputl.append("px; top:" + str(globaly) + "px;")
    outputl.append(" height:****" + str(linenum) + "****px;\"")
    # Store line that needs a replace
    outputreplacemap[str(linenum)] = len(outputl) - 1
    outputl.append(" onmouseover=\"popupmouseover('" + job + "','" + str(linenum) + "','" + time + "'")
    outputl.append(",'" + prev + "','" + input + "','" + cgi.escape(newstate) + "','" + thread + "','" + module + "','" + line + "')\"")
    outputl.append(" onmouseout=\"popupmouseout()\">" + cgi.escape(newstate) + "<br/><i>&nbsp;&nbsp;" + input + "</i></div>\n")

    # Update column info to know at which point the state item starts
    jobtype.oldy = globaly
    jobtype.lineid = str(linenum)

    # End columns that reach the end state of the state machines
    endstates = [ 'E_BDL_INT_STATE_NONE',
                  'E_JOB_MAN_SRV_INT_ST_NULL',
                  'E_LIVE_STATE_NONE',
                  'E_PLAY_INT_STATE_NONE',
                  'E_PLE_STATE_NONE',
                  'E_RECORD_INT_STATE_NONE',
                  'VRM_IMP_OBJECT_INT_STATE_NULL',
                  'VRM_IMP_SESSION_INT_STATE_NULL',
                  'VRM_MONITOR_INT_STATE_NULL',
                  'E_FROM_HTTP_STATE_NONE',
                  'E_VRP_RBREC_STATE_NULL',
                  'E_PLE_POS_STATE_NONE',
                  'E_RESOURCE_RESERVE_INT_STATE_NONE',
                  '<End State>' ] 
    if newstate in endstates:
      if jobtype.lineid in outputreplacemap:
        outputl[outputreplacemap[jobtype.lineid]] = outputl[outputreplacemap[jobtype.lineid]].replace("****" + jobtype.lineid + "****", str(globaly - jobtype.oldy + itemheight))
        del outputreplacemap[jobtype.lineid]
      else:
        sys.stderr.write("Couldn't look up '" + jobtype.lineid + "' in outputreplacemap\n")
      jobtype.lineid = ""

    globaly += itemheight

  else:
    # Not a FSM Change line but is it something else we're interested in
    # VRM Milestone/Fatal/Error/warn line that isn't FSM
    if "VRMS" in line:
      m = loglineregex.search(line)
      if None != m:
        time    = m.group(1).strip()
        level   = m.group(2).strip()
        thread  = m.group(3).strip()
        module  = m.group(4).strip()
        line    = m.group(5).strip()
        message = m.group(6).strip()

        if level in ["MIL", "FATAL", "ERROR", "WARN"]:

          # Prepare message for html output, escape quotes
          message = message.replace("&", "&amp;");
          message = message.replace("\"", "&quot;");
          message = message.replace("'", "&#039;");
          message = message.replace("<", "&lt;");
          message = message.replace(">", "&gt;");

          # Output line number and time div
          linediv = """<div class="linebox" style="top: %spx;">line: %s<br/>%s</div>
"""
          linebarlines.append(linediv % (str(globaly), str(linenum), time))

          outputl.append("<div class=\"lineitem " + level + "\" style=\"left: " + str(linenumboxwidth) + "px; height: " + str(itemheight) + "px;")
          outputl.append(" top: " + str(globaly) +"px; width: ****LONGROW****px;\" ")
          outputlongrowlist.append(len(outputl) - 1)
          outputl.append(" onmouseover=\"popupmouseoverlinemess('" + level + "','" + str(linenum) + "','" + time + "'")
          outputl.append(",'" + message + "','" + thread + "','" + module + "','" + line + "')\"")
          outputl.append(" onmouseout=\"popupmouseout()\">" + message + "</div>\n")
          globaly += itemheight

# End of log, close filehandle
f.close()

# End of log, extend columns of any unfinished jobs
for jobtype in jobtypes:
  if jobtype.lineid != "":
    outputl[outputreplacemap[jobtype.lineid]] = outputl[outputreplacemap[jobtype.lineid]].replace("****" + jobtype.lineid + "****", str(globaly - jobtype.oldy))

# End of log, extend width of all cross column content
for linenum in outputlongrowlist:
  outputl[linenum] = outputl[linenum].replace("****LONGROW****", str(globalx - linenumboxwidth))


# Draw grid lines to help people see the log line for any column
liney = itemheight
while liney <= globaly:
  outputl.append("<div class=\"grid\" style=\"top: " + str(liney) + "px; width: " + str(globalx) + "px;\"></div>\n")
  liney += itemheight

# Add on top bar for job titles
outputl.append("<div id=\"headerbar\" class=\"headerbar\">\n")
for line in headerbarlines:
  outputl.append(line)
outputl.append("</div>\n");

# Add on side bar for log line numbers
outputl.append("<div id=\"linebar\" class=\"linebar\">\n")
for line in linebarlines:
  outputl.append(line)
outputl.append("</div>\n");

# Add on Page footer
outputl.append("</body>\n</html>\n")

try:
  f = open(filename+".html", "wb")
except:
  sys.stderr.write("Error opening file '" + filename + ".html' for output\n")
  sys.exit(1)


for line in outputl:
 f.write(line)

f.close()




