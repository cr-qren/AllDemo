#include "utility/videostream.h"


VideoStream::VideoStream()
{
  videoStreamIndex = 0;
  videoWidth = 1280;
  videoHeight = 720;
  m_i_frameFinished = 0;
  start_Reco = false;
  resultImg = new QImage(DISP_WIDTH*0.25, DISP_HEIGHT*0.5, QImage::Format_ARGB32_Premultiplied);
  resultImg->fill(qRgb(210, 233, 255));

  m_timerPlay = new QTimer(this);
  m_timerPlay->setInterval(10);
  QObject::connect(m_timerPlay, SIGNAL(timeout()), this, SLOT(playSlots()));

  QObject::connect(this,SIGNAL(GetImage(QImage)),this,SLOT(SetImageSlots(QImage)));

  main_layout = new QHBoxLayout();
  disp_layout = new QVBoxLayout();
  button_layout = new QHBoxLayout();
  result_layout = new QVBoxLayout();
  button_layout1 = new QHBoxLayout();

  disp_label = new QLabel("display");
  disp_label->setMinimumSize(DISP_WIDTH*0.5, DISP_HEIGHT*0.5);
  disp_label->setMaximumSize(DISP_WIDTH*0.5, DISP_HEIGHT*0.5);

  result_label = new QLabel("result");
  result_label->setMinimumSize(DISP_WIDTH*0.25, DISP_HEIGHT*0.5);
  result_label->setMaximumSize(DISP_WIDTH*0.25, DISP_HEIGHT*0.5);

  start_button = new QPushButton("display");
  QObject::connect(start_button, SIGNAL(clicked()), this, SLOT(startStream()));
  start_button->setMinimumSize(DISP_WIDTH*0.05, DISP_HEIGHT*0.03);
  start_button->setMaximumSize(DISP_WIDTH*0.05, DISP_HEIGHT*0.03);

  stop_button = new QPushButton("stop");
  QObject::connect(stop_button, SIGNAL(clicked()), this, SLOT(stopStream()));
  stop_button->setMinimumSize(DISP_WIDTH*0.05, DISP_HEIGHT*0.03);
  stop_button->setMaximumSize(DISP_WIDTH*0.05, DISP_HEIGHT*0.03);

  result_button = new QPushButton("detect");
  QObject::connect(result_button, SIGNAL(clicked()), this, SLOT(detect()));
  result_button->setMinimumSize(DISP_WIDTH*0.05, DISP_HEIGHT*0.03);
  result_button->setMaximumSize(DISP_WIDTH*0.05, DISP_HEIGHT*0.03);


  close_button = new QPushButton("close");
  QObject::connect(close_button, SIGNAL(clicked()), this, SLOT(close()));
  close_button->setMinimumSize(DISP_WIDTH*0.05, DISP_HEIGHT*0.03);
  close_button->setMaximumSize(DISP_WIDTH*0.05, DISP_HEIGHT*0.03);

  button_layout->addWidget(start_button);
  button_layout->addWidget(stop_button);
  disp_layout->addWidget(disp_label);
  disp_layout->addLayout(button_layout);

  button_layout1->addWidget(result_button);
  button_layout1->addWidget(close_button);
  result_layout->addWidget(result_label);
  result_layout->addLayout(button_layout1);

  main_layout->addLayout(disp_layout);
  main_layout->addLayout(result_layout);

  setLayout(main_layout);
}

VideoStream::~VideoStream()
{

}

void VideoStream::detect()
{
  //this->client = new Cr_tcp("192.168.2.3", 1234, CLIENT);
  //this->start_Reco = true;
  //m_timerPlay_1->start();
  pthread_create(&tid, NULL, mythread_1, (void *)this);
  pthread_detach(tid);

}

void VideoStream::setUrl(QString url)
{
  m_str_url = url;
}

