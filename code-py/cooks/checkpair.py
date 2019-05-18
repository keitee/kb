#!/usr/local/bin/python
# NOTE. don't supprts non-ascii chars in a script.
# keitee.

# :173 > VGC_API_PpvAction_CreateObject EXIT. obj_handle 0xa66
# :173 > VGC_API_PpvAction_DestroyObject ENTER obj_handle 0xa66

import sys
import re

#
if len(sys.argv) < 2:
	print """
	ndsmatch: usuage: py log. NOTE! log file will be updated.
	"""
	sys.exit()

# read a 'dict' file
#
try:
	# fdicname = '/home/NDS-UK/parkkt/bins/fusion.dic'
	# fdic = open( fdicname )
	flogname = sys.argv[1]	
	flog = open( flogname )
	
except IOError:
	print 'ndsmatch: %s is not found.' % flogname
	sys.exit()

except:
	print 'ndsmatch: unknown exception when reading a dict.'
	sys.exit()


# count vars
lineno = 0

#
while True:
  # read a line of dict
  line = flog.readline()

  # got an EOF
  if len(line) == 0:
    break

  # inc dic count
  lineno += 1
  matches = re.search(r'^.*VGC_API_PpvAction_.*obj_handle 0x([0-9][a-z]*)', line)

  if matches == None:
    pass
  else:
    handle = int(matchDic.group(1))
    print 'lineno = %8d, pos = %8d, pre = %8d, diff = %4d' % (lineno, handle)

flog.close()
