#!/bin/sh

if [ -z "$1" ]
then
  echo "Usage: init_linux.sh IP"
  exit 1
fi

ifconfig ens6 up $1

if (ifconfig -a | grep -q ens6)
then
  ~/dpdk-inst/sbin/dpdk-devbind -b ena 0000:00:06.0
else
  echo
fi
