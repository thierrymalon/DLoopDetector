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

//static const char *VOC_FILE = "./resources/surf64_k10L6.voc.gz";
static const char *VOC_FILE = "/home/terry/datasetsNQueries/ImageDataset_CapitoleTLS/output/vocCapitoleDI.yml.gz";
static const char *IMAGE_DIR = "./resources/images";
static const char *POSE_FILE = "./resources/pose.txt";
static const int IMAGE_W = 640; // image size
static const int IMAGE_H = 480;

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

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

int main()
{
  // prepares the demo
  demoDetector<SiftVocabulary, SiftLoopDetector, FSift::TDescriptor>
    demo(VOC_FILE, IMAGE_DIR, POSE_FILE, IMAGE_W, IMAGE_H);

  try
  {
    // run the demo with the given functor to extract features
    SiftExtractor extractor;
    demo.run("SIFT", extractor);
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
  descriptors.resize((plain.dataend - plain.datastart) / 128);
  unsigned int j = 0;
  for(unsigned int i = 0; i < plain.dataend - plain.datastart; i += 128, ++j)
  {
    descriptors[j].resize(128);
    descriptors[j].assign(plain.datastart + i, plain.datastart + i + 128);
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

