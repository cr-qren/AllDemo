#ifndef _FeatExt_Matcher_H
#define _FeatExt_Matcher_H

#include <dlib/dnn.h>
#include <dlib/string.h>
#include <dlib/image_io.h>
#include <dlib/matrix.h>
#include <dlib/opencv.h>

#include <sys/time.h>
#include <typeinfo>
#include <opencv2/opencv.hpp>
#include <string>
#include <iostream>
#include <vector>
#include <unordered_map>

#include "Detector/CrDetector.hpp"

#include <dlib/clustering.h>
#include <dlib/image_io.h>
#include <dlib/image_processing/frontal_face_detector.h>
#include <dlib/gui_widgets.h>

namespace FaceSDK {

  using namespace dlib;
  using namespace std;
  using namespace cv;



  /*******************************************************************************
  // extract feature using resnet-34
  Class CrFeature function: void extract_feature()
  three arguments needed:
  std::vector<Mat> input_face ---> input face of objects
  anet_type net ---> resnet-34 model
  std::vector<matrix<float,0,1>> face_feature ---> variable storing faces' features
   *******************************************************************************/
  class CrFeature
  {
  private:
    template <template <int,template<typename>class,int,typename> class block, int N, template<typename>class BN, typename SUBNET>
      using residual = add_prev1<block<N,BN,1,tag1<SUBNET>>>;

    template <template <int,template<typename>class,int,typename> class block, int N, template<typename>class BN, typename SUBNET>
      using residual_down = add_prev2<avg_pool<2,2,2,2,skip1<tag2<block<N,BN,2,tag1<SUBNET>>>>>>;

    template <int N, template <typename> class BN, int stride, typename SUBNET>
      using block  = BN<con<N,3,3,1,1,relu<BN<con<N,3,3,stride,stride,SUBNET>>>>>;

    template <int N, typename SUBNET> using ares      = relu<residual<block,N,affine,SUBNET>>;
    template <int N, typename SUBNET> using ares_down = relu<residual_down<block,N,affine,SUBNET>>;

    template <typename SUBNET> using alevel0 = ares_down<256,SUBNET>;
    template <typename SUBNET> using alevel1 = ares<256,ares<256,ares_down<256,SUBNET>>>;
    template <typename SUBNET> using alevel2 = ares<128,ares<128,ares_down<128,SUBNET>>>;
    template <typename SUBNET> using alevel3 = ares<64,ares<64,ares<64,ares_down<64,SUBNET>>>>;
    template <typename SUBNET> using alevel4 = ares<32,ares<32,ares<32,SUBNET>>>;

    using anet_type = loss_metric<fc_no_bias<128,avg_pool_everything<
      alevel0<
      alevel1<
      alevel2<
      alevel3<
      alevel4<
      max_pool<3,3,2,2,relu<affine<con<32,7,7,2,2,
      input_rgb_image_sized<150>
        >>>>>>>>>>>>;

    std::vector<matrix<float,0,1>> face_feature;
    matrix<rgb_pixel> face;

    //add 10:28
    shape_predictor sp;

    Size size;
    cv::Mat resizeImg;
    anet_type net;

  public:
    CrFeature(string net_def_path, string landmark_path)
    {
      LOG(INFO)<<"Init FeatExtractor";
      size = Size(150, 150);
      //deserialize("/home/chao/cpp_proj/faceDemo/build/dlib_face_recognition_resnet_model_v1.dat") >> (*net);
      deserialize(net_def_path.c_str()) >> net;
      deserialize(landmark_path.c_str()) >> sp;
      LOG(INFO)<<"Init FeatExtractor Finish";
    }
    ~CrFeature() {
      //LOG(INFO) << "CrFeat dealloc";
    }

    void extract_feature(std::vector<Mat>& input_face, std::vector<matrix<float,0,1>>& face_feature)
    {

      std::vector<matrix<rgb_pixel>> faces;
      for (unsigned int i = 0; i < input_face.size(); i++)
      {
        cv::resize(input_face[i], resizeImg, size);
        assign_image(face, cv_image<rgb_pixel>(resizeImg));
        faces.push_back(move(face));
      }
      face_feature = net(faces);
      //cout <<  face_feature[0] << endl;
    }