void VideoStream::startStream()
{
  videoStreamIndex=-1;
  av_register_all();//注册库中所有可用的文件格式和解码器
  avformat_network_init();//初始化网络流格式,使用RTSP网络流时必须先执行
  pAVFormatContext = avformat_alloc_context();//申请一个AVFormatContext结构的内存,并进行简单初始化
  pAVFrame=av_frame_alloc();
  if (this->Init())
  {
    m_timerPlay->start();
  }
}
void VideoStream::stopStream()
{
  m_timerPlay->stop();

  avformat_free_context(pAVFormatContext);
  av_frame_free(&pAVFrame);
  sws_freeContext(pSwsContext);
}

void VideoStream::SetImageSlots(const QImage &image)
{
  if (image.height()>0){
    QPixmap pix = QPixmap::fromImage(image.scaled(DISP_WIDTH/2, DISP_HEIGHT/2));
    disp_label->setPixmap(pix);
  }
  if (resultImg->height()>0){
    this->mutex_1.lock();
    QPixmap pix = QPixmap::fromImage(*resultImg);
    this->mutex_1.unlock();
    result_label->setPixmap(pix);
  }
}

bool VideoStream::Init()
{
  if(m_str_url.isEmpty())
    return false;
  //打开视频流
  int result=avformat_open_input(&pAVFormatContext, m_str_url.toStdString().c_str(),NULL,NULL);
  if (result<0){
    //qDebug()<<"打开视频流失败";
    return false;
  }

  //获取视频流信息
  result=avformat_find_stream_info(pAVFormatContext,NULL);
  if (result<0){
    //qDebug()<<"获取视频流信息失败";
    return false;
  }

  //获取视频流索引
  videoStreamIndex = -1;
  for (uint i = 0; i < pAVFormatContext->nb_streams; i++) {
    if (pAVFormatContext->streams[i]->codec->codec_type == AVMEDIA_TYPE_VIDEO) {
      videoStreamIndex = i;
      break;
    }
  }

  if (videoStreamIndex==-1){
    //qDebug()<<"获取视频流索引失败";
    return false;
  }

  //获取视频流的分辨率大小
  pAVCodecContext = pAVFormatContext->streams[videoStreamIndex]->codec;
  videoWidth=pAVCodecContext->width;
  videoHeight=pAVCodecContext->height;

  avpicture_alloc(&pAVPicture,AV_PIX_FMT_RGB24,videoWidth,videoHeight);

  AVCodec *pAVCodec;

  //获取视频流解码器
  pAVCodec = avcodec_find_decoder(pAVCodecContext->codec_id);
  pSwsContext = sws_getContext(videoWidth,videoHeight,AV_PIX_FMT_YUV420P,videoWidth,videoHeight,AV_PIX_FMT_RGB24,SWS_BICUBIC,0,0,0);

  //打开对应解码器
  result=avcodec_open2(pAVCodecContext,pAVCodec,NULL);
  if (result<0){
    //qDebug()<<"打开解码器失败";
    return false;
  }

  //qDebug()<<"初始化视频流成功";
  return true;
}

void VideoStream::playSlots()
{
  //一帧一帧读取视频
  if (av_read_frame(pAVFormatContext, &pAVPacket) >= 0){

    if(pAVPacket.stream_index==videoStreamIndex){

      avcodec_decode_video2(pAVCodecContext, pAVFrame, &m_i_frameFinished, &pAVPacket);

      if (m_i_frameFinished){

        mutex.lock();

        sws_scale(pSwsContext,(const uint8_t* const *)pAVFrame->data,pAVFrame->linesize,0,videoHeight,pAVPicture.data,pAVPicture.linesize);

        QImage image(pAVPicture.data[0],videoWidth,videoHeight,QImage::Format_RGB888);
        emit GetImage(image);

        mutex.unlock();

      }
    }
  }
  av_free_packet(&pAVPacket);//释放资源,否则内存会一直上升
}
