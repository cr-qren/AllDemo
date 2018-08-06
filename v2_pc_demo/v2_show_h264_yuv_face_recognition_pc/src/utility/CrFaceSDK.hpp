#ifndef _CrFaceSDK_FLAG
#define _CrFaceSDK_FLAG

#include "CrCommon.h"
#include "Detector/CrDetector.hpp"
#include "Tracker/tracking.h"
#include "FeatExtractor/CrFeatureID.h"
#include <deque>

namespace FaceSDK {

class CrFaceSDK {
    public:
        CrFaceSDK(string dfg_def_file, string weights_path, string post_param_file, string dlib_model_path, string dlib_model_path_1) {
            ptr_detector = new CrDetector(det_mid);
            ptr_detector->CrDetetorInit(dfg_def_file, weights_path, post_param_file);
            ptr_tracker = new Tracker();
            ptr_feat = new CrFeature(dlib_model_path, dlib_model_path_1);
            ptr_faceid = new CrFaceID();

            LOG(INFO) << "Init CrFaceSDK Succeed!";
        }

        ~CrFaceSDK() {
            delete ptr_detector;
            delete ptr_tracker;
            delete ptr_feat;
            delete ptr_faceid;
        }

        std::deque<int> get_frameID() {
            return frame_id;
        }

        std::deque<cv::Mat> get_frames() {
            return frame_list;
        }

        //std::deque< std::vector<BoundingBox> > get_rects() {
        //    return rects;
        //}

        std::vector<BoundingBox> get_rects() {
            auto frame_id = ptr_tracker-> get_current_tracking_frame_id();
            std::vector<BoundingBox> rect_cache;
            auto activeTracks = ptr_tracker->get_active_tracks();
            cout << "active track num: " << activeTracks.size() << endl;
            for (int i=0; i<activeTracks.size(); i++) {
                auto dt = activeTracks[i];
                int boxIndex = frame_id - dt.startFrame;
                cout << "boxIndex: " << boxIndex << endl;
                cout << "box size: " << dt.boxes.size() << endl;
                if (boxIndex < dt.boxes.size()) {
                    rect_cache.push_back(dt.boxes[boxIndex]);
                }
            }
            return rect_cache;
        }

        std::vector<PaintTrack> get_tracks() {
            auto frame_id = ptr_tracker-> get_current_tracking_frame_id();
            //std::vector<BoundingBox> rect_cache;
            std::vector<PaintTrack> rect_cache;
            auto activeTracks = ptr_tracker->get_active_tracks();
            //cout << "active track num: " << activeTracks.size() << endl;
            for (int i=0; i<activeTracks.size(); i++) {
                auto dt = activeTracks[i];
                int boxIndex = frame_id - dt.startFrame;
                //cout << "boxIndex: " << boxIndex << endl;
                //cout << "box size: " << dt.boxes.size() << endl;
                if (boxIndex < dt.boxes.size()) {
                    PaintTrack tmp;
                    tmp.bbox = dt.boxes[boxIndex];
                    tmp.track_id = dt.id;
                    rect_cache.push_back(tmp);
                }
            }
            return rect_cache;
        }


        // ToDo funct: result handling with mutex_lock

        Faces get_faces() {
            return tracked_faces;
        }


        void detect(int id, Mat cur_frame) {
            ptr_detector->run(frame_id, frame_list, rects, id, cur_frame);
        }

        Faces track() {
            Faces out_faces = ptr_tracker->tracking(frame_id, frame_list, rects, -1);
            pthread_mutex_lock(&det_mid);
            if (rects.size()>=5) {
                LOG(INFO) << "clearing deque ...";
                frame_id.pop_front();
                frame_list.pop_front();
                rects.pop_front();
            }
            pthread_mutex_unlock(&det_mid);
            return out_faces;
        }

        void reset_track(int id) {
            ptr_tracker->reset_track_by_id(id);
        }

        void addToDatabase(string input_filename, string ID) {
            ptr_faceid->addToDatabase(ptr_detector, ptr_feat, input_filename, ID);
        }

//std::vector<std::string> match(std::vector<cv::Mat>& detected_faces) {
//            return ptr_faceid->matchID(ptr_feat, detected_faces);
//        }

        std::vector<std::string> match(std::vector<FaceGroup>& detected_faces) {
            //pthread_create(&tid, NULL, raintime_thread_wrapper, (void*)(this));
            //pthread_detach(tid);
            return ptr_faceid->matchID(ptr_feat, detected_faces);
        }


    private:
        std::deque<int> frame_id;
        std::deque<Mat> frame_list;
        std::deque<std::vector<BoundingBox>> rects;
        Faces tracked_faces;

        pthread_mutex_t det_mid, matcher_mid, data_mid;
        pthread_t tid;


        CrDetector* ptr_detector;
        Tracker* ptr_tracker;
        CrFeature* ptr_feat;
        CrFaceID* ptr_faceid;

        bool matcher_running = false;


};

}

#endif
