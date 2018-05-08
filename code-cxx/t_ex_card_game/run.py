#!/usr/bin/env python

import sys
import subprocess

if __name__ == "__main__":

    for i in range(1000):
        print ("--------- %d" % i)
        subprocess.call(["./game_test"])

