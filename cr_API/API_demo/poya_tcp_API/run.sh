#!/bin/bash
g++ -c ./cr_tcp/cr_tcp.cpp ./poyaData/poyaData.cpp

ar -cvr ./libpoya.a ./*.o

g++ ./demo/recv.cpp -L./ -lpoya -o recv
g++ ./demo/send.cpp -L./ -lpoya -o send

rm ./*.o ./libpoya.a 
