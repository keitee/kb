#!/home/NDS-UK/parkkt/bins/usr/local/bin/python
# NOTE. don't supprts non-ascii chars in a script.
# keitee.

import sys
import re

#
if len(sys.argv) < 3:
	print """
	ndsmatch: usuage: py dict log. NOTE! log file will be updated.
	"""
	sys.exit()

#
# read a 'dict' file
#
try:
	# fdicname = '/home/NDS-UK/parkkt/bins/fusion.dic'
	# fdic = open( fdicname )
	fdicname = sys.argv[1]	
	fdic = open( fdicname )
	
except IOError:
	print 'ndsmatch: %s is not found.' % fdicname
	sys.exit()

except:
	print 'ndsmatch: unknown exception when reading a dict.'
	sys.exit()


#
# open a 'nds' file
#
#fnameOutput = sys.argv[2].replace('log', 'nds')

print 'nm: ndsmatch: %s, fout: %s' % (fdicname, sys.argv[2])

try:
	fout = open( sys.argv[2], 'r+' )

except IOError:
	print 'ndsmatch: %s is not found.' % sys.argv[2]
	sys.exit()

except:
	print 'ndsmatch: unknown exception when opening an output: ', sys.argv[2]
	sys.exit()


#
# count vars
#
cntDicLine = 0
cntOutLine = 0

