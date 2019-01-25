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
pts_old= 0

print 'pts'
print '======================'

#
while True:
	# read a line of dict
	lineDic = flog.readline()

	# got an EOF
	if len(lineDic) == 0:
		break
		
	matchDic = re.search(r'^(\d+).*, PTS \(0,(\d+)\)', lineDic )
	
	if matchDic == None:
            pass
		# print 'ndsmatch: why is this dic item not matched? (%d)th : %s' % (cntDicLine, lineDic)
	else:
            lineno = int(matchDic.group(1))
            pts_new = int(matchDic.group(2))
            pts_diff = pts_new - pts_old

            if (pts_diff/90000) > 0:
                print '== line=%8d, new pts = %8d, old pts = %8d, diff = %8d, diff =%d' % (lineno, pts_new, pts_old, pts_diff, (pts_diff/90000))
            else:
                print 'line=%8d, new pts = %8d, old pts = %8d, diff = %8d' % (lineno, pts_new, pts_old, pts_diff)
            pts_old = pts_new

flog.close()
