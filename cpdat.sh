#!/bin/sh
# Usage:
# chmod 774 cpdat.sh
# ./cpdat.sh 001


cp /usr/local/4301/data/stage0/"$1".dat .
cp /usr/local/4301/data/stage0/"$1".lst .
cp /usr/local/4301/data/stage0/"$1".asm .