#!/bin/bash
cd /home/cr_projects/librainman/rainman_board/build
rm *.jpg

i=$1
echo $i
if [ $i -eq 0 ]; then #face detection
    ps -aux|grep ./bin/insulator_demo| grep -v grep| awk '{print $2}' | xargs kill    
elif [ $i -eq 1 ]; then #truck detection
    ./bin/ssd_demo_thread_v2 -d ../data/ssd_6b_data_20_11 -s 192.168.123.8 -o out.bin -t 1
elif [ $i -eq 2 ]; then #truck detection
    ./bin/ssd_demo_v16 -d ../data/2018_03_5_data -s 192.168.123.8 -o out.bin -t 100000
elif [ $i -eq 3 ]; then #truck detection
    ./bin/insulator_demo -d ../data/insulator -s 192.168.123.8 -o out.bin -t 1
elif [ $i -eq 4 ]; then #truck detection
    ./bin/both_demo -d ../data/both -s 192.168.123.8 -o out.bin -t 1
else 
    echo "face detection: 1 ; truck detection: 2"
fi
