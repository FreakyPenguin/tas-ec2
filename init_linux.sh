#!/bin/sh

if [ -z "$1" ]
then
  echo "Usage: init_linux.sh IP"
  exit 1
fi


if (ifconfig -a | grep -q ens6)
then
  echo
else
  dpdk-devbind.py -b ena 0000:00:06.0
fi
ifconfig ens6 up $1
