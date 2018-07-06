#include "mainwindow.hpp"

MainWindow::MainWindow()
{
  disp_layout = new QHBoxLayout();
  list_layout = new QVBoxLayout();
  main_cntl_layout = new QHBoxLayout();
  main_layout = new QVBoxLayout();

  disp_label = new QLabel("display");
  disp_label->setMinimumSize(PICTURE_WIDTH*0.5,PICTURE_HEIGHT*0.5);
  disp_label->setMaximumSize(PICTURE_WIDTH*0.5,PICTURE_HEIGHT*0.5);

  src_pic_1_label = new QLabel("dir_1");
  src_pic_1_label->setMinimumSize(66, 66);
  src_pic_1_label->setMaximumSize(66, 66);

  src_pic_2_label = new QLabel("dir_2");
  src_pic_2_label->setMinimumSize(66, 66);
  src_pic_2_label->setMaximumSize(66, 66);

  src_pic_3_label = new QLabel("dir_3");
  src_pic_3_label->setMinimumSize(66, 66);
  src_pic_3_label->setMaximumSize(66, 66);

  src_pic_4_label = new QLabel("dir_4");
  src_pic_4_label->setMinimumSize(66, 66);
  src_pic_4_label->setMaximumSize(66, 66);

  change_dir_1_button = new QPushButton("dir_1");
  QObject::connect(change_dir_1_button, SIGNAL(clicked()), this, SLOT(change_dir_1()));
  change_dir_1_button->setMinimumSize(PICTURE_WIDTH*0.03,PICTURE_HEIGHT*0.02);
  change_dir_1_button->setMaximumSize(PICTURE_WIDTH*0.03,PICTURE_HEIGHT*0.02);

  change_dir_2_button = new QPushButton("dir_2");
  QObject::connect(change_dir_2_button, SIGNAL(clicked()), this, SLOT(change_dir_2()));
  change_dir_2_button->setMinimumSize(PICTURE_WIDTH*0.03,PICTURE_HEIGHT*0.02);
  change_dir_2_button->setMaximumSize(PICTURE_WIDTH*0.03,PICTURE_HEIGHT*0.02);

  change_dir_3_button = new QPushButton("dir_3");
  QObject::connect(change_dir_3_button, SIGNAL(clicked()), this, SLOT(change_dir_3()));
  change_dir_3_button->setMinimumSize(PICTURE_WIDTH*0.03,PICTURE_HEIGHT*0.02);
  change_dir_3_button->setMaximumSize(PICTURE_WIDTH*0.03,PICTURE_HEIGHT*0.02);

  change_dir_4_button = new QPushButton("dir_4");
  QObject::connect(change_dir_4_button, SIGNAL(clicked()), this, SLOT(change_dir_4()));
  change_dir_4_button->setMinimumSize(PICTURE_WIDTH*0.03,PICTURE_HEIGHT*0.02);
  change_dir_4_button->setMaximumSize(PICTURE_WIDTH*0.03,PICTURE_HEIGHT*0.02);

  stop_button = new QPushButton("stop");
  QObject::connect(stop_button, SIGNAL(clicked()), this, SLOT(stop_run()));
  stop_button->setMinimumSize(PICTURE_WIDTH*0.05,PICTURE_HEIGHT*0.02);
  stop_button->setMaximumSize(PICTURE_WIDTH*0.05,PICTURE_HEIGHT*0.02);

  start_button = new QPushButton("start");
  QObject::connect(start_button, SIGNAL(clicked()), this, SLOT(start_run()));
  start_button->setMinimumSize(PICTURE_WIDTH*0.05,PICTURE_HEIGHT*0.02);
  start_button->setMaximumSize(PICTURE_WIDTH*0.05,PICTURE_HEIGHT*0.02);

  close_button = new QPushButton("close");
  QObject::connect(close_button, SIGNAL(clicked()), this, SLOT(close()));
  close_button->setMinimumSize(PICTURE_WIDTH*0.05,PICTURE_HEIGHT*0.02);
  close_button->setMaximumSize(PICTURE_WIDTH*0.05,PICTURE_HEIGHT*0.02);

  board_start_button = new QPushButton("board_start");
  QObject::connect(board_start_button, SIGNAL(clicked()), this, SLOT(board_start()));
  board_start_button->setMinimumSize(PICTURE_WIDTH*0.05,PICTURE_HEIGHT*0.02);
  board_start_button->setMaximumSize(PICTURE_WIDTH*0.05,PICTURE_HEIGHT*0.02);

  board_stop_button = new QPushButton("board_stop");
  QObject::connect(board_stop_button, SIGNAL(clicked()), this, SLOT(board_stop()));
  board_stop_button->setMinimumSize(PICTURE_WIDTH*0.05,PICTURE_HEIGHT*0.02);
  board_stop_button->setMaximumSize(PICTURE_WIDTH*0.05,PICTURE_HEIGHT*0.02);

  this->tcprasp = new TcpRasp("192.168.123.8", 8000);
  this->start_flag = false;
  this->board_flag = true;
  this->change_dir_1_button_push = false;
  this->change_dir_2_button_push = false;
  this->change_dir_3_button_push = false;
  this->change_dir_4_button_push = false;

  this->pic_dir = new char[FILE_NAME_SIZE];
  strcpy(this->pic_dir, "../data/pic");

  this->pic_num = 0;

  this->pic_1_list = pic_list(SRC_PIC_1_DIR);
  this->pic_2_list = pic_list(SRC_PIC_2_DIR);
  this->pic_3_list = pic_list(SRC_PIC_3_DIR);
  this->pic_4_list = pic_list(SRC_PIC_4_DIR);

  for(int i = 0; i < 10; i++)
  {
    char file_1[FILE_NAME_SIZE];
    char file_2[FILE_NAME_SIZE];
    char file_3[FILE_NAME_SIZE];
    char file_4[FILE_NAME_SIZE];
    sprintf(file_1, "%s/%s", SRC_PIC_1_DIR, this->pic_1_list[i].c_str());
    sprintf(file_2, "%s/%s", SRC_PIC_2_DIR, this->pic_2_list[i].c_str());
    sprintf(file_3, "%s/%s", SRC_PIC_3_DIR, this->pic_3_list[i].c_str());
    sprintf(file_4, "%s/%s", SRC_PIC_4_DIR, this->pic_4_list[i].c_str());

    QImage img1(file_1, "JPEG");
    QImage img2(file_2, "JPEG");
    QImage img3(file_3, "JPEG");
    QImage img4(file_4, "JPEG");

    this->pic_dir_1.push_back(img1);
    this->pic_dir_2.push_back(img2);
    this->pic_dir_3.push_back(img3);
    this->pic_dir_4.push_back(img4);
  }

  list_layout->addWidget(src_pic_1_label);
  list_layout->addWidget(change_dir_1_button);
  list_layout->addWidget(src_pic_2_label);
  list_layout->addWidget(change_dir_2_button);
  list_layout->addWidget(src_pic_3_label);
  list_layout->addWidget(change_dir_3_button);
  list_layout->addWidget(src_pic_4_label);
  list_layout->addWidget(change_dir_4_button);

  disp_layout->addWidget(disp_label);
  disp_layout->addLayout(list_layout);

  main_cntl_layout->addWidget(stop_button);
  main_cntl_layout->addWidget(start_button);
  main_cntl_layout->addWidget(close_button);
  main_cntl_layout->addWidget(board_start_button);
  main_cntl_layout->addWidget(board_stop_button);

  main_layout->addLayout(disp_layout);
  main_layout->addLayout(main_cntl_layout);

  setLayout(main_layout);

  pthread_create(&thread2, NULL, thread_2, this);
  pthread_detach(thread2);
 

#if 0
  pthread_create(&thread4, NULL, thread_4, this);
  pthread_detach(thread4);

  pthread_create(&thread5, NULL, thread_5, this);
  pthread_detach(thread5);
#endif
}

