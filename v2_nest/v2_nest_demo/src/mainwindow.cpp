#include "mainwindow.hpp"

MainWindow::MainWindow()
{
  run_button = new QPushButton("Run");
  QObject::connect(run_button, SIGNAL(clicked()), this, SLOT(backend_run()));

  close_button = new QPushButton("Close");
  QObject::connect(close_button, SIGNAL(clicked()), this, SLOT(close()));

  select_button = new QPushButton("Select");
  QObject::connect(select_button, SIGNAL(clicked()), this, SLOT(select_image()));

  disp_label = new QLabel("Display");
  disp_label->setMinimumSize(1920/2, 1080/2);
  disp_label->setMaximumSize(1920/2, 1080/2);

  select_text = new QLineEdit("Please select image file!");
  select_text->setFocusPolicy(Qt::NoFocus);

  select_layout = new QHBoxLayout();
  select_layout->addWidget(select_text);
  select_layout->addWidget(select_button);
  select_layout->setStretchFactor(select_text, 4);
  select_layout->setStretchFactor(select_button, 1);

  disp_layout = new QHBoxLayout();
  disp_layout->addWidget(disp_label);

  button_layout = new QHBoxLayout();
  button_layout->addWidget(run_button);
  button_layout->addWidget(close_button);

  main_layout = new QVBoxLayout();
  main_layout->addLayout(disp_layout);
  main_layout->addLayout(select_layout);
  main_layout->addLayout(button_layout);

//  system("bash ../data/test.sh 2");
//  usleep(2000);
  pthread_create(&thread1, NULL, thread_1, this);
  pthread_detach(thread1);

   
  this->setMinimumSize(1920/2+100, 1080/2+100);
  this->setMaximumSize(1920/2+100, 1080/2+100);

  start_flag = false;
  setLayout(main_layout);
}

MainWindow::~MainWindow()
{

}
void MainWindow::backend_run()
{
  if(start_flag == false)
  {
    this->tcprasp = new TcpRasp("192.168.123.8", 8000);
    start_flag = true;
  }

  char bin_name[512];

  char *send_file = (char *)filename.toStdString().c_str();
  char *send_dir = (char *)dir.toStdString().c_str();

  tcprasp->send_file(send_file, send_dir);
  tcprasp->recv_file(bin_name, "../data");

  FILE *fp = fopen("../data/out.bin", "rb");
  if(fp == NULL)
  {
    perror("fopen");
    exit(1);
  }

  fseek(fp, 0, SEEK_END);
  int size = ftell(fp);
  fseek(fp, 0, SEEK_SET);

  float buffer[size/sizeof(float)];

  int ret = fread(buffer, sizeof(float), size/sizeof(float), fp);
  if(ret < 0)
  {
    perror("fread");
    fclose(fp);
    exit(1);
  }
  fclose(fp);

  QPainter paint(img);
  paint.setBrush(Qt::NoBrush);
  paint.setPen(QPen(QColor(0,255,0),6));

  for(int i = 0; i < size/sizeof(float)/RECT_ARG_NUM; i++)
  {
    paint.drawRect(img_w  * buffer[i*RECT_ARG_NUM + 1],
                   img_h * buffer[i*RECT_ARG_NUM + 2],
                   img_w * (buffer[i*RECT_ARG_NUM + 3] - buffer[i*RECT_ARG_NUM + 1]),
                   img_h  * (buffer[i*RECT_ARG_NUM + 4] - buffer[i*RECT_ARG_NUM + 2]));
    printf("\n\n %f %f %f %f \n\n",buffer[i*RECT_ARG_NUM + 1],buffer[i*RECT_ARG_NUM + 2],buffer[i*RECT_ARG_NUM + 3], buffer[i*RECT_ARG_NUM + 4]);
  }
  paint.end();

  QPixmap pix = QPixmap::fromImage(img->scaled(1920/2, 1080/2));
  this->disp_label->setPixmap(pix);
  this->disp_label->show();
  //delete this->tcprasp;
}

void MainWindow::select_image()
{
  //select imagefile
  QFileDialog dlg(new QWidget(),"Open image file");
  dlg.resize(1920,1080);
  dlg.setAcceptMode(QFileDialog::AcceptOpen);
  dlg.setNameFilter("Images (*.png *.bmp *.jpg *.jpeg)");
  dlg.exec();
  QStringList files = dlg.selectedFiles();
  if(!files.isEmpty()){
    QString file = files.at(0);
    this->select_text->setText(file);
  }

  //paint image on the label
  Qt::CaseSensitivity cs = Qt::CaseSensitive;
  img = new QImage(1920, 1080, QImage::Format_RGB888);

  if(this->select_text->text().compare("Please select image file!", cs) != 0)
  {
    QStringList file_name_split = (this->select_text->text()).split('.');

    if (file_name_split.at(1).compare("jpg", cs) == 0)
    {
      img->load(this->select_text->text(), "jpg");
    }
    else if (file_name_split.at(1).compare("jpeg", cs) == 0)
    {
      img->load(this->select_text->text(), "jpeg");
    }
    else if (file_name_split.at(1).compare("png", cs) == 0)
    {
      img->load(this->select_text->text(), "png");
    }
    else if (file_name_split.at(1).compare("bmp", cs) == 0)
    {
      img->load(this->select_text->text(), "bmp");
    }

    QFileInfo fi(this->select_text->text());

    filename = fi.fileName();
    dir = fi.absolutePath();
    this->img_w = img->width();
    this->img_h = img->height();
    QPixmap pix = QPixmap::fromImage(img->scaled(1920/2, 1080/2));
    this->disp_label->setPixmap(pix);
    this->disp_label->show();
  }
}
