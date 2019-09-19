#!/bin/sh

modprobe vfio-pci
modprobe rte_kni
echo 1 >/sys/module/vfio/parameters/enable_unsafe_noiommu_mode

if (ifconfig -a | grep -q $TAS_INTF_NAME)
then
  ifconfig $TAS_INTF_NAME down
  dpdk-devbind.py -b vfio-pci $TAS_INTF_PCI
fi

for n in /sys/devices/system/node/node* ; do
  echo 1024 >$n/hugepages/hugepages-2048kB/nr_hugepages
done


exec /usr/lib/tas-wrapper/wrapper \
	tas \
	--kni-name=$TAS_INTF_NAME $TAS_PARAMS_EXTRA
