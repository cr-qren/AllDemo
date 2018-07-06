#ifndef MAINWINDOW_HPP
#define MAINWINDOW_HPP

#include <pthread.h>
#include <string.h>
#include <unistd.h>

#include <vector>

#include <QMainWindow>
#include <QApplication>
#include <QLabel>
#include <QPushButton>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QMutex>

#include "mythreads.hpp"
#include "tcprasp.hpp"

#define THREAD_NUM 2

#define PICTURE_WIDTH 1920
#define PICTURE_HEIGHT 1080

#define SRC_PIC_1_DIR "../data/pic1"
#define SRC_PIC_2_DIR "../data/pic2"
#define SRC_PIC_3_DIR "../data/pic3"
#define SRC_PIC_4_DIR "../data/pic4"

#define FILE_NAME_SIZE 512

using namespace std;

class MainWindow : public QWidget
{
  Q_OBJECT
public:
  MainWindow();
  ~MainWindow();

  QMutex mutex_dummy;
  QMutex mutex_pic;
  QMutex mutex_board;

  pthread_t thread1;
  pthread_t thread2;
  pthread_t thread3;
  pthread_t thread4;
  pthread_t thread5;

public slots:
  void start_run();
  void stop_run();
  void board_start();
  void board_stop();

  void change_dir_1();
  void change_dir_2();
  void change_dir_3();
  void change_dir_4();
public:
  QHBoxLayout *disp_layout;
  QVBoxLayout *list_layout;
  QHBoxLayout *main_cntl_layout;
  QVBoxLayout *main_layout;

  QLabel *disp_label;
  QLabel *src_pic_1_label;
  QLabel *src_pic_2_label;
  QLabel *src_pic_3_label;
  QLabel *src_pic_4_label;

  QPushButton *change_dir_1_button;
  QPushButton *change_dir_2_button;
  QPushButton *change_dir_3_button;
  QPushButton *change_dir_4_button;

  QPushButton *stop_button;
  QPushButton *start_button;
  QPushButton *close_button;
  QPushButton *board_start_button;
  QPushButton *board_stop_button;

  vector<QImage> pic_dir_1;
  vector<QImage> pic_dir_2;
  vector<QImage> pic_dir_3;
  vector<QImage> pic_dir_4;

  vector<string> pub_list;
  vector<string> pic_1_list;
  vector<string> pic_2_list;
  vector<string> pic_3_list;
  vector<string> pic_4_list;

public:
  TcpRasp *tcprasp;
  bool change_dir_1_button_push;
  bool change_dir_2_button_push;
  bool change_dir_3_button_push;
  bool change_dir_4_button_push;
  bool start_flag;
  bool board_flag;
  char *pic_dir;
  int pic_num;
};

#endif
