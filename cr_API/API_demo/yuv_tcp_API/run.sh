#!/bin/bash
g++ -c ./cr_tcp/cr_tcp.cpp ./yuvData/YUV_transer.cpp

ar -cvr ./libyuv.a ./*.o

g++ ./demo/recv.cpp -L./ -lyuv -o recv
g++ ./demo/send.cpp -L./ -lyuv -o send

rm ./*.o ./libyuv.a
