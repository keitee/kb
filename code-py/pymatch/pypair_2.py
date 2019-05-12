#!/usr/bin/python

# NOTE. don't supprts non-ascii chars in a script.
# keitee.

# The problem:
#
# in the log, there are calls to allocate resource and deallocate
# resources which are two API calls respectably, for example:
#
# NDS: ^[2018:05:21 12:43:09]0946685667.054875 !MIL    -ASM < p:0x00000260 P:MW t:0x02055520 T:VRM_SRV M:asmlib_so.c F:ASMLIB_SO_CreateSubstitutionObject L:4592 > status=0xe803000, *ppSubstitutionObject = 0x1f5ad44 
# NDS: ^[2018:05:21 12:47:55]0946685953.659482 !INFO   -ASM < p:0x00000260 P:MW t:0x01f15520 T:ASMLIB_MAIN_THREAD M:asmlib_so.c F:ASMLIB_SO_DestroySubstitutionObject L:4673 > pSubstitutionObject = 0x1f5ad44 
# 
# the problem shows that mw runs out resource and simple grepping shows that
# there are mismatch between the number of allocation and deallocation calls.
# However, the difficulty is that as you can see, log lines prints handle in hex
# and need to have a way to match two calls for the same handle. 
# 
# So use regex, pick ups handle and function name, build a map. If there is a
# map entry which has only one function name, then it means mismatch, leaks.
#
# Later, found that the same handle is used over and over again which is
# different from assumption above. This make a map entry which has many creation
# and destroy calls. So changed to print number of function calls for each
# resouce. If it's odd number, then it's mismatched. 

import sys
import re

#
if len(sys.argv) < 2:
    print """
        ndsmatch: usuage: py log. NOTE! log file will be updated.
        """
    sys.exit()

try:
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
line_count = 0
handle_map = {} 

#
while True:
    # read a line of dict
    line = flog.readline()

    # got an EOF
    if len(line) == 0:
        break

    # inc dic count
    line_count += 1

    # For 35786 log_file_2,
    #
    # match = re.search(r'^.*F:(\w+).*(\*ppSubstitutionObject|> pSubstitutionObject) = (\w+)', line)
    # real    0m1.769s
    # user    0m1.344s
    # sys     0m0.000s
    #
    # match = re.search(r'.*F:(\w+).*(\*ppSubstitutionObject|> pSubstitutionObject) = (\w+)', line)
    # real    2m40.758s
    # user    2m37.312s
    # sys     0m0.004s

    # F:ASMLIB_SO_CreateSubstitutionObject L:4592 > status=0xe803000, *ppSubstitutionObject = 0x1f5ad44 
    match = re.search(r'^.*F:(\w+).*(\*ppSubstitutionObject|> pSubstitutionObject) = (\w+)', line)
    if match == None:
        pass
    else:
        # print(match.group(0))
        # print(match.group(3))
        # print(match.group(1))
        handle = match.group(3)
        function = match.group(1)
        state = True if function == 'ASMLIB_SO_CreateSubstitutionObject' else False

        if handle not in handle_map:
            client = {}                     # client dict
            client['requests'] = []         # request list
            handle_map[handle] = client

        handle_map[handle]['state'] = state
        handle_map[handle]['requests'].append(tuple([state, function]))

flog.close()


for client in handle_map:
    if handle_map[client]['state'] == True:
        print('\nhandle : %s, calls : %d' % (client, len(handle_map[client]['requests'])))
        for request in handle_map[client]['requests']:
            # print(request)
            sys.stdout.write('C' if request[0] == True else 'D:')
        print
    else:
        print('\nflase handle : %s' % client)
print

# kyoupark@kit-debian64:~/git/kb/code-py/pymatch$ time ./pypair_2.py log_file_2 
# 
# handle : 0x1f5ad44, calls : 809
# ...
#
# handle : 0x1f5ad74, calls : 447
# 
# handle : 0x1f5ada4, calls : 451
# 
# handle : 0x1f5add4, calls : 1
# C
# 
# handle : 0x1f5ae04, calls : 7
# CD:CD:CD:C

