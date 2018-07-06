#include "facereco.h"

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

FaceSDK::CrFaceSDK *Init()
{
  FaceSDK::CrFaceSDK* sdk =
    new FaceSDK::CrFaceSDK("../files/face_6b_bn_model/pbtxt/ssd_bn_6b_opt_dfg.pbtxt",
                           "../files/face_6b_bn_model/weights/float_little/",
                           "../files/face_6b_bn_model/param/",
                           "../files/dlib_face_recognition_resnet_model_v1.dat");

  ifstream fid("../img_dataset/list (copy).txt");
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

  return sdk;
}


unordered_map<int, string> faceRecognition(FaceSDK::CrFaceSDK *sdk, FaceSDK::Faces faces2save)
{
    unordered_map<int, string> id2name;

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

    for(unordered_map<int, string>::iterator it = id2name.begin(); it!=id2name.end(); it++)
      cout<<"id :" << it->first <<" name : "<< it->second <<endl;

    return id2name;
}

//int main(int argc, char* argv[]) {
//  google::InitGoogleLogging(argv[0]);
//
//  FaceSDK::CrFaceSDK *sdk = Init();
//
//  while(1)
//  {
//    cv::Mat cur_frame = cv::imread("../build/faces2save.jpg");
//    FaceSDK::Faces faces2save;
//    faces2save.faceCount.totalCount = 1;
//    faces2save.faceCount.currentCount = 1;
//    FaceSDK::FaceGroup kk;
//    kk.faceID = 1;
//    kk.faceImage = cur_frame;
//    faces2save.faceGroups.push_back(kk);
//
//    cur_frame = cv::imread("../build/faces2save_1.jpg");
//    faces2save.faceCount.totalCount = 1;
//    faces2save.faceCount.currentCount = 1;
//    kk.faceID = 2;
//    kk.faceImage = cur_frame;
//    faces2save.faceGroups.push_back(kk);
//
//    unordered_map<int, string> id2name =  faceReco(sdk, faces2save);
//    for(unordered_map<int, string>::iterator it = id2name.begin(); it!=id2name.end(); it++)
//      cout<<"id :" << it->first <<" name : "<< it->second <<endl;
//  }
//  return 0;
//}
