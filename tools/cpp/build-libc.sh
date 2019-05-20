#!/bin/bash

set -e
set -u
set -o pipefail

curl -O 'http://gnu.askapache.com/libc/glibc-2.28.tar.xz'
tar xaf glibc-2.28.tar.xz
mkdir glibc-obj/
cd glibc-obj/
mkdir /tmp/glibc-2.28/
../glibc-2.28/configure --prefix=/tmp/glibc-2.28/
make -j"$(nproc)"
make install
tar -cvaf ../glibc-2.28-built.tar \
    --transform 's,^\.,./usr,' \
    --mtime=@0 \
    --owner=root \
    --group=root \
    -C /tmp/glibc-2.28/ \
    .
xz -9 ../glibc-2.28-built.tar
cd ../
