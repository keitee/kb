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
cntDicLine = 0
#startpos= 2760000
#prepos= 2760000
startpos= 2761517
prepos= 2761517
starttime= 0

Jpos = 0
Jtime = 0
Jdelta = 0

#
#
while True:
	# read a line of dict
	lineDic = flog.readline()

	# got an EOF
	if len(lineDic) == 0:
		break
		
	# inc dic count
	cntDicLine += 1
	matchDic = re.search(r'.*streamPosition=(\d+).*time=(\d+)', lineDic )
	# old one: matchDic = re.search(r'^NDS:.*!(FATAL|ERROR|WARN|MIL)\s+-(\S+).*(M:\S+)\s+(F:\S+).*>\s?(.*)', lineDic )
	
	if matchDic == None:
		print 'ndsmatch: why is this dic item not matched? (%d)th : %s' % (cntDicLine, lineDic)
	else:
                newpos = int(matchDic.group(1))
                newtime = int(matchDic.group(2))
                diff = newpos - prepos
                tdiff = newtime - starttime
                jpos = newpos - startpos
                delta = jpos - newtime
                print 'streamPos = %8d, pstreamPos = %8d, posDiff = %4d, timeDiff= %4d || jpos = %5d, jtime = %5d, delta = %5d' % (newpos, prepos, diff, tdiff, jpos, newtime, delta)
                prepos = newpos
                starttime = newtime
flog.close()
