#!/bin/sh
# Usage:
# chmod 774 cpdat.sh
# ./cpdat.sh 001

if [[ $1 -lt 100 ]]
then
   scp jcaldwell2@csunix.angelo.edu:/usr/local/4301/data/stage0/"$1".dat .
   scp jcaldwell2@csunix.angelo.edu:/usr/local/4301/data/stage0/"$1".lst .
   scp jcaldwell2@csunix.angelo.edu:/usr/local/4301/data/stage0/"$1".asm .
elif [[ $1 -gt 100 ]] && [[ $1 -lt 200 ]]
then
   scp jcaldwell2@csunix.angelo.edu:/usr/local/4301/data/stage1/"$1".dat .
   scp jcaldwell2@csunix.angelo.edu:/usr/local/4301/data/stage1/"$1".lst .
   scp jcaldwell2@csunix.angelo.edu:/usr/local/4301/data/stage1/"$1".asm .
else
   scp jcaldwell2@csunix.angelo.edu:/usr/local/4301/data/stage1/"$1".dat .
   scp jcaldwell2@csunix.angelo.edu:/usr/local/4301/data/stage1/"$1".lst .
   scp jcaldwell2@csunix.angelo.edu:/usr/local/4301/data/stage1/"$1".asm .
fi