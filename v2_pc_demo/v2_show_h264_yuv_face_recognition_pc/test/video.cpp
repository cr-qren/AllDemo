#include "CrFaceSDK.hpp"
#include "utility/videostream.h"
#include <QApplication>
int main(int argc, char *argv[])
{
  google::InitGoogleLogging(argv[0]);

  QApplication app(argc, argv);

  VideoStream video;
  video.Init();
  //video.setUrl("rtsp://admin:CPFEGI@192.168.1.168:554/h264/ch1/main/av_stream");
  video.setUrl("rtsp://admin:CPFEGI@192.168.1.168:554/h264/ch2/av_stream");
  //video.setUrl("rtsp://admin:admin@192.168.1.168:554/1");
  //video.setUrl("rtsp://admin:admin@192.168.1.168:554/");
  video.show();

  app.processEvents();
  app.exec();
  return 0;
}
