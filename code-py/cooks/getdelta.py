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
delta= 0

print 'delta'
print '======================'

#
#
while True:
	# read a line of dict
	lineDic = flog.readline()

	# got an EOF
	if len(lineDic) == 0:
		break
		
	# inc dic count
	matchDic = re.search(r'.*delta=(\d+)', lineDic )
	# old one: matchDic = re.search(r'^NDS:.*!(FATAL|ERROR|WARN|MIL)\s+-(\S+).*(M:\S+)\s+(F:\S+).*>\s?(.*)', lineDic )
	
	if matchDic == None:
            pass
		# print 'ndsmatch: why is this dic item not matched? (%d)th : %s' % (cntDicLine, lineDic)
	else:
                delta = int(matchDic.group(1))
                print '%4d' % (delta)
flog.close()
