#!/bin/bash

set -e
set -u
set -o pipefail

curl -C - -O 'https://cdn.kernel.org/pub/linux/kernel/v3.x/linux-3.16.62.tar.xz'
tar xaf linux-3.16.62.tar.xz
mkdir linux-headers/
cd linux-headers/
make -C ../linux-3.16.62/ headers_install INSTALL_HDR_PATH="${PWD}"
tar -cvaf ../linux-headers-3.16.62.tar \
    --transform 's,^\.,./usr,' \
    --mtime=@0 \
    --owner=root \
    --group=root \
    .
xz -9 ../linux-headers-3.16.62.tar
cd ../