MainWindow::~MainWindow()
{
#if 0
  delete []pic_dir;
  delete close_button;
  delete start_button;
  delete stop_button;

  delete change_dir_4_button;
  delete change_dir_3_button;
  delete change_dir_2_button;
  delete change_dir_1_button;

  delete src_pic_4_label;
  delete src_pic_3_label;
  delete src_pic_2_label;
  delete src_pic_1_label;
  delete disp_label;

  delete main_layout;
  delete main_cntl_layout;
  delete list_layout;
  delete disp_layout;
#endif
}


void MainWindow::start_run()
{

  if(!this->start_flag)
  {
    pthread_create(&thread1, NULL, thread_1, this);
    pthread_detach(thread1);
  }
  this->start_flag = true;
}

void MainWindow::stop_run()
{
  this->start_flag = false;
}

void MainWindow::board_start()
{
  this->board_flag = true;
  cout << "Board flag has been set: TRUE" << endl;
}

void MainWindow::board_stop()
{
  this->board_flag = false;
  cout << "Board flag has been set FALSE" << endl;
}

void MainWindow::change_dir_1()
{
  this->mutex_dummy.lock();
  bzero(this->pic_dir,FILE_NAME_SIZE);
  strcpy(this->pic_dir, SRC_PIC_1_DIR);
  this->pub_list = this->pic_1_list;
  this->mutex_dummy.unlock();

  this->change_dir_1_button_push = true;
  this->change_dir_2_button_push = false;
  this->change_dir_3_button_push = false;
  this->change_dir_4_button_push = false;
}
void MainWindow::change_dir_2()
{
  this->mutex_dummy.lock();
  bzero(this->pic_dir,FILE_NAME_SIZE);
  strcpy(this->pic_dir, SRC_PIC_2_DIR);
  this->pub_list = this->pic_2_list;
  this->mutex_dummy.unlock();

  this->change_dir_1_button_push = false;
  this->change_dir_2_button_push = true;
  this->change_dir_3_button_push = false;
  this->change_dir_4_button_push = false;
}
void MainWindow::change_dir_3()
{
  this->mutex_dummy.lock();
  bzero(this->pic_dir,FILE_NAME_SIZE);
  strcpy(this->pic_dir, SRC_PIC_3_DIR);
  this->pub_list = this->pic_3_list;
  this->mutex_dummy.unlock();

  this->change_dir_1_button_push = false;
  this->change_dir_2_button_push = false;
  this->change_dir_3_button_push = true;
  this->change_dir_4_button_push = false;
}
void MainWindow::change_dir_4()
{
  this->mutex_dummy.lock();
  bzero(this->pic_dir,FILE_NAME_SIZE);
  strcpy(this->pic_dir, SRC_PIC_4_DIR);
  this->pub_list = this->pic_4_list;
  this->mutex_dummy.unlock();

  this->change_dir_1_button_push = false;
  this->change_dir_2_button_push = false;
  this->change_dir_3_button_push = false;
  this->change_dir_4_button_push = true;
}



