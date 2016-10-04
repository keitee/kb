#!/bin/bash
#set -x
#
# $0 [-f] 
#

#MATCHFULL=0
#
#if [ "$1" = "-f" ]; then
#	MATCHFULL=1
#	shift
#fi

BASE=$(readlink -f `dirname $0`)
ERRDICT=$BASE/nmerr.dic
MILDICT=$BASE/nmmil.dic
MATDICT=$BASE/nmmat.dic 

# =============================================================
echo "nm:--------------------------"
echo "nm: $BASE"

if [ ! -f $ERRDICT -a ! -f $MILDICT ]; then
	echo "nm: No $ERRDICT and $MILDICT file !"
	echo 0 0
	exit 0
fi

if [ ! -f LOG -a ! -f LOGlast -a ! -f LOGflash -a ! -f LOGlastrun_realtime ]; then
	echo "nm: No LOG file!"
	echo 0 0
	exit 0
fi

if [ -f LOG ]; then
	LOGFILE=LOG
fi

if [ -f LOGlastrun_realtime ]; then
	LOGFILE=LOGlastrun_realtime
	touch .translated
fi

if [ -f LOGflash ]; then
	LOGFILE=LOGflash
fi

if [ -f LOGflash ]; then
	LOGFILE=LOGflash
fi

if [ ! -f .translated ]; then
	to_dates $LOGFILE
	touch .translated
	echo "nm: translate $LOGFILE..."
fi

echo "nm: $LOGFILE is used..."
	
# =============================================================
# extract only error and fatals. additionaly warnings
# $LOGFILE > .nm.raw & .nm.raw.full
#
NMRAWOUT=.nm.raw
NMFULOUT=.nm.raw.full
egrep -an "(\!ERROR|\!FATAL)" $LOGFILE > $NMRAWOUT
egrep -an "(\!ERROR|\!FATAL|\!WARN)" $LOGFILE > $NMFULOUT

# =============================================================
# print out already known errors from the error dic and merge them into
# $NMFULOUT > nm_known.nds
#
echo "nm: running (nmprn.py $ERRDICT $NMFULOUT)"
echo "nm:--------------------------"
nmprn.py $ERRDICT $NMFULOUT | sort -nu | tee nm_kwn.nds
echo "nm:--------------------------"

# remove epg errors from interesting errors
NMMWOUT=.nm.mw
egrep -v "(-EPG)" $NMRAWOUT > $NMMWOUT
cp $NMMWOUT nm_mwb.nds


# =============================================================
# run the main dic on mw errors. nmmacth modifies the input file.
#
echo "nm: running (nmmatch.py $MATDICT $NMMWOUT)"
nmmatch.py $MATDICT $NMMWOUT

# tidy up. 
egrep -v "^\+" $NMMWOUT > nm_mwa.nds


# =============================================================
# merge interesting errors and milestones. note that the mil dic includes warnings.
# input : LOGFILE > nm_out.nds
#
NMMILOUT=.nm.mil
egrep -an "(\!MIL|\!WARN)" $LOGFILE > .nm.mil

echo "nm: running (nmprn.py $MILDICT .nm.mil)"
nmprn.py $MILDICT .nm.mil > nm_mil.nds
#nmprn.py $MILDICT .nm.mil | egrep -v "RECORD_STATE_UNDEF|EREX" | sort -n > nm_mil.nds

# merge together
sort -un nm_mwa.nds nm_mil.nds nm_kwn.nds > nm_out.nds


# =============================================================
# add temp bit to the nm_out.nds
#
egrep -an "(SI-6113)" $LOGFILE > .mm.tmp
sort -nu .mm.tmp nm_out.nds > nm_tmp.nds

# =============================================================
# tidy up
#rm err_m.tmp err_m_mw.tmp
#
#sort -n err_m_kwn.tmp > err_m_kwn.nds
#rm err_m_kwn.tmp

echo "nm_mil.nds      : shows only intersting mils"
echo "nm_kwn.nds      : shows only knowns"
echo "nm_mwb/a.nds : shows only interesting mw errors before/after matching"
echo "nm_out.nds      : shows only interesting mw errors and milestones"

ls -al *.nds
echo "--------------------------"
