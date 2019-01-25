#!/usr/local/bin/python
# NOTE. don't supprts non-ascii chars in a script.
# keitee.

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
#
lineno= 0
clock_old= 0

print 'clock'
print '======================'

#
while True:
	# read a line of dict
	lineDic = flog.readline()

	# got an EOF
	if len(lineDic) == 0:
		break
		
	# inc dic count
	matchDic = re.search(r'^(\d+).*ClockTask.onTimeout at (\d+)', lineDic )
	
	if matchDic == None:
            pass
		# print 'ndsmatch: why is this dic item not matched? (%d)th : %s' % (cntDicLine, lineDic)
	else:
            lineno = int(matchDic.group(1))
            clock_new = int(matchDic.group(2))
            clock_diff = clock_new - clock_old
            print 'line = %8d, new clock = %8d, old clock = %8d, diff = %8d' % (lineno, clock_new, clock_old, clock_diff)
            clock_old = clock_new

flog.close()