    void extract_feature(std::vector<FaceGroup>& input_face, std::vector<matrix<float,0,1>>& face_feature)
    {
      std::vector<matrix<rgb_pixel>> faces;
      matrix<rgb_pixel> face_chip;
      cv::Mat image_border;
      // Scalar value(0, 0, 0);
      for (unsigned int i = 0; i < input_face.size(); i++)
      {
        int height = input_face[i].faceImage.rows;
        int width = input_face[i].faceImage.cols;

        copyMakeBorder(input_face[i].faceImage, image_border, height, height, width, width, 0);

        long left = width;
        long top = height;
        long right = 2*width;
        long bottom = 2*height;
        dlib::rectangle coord = dlib::rectangle(left, top, right, bottom);
        // cout << "coordinate: " << coord << endl;
        assign_image(face, cv_image<rgb_pixel>(image_border));
        auto shape = sp(face, coord);
        extract_image_chip(face, get_face_chip_details(shape,150,0.25), face_chip);
        faces.push_back(move(face_chip));
      }
      face_feature = net(faces);
      //     std::vector<matrix<rgb_pixel>> faces;
      //     for (unsigned int i = 0; i < input_face.size(); i++)
      //     {
      //       cv::resize(input_face[i].faceImage.faceImage, resizeImg, size);
      //       assign_image(face, cv_image<rgb_pixel>(resizeImg));
      //       faces.push_back(move(face));
      //     }
      //     face_feature = net(faces);
      //cout <<  face_feature[0] << endl;
    }


  };

  /*****************************************************************************************************************
  // identify people in videos
  Class CrFaceID function: void searchMaxSimilarity()
  three arguments needed:
  unordered_map<string, std::vector<matrix<float, 0, 1>>> idDatabase ---> face identification database vector<128, 1>
  std::vector<matrix<float,0,1>> faces ---> detected faces feature
  std::vector<string> belong_id --->  ids that each face belong to
   *****************************************************************************************************************/
  class CrFaceID
  {
  private:
    // each face's similarity compared with ID database
    //unordered_map<float, string> sim_id;
    std::vector<pair<string, float> > id_dist;

    unordered_map<string, std::vector<matrix<float, 0, 1>>> idDatabase;

    float dot, denom1, denom2, max_sim_ofdatabase, maxsim_per_id1,similarity, threshold;

    // calculate cosine similarity between each face and database
    float cosine_similarity(matrix<float, 0, 1>& c1, std::vector<matrix<float, 0, 1>>& database)
    {
      maxsim_per_id1 = 0.0;
      for(unsigned int k = 0 ; k < database.size(); k++)
      {
        denom1 = 0.0;
        denom2 = 0.0 ;
        dot = trans(c1)*database[k];
        for(unsigned int i = 0; i < c1.nr(); ++i)
        {
          denom1 += c1(i) * c1(i) ;
          denom2 += database[k](i) * database[k](i) ;
        }
        similarity = dot / (sqrt(denom1) * sqrt(denom2));
        if(maxsim_per_id1 < similarity)
        {
          maxsim_per_id1 = similarity;
        }
      }
      return maxsim_per_id1;
    }
    void euclidean_distance(matrix<float, 0, 1>& m, const string& id, std::vector<matrix<float, 0, 1>>& database)
    {
      for(unsigned int i = 0 ; i < database.size(); i++)
      {
        //float mean_data = mean(database[k]);
        auto distance = length(m - database[i]);
        if(distance <= threshold)
        {
          id_dist.push_back(make_pair(id, distance));
        }
      }
    }

