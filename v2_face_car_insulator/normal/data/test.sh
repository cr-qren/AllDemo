#!/bin/bash
i=$1
echo $i
if [ $i -eq 0 ]; then #stop
  sshpass -p "letmein" ssh root@192.168.123.8 'bash all_in_one_demo.sh 0'
elif [ $i -eq 1 ]; then #face detection
  sshpass -p "letmein" ssh root@192.168.123.8 'bash all_in_one_demo.sh 1'
elif [ $i -eq 2 ]; then #truck detection
  sshpass -p "letmein" ssh root@192.168.123.8 'bash all_in_one_demo.sh 2'
elif [ $i -eq 3 ]; then #insulator detection
  sshpass -p "letmein" ssh root@192.168.123.8 'bash all_in_one_demo.sh 3'
else
  echo "Corerain"
fi