#
#
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

	# inc dic count
	cntDicLine += 1

	# Sample line:
	# NDS: ^0946684966.710878 !ERROR -SRM          < p:000000c1 t:016121b0 T:SRM_LP_THREAD M:srm_utils.c F:SRM_SystemStringToCString L:00287 > **** SRM ERR in srm_utils.c:287
	# 
	# currently supports match = {1, 2, 3, 4, 5}
	#	error type such as FATAL, ERROR, WARN, MIL and which is GROUP(1)
	#	component type such as SRM and which is GROUP(2)
	#	filename which is GROUP(3)
	#	func which is GROUP(4)
	#	free text which is GROUP(5)
	
	# \s : any whitespace char
	# \S : any not-whilespace char
	# '?': causes the resulting RE to match 0 or 1 repetitions of the preceding RE. ab? will match either 'a' or 'ab'.
	
	# NOTE. there's no support for a leading numbers of the dict line.
	# NOTE. added MIL just for usefulness.
	# NOTE. possibly add 'T:' group?
	
	# extract interested groups from a dict entry. grab the whole free text.
	matchDic = re.search(r'^NDS:.*!(FATAL|ERROR|WARN|MIL)\s+-(\S+).*(M:\S+)\s+(F:\S+).*>(.*)', lineDic )
	# old one: matchDic = re.search(r'^NDS:.*!(FATAL|ERROR|WARN|MIL)\s+-(\S+).*(M:\S+)\s+(F:\S+).*>\s?(.*)', lineDic )
	
	if matchDic == None:
		print
		print 'ndsmatch: ******************************'
		print 'ndsmatch: why is this dic item not matched? (%d)th : %s' % (cntDicLine, lineDic)
		print 'ndsmatch: ******************************'
	else:

		# NOTE. 2012 Sep 12. Now supports free text using  re.escape(string). HOLDING!!
		# re.escape(string)
		# Return string with all non-alphanumerics backslashed; this is useful if you want to match an arbitrary literal string 
		# that may have regular expression metacharacters in it.

		# NOTE. 2011 June 22.
		# add '\b' in the pattern to avoid following cases: UAMC and UAM in component name and xxx and xxx2 in function name.
		#
		# e.g.
		# !WARN  -MSM          < M:msm_init.c F:MSM_RunLevelRequest L:00120 > Milestone : Move to SYSINIT_RunLevelRequest 3, Starts...
		# !WARN  -MSM          < M:msm_init.c F:MSM_RunLevelRequest2 L:00152 > Milestone : Moving to SYSINIT_RunLevelRequest 4 cont (current_level = 3)
		#
		# \b
		# Matches the empty string, but only at the beginning or end of a word. A word is defined as a sequence of alphanumeric or 
		# underscore characters, so the end of a word is indicated by whitespace or a non-alphanumeric, non-underscore character. 
		# Note that formally, \b is defined as the boundary between a \w and a \W character (or vice versa), or between \w and the beginning/end 
		# of the string, so the precise set of characters deemed to be alphanumeric depends on the values of the UNICODE and LOCALE flags. 
		# For example, r'\bfoo\b' matches 'foo', 'foo.', '(foo)', 'bar foo baz' but not 'foobar' or 'foo3'. Inside a character range, 
		# \b represents the backspace character, for compatibility with Python's string literals.
		
		# HOLDING!!
		# escapedStr = re.escape( matchDic.group(5) )
		
		# NOTE.
		# To supports the leading numbers, can add (\d+) to the pattern but this changes group(#) so causes the problem when use group(#) later
		# as the total num of groups will be +1 and group(1) will be null when there's no line number in the input.
		# for example: ^(\d+:)?NDS:.*(ERROR)\s+-(VGC_JAVA).*(M:natdiag.c)\s+(F:DIAG_JAVA_natLogError).*>\s+(.*))		
		# 
		# Support the leading line number as the input log has the line number as it's the extracted only for errors in the fdic system.
		#
		
		# match = {1, 2, 3, 4, 5}
		#patternDict = r'^(\d+:)?NDS:.*(' + matchDic.group(1) + ')\s+-(' + matchDic.group(2) + '\\b).*(' + matchDic.group(3) + ')\s+(' + \
		#		matchDic.group(4) +'\\b).*>' + escapedStr
		patternDict = r'^(\d+:)?NDS:.*(' + matchDic.group(1) + ')\s+-(' + matchDic.group(2) + '\\b).*(' + matchDic.group(3) + ')\s+(' + \
				matchDic.group(4) +').*>.*'
		
		# example:
		# patternDict: (xxxx)^NDS:.*(ERROR)\s+-(SRM\b).*(M:srm_utils.c)\s+(F:SRM_SystemStringToCString\b).*>\s\*\*\*\*\ SRM\ ERR\ in\ srm\_utils\.c\:287
		
		# while for writing output file
		while True:
			posOut = fout.tell()
			lineOut = fout.readline()

			# got an eof
			if len(lineOut) == 0:
				break
				
			if lineOut == '\n':
				continue

			# inc log count
			cntOutLine += 1
			
			matchLog = re.search( patternDict, lineOut )
			
			# do only when there is a match
			if matchLog != None:
				# print 'patternDict: (%d)th, %s' % ( cntDicLine, patternDict )
				# print 'matchLog: (%s)' % matchLog.group(0)
				# print matchLog.groups()
				
				# make changes to log file with modified str as the log file is not a LOG but the extraced with only errors.
				# construct out mesg depending on when starts with NDS or linenum:NDS.
				
				# NOTE. handling when a func name has ().
				# comment off the 'len' check because when the group has '()', it is considered a group and increase the len.
				# e.g.
				# when the line has 'F:SIM_dbc_query_LocateXsiProgInstObject()', the pattern become '(F:SIM_dbc_query_LocateXsiProgInstObject())'
				# and matchLog is '('26625:', 'ERROR', 'SQ13', 'M:sim_dbc_locators.c', 'F:SIM_dbc_query_LocateXsiProgInstObject', '')'.
				# so the last group comes from the '()' of 'F:...()'.
				#
				#if len( matchLog.groups() ) == 5:
				#	strOutput = '+--{' + patternDict + '}'
				#else:
				#	print '== %d dict ==' % (cntDicLine)
				#	print 'pdict: %s' % ( patternDict )
				#	print 'match: %s' % matchLog.group(0)
				#	print matchLog.groups()
				
				strOutput = '+--{' + matchLog.group(1) + ':' + patternDict + '}'					
				
				# make a clean mesg
				lineOut = re.sub(r'.', ' ', lineOut)				
				lineOut = strOutput + lineOut[ len(strOutput): ]				

				#lineOut.strip()
				
				fout.seek(posOut)
				fout.write(lineOut)				
			#else:
			#	print 'patternDict: (%d:%d)th, p:%s l:%s' % ( cntDicLine, cntOutLine, patternDict, lineOut )
				
		# end of while
		# reset vars of logs
		cntOutLine = 0
		fout.flush()
		fout.seek(0)

print 'nm: ndsmatch: done'
fout.flush()
fout.close()
fdic.close()
