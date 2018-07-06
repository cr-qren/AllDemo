#include "mainwindow.hpp"
#include "mythreads.hpp"
#include "poya.hpp"
#include <thread>

MainWindow::MainWindow()
{
  main_layout = new QVBoxLayout();
  main_cntl_layout1 = new QHBoxLayout();
  main_cntl_layout2 = new QHBoxLayout();

  disp_label = new QLabel("display");
  disp_label->setMinimumSize(PICTURE_WIDTH*0.5,PICTURE_HEIGHT*0.5);
  disp_label->setMaximumSize(PICTURE_WIDTH*0.5,PICTURE_HEIGHT*0.5);

  select_list = new QComboBox();
  select_list->addItem(tr("mode 1:face"));
  select_list->addItem(tr("mode 2:car"));
  select_list->addItem(tr("mode 3:insulator"));
  select_list->addItem(tr("mode 4:both"));
  select_list->addItem(tr("mode 5:___"));
  select_list->addItem(tr("mode 6:___"));
  select_list->addItem(tr("mode 7:test"));
  select_list->setMinimumSize(PICTURE_WIDTH*0.15,PICTURE_HEIGHT*0.02);
  select_list->setMaximumSize(PICTURE_WIDTH*0.15,PICTURE_HEIGHT*0.02);

  camera_start_button = new QPushButton("camera_start");
  QObject::connect(camera_start_button, SIGNAL(clicked()), this, SLOT(camera_start()));
  camera_start_button->setMinimumSize(PICTURE_WIDTH*0.05,PICTURE_HEIGHT*0.02);
  camera_start_button->setMaximumSize(PICTURE_WIDTH*0.05,PICTURE_HEIGHT*0.02);

  board_start_button = new QPushButton("board_start");
  QObject::connect(board_start_button, SIGNAL(clicked()), this, SLOT(board_start()));
  board_start_button->setMinimumSize(PICTURE_WIDTH*0.05,PICTURE_HEIGHT*0.02);
  board_start_button->setMaximumSize(PICTURE_WIDTH*0.05,PICTURE_HEIGHT*0.02);

  board_stop_button = new QPushButton("board_stop");
  QObject::connect(board_stop_button, SIGNAL(clicked()), this, SLOT(board_stop()));
  board_stop_button->setMinimumSize(PICTURE_WIDTH*0.05,PICTURE_HEIGHT*0.02);
  board_stop_button->setMaximumSize(PICTURE_WIDTH*0.05,PICTURE_HEIGHT*0.02);

  close_all_button = new QPushButton("close");
  QObject::connect(close_all_button, SIGNAL(clicked()), this, SLOT(close()));
  close_all_button->setMinimumSize(PICTURE_WIDTH*0.05,PICTURE_HEIGHT*0.02);
  close_all_button->setMaximumSize(PICTURE_WIDTH*0.05,PICTURE_HEIGHT*0.02);

  main_cntl_layout1->addWidget(disp_label);

  main_cntl_layout2->addWidget(camera_start_button);
  main_cntl_layout2->addWidget(select_list);
  main_cntl_layout2->addWidget(board_start_button);
  main_cntl_layout2->addWidget(board_stop_button);
  main_cntl_layout2->addWidget(close_all_button);

  main_layout->addLayout(main_cntl_layout1);
  main_layout->addLayout(main_cntl_layout2);

  cam_tcp = new TcpSocket("192.168.1.168", 31003, false);
  board_tcp = new TcpSocket("192.168.123.3", 8000, true);

  setLayout(main_layout);

  start_flag = false;
  close_flag = false;
  board_start_flag = false;
  board_stop_flag = false;
  show_flag = false;

  img_index = 0;
}

MainWindow::~MainWindow()
{

}

void MainWindow::camera_start()
{
  start_flag = true;
  close_flag = false;
  pthread_create(&thread1, NULL, thread_1, this);
  pthread_detach(thread1);
}

void MainWindow::board_start()
{
  board_start_flag = true;
  board_stop_flag = false;
  pthread_create(&thread2, NULL, thread_2, this);
  pthread_detach(thread2);
}

void MainWindow::board_stop()
{
  board_stop_flag = true;
  board_start_flag = false;
  pthread_create(&thread3, NULL, thread_3, this);
  pthread_detach(thread3);
}

