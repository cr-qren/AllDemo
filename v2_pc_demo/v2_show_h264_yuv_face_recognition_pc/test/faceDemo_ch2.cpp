#include "CrFaceSDK.hpp"
#include "jpgtobin.hpp"
#include "opencv2/opencv.hpp"
#include <deque>
#include <fstream>

using namespace cv;
using namespace std;

#define ID_FLAG true
#define tracker

unordered_map<int, string> id2name;
std::vector<string> str_split(string s, string c);

int main(int argc, char* argv[]) {
  google::InitGoogleLogging(argv[0]);

  Mat cur_frame;
  FaceSDK::Faces faces2save;

  FaceSDK::CrFaceSDK* sdk =
    new FaceSDK::CrFaceSDK("../files/face_6b_bn_model/pbtxt/ssd_bn_6b_opt_dfg.pbtxt",
                           "../files/face_6b_bn_model/weights/float_little/",
                           "../files/face_6b_bn_model/param/",
                           "../files/dlib_face_recognition_resnet_model_v1.dat");

  ifstream fid("../img_dataset/list.txt");
  string delimiter(".");
  string basedir = "../img_dataset/";

  while (!fid.eof()) {
    string filename;
    fid >> filename;
    if (filename.size()<=0) break;
    cout << "Processing " << filename << endl;
    auto tokens = str_split(filename, ".");
    cout << "adding " << tokens[0] << " to database" << endl;
    sdk->addToDatabase(basedir+filename, tokens[0]);
    cout << "finish add to database" << endl << endl;
  }

  cur_frame = cv::imread("../build/faces2save.jpg");
  faces2save.faceCount.totalCount = 1;
  faces2save.faceCount.currentCount = 1;
  FaceSDK::FaceGroup kk;
  kk.faceID = 1;
  kk.faceImage = cur_frame;
  faces2save.faceGroups.push_back(kk);

  cur_frame = cv::imread("../build/faces2save_1.jpg");
  faces2save.faceCount.totalCount = 1;
  faces2save.faceCount.currentCount = 1;
  kk.faceID = 2;
  kk.faceImage = cur_frame;
  faces2save.faceGroups.push_back(kk);

  int height = cur_frame.rows, width = cur_frame.cols;
  if (height > 0 && width > 0) {

    auto ids = sdk->match(faces2save.faceGroups);
    char filename[100];

    for (int k=0; k<faces2save.faceGroups.size(); k++) {
      auto id_key = faces2save.faceGroups[k].faceID;

      if (ids[k]=="None") {
        sdk->reset_track(id_key);
      }

      if (id2name.count(id_key)) {
        id2name[id_key] = ids[k];
      }
      else {
        id2name.insert(std::pair<int, string>(id_key, ids[k]));
      }
    }
  }
  else {
    cout << cur_frame.size() << endl;
  }
  for(unordered_map<int, string>::iterator it = id2name.begin(); it!=id2name.end(); it++)
  cout<<"id :" << it->first <<" name : "<< it->second <<endl;
  return 0;
}

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

