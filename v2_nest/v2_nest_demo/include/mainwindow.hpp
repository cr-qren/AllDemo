#include <pthread.h>

#include <QObject>
#include <QMutex>
#include <QLabel>
#include <QLineEdit>
#include <QPushButton>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QString>
#include <QStringList>
#include <QFileDialog>
#include <QFileInfo>
#include <QPainter>

#include "tcprasp.hpp"
#include "mythread.hpp"

#define RECT_ARG_NUM 5
#define PICTURE_WIDTH 4288  
#define PICTURE_HEIGHT 2848

#ifndef MAINWINDOW_HPP
#define MAINWINDOW_HPP

class MainWindow:public QWidget
{
  Q_OBJECT
public slots:
    void backend_run();
    void select_image();

public:
  MainWindow();
  ~MainWindow();

  QMutex mutex_dummy;

  pthread_t thread1;


private:
  QPushButton *run_button;
  QPushButton *close_button;
  QPushButton *select_button;

  QLabel *disp_label;
  QLineEdit *select_text;
  QHBoxLayout *select_layout;
  QVBoxLayout *main_layout;
  QHBoxLayout *disp_layout;
  QHBoxLayout *button_layout;

  QImage *img;
  QString filename;
  QString dir;

  TcpRasp *tcprasp;
  bool start_flag;
  int img_w;
  int img_h;
};


#endif
