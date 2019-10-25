# 5.3. Create the first recipe and task
# Our first recipe will just print a log message. We put it into the first
# group, we will call it first, and it has the version 0.1.
# 
# The task do_build overrides the empty global build task from base.bbclass.
# 
# PR is the internal revision number which should be updated after each change.
# 
# Setting a description should be self explaining.
# 
# If everything is done correct we can ask bitbake to list the available recipes.
# 
# [~/bbTutorial/build]
# bitbake  -s
# Parsing recipes: 100% ...
# Parsing of 1 .bb files complete...
# Recipe Name    Latest Version   Preferred Version
# ===========    ==============   =================
# first                 :0.1-r1
# and we can run from the build directory
# 
# [~/bbTutorial/build]
# bitbake first
# 
# NOTE: Not using a cache. Set CACHE = <directory> to enable.
# Parsing recipes: 100% |########################################################################################################################################################################################################| Time: 0:00:00
# Parsing of 1 .bb files complete (0 cached, 1 parsed). 1 targets, 0 skipped, 0 masked, 0 errors.
# NOTE: Resolving any missing task queue dependencies
# Initialising tasks: 100% |#####################################################################################################################################################################################################| Time: 0:00:00
# NOTE: Executing RunQueue Tasks
# NOTE: Tasks Summary: Attempted 1 tasks of which 0 didn't need to be rerun and all succeeded.
#
# Now check tmp/work/first-0.1-r1/temp, there are a lot of interesting files,
# for example:
#
# keitee@keitee-mint:~/git/kb/code-make/bitbake/tut1/build/tmp/work/first-0.1-r1/temp$ ll
# total 28K
# drwxr-xr-x 3 keitee keitee 4.0K Oct 25 15:05 ../
# -rwxrwxr-x 1 keitee keitee 1.6K Oct 25 15:05 run.do_build.20994*
# -rw-r--r-- 1 keitee keitee  123 Oct 25 15:05 log.do_build.20994
# -rwxrwxr-x 1 keitee keitee 1.6K Oct 25 15:08 run.do_build.21445*
# lrwxrwxrwx 1 keitee keitee   18 Oct 25 15:08 run.do_build -> run.do_build.21445*
# -rw-r--r-- 1 keitee keitee   74 Oct 25 15:08 log.task_order
# lrwxrwxrwx 1 keitee keitee   18 Oct 25 15:08 log.do_build -> log.do_build.21445
# -rw-r--r-- 1 keitee keitee  123 Oct 25 15:08 log.do_build.21445
# drwxr-xr-x 2 keitee keitee 4.0K Oct 25 15:08 ./
#
# keitee@keitee-mint:~/git/kb/code-make/bitbake/tut1/build/tmp/work/first-0.1-r1/temp$ cat log.do_build
# DEBUG: Executing shell function do_build
# first: some shell script running as build
# DEBUG: Shell function do_build finished

DESCRIPTION = "I am the first recipe"
PR = "r1"
do_build () {
  echo "first: some shell script running as build"
}
