#!/bin/sh

./function_hook/scheduler $1  &
 sleep 0.01
 cd tftpd 
 LD_PRELOAD=./server_hook_for_netkit.so ./tftpd &
 cd ..
 sleep 0.01
 cd tftp
 LD_PRELOAD=./client_hook_for_netkit.so ./tftp localhost 6106 

