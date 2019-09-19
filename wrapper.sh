#!/bin/sh
INTF=ens6
PCI=0000:00:06.0

modprobe vfio-pci
modprobe rte_kni
echo 1 >/sys/module/vfio/parameters/enable_unsafe_noiommu_mode

if (ifconfig -a | grep -q $INTF)
then
  ifconfig $INTF down
  dpdk-devbind.py -b vfio-pci $PCI
fi

for n in /sys/devices/system/node/node* ; do
  echo 1024 >$n/hugepages/hugepages-2048kB/nr_hugepages
done


exec /home/ubuntu/tas/ec2/linux-wrapper/wrapper \
	/home/ubuntu/tas/code/tas/tas \
	--kni-name=$INTF --ip-addr=10.0.0.32/24
