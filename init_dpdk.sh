#!/bin/sh
#mkdir -p /mnt/huge
#(mount | grep -q /mnt/huge) || mount -t hugetlbfs nodev /mnt/huge

modprobe vfio-pci
echo 1 >/sys/module/vfio/parameters/enable_unsafe_noiommu_mode

if (ifconfig -a | grep -q ens6)
then
  ifconfig ens6 down
  dpdk-devbind.py -b vfio-pci 0000:00:06.0
fi

for n in /sys/devices/system/node/node* ; do
  echo 1024 >$n/hugepages/hugepages-2048kB/nr_hugepages
done
