#!/usr/bin/env bash

KERNEL=Pati21-Kernel-Snapdragon
KERNEL_FB=Pati21-Kernel-Snapdragon
INITRD=initramfs.cpio.gz
DISK=pati_disk.img
BOOTMODE=""
SQFS=""
MEM=1024

if [ "$1" = "-r" ]; then BOOTMODE=" bootmode=recovery"; fi
if [ "$1" = "-f" ]; then BOOTMODE=" bootmode=fastboot"; fi
if [ "$1" = "-sqfs" ]; then SQFS="1"; fi

if [ "$1" = "-g" ]; then
  DISPLAY_OPTS="-display sdl"
  APPEND_OPTS="rdinit=/init console=ttyAMA0 console=tty0 video=1024x768"
  GRAPHIC_DEVICE="-device virtio-gpu-pci -device qemu-xhci -device usb-tablet -device usb-kbd"
  KERNEL=$KERNEL_FB
  MEM=256
elif [ "$1" = "-v" ]; then
  DISPLAY_OPTS="-display vnc=:1"
  APPEND_OPTS="rdinit=/init console=tty0 video=1920x1080"
  GRAPHIC_DEVICE="-device virtio-gpu-pci -device qemu-xhci -device usb-tablet -device usb-kbd"
  KERNEL=$KERNEL_FB
  MEM=256
else
  DISPLAY_OPTS="-nographic"
  APPEND_OPTS="rdinit=/init console=ttyAMA0 net.ifnames=0$BOOTMODE"
  GRAPHIC_DEVICE=""
fi

SQFS_DRIVE=""
if [ -n "$SQFS" ]; then
  SQFS_DRIVE="-drive if=none,file=rootfs.squashfs,format=raw,id=sq0 -device virtio-blk-pci,drive=sq0"
fi

qemu-system-aarch64 -M virt -cpu cortex-a57 -m "$MEM" \
  -kernel "$KERNEL" \
  -initrd "$INITRD" \
  -drive if=none,file="$DISK",format=raw,id=hd0 \
  -device virtio-blk-pci,drive=hd0 \
  $SQFS_DRIVE \
  $GRAPHIC_DEVICE \
  -device virtio-net-pci,netdev=net0 \
  -netdev user,id=net0 \
  $DISPLAY_OPTS \
  -append "$APPEND_OPTS"
