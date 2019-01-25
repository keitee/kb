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
		
	# inc dic count
  # !INFO   -MSM_MS       		< P:MW  T:EVENT_MGR_THREAD M:display_chunk.cpp F:(CalcUserPosByPtsAndNotifyItem) L:10245 > < splicer=0x7 hMC=0x7 >" "Using PTS [0,250212598] to calc position, Audio only=XFALSE 
	matchDic = re.search(r'^(\d+).*Using PTS \[0,(\d+)\]', lineDic )
	# matchDic = re.search(r'.*Using PTS \[0,(\d+)\]', lineDic )
	
	if matchDic == None:
            pass
		# print 'ndsmatch: why is this dic item not matched? (%d)th : %s' % (cntDicLine, lineDic)
	else:
            lineno = int(matchDic.group(1))
            pts_new = int(matchDic.group(2))
            pts_diff = pts_new - pts_old
            print 'line = %8d, new pts = %8d, old pts = %8d, diff = %8d' % (lineno, pts_new, pts_old, pts_diff)
            pts_old = pts_new

flog.close()
