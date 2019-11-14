#!/bin/sh

. common.sh

if [ -z "$1" ]
then
  echo "Usage: init_linux.sh IP"
  exit 1
fi

echo $ETHDEV
exit 0

if (ifconfig -a | grep -q $ETHDEV)
then
  echo
else
  dpdk-devbind.py -b $DRIVER $PCIDEV
fi

sleep 1

ifconfig $ETHDEV up $1
