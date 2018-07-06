#include "mainwindow.hpp"
#include "mythreads.hpp"
#include "tcpsocket.hpp"
#include "board.hpp"
#include "poya.hpp"


void *thread_1(void *arg)
{
  MainWindow *pwindow = (MainWindow *)arg;

  unsigned long recv_size;

  FRAME_INFO frame_info;
  {
    recv_size = sizeof(frame_info);
    pwindow->cam_tcp->recv_data(&frame_info, recv_size);

    pwindow->imgs[pwindow->img_index].size = frame_info.dataLen;
    pwindow->cam_tcp->recv_data(pwindow->imgs[pwindow->img_index].buffer, pwindow->imgs[pwindow->img_index].size);

    for(int i = 1; i < 20; i++)
    {
      pwindow->imgs[i].size = pwindow->imgs[pwindow->img_index].size;
      memcpy(pwindow->imgs[i].buffer, pwindow->imgs[pwindow->img_index].buffer, pwindow->imgs[pwindow->img_index].size);
    }
  }

    pthread_create(&pwindow->thread4, NULL, thread_4, arg);
    pthread_detach(pwindow->thread4);

  while(!pwindow->close_flag)
  {

    recv_size = sizeof(frame_info);
    pwindow->cam_tcp->recv_data(&frame_info, recv_size);

    pwindow->mutex_dummy.lock();
    pwindow->imgs[pwindow->img_index].size = frame_info.dataLen;
    pwindow->cam_tcp->recv_data(pwindow->imgs[pwindow->img_index].buffer,
                                pwindow->imgs[pwindow->img_index].size);
    pwindow->mutex_dummy.unlock();

    pwindow->img_index == 19 ? pwindow->img_index = 0 : pwindow->img_index++ ;

    pwindow->show_flag = true;
  }
}

void *thread_2(void *arg)
{
  MainWindow *pwindow = (MainWindow *)arg;

  pthread_create(&pwindow->thread5, NULL, thread_5, arg);
  pthread_detach(pwindow->thread5);

  pthread_create(&pwindow->thread6, NULL, thread_6, arg);
  pthread_detach(pwindow->thread6);
}

void *thread_3(void *arg)
{
  MainWindow *pwindow = (MainWindow *)arg;
  system("bash ../data/test.sh 0");
}

void *thread_4(void *arg)
{
  MainWindow *pwindow = (MainWindow *)arg;

  while(!pwindow->close_flag)
  {
    if(pwindow->show_flag)
    {
      QImage img(PICTURE_WIDTH, PICTURE_HEIGHT, QImage::Format_RGB888);

      pwindow->mutex_dummy.lock();
      img.loadFromData(pwindow->imgs[(pwindow->img_index+20-8)%20].buffer,
                       pwindow->imgs[(pwindow->img_index+20-8)%20].size,
                       "JPEG");
      pwindow->mutex_dummy.unlock();

      /*paint the image*/
      QPainter pain(&img);
      pain.setBrush(Qt::NoBrush);
      pain.setPen(QPen(QColor(0,255,0),3));

      pwindow->mutex_dummy_1.lock();
      for(int i = 0; i < pwindow->rect.size()/5 ; i++)
      {
        pain.drawRect(PICTURE_WIDTH  * pwindow->rect[i*5 + 1],
                      PICTURE_HEIGHT * pwindow->rect[i*5 + 2],
                      PICTURE_WIDTH  * (pwindow->rect[i*5 + 3]-
                                        pwindow->rect[i*5 + 1]),
                      PICTURE_HEIGHT * (pwindow->rect[i*5 + 4]-
                                        pwindow->rect[i*5 + 2]));
      }
      pwindow->mutex_dummy_1.unlock();
      pain.end();
      /*set the image on label*/
      QPixmap pix = QPixmap::fromImage(img.scaled(PICTURE_WIDTH/2, PICTURE_HEIGHT/2));
      pwindow->disp_label->setPixmap(pix);
      pwindow->disp_label->show();

      pwindow->show_flag = false;
    }
  }
}

void *thread_5(void *arg)
{
  MainWindow *pwindow = (MainWindow *)arg;

  char *str = (char *)pwindow->select_list->currentText().toStdString().c_str();

  if(strcmp(str, "mode 1:face") == 0)
  {
    system("bash ../data/test.sh 1");
  }
  if(strcmp(str, "mode 2:car") == 0)
  {
    system("bash ../data/test.sh 2");
  }
  if(strcmp(str, "mode 3:insulator") == 0)
  {
    system("bash ../data/test.sh 3");
  }
  if(strcmp(str, "mode 4:both") == 0)
  {
    system("bash ../data/test.sh 4");
  }
  if(strcmp(str, "mode 5:___") == 0)
  {
    system("bash ../data/test.sh 5");
  }
  if(strcmp(str, "mode 6:___") == 0)
  {
    system("bash ../data/test.sh 6");
  }
  if(strcmp(str, "mode 7:test") == 0)
  {

  }
}

void *thread_6(void *arg)
{
  MainWindow *pwindow = (MainWindow *)arg;

  unsigned char signal = 0;
  unsigned long recv_size;

  char filename[512];
  unsigned long rect_size;

  while(!pwindow->board_stop_flag)
  {
    recv_size = sizeof(unsigned char);
    pwindow->board_tcp->setconnect_fp();
    pwindow->board_tcp->recv_data(&signal, recv_size);

    if(signal == 1)
    {
      recv_size = sizeof(filename);
      sprintf(filename, "%s", "data.jpeg");
      pwindow->board_tcp->send_data(filename, recv_size);
      pwindow->board_tcp->send_data(pwindow->imgs[(pwindow->img_index+20-1)%20].buffer,
                                    pwindow->imgs[(pwindow->img_index+20-1)%20].size);

      pwindow->board_tcp->closeconnect_fp();
    }

    if(signal == 2)
    {
      recv_size = sizeof(filename);
      pwindow->board_tcp->recv_data(filename, recv_size);

      recv_size = sizeof(rect_size);
      pwindow->board_tcp->recv_data(&rect_size, recv_size);

      float rect_buffer[rect_size/sizeof(float)];
      pwindow->board_tcp->recv_data(rect_buffer, rect_size);
      pwindow->board_tcp->closeconnect_fp();

      pwindow->mutex_dummy_1.lock();
      pwindow->rect.erase(pwindow->rect.begin(), pwindow->rect.end());
      for(int i = 0; i < rect_size/sizeof(float); i++)
      {
        pwindow->rect.push_back(rect_buffer[i]);
      }
      pwindow->mutex_dummy_1.unlock();
    }
  }
}
