#!/bin/bash
gcc -c ./serial/serial.c ./poyaData/poyaData.c 

ar -cvr ./libpoya.a ./*.o
gcc ./demo/mix.c -L./ -lpoya -o mix
gcc ./demo/recv.c -L./ -lpoya -o recv
gcc ./demo/send.c -L./ -lpoya -o send

rm ./*.o ./libpoya.a 
