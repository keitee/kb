#!/bin/bash -x
if [ $# -lt 1 ]
then
  echo "Usuage: $0 <ip>"
  exit 1
fi

ip=10.148.104.93

echo $ip

# mount to to kit-debian
# mount -o rw -t nfs 10.209.56.103:/home/kyoupark/si_logs /mnt/tmp
DIR=/home/kyoupark/si_logs/glog

mkdir $DIR
chmod 777 $DIR
touch $DIR/.mounted 
cd $DIR
MOUNTDIR=10.209.56.103:$DIR
LOCALDIR=/tmp/gl$$

ping -c 2 $ip &&
(
sleep 5
echo "root"
sleep 5
echo "mkdir $LOCALDIR"
echo "echo mounting xxx"
echo "mount -o rw -t nfs $MOUNTDIR $LOCALDIR"
echo "if [ ! -f $LOCALDIR/.mounted ]"
echo 'then'
echo '  echo "FAILED TO MOUNT"'
echo '  exit'
echo 'fi'
echo "ls -al $LOCALDIR"
echo "ps -a > $LOCALDIR/ps.out"
echo "cp /mnt/nds/dev_7/part_0/FSN_DATA/LOG $LOCALDIR"
echo "dd if=/mnt/nds/dev_7/part_0/FSN_DATA/LOG of=$LOCALDIR/LOG bs=1024"
echo "umount $LOCALDIR"
) | telnet $ip

