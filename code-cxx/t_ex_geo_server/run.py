#!/usr/bin/env python

import sys
import subprocess

if __name__ == "__main__":

    for i in range(100):
        print ("--------- %d" % i)
        subprocess.call(["./geoserver_test"])

