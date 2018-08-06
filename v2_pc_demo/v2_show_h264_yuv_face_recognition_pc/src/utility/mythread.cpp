#include "mythread.h"
/*
std::vector<string> str_split(string s,  string c)
{
  std::string::size_type pos1, pos2;
  std::vector<string> v;
  pos2 = s.find(c);
  pos1 = 0;
  while(std::string::npos != pos2)
  {
    v.push_back(s.substr(pos1, pos2-pos1));

    pos1 = pos2 + c.size();
    pos2 = s.find(c, pos1);
  }
  if(pos1 != s.length())
    v.push_back(s.substr(pos1));

  return v;
}
*/
void *mythread_1(void *arg)
{
  VideoStream *vs = (VideoStream *)arg;

  vs->client = new Cr_tcp("192.168.2.3", 8002, CLIENT);
  vs->start_Reco = true;

  FaceSDK::CrFaceSDK *sdk = Init();
  int x = 0;
  int y = 0;
  int count = 0;
  while(1)
  {
    YUV_DATA yuvData;
    printf("x over !\n");

    recv_yuv((void *)vs->client, &yuvData);

    printf("id: %d\n", yuvData.yuvPram->Id);
    printf("W: %d\n", yuvData.yuvPram->Width);
    printf("H: %d\n", yuvData.yuvPram->Height);
    printf("C: %d\n", yuvData.yuvPram->Channel);
    printf("SIZE: %d\n", yuvData.yuvPram->BuffSize);
    /*        yuv -> Mat        */

    cv::Mat xx = YUV420Img2RGB_Mat(yuvData.yuvBuff, yuvData.yuvPram->Height, yuvData.yuvPram->Width);
    //        char buffy[256];
    //sprintf(buffy, "yuv_%d_%d.jpg",x,y);
    //imwrite(buffy, xx);
    //
    //    cv::Mat cur_frame = cv::imread("../img_dataset/faces2save_1.jpg");
    cv::Mat cur_frame = xx;
    FaceSDK::Faces faces2save;
    faces2save.faceCount.totalCount = 1;
    faces2save.faceCount.currentCount = 1;
    FaceSDK::FaceGroup kk;
    kk.faceID = 1;
    kk.faceImage = cur_frame;
    faces2save.faceGroups.push_back(kk);


    unordered_map<int, string> id2name = faceRecognition(sdk, faces2save);
    for(unordered_map<int, string>::iterator it = id2name.begin(); it!=id2name.end(); it++)
    {
      cout<<"id :" << it->first <<" name : "<< it->second <<endl;
      if(it->second.compare("None")==0)
      {
        char buffy[256];
        sprintf(buffy, "%05d.jpg",count);
 //       imwrite(buffy, xx);
        count ++;
      }
      /*      show info    <mat, string>     */

      if(y == 5)
      {
        y = 0;
        x ++;
      }
      if(x == 5)
      {
        x = 0;
      }
      char buffxx[256];
      sprintf(buffxx, "%d_%d_%d.jpg",x,y,count);
      vs->mutex_1.lock();
      imageMosaic(vs->resultImg, cur_frame, str_split(it->second,"_")[0], x, y, vs->resultImg->width()/5, vs->resultImg->height()/5); 
      //vs->resultImg->save(buffxx, "JPG");
      vs->mutex_1.unlock();

      y ++;
    }

    free_yuvPram_mem(yuvData.yuvPram);
    free_yuv_mem(yuvData.yuvBuff);
  }
}
