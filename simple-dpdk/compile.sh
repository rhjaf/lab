#!/bin/sh
. ./setup.sh

make O=$RTE_TARGET $*
echo ===
ls -la $RTE_TARGET/simple
echo ===
ldd $RTE_TARGET/simple