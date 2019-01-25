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
prepts= 0
diff=0
startpts=25379700

#
while True:
	# read a line of dict
	lineDic = flog.readline()

	# got an EOF
	if len(lineDic) == 0:
		break
		
	# inc dic count
	cntDicLine += 1
	# matchDic = re.search(r'^(\d+).*streamPosition=(\d+)', lineDic )
        splited = lineDic.split(",")	
        # newpts = int(splited[3].strip())
        print splited

        # reducedpts = (newpts)/300
	# if newpts == None:
        #     pass
	# else:
        #     diff = (newpts - prepts)/300
        #     print 'line=%8d, npts=%8d, ppts=%8d, diff=%4d' % (cntDicLine, reducedpts, newpts, diff)
        #     prepts = newpts
flog.close()
