/**
 * File: demo_sift.cpp
 * Date: November 2011
 * Author: Dorian Galvez-Lopez
 * Description: demo application of DLoopDetector
 * License: see the LICENSE.txt file
 */

#include <iostream>
#include <vector>
#include <string>

// DLoopDetector and DBoW2
#include <DBoW2/DBoW2.h> // defines SiftVocabulary
#include "DLoopDetector.h" // defines SiftLoopDetector
#include <DUtilsCV/DUtilsCV.h> // defines macros CVXX

// OpenCV
#include <opencv/cv.h>
#include <opencv/highgui.h>
#if CV24
#include <opencv2/nonfree/features2d.hpp>
#endif

// Demo
#include "demoDetector.h"

using namespace DLoopDetector;
using namespace DBoW2;
using namespace std;

// ----------------------------------------------------------------------------

const char* keys =
"{h | help      | false | print this message                                         }"
"{q | query     | ./resources/images | path to the directory containing query images }"
"{v | vocName   | ../vocNdb/vocCapitoleDI.yml.gz | name of the vocabulary file       }"
"{b | dbName    | ../vocNdb/dbCapitoleDI.yml.gz  | name of the database file         }"
"{p | poseFile  | ./resources/pose.txt              | path to the pose file          }"
"{W | width     | 640 | images width                                                 }"
"{H | height    | 480 | images height                                                }"
;

// ----------------------------------------------------------------------------

/// This functor extracts SIFT descriptors in the required format
class SiftExtractor: public FeatureExtractor<FSift::TDescriptor>
{
public:
  /**
   * Extracts features from an image
   * @param im image
   * @param keys keypoints extracted
   * @param descriptors descriptors extracted
   */
  virtual void operator()(const cv::Mat &im,
    vector<cv::KeyPoint> &keys, vector<vector<float> > &descriptors) const;
};

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

int main(int argc, const char **argv)
{
  cv::CommandLineParser parser(argc, argv, keys);
  if (parser.get<bool>("h") || parser.get<bool>("help"))
  {
    parser.printParams();
    return EXIT_SUCCESS;
  }

  parser.printParams();
  static string VOC_FILE = parser.get<string>("v");
  static string DB_FILE = parser.get<string>("b");
  static string IMAGE_DIR = parser.get<string>("q");
  static string  POSE_FILE = parser.get<string>("p");

  static const int IMAGE_W = parser.get<int>("W");
  static const int IMAGE_H = parser.get<int>("H");

  // prepares the demo
  demoDetector<SiftVocabulary, SiftLoopDetector, FSift::TDescriptor>
    demo(VOC_FILE, IMAGE_DIR, POSE_FILE, IMAGE_W, IMAGE_H);

  try
  {
    // run the demo with the given functor to extract features
    SiftExtractor extractor;
    demo.run("SIFT", extractor, DB_FILE);
  }
  catch(const std::string &ex)
  {
    cout << "Error: " << ex << endl;
  }

  return 0;
}

// ----------------------------------------------------------------------------

void SiftExtractor::operator() (const cv::Mat &im,
  vector<cv::KeyPoint> &keys, vector<vector<float> > &descriptors) const
{
  // extract surfs with opencv
  static cv::SIFT sift_detector(15000, 3, 0.04, 10, 1.6 );
  keys.clear(); // opencv 2.4 does not clear the vector
  cv::Mat plain;
  cv::Mat mask;
  sift_detector(im, mask, keys, plain);

  // change descriptor format
  descriptors.resize(keys.size());
  unsigned int j = 0;
  for (unsigned int i = 0; i < keys.size(); ++i)
  {
    descriptors[i].resize(128);
    plain.row(i).copyTo(descriptors[i]);
//    descriptors[j].assign(plain.datastart + i, plain.datastart + i + 128);
  }

  // Root SIFT
  for (unsigned int j = 0; j < descriptors.size(); ++j)
  {
    float sum = 0;
    for (unsigned int i = 0; i < 128; ++i)
    {
      sum += descriptors[j][i];
    }
    for (unsigned int k = 0; k < 128; ++k)
    {
      descriptors[j][k] = sqrt(descriptors[j][k]/sum);
    }
  }
}

// ----------------------------------------------------------------------------

