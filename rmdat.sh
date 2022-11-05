#!/bin/sh
# Usage:
# chmod 774 rmdat.sh
# ./rmdat.sh 001

rm dev_"$1".dat "$1".dat
rm dev_"$1".lst "$1".lst
rm dev_"$1".asm "$1".asm