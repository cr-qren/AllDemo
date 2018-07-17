#!/bin/bash
g++ -c ./cr_tcp_one2n/cr_tcp.cpp ./yuvData/yuv_data.cpp

ar -cvr ./libyuv.a ./*.o

g++ ./demo/recv.cpp -L./ -lyuv -o recv
g++ ./demo/send.cpp -L./ -lyuv -o send

rm ./*.o ./libyuv.a
