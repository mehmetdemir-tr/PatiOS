KERNEL=Pati21-Kernel-Snapdragon
INITRD=initramfs.cpio.gz
DISK=pati_disk.img
BOOTMODE=""
if [ "$1" = "-r" ]; then BOOTMODE=" bootmode=recovery"; fi
if [ "$1" = "-f" ]; then BOOTMODE=" bootmode=fastboot"; fi
if [ "$1" = "-v" ]; then
  DISPLAY_OPTS="-display vnc=:1"
  APPEND_OPTS="rdinit=/init console=tty0 video=1920x1080"
  GRAPHIC_DEVICE="-device virtio-gpu-pci"
  KERNEL=$KERNEL
  MEM=256
  echo "[BOOTLOADER] VNC localhost:5901 (:1)"
else
  DISPLAY_OPTS="-nographic"
  APPEND_OPTS="rdinit=/init console=ttyAMA0 net.ifnames=0$BOOTMODE"
  GRAPHIC_DEVICE=""
  MEM=1024
fi
qemu-system-aarch64 -M virt -cpu cortex-a57 -m $MEM \
  -kernel $KERNEL \
  -initrd $INITRD \
  -drive if=none,file=$DISK,format=raw,id=hd0 \
  -device virtio-blk-pci,drive=hd0 \
  $GRAPHIC_DEVICE \
  -device virtio-net-pci,netdev=net0 \
  -netdev user,id=net0 \
  $DISPLAY_OPTS \
  -append "$APPEND_OPTS"