    void searchMinDistance(std::vector<string>& belong_id)
    {
      int max_freq = 0;
      std::vector<string> max_freq_id;
      // search top-k min distance
      int k = 3;
      std::map<string, int> id_freq;
      if(id_dist.empty())
      {
        belong_id.push_back("None");
      }
      else
      {
        // important to compare size
        if(k > id_dist.size())
        {
          k = id_dist.size();
        }
        sort(id_dist.begin(), id_dist.end(), [](pair<string, float> a, pair<string, float> b){
             return a.second <= b.second;
             });
        // calculate frequency of each id in top-k id
        for(int i = 0; i < k ; i++)
        {	
          if(id_freq.find(id_dist[i].first) == id_freq.end())
            id_freq[id_dist[i].first] = 1;
          else
            id_freq[id_dist[i].first]++;
        }
        // the max frequency
        for(auto iter = id_freq.begin(); iter != id_freq.end(); iter++)
        {
          if(iter->second > max_freq)
          {
            max_freq = iter->second;
          }
        }
        // search the corresponding ids of max frequency
        for(auto it = id_freq.begin(); it != id_freq.end(); it++)
        {
          if (it->second == max_freq)
          {
            max_freq_id.push_back(it->first);
          }
        }
        if(max_freq_id.size()==1)
        {
          belong_id.push_back(max_freq_id[0]);
        }
        // search the id of minimum_distance while several ids has same frequencies
        // else
        // {
        // 	for(int j = 0; j < k; j++)
        // 	{
        // 		if (find(max_freq_id.begin(), max_freq_id.end(), id_dist[j].first) != max_freq_id.end())
        // 		{
        // 			belong_id.push_back(id_dist[j].first);
        // 			break;
        // 		}
        // 	}
        // }

        // serach the id of minimum average distance while several ids has same frequencies 
        else
        {
          int min_dis = 100;
          string id;
          std::map<string, float> id_mindistance;
          for(unsigned int index = 0; index < max_freq_id.size(); index++)
          {
            id_mindistance[max_freq_id[index]] = 0.0;
            for(unsigned int index2 = 0 ; index2 < id_dist.size(); index2++)
            {
              if(max_freq_id[index] == id_dist[index2].first)
              {
                id_mindistance[max_freq_id[index]] += id_dist[index2].second;
              }		    						
            }
            if((id_mindistance[max_freq_id[index]] / max_freq) < min_dis)
            {
              min_dis = id_mindistance[max_freq_id[index]] / max_freq;
              id = max_freq_id[index];
            }
          }
          belong_id.push_back(id);
        }
      }
      //LOG(INFO) << "min dist: " << min_dis_database;
    }



    // search corresponding id of max similarity
    void searchMaxSimilarity(unordered_map<float, string>& sim, std::vector<string>& belong_id)
    {
      max_sim_ofdatabase = 0.0;
      unordered_map<float, string>::iterator j;
      for (j = sim.begin(); j != sim.end(); j++)
      {
        if( j->first >= max_sim_ofdatabase)
        {
          max_sim_ofdatabase = j->first;
        }
      }
      if (max_sim_ofdatabase >= threshold)
      {
        LOG(INFO) << "similarity" << max_sim_ofdatabase;
        j = sim.find(max_sim_ofdatabase);
        belong_id.push_back(j->second);
      }
      else
      {
        belong_id.push_back("None");
      }
    }

  public:
    CrFaceID() {
      threshold = 0.42;
      LOG(INFO)<<"Init CrFaceID";}
    ~CrFaceID() {}
    // idDatabase -- face identification database vector<128, 1>
    // faces -- detected faces feature
    // belong_id -- the vector containts id that each face belong to
    std::vector<std::string> matchID(FaceSDK::CrFeature* ptr_feat, std::vector<FaceGroup>& input_img_array)
    {
      if (input_img_array.size() <=0) return std::vector<std::string>();
      std::vector<matrix<float,0,1>> faces;
      std::vector<string> belong_id;
      ptr_feat->extract_feature(input_img_array, faces);
      LOG(INFO) << "face numbers: " << faces.size();
      for (unsigned int faceIndex = 0; faceIndex < faces.size(); faceIndex++)
      {

        for (auto it = idDatabase.begin(); it != idDatabase.end(); it++)
        {
          //maxsim_per_id2 = cosine_similarity(faces[faceIndex], it->second);
          // LOG(INFO) << "each similarity: " << maxsim_per_id2;
          euclidean_distance(faces[faceIndex], it->first, it->second);
        }
        // search the id of max similarity
        // searchMaxSimilarity(sim_id, belong_id);
        searchMinDistance(belong_id);
        id_dist.clear();
      }
      return belong_id;
    } 

    void addToDatabase(FaceSDK::CrDetector* ptr_det, FaceSDK::CrFeature* ptr_feat,
                       string input_filename, string ID) {
      Mat img = imread(input_filename.c_str());
      LOG(INFO) << "offline_det input size " << img.size();
      string img_name = ID;
      LOG(INFO) << "start detection";
      auto rects = ptr_det->offline_det(img);
      LOG(INFO) << "end detection";
      if (rects.size() <=0) LOG(ERROR) << "No face detected when adding to the dataset";

      Rect roi(rects[0].x, rects[0].y, rects[0].w, rects[0].h);
      LOG(INFO) << "Result roi: " << roi;
      Mat crop_img = img(roi).clone();

      std::vector<cv::Mat> input_img_array;
      input_img_array.push_back(crop_img);
      std::vector<matrix<float,0,1>> feature;
      ptr_feat->extract_feature(input_img_array, feature);

      idDatabase[img_name].push_back(feature[0]);

    }
  };

}

#endif
