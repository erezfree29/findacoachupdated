TARGETS = console-setup mountkernfs.sh hostname.sh resolvconf udev networking urandom hwclock.sh mountdevsubfs.sh checkroot.sh mountall.sh checkfs.sh checkroot-bootclean.sh bootmisc.sh mountall-bootclean.sh kmod mountnfs-bootclean.sh mountnfs.sh procps
INTERACTIVE = console-setup udev checkroot.sh checkfs.sh
udev: mountkernfs.sh
networking: mountkernfs.sh urandom procps resolvconf
urandom: hwclock.sh
hwclock.sh: mountdevsubfs.sh
mountdevsubfs.sh: mountkernfs.sh udev
checkroot.sh: hwclock.sh mountdevsubfs.sh hostname.sh
mountall.sh: checkfs.sh checkroot-bootclean.sh
checkfs.sh: checkroot.sh
checkroot-bootclean.sh: checkroot.sh
bootmisc.sh: udev mountall-bootclean.sh mountnfs-bootclean.sh checkroot-bootclean.sh
mountall-bootclean.sh: mountall.sh
kmod: checkroot.sh
mountnfs-bootclean.sh: mountnfs.sh
mountnfs.sh: networking
procps: mountkernfs.sh udev
