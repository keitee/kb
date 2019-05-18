#!/usr/bin/env python

# The problem:
# 
# The issue is that when runs a production box for long time, HDD didn't go to
# spin-down state when it's supposed to do and do not meet power consumption
# requirement. Interestingly, only happens on production box and for days run.
#
# In Fusion MW, PDM manages HDD resouce and provides APIs to clients. Various
# clients blindly calls API to get or release HDD resouce. 
# 
# So PDM server has own count to make call to HDD up(when count 0->1) or down(1->0)
# based on number of calls from clients. Also, FDM client maintains own count to
# control PDM API calls to make release/request calls. 
# 
# The problem is that FDM client which has own count get the count wrong and do
# not make release call to PDM. This leaves PDM count one more and this prevents
# HDD down when other clients tries to do. THis make HDD is on since because PDM
# uses this count to control spin down or on of HDD via calls to device. 
# 
# How about using smart pointers to this problem? see cxx-sp-weak
#
# This script is to find out which client type is out of sync as to number of
# calls to request and release.
# 
# 1. multiple client(thread) can be in the same client type; means use the same
# client handle.
#
# 2. this assumes that if last state is false, then there are right number of
# calls.
#
# ture is request and flase is release
#
# true, false, true         -> last state is true so release call is missed
# true, false, true, false  -> last state flase so is in sync.
# true, true, true          -> covered
# false, false, false       -> not covered

import argparse
import re
import os.path
import sys

class ParseFile:
    def __init__(self, filename, regex):
        self.filename = filename
        self.regex    = regex

    def __iter__(self):
        with open(self.filename) as file:
            for line in file:
                # this is original code which do not work so changed to use
                # py-re-search
                # match = re.match(self.regex, line)
                match = re.search(self.regex, line)
                if match == None:
                    continue
                yield match
        raise StopIteration

def parse_forced_reqs_by_handle(log_file):

    reqs_by_handle = {} # Top level dictionary that stores forced requests for each client by handle

    # NDS: ^[2019:05:10 13:02:41]0946685004.763406 !ERROR  -PDM_MW       		< p:0x000007e3 P:MW t:0x01bbc520 T:SIM_CMC_THREAD M:pdm_api_fs.c F:PDM_FS_SetForcedRequest L:861 > handle=0x70102 requested=true f_pdm_setTrueCnt=0 f_pdm_setForcedFalseAlarmHandle=0xffffffff 
    # NDS: ^[2019:05:10 13:02:41]0946685004.866092 !ERROR  -PDM_MW       		< p:0x000007e3 P:MW t:0x01b32e80 T:no name M:pdm_api_fs.c F:PDM_FS_SetForcedRequest L:861 > handle=0xb0206 requested=false f_pdm_setTrueCnt=1 f_pdm_setForcedFalseAlarmHandle=0xffffffff 
    #
    # kyoupark@st-castor-03:~/si_logs/SI-11653$ ./x.py LOGlastrun_realtime_a791
    # 
    # Client handle     : 0xd0107
    # Threads           : FDM_MHC_EventListener_thread SPM_XXSI_CACHING
    # SetForced History : LULULULULULULULULULULULULULULULUL
    # Last Request      : NDS: ^[2019:05:10 13:04:05]0946685088.631261 !ERROR  -PDM_MW                < p:0x000007e3 P:MW t:0x02496520 T:SPM_XXSI_CACHING M:pdm_api_fs.c F:PDM_FS_SetForcedRequest L:861 > handle=0xd0107 requested=true
    # 
    # Client handle     : 0x16010d
    # Threads           : VRM_SRV
    # SetForced History : LULUL
    # Last Request      : NDS: ^[2019:05:10 13:05:12]0946685155.252747 !ERROR  -PDM_MW                < p:0x000007e3 P:MW t:0x01ecb520 T:VRM_SRV M:pdm_api_fs.c F:PDM_FS_SetForcedRequest L:861 > handle=0x16010d requested=true

    # handle(dict) -> client(dict) (thread(dict), requests(list), request(tuple)

    for entry in ParseFile(log_file, '^.* T:(\S+) .*F:PDM_FS_SetForcedRequest .* handle=(0x[0-9a-f]+) requested=([a-z]+)'):
        thread = entry.group(1)                              # Client thread name
        handle = entry.group(2)                              # Client handle
        forced = True if entry.group(3) == 'true' else False # Request state i.e. true or false

        if handle not in reqs_by_handle:
            client                   = {}     # Create a new client entry dictionary
            client['threads']        = {}     # Setup dictionary for keeping track of all threads associated with client
            client['requests']       = []     # Setup array for storing log lines for all forced requests
            reqs_by_handle[handle]   = client # Store client entry dictionary by handle 

        request = (forced, entry.group(0))                 # Build a tuple comprising the request state and entire log line
        reqs_by_handle[handle]['requests'].append(request) # Append request to history of all requests
        reqs_by_handle[handle]['state'] = forced           # Store the current request state
        reqs_by_handle[handle]['threads'][thread] = None   # Store thread name

    return reqs_by_handle

def check_forced_requests(log_file):
    reqs_by_handle = parse_forced_reqs_by_handle(log_file)

    for client in reqs_by_handle:
        # when last state is true, something is missed
        if reqs_by_handle[client]['state'] == True:
            print "\nClient handle     : %s" % client
            print "Threads           :",
            for thread in reqs_by_handle[client]['threads']:
                print thread,
            print
            print "SetForced History : ",
            for request in reqs_by_handle[client]['requests']:
                sys.stdout.write('L' if request[0] == True else 'U')
            print

            # second of last request tuple, reqs_by_handle[client]['requests'][-1][1], which is log line
            print "Last Request      :", reqs_by_handle[client]['requests'][-1][1]
        # else:
        #     print "\nClient handle     : %s in sync" % client

    # print line after for loop
    print

def main():
    parser = argparse.ArgumentParser(description='Parses log to analyse disk spin down issues (requires PDM_DISK_SPINDOWN_LOG enabled)')
    parser.add_argument('log_file', help='Log file')
    args = parser.parse_args()

    check_forced_requests(args.log_file)

if __name__ == '__main__':
    main()
