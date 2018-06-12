#!/usr/bin/python
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
startpos= 0
starttime= 0
lineno = 0

handle_map={} 

#
while True:
	# read a line of dict
	lineDic = flog.readline()

	# got an EOF
	if len(lineDic) == 0:
		break
		
	# inc dic count
	cntDicLine += 1

        # problem:
        # in the log, there are calls to allocate resource and deallocate
        # resources which are two API calls respectably, for example:
        #
        # 58670:NDS: ^[2018:05:21 12:43:09]0946685667.054875 !MIL    -ASM          		< p:0x00000260 P:MW t:0x02055520 T:VRM_SRV M:asmlib_so.c F:ASMLIB_SO_CreateSubstitutionObject L:4592 > status=0xe803000, *ppSubstitutionObject = 0x1f5ad44 
        # 65975:NDS: ^[2018:05:21 12:47:55]0946685953.659482 !INFO   -ASM          		< p:0x00000260 P:MW t:0x01f15520 T:ASMLIB_MAIN_THREAD M:asmlib_so.c F:ASMLIB_SO_DestroySubstitutionObject L:4673 > pSubstitutionObject = 0x1f5ad44 
        # 
        # the problem shows that mw runs out resource and simple grepping shows
        # that there are mismatch between allocation and deallocation calls.
        # However, the difficulty is that as you can see, log lines prints
        # handle in hex and need to have way to match two calls for the same
        # handdle. 
        # 
        # So use regex, pick ups handle and function name, build a map. If there
        # is a map entry which has only one function name, then it means
        # problematic resource. 
        #
        # F:ASMLIB_SO_CreateSubstitutionObject L:4592 > status=0xe803000, *ppSubstitutionObject = 0x1f5ad44 
        # F:ASMLIB_SO_DestroySubstitutionObject L:4673 > pSubstitutionObject = 0x1f5ad44 
        #
        # Works well. 
        # 
        # The second is that found that the same handle is used over and over
        # again which is different from assumption above. This make a map entry
        # which has many creation and destroy calls. So changed to print number
        # of function calls for each resouce. If it's odd number, then it's
        # mismatched. 

        matchDic = re.search(r'.*F:(\w+).*(\*ppSubstitutionObject|> pSubstitutionObject) = (\w+)', lineDic )
	
	if matchDic == None:
            pass
	else:
            handle = matchDic.group(3)
            fname = matchDic.group(1)
            if handle not in handle_map:
                handle_map[handle] = [fname]
            else:
                handle_map[handle].append(fname)

flog.close()

for k in handle_map:
    print '%s -> { %d }' % (k, len(handle_map[k]))

