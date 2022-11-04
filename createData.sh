#/bin/bash
make stage0

rm -r outputs
mkdir outputs
for i in {1..9}
do
   echo ${i} ha ha ha
	touch m${i}.lst m${i}.asm
	./stage0 00${i}.dat m${i}.lst m${i}.asm
   
   
   if test -f 00${i}.lst
   then
      diff m${i}.lst 00${i}.lst > ld${i}.out
      mv ld${i}.out outputs
      cp 00${i}.lst outputs
   fi
   
   if test -f 00${i}.asm
   then
      diff m${i}.asm 00${i}.asm > ad${i}.out
      mv ad${i}.out outputs
      cp 00${i}.asm outputs
   fi
   
	mv m${i}.lst m${i}.asm outputs
	cp 00${i}.dat outputs
	cd outputs
	mkdir 0${i}
	mv 00${i}.dat m${i}.lst m${i}.asm 0${i} 
   
   if test -f 00${i}.lst
   then
      mv ld${i}.out 00${i}.lst 0${i} 
   fi
   
   if test -f 00${i}.asm
   then
      mv ad${i}.out 00${i}.asm 0${i} 
   fi
   
	cd ..
    
done

for i in {10..52}
do
   echo ${i} ha ha ha
	touch m${i}.lst m${i}.asm
	./stage0 0${i}.dat m${i}.lst m${i}.asm
   
   if test -f 0${i}.lst
   then
      diff m${i}.lst 0${i}.lst > ld${i}.out
      mv ld${i}.out outputs
      cp 0${i}.lst outputs
   fi
   
   if test -f 0${i}.asm
   then
      diff m${i}.asm 0${i}.asm > ad${i}.out
      mv ad${i}.out outputs
      cp 0${i}.asm outputs
   fi
   
	mv m${i}.lst m${i}.asm outputs
	cp 0${i}.dat outputs
	cd outputs
	mkdir ${i}
	mv 0${i}.dat m${i}.lst m${i}.asm ${i}
   
   if test -f 0${i}.lst
   then
      mv ld${i}.out 0${i}.lst ${i} 
   fi
   
   if test -f 0${i}.asm
   then
      mv ad${i}.out 0${i}.asm ${i} 
   fi
   
	cd ..
done


