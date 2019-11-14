#!/bin/sh
#mkdir -p /mnt/huge
#(mount | grep -q /mnt/huge) || mount -t hugetlbfs nodev /mnt/huge

. common.sh

modprobe vfio-pci
echo 1 >/sys/module/vfio/parameters/enable_unsafe_noiommu_mode

if (ifconfig -a | grep -q $ETHDEV)
then
  ifconfig $ETHDEV down
  dpdk-devbind.py -b vfio-pci $PCIDEV
fi

for n in /sys/devices/system/node/node* ; do
  echo 1024 >$n/hugepages/hugepages-2048kB/nr_hugepages
done
