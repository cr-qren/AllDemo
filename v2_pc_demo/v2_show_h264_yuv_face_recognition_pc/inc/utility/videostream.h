
#ifndef VideoStream_h
#define VideoStream_h

#include <QString>
#include <QImage>
#include <QPixmap>
#include <QMutex>
#include <QDateTime>
#include <QTimer>
#include <QThread>
#include <QLabel>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QPushButton>
#include <QDebug>


//C++引用C语言的头文件
extern "C"
{
#include <libavcodec/avcodec.h>
#include <libavdevice/avdevice.h>
#include <libavformat/avformat.h>
#include <libswscale/swscale.h>
#include <libavutil/pixfmt.h>
#include <libavutil/frame.h>
}

#include "cr_tcp.h"
#include "YUV_transer.h"
#include "mythread.h"

#define DISP_WIDTH 1920
#define DISP_HEIGHT 1080

class VideoStream:public QWidget
{
  Q_OBJECT
private:
  QMutex mutex;
  QTimer *m_timerPlay;
  QTimer *m_timerPlay_1;
  QString m_str_url;

  QHBoxLayout *main_layout;
  QVBoxLayout *disp_layout;
  QHBoxLayout *button_layout;
  QVBoxLayout *result_layout;
  QHBoxLayout *button_layout1;
  QLabel *disp_label;
  QLabel *result_label;
  QPushButton *start_button;
  QPushButton *stop_button;
  QPushButton *result_button;
  QPushButton *close_button;

  AVPicture  pAVPicture;
  AVFormatContext *pAVFormatContext;
  AVCodecContext *pAVCodecContext;
  AVFrame *pAVFrame;
  SwsContext * pSwsContext;
  AVPacket pAVPacket;

  int videoStreamIndex;
  int videoHeight;
  int videoWidth;
  int m_i_frameFinished;

public:
  bool start_Reco;
  Cr_tcp *client;
  pthread_t tid;
  QImage *resultImg;
  QMutex mutex_1;

public:
  VideoStream();
  ~VideoStream();
  bool Init();
  void setUrl(QString url);

public slots:
  void startStream();
  void stopStream();
  void SetImageSlots(const QImage &image);
  void playSlots();
  void detect();

signals:
  void GetImage(QImage image);
};
#endif
