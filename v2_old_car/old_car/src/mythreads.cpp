#include "mythreads.hpp"

void *thread_1(void *ptr)
{
  int pic_num = 0;
  MainWindow *temp_window = (MainWindow *)ptr;

  temp_window->mutex_pic.lock();
  pic_num = temp_window->pic_num;
  temp_window->mutex_pic.unlock();

  while(temp_window->start_flag)
  {
    cout << __FUNCTION__ << endl;
    char file_name[FILE_NAME_SIZE];
    char dir[FILE_NAME_SIZE];
    temp_window->mutex_dummy.lock();

    vector<string> v = temp_window->pub_list;
    memcpy(dir, temp_window->pic_dir, FILE_NAME_SIZE);
    temp_window->mutex_dummy.unlock();

    sprintf(file_name,"%s/%s",dir,v[pic_num].c_str());
	  QImage *img = new QImage(PICTURE_WIDTH, PICTURE_HEIGHT, QImage::Format_RGB888);
    cout << file_name << endl;
    img->load(file_name, "JPEG");

    bool board = temp_window->board_flag;
    cout << temp_window->board_flag <<endl;
    if(board)
    {
      //  temp_window->tcprasp->send_data(&pic_num, sizeof(int));
      char temp_name[FILE_NAME_SIZE];
      sprintf(temp_name, "%s", v[pic_num].c_str());
      cout << temp_name << endl;
      temp_window->tcprasp->send_file(temp_name, dir);
    }
    float *buffer;
    unsigned long size;
    if(board)
    {
      char bin_name[FILE_NAME_SIZE];
      sprintf(bin_name,"%s", "out.bin");
      cout << "bin_name" << bin_name <<endl;
      temp_window->tcprasp->recv_file(bin_name, "../data");
      char dir_name[FILE_NAME_SIZE];
      sprintf(dir_name, "%s%s", "../data/",bin_name);
      load_file<float>("../data/out.bin", buffer, size);
      draw_rect(img, buffer, size);

      delete []buffer;
    }
    else
    {
      usleep(700000);
    }

		QPixmap pix = QPixmap::fromImage(img->scaled(PICTURE_WIDTH/2, PICTURE_HEIGHT/2));
		temp_window->disp_label->setPixmap(pix);
		temp_window->disp_label->show();

    pic_num++;
    if(pic_num == v.size())
    {
      cout << v.size() << "::" << pic_num <<endl;
      pic_num = 0;
    }

    delete img;
  }
}

