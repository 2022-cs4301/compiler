#/bin/sh
# Usage:
# chmod 774 runc.sh
# ./runc.sh 001

make clean
make stage0
./stage0 "$1".dat dev_"$1".lst dev_"$1".asm