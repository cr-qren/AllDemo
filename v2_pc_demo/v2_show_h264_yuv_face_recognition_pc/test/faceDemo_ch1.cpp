#include "CrFaceSDK.hpp"
#include "jpgtobin.hpp"
#include "opencv2/opencv.hpp"
#include <deque>
#include <fstream>

using namespace cv;
using namespace std;

#define ID_FLAG true
#define tracker

bool is_running = false;
pthread_t tid;
unordered_map<int, string> id2name;

std::vector<string> str_split(string s, string c);

void* faceID_thread(void* data);

typedef struct {
    FaceSDK::CrFaceSDK *sdk;
    FaceSDK::Faces *faces2save;
    int last_id;
} transfer_data;


int main(int argc, char* argv[]) {
    google::InitGoogleLogging(argv[0]);

    VideoCapture cap(0);
    cap.set(CV_CAP_PROP_FPS, 15);


    int id = 0;
    Mat show_img, cur_frame;
    int last_id;
    vector<FaceSDK::BoundingBox> rects;
    FaceSDK::Faces faces2save;
    int length;

    /*init face database*/
    //char filename[150] = {"/home/chao/timg.jpeg"};
    //char ID[50] = {"someone"};

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

    transfer_data* tmp = new transfer_data;
    while (true) 
    {
        //cap >> cur_frame;
        cur_frame = cv::imread("../img_dataset/junjie.jpg");
//        cur_frame = cv::imread("../build/faces2save.jpg");
        int height = cur_frame.rows, width = cur_frame.cols;
        if (height > 0 && width > 0) {

            sdk->detect(id, cur_frame);
            faces2save = sdk->track();
            cout << "totalCount: " << faces2save.faceCount.totalCount << endl;
            if(faces2save.faceCount.totalCount != 0)
            {
            cout << "currentCount: " << faces2save.faceCount.currentCount << endl;
            cout << "faceID: " << faces2save.faceGroups[0].faceID << endl;
            imwrite("faces2save.jpg",faces2save.faceGroups[0].faceImage);
            }
            //cout << "***** Stop 1? *****" << endl;
//            faces2save.faceCount.totalCount = 1;
//            faces2save.faceCount.currentCount = 1;
//            FaceSDK::FaceGroup kk;
//            kk.faceID = 1;
//            kk.faceImage = cur_frame;
//
//            faces2save.faceGroups.push_back(kk);

            cout << "***** Stop 2? *****" << endl;
            auto frame_id = sdk->get_frameID();
            int cache_size = frame_id.size();
            if (cache_size<=0) {id++; continue;}
            auto track_list = sdk->get_tracks();
            //auto rect_list = track_list.bbox;
            length = track_list.size();
            //cout << "rect num: " << track_list.size() << endl;
            //cout << "fs: " << fs.size() << endl;
            for (int j=0; j<length; j++) {
                int x = track_list[j].bbox.x;
                int y = track_list[j].bbox.y;
                int w = track_list[j].bbox.w;
                int h = track_list[j].bbox.h;
                cv::Rect draw_rect(x, y, w, h);
                cv::rectangle(cur_frame, draw_rect, cv::Scalar(0, 0, 255), 2, 1);
                auto it = id2name.find(track_list[j].track_id);
                if (it!=id2name.end()) {
                    cv::putText(cur_frame, it->second, cv::Point(x, y), cv::FONT_HERSHEY_PLAIN, 3,
                            cv::Scalar(255, 0, 0), 3);
                }
            }

            if (ID_FLAG) {
                if (!is_running) {
                    //cout << "Enter Thread" << endl;
                    tmp->sdk = sdk;
                    tmp->faces2save = &faces2save;
                    tmp->last_id = frame_id[cache_size-1];
                    //cout << "get vars" << endl;

                    cout << "***** Stop 3? *****" << endl;
                    pthread_create(&tid, NULL, faceID_thread, (void*)(tmp));
                    pthread_detach(tid);
                  //cout << "Leave Thread" << endl;
                }
            }



            cv::imshow("img",cur_frame);
            cv::waitKey(1);
            //cout << fid.size() << endl;
            id++;
        }
        else {
            cout << cur_frame.size() << endl;
        }

    }
    delete tmp;

    return 0;

}


void* faceID_thread(void* data) {
    is_running = true;
    transfer_data* input_faces = (transfer_data*)data;
    auto faces2save = *(input_faces->faces2save);

    //cout << "thread face Num: " << faces2save.faceGroups.size() << endl;
    //cout << "start recognition" << endl;
    auto ids = input_faces->sdk->match(faces2save.faceGroups);
    //cout << "after recognition -->" << faces2save.faceGroups.size() << endl;
    char filename[100];

    cout << "***** Stop 4? *****" << endl;
    for (int k=0; k<faces2save.faceGroups.size(); k++) {
        auto id_key = faces2save.faceGroups[k].faceID;

        if (ids[k]=="None") {
            input_faces->sdk->reset_track(id_key);
        }

        if (id2name.count(id_key)) {
            id2name[id_key] = ids[k];
        }
        else {
            id2name.insert(std::pair<int, string>(id_key, ids[k]));
        }
        cout<<"name :" << ids[k]<< endl;
    }

    //cout << "finish recognition" << endl;
    is_running = false;
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





