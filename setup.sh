#/bin/bash
rm -r data
mkdir data
cd data
mkdir outputs

num=1

cp /usr/local/4301/src/Makefile .
cp /usr/local/4301/include/stage0.h .
cp /usr/local/4301/src/stage0main.C .

for i in {1..9}
do

   cp /usr/local/4301/data/stage0/00${i}.dat .
   cp /usr/local/4301/data/stage0/00${i}.lst .
   cp /usr/local/4301/data/stage0/00${i}.asm .
   
done

for i in {10..52}
do
   
   cp /usr/local/4301/data/stage0/0${i}.dat .
   cp /usr/local/4301/data/stage0/0${i}.lst .
   cp /usr/local/4301/data/stage0/0${i}.asm .
   
done

clear
ls