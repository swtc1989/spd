 #!/bin/sh
killall qemu-system-i386 2>/dev/null; 
killall allinone 2>/dev/null;
killall sleep 2>/dev/null;
killall ter 2>/dev/null;

echo "begin exp"

~/Desktop/Resources/Runtime_database_controller/allinone 0 0 0 1 0 &
~/Desktop/Resources/Runtime_database_controller/ter &
cd ~/Desktop/Resources/SPD/build/qemu-release
sudo sh ~/Desktop/Resources/SPD/build/qemu-release/script_kill.sh &  
sudo ./i386-s2e-softmmu/qemu-system-i386 s2e_disk.raw.s2e -s2e-config-file simple.lua -loadvm issta-fault2-NSPR1  -s2e-verbose ;

#echo "finish $k th exp" 
killall allinone 2>/dev/null;
killall sleep 2>/dev/null;
killall ter 2>/dev/null;

sudo chmod 777 -R s2e-last 
cd -
rm last-run-info -rf 2>/dev/null
mv ~/Desktop/Resources/SPD/build/qemu-release/s2e-last last-run-info
