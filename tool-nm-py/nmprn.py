#!/home/NDS-UK/parkkt/bins/usr/local/bin/python
# NOTE. don't supprts non-ascii chars in a script.
# keitee.

import sys
import re

# to print out matches that matchs one in dict file and log file. use stdin for a log file.
#
if len(sys.argv) < 3:
	print """
	prnmatch: py <dict> <log> 
	"""
	sys.exit()

# read a 'dict' file
#
try:
	fdic = open( sys.argv[1] )

except IOError:
	print 'prnmatch: %s is not found.' % sys.argv[1]
	sys.exit()

except:
	print 'prnmatch: unknown exception when reading a dict.'
	sys.exit()

# open a log file
try:
	flog = open( sys.argv[2], 'r+' )

except IOError:
	print 'prnmatch: %s is not found.' % sys.argv[2]
	sys.exit()

except:
	print 'prnmatch: unknown exception when opening an output: ', sys.argv[2]
	sys.exit()


cntDicLine = 0

#
while True:
	# read a line of dict
	lineDic = fdic.readline()

	# got an EOF
	if len(lineDic) == 0:
		break
		
	# skip a comment
	if lineDic[:1] == '#':
		cntDicLine += 1
		continue

	cntDicLine += 1

	# old: matchDic = re.search(r'^NDS:.*!(FATAL|ERROR|WARN|MIL)\s+-(\S+).*(M:\S+)\s+(F:\S+).*>\s?(.*)', lineDic )
	matchDic = re.search(r'^NDS:.*!(FATAL|ERROR|WARN|MIL)\s+-(\S+).*(M:\S+)\s+(F:\S+).*>.*', lineDic )
	
	if matchDic == None:
		print 'prnmatch: not matched in %s:%d : %s' % (sys.argv[1], cntDicLine, lineDic)
	else:
		# NOTE. DO NOT SUPPORT FREE TEXT
		#estr = re.escape( xstr )
		#print 'estr: %s' % ( estr )

		#patternDict = r'^(\d+:)?NDS:.*(' + matchDic.group(1) + ')\s+-(' + matchDic.group(2) + '\\b).*(' + matchDic.group(3) + ')\s+(' + \
		#		matchDic.group(4) +'\\b).*>' + xstr
		patternDict = r'^(\d+:)?NDS:.*(' + matchDic.group(1) + ')\s+-(' + matchDic.group(2) + '\\b).*(' + matchDic.group(3) + '.*)\s+(' + \
				matchDic.group(4) +').*>.*'
		
		# print matchDic.groups()
		# print 'patternDict: %s' % ( patternDict )

		#
		# while for log file
		#
		flog.seek(0)

		while True:
			lineOut = flog.readline()

			#print patternDict
			#print lineOut
			
			# got an EOF
			if len(lineOut) == 0:
				break
				
			if lineOut == '\n':
				continue

			matchLog = re.search( patternDict, lineOut )
			
			# do only when there is a match
			# match = {1, 2, 3, 4, 5, 6}. so starts from 1. 
			# use rstrip to remove a newline of readline
			if matchLog != None:
				#print '===='
				#print 'pdict: %s' % ( patternDict )
				#print 'match: %d, %s' %( len( matchLog.groups() ), matchLog.groups() )
				print '%s' % (lineOut.rstrip())

		# end of while

flog.close()
fdic.close()

