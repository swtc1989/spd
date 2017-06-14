 #!/bin/sh
date > log.txt
date > nofound.txt
sudo rm -r ../../hostfiles/mutants ;
sudo cp -r ../../hostfiles/mutants_$1 ../../hostfiles/mutants

for k in $(seq 15 15)
do
#if [ $k -eq 12 ] ; then echo "break $k" ; break
#fi
killall qemu-system-i386; 
killall allinone;
killall sleep;

sleep 1m;

echo "begin $k th exp" >>log.txt
date >> log.txt
cp ../../hostfiles/mutants/all$k.sh ../../hostfiles/all.sh
#~/allinone/allinone 0 0 0 0 0 &

sudo sh script_kill.sh 5m  qemu-system-i386 $k  &

sudo ./i386-s2e-softmmu/qemu-system-i386 s2e_disk.raw.s2e -s2e-config-file simple.lua -nographic -loadvm new_exp_ter -s2e-verbose 

echo "finish $k th exp" >>log.txt
date >> log.txt
done

killall allinone;
killall sleep;

