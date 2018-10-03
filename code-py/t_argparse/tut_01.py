import argparse

parser = argparse.ArgumentParser()
parser.add_argument("--host", help="build for host", action="store_true")
parser.add_argument("--target", help="build for target", action="store_true")
parser.add_argument("--clang", help="build for clang", action="store_true")
args = parser.parse_args()
if args.host:
    print "host turned on"

if args.target:
    print "target turned on"

if args.clang:
    print "clang turned on"
