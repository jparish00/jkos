#!/bin/sh
set -e
. ./build.sh

mkdir -p isodir
mkdir -p isodir/boot
mkdir -p isodir/boot/grub

cp sysroot/boot/jkos.kernel isodir/boot/jkos.kernel
cat > isodir/boot/grub/grub.cfg << EOF
menuentry "jkos" {
	multiboot /boot/jkos.kernel
}
EOF
grub2-mkrescue -o jkos.iso isodir