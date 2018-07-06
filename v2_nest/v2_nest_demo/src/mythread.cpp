#include "mythread.hpp"
#include "mainwindow.hpp"

void *thread_1(void *ptr)
{
  system("bash ../data/nest.sh 2");
}
