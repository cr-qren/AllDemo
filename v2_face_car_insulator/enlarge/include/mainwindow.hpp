#ifndef MAINWINDOW_HPP
#define MAINWINDOW_HPP

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <pthread.h>

#include <QMainWindow>
#include <QApplication>
#include <QPushButton>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QComboBox>
#include <QMutex>
#include <QLabel>
#include <QPixmap>
#include <QPen>
#include <QPainter>

#include "tcpsocket.hpp"

#define THREAD_NUM 2

#define PICTURE_WIDTH 3840  
#define PICTURE_HEIGHT 1728

#define FILE_NAME_SIZE 512


using namespace std;

typedef struct IMAGES_BUFFER_TYPE
{
  unsigned long size;
  unsigned char buffer[400000];
}images_buffer_t;

class MainWindow : public QWidget
{
  Q_OBJECT
public:
  MainWindow();
  ~MainWindow();

  QMutex mutex_dummy;
  QMutex mutex_dummy_1;
  pthread_t thread1;
  pthread_t thread2;
  pthread_t thread3;
  pthread_t thread4;
  pthread_t thread5;
  pthread_t thread6;

public slots:
  void camera_start();
  void board_start();
  void board_stop();

public:
  QVBoxLayout *main_layout;
  QHBoxLayout *main_cntl_layout1;
  QHBoxLayout *main_cntl_layout2;

  QLabel      *disp_label;

  QComboBox   *select_list;

  QPushButton *camera_start_button;
  QPushButton *board_start_button;
  QPushButton *board_stop_button;
  QPushButton *close_all_button;

public:
  bool start_flag;
  bool close_flag;
  bool board_start_flag;
  bool board_stop_flag;
  bool show_flag;

  char *select_model;
  TcpSocket *cam_tcp;
  TcpSocket *board_tcp;
  int img_index;
  images_buffer_t imgs[20];
  vector<float> rect;
};

#endif