void *thread_2(void *ptr)
{
  int pic_num = 0;
  MainWindow *temp_window = (MainWindow *)ptr;
  while(1)
  {
    if(!temp_window->change_dir_1_button_push)
    {
      QPixmap pix = QPixmap::fromImage(temp_window->pic_dir_1[pic_num].scaled(66, 66));
      temp_window->src_pic_1_label->setPixmap(pix);
      temp_window->src_pic_1_label->show();
    }
    if(!temp_window->change_dir_2_button_push)
    {
      QPixmap pix = QPixmap::fromImage(temp_window->pic_dir_2[pic_num].scaled(66, 66));
      temp_window->src_pic_2_label->setPixmap(pix);
      temp_window->src_pic_2_label->show();
    }
    if(!temp_window->change_dir_3_button_push)
    {
      QPixmap pix = QPixmap::fromImage(temp_window->pic_dir_3[pic_num].scaled(66, 66));
      temp_window->src_pic_3_label->setPixmap(pix);
      temp_window->src_pic_3_label->show();
    }
    if(!temp_window->change_dir_4_button_push)
    {
      QPixmap pix = QPixmap::fromImage(temp_window->pic_dir_4[pic_num].scaled(66, 66));
      temp_window->src_pic_4_label->setPixmap(pix);
      temp_window->src_pic_4_label->show();
    }
    sleep(1);
    pic_num++;
    if(pic_num == 10)
    {
      pic_num = 0;
    }
    temp_window->mutex_pic.lock();
    temp_window->pic_num = pic_num;
    temp_window->mutex_pic.unlock();
  }
#if 0
  while(!temp_window->change_dir_1_button_push)
  {
    QPixmap pix = QPixmap::fromImage(temp_window->pic_dir_1[pic_num++].scaled(66, 66));
    temp_window->src_pic_1_label->setPixmap(pix);
    temp_window->src_pic_1_label->show();
    sleep(1);
    if(pic_num == 3)
    {
      pic_num = 0;
    }
  }
#endif
#if 0
  int pic_num = 0;
  int count_pic = count_files(SRC_PIC_1_DIR);

  MainWindow *temp_window = (MainWindow *)ptr;

  while(temp_window->start_flag)
  {
    if(!temp_window->change_dir_1_button_push)
    {
      cout << __FUNCTION__ << endl;

      char file_name[FILE_NAME_SIZE];
      sprintf(file_name, "%s/pic%d.jpeg", SRC_PIC_1_DIR, pic_num++);

      QImage *img = new QImage(PICTURE_WIDTH, PICTURE_HEIGHT, QImage::Format_RGB888);
      img->load(file_name, "JPEG");

      QPixmap pix = QPixmap::fromImage(img->scaled(66, 66));
      temp_window->src_pic_1_label->setPixmap(pix);
      temp_window->src_pic_1_label->show();

      if(pic_num == count_pic)
      {
        pic_num = 0;
      }
      delete img;
    }
  }
#endif
}
void *thread_3(void *ptr)
{
#if 0
  int pic_num = 0;
  MainWindow *temp_window = (MainWindow *)ptr;
  while(!temp_window->change_dir_2_button_push)
  {
    QPixmap pix = QPixmap::fromImage(temp_window->pic_dir_2[pic_num++].scaled(66, 66));
    temp_window->src_pic_2_label->setPixmap(pix);
    temp_window->src_pic_2_label->show();
    sleep(1);
    if(pic_num == 3)
    {
      pic_num = 0;
    }
  }
#endif
#if 0
  int pic_num = 0;
  int count_pic = count_files(SRC_PIC_2_DIR);

  MainWindow *temp_window = (MainWindow *)ptr;

  while(temp_window->start_flag)
  {
    if(!temp_window->change_dir_2_button_push)
    {
      cout << __FUNCTION__ << endl;

      char file_name[FILE_NAME_SIZE];
      sprintf(file_name, "%s/pic%d.jpeg", SRC_PIC_2_DIR, pic_num++);

      QImage *img = new QImage(PICTURE_WIDTH, PICTURE_HEIGHT, QImage::Format_RGB888);
      img->load(file_name, "JPEG");

      QPixmap pix = QPixmap::fromImage(img->scaled(66, 66));
      temp_window->src_pic_2_label->setPixmap(pix);
      temp_window->src_pic_2_label->show();

      if(pic_num == count_pic)
      {
        pic_num = 0;
      }
      delete img;
    }
  }
#endif
}
void *thread_4(void *ptr)
{
#if 0
  int pic_num = 0;
  MainWindow *temp_window = (MainWindow *)ptr;
  while(!temp_window->change_dir_3_button_push)
  {
    QPixmap pix = QPixmap::fromImage(temp_window->pic_dir_3[pic_num++].scaled(66, 66));
    temp_window->src_pic_3_label->setPixmap(pix);
    temp_window->src_pic_3_label->show();
    sleep(1);
    if(pic_num == 3)
    {
      pic_num = 0;
    }
  }
#endif
#if 0
  int pic_num = 0;
  int count_pic = count_files(SRC_PIC_3_DIR);

  MainWindow *temp_window = (MainWindow *)ptr;

  while(temp_window->start_flag)
  {
    if(!temp_window->change_dir_3_button_push)
    {
      cout << __FUNCTION__ << endl;

      char file_name[FILE_NAME_SIZE];
      sprintf(file_name, "%s/pic%d.jpeg", SRC_PIC_3_DIR, pic_num++);

      QImage *img = new QImage(PICTURE_WIDTH, PICTURE_HEIGHT, QImage::Format_RGB888);
      img->load(file_name, "JPEG");

      QPixmap pix = QPixmap::fromImage(img->scaled(66, 66));
      temp_window->src_pic_3_label->setPixmap(pix);
      temp_window->src_pic_3_label->show();

      if(pic_num == count_pic)
      {
        pic_num = 0;
      }
      delete img;
    }
  }
#endif
}
void *thread_5(void *ptr)
{
#if 0
  int pic_num = 0;
  MainWindow *temp_window = (MainWindow *)ptr;
  while(!temp_window->change_dir_4_button_push)
  {
    QPixmap pix = QPixmap::fromImage(temp_window->pic_dir_4[pic_num++].scaled(66, 66));
    temp_window->src_pic_4_label->setPixmap(pix);
    temp_window->src_pic_4_label->show();
    sleep(1);
    if(pic_num == 3)
    {
      pic_num = 0;
    }
  }
#endif
#if 0
  int pic_num = 0;
  int count_pic = count_files(SRC_PIC_4_DIR);

  MainWindow *temp_window = (MainWindow *)ptr;

  while(temp_window->start_flag)
  {
    if(!temp_window->change_dir_4_button_push)
    {
      cout << __FUNCTION__ << endl;

      char file_name[FILE_NAME_SIZE];
      sprintf(file_name, "%s/pic%d.jpeg", SRC_PIC_4_DIR, pic_num++);

      QImage *img = new QImage(PICTURE_WIDTH, PICTURE_HEIGHT, QImage::Format_RGB888);
      img->load(file_name, "JPEG");

      QPixmap pix = QPixmap::fromImage(img->scaled(66, 66));
      temp_window->src_pic_4_label->setPixmap(pix);
      temp_window->src_pic_4_label->show();

      if(pic_num == count_pic)
      {
        pic_num = 0;
      }
      delete img;
    }
  }
#endif
}
