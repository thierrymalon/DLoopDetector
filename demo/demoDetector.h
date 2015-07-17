/**
 * File: demoDetector.h
 * Date: November 2011
 * Author: Dorian Galvez-Lopez
 * Description: demo application of DLoopDetector
 * License: see the LICENSE.txt file
 */

#ifndef __DEMO_DETECTOR__
#define __DEMO_DETECTOR__

#include <iostream>
#include <vector>
#include <string>
#include <locale>
#include <dirent.h>

// OpenCV
#include <opencv/cv.h>
#include <opencv/highgui.h>

// DLoopDetector and DBoW2
#include <DBoW2/DBoW2.h>
#include "DLoopDetector.h"
#include <DUtils/DUtils.h>
#include <DUtilsCV/DUtilsCV.h>
#include <DVision/DVision.h>

using namespace DLoopDetector;
using namespace DBoW2;
using namespace std;

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

/// Generic class to create functors to extract features
template<class TDescriptor>
class FeatureExtractor
{
public:
  /**
   * Extracts features
   * @param im image
   * @param keys keypoints extracted
   * @param descriptors descriptors extracted
   */
  virtual void operator()(const cv::Mat &im,
    vector<cv::KeyPoint> &keys, vector<TDescriptor> &descriptors) const = 0;
};

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

/// @param TVocabulary vocabulary class (e.g: SiftVocabulary)
/// @param TDetector detector class (e.g: SiftLoopDetector)
/// @param TDescriptor descriptor class (e.g: vector<float> for SIFT)
template<class TVocabulary, class TDetector, class TDescriptor>
/// Class to run the demo
class demoDetector
{
public:

  /**
   * @param vocfile vocabulary file to load
   * @param imagedir directory to read images from
   * @param posefile pose file
   * @param width image width
   * @param height image height
   */
  demoDetector(const std::string &vocfile, const std::string &imagedir,
    const std::string &descdir, const std::string &posefile,
    int width, int height);

  ~demoDetector(){}

  /**
   * Runs the demo
   * @param name demo name
   * @param extractor functor to extract features
   */
  void run(const std::string &name,
    const FeatureExtractor<TDescriptor> &extractor,
    const std::string &db_path, int di_lvl, string LIST_FILE);

protected:

  /**
   * Reads the robot poses from a file
   * @param filename file
   * @param xs
   * @param ys
   */
  void readPoseFile(const char *filename, std::vector<double> &xs,
    std::vector<double> &ys) const;

protected:

  std::string m_vocfile;
  std::string m_imagedir;
  std::string m_descdir;
  std::string m_posefile;
  int m_width;
  int m_height;
};

// ---------------------------------------------------------------------------

template<class TVocabulary, class TDetector, class TDescriptor>
demoDetector<TVocabulary, TDetector, TDescriptor>::demoDetector
  (const std::string &vocfile, const std::string &imagedir,
   const std::string &descdir, const std::string &posefile,
   int width, int height)
  : m_vocfile(vocfile), m_imagedir(imagedir),
    m_descdir(descdir), m_posefile(posefile),
    m_width(width), m_height(height)
{
}

// ---------------------------------------------------------------------------

void storeImages(const char* imagesDirectory, vector<string>& imagesNames, bool desc)
{
    // images extensions
    vector<string> extensions;
    if (desc)
    {
        extensions.push_back("desc");
    }
    else
    {
        extensions.push_back("png");
        extensions.push_back("jpg");
    }

    DIR * repertoire = opendir(imagesDirectory);

    if ( repertoire == NULL)
    {
        cout << "The images directory: " << imagesDirectory
            << " cannot be found" << endl;
    }
    else
    {
        struct dirent * ent;
        while ( (ent = readdir(repertoire)) != NULL)
        {
            string file_name = ent->d_name;
            string extension = file_name.substr(file_name.find_last_of(".") +1);
            locale loc;
            for (std::string::size_type j = 0; j < extension.size(); j++)
            {
                extension[j] = std::tolower(extension[j], loc);
            }
            for (unsigned int i = 0; i < extensions.size(); i++)
            {
                if (extension == extensions[i])
                {
                    imagesNames.push_back(imagesDirectory+file_name);
                }
            }
        }
    }
    closedir(repertoire);
}

// ---------------------------------------------------------------------------

std::vector<float> readDescFromBinFile(const char* path)
{
    fstream fs;
    size_t ndesc;

    // Open file and get the number of descriptors
    fs.open(path, ios::in | ios::binary);

    // get the number of descriptors
    fs.read((char*) &ndesc, sizeof(size_t));

    std::vector<float> res;

    // Fill the matrix in the air
    for (int i = 0; i < ndesc*128; i++)
    {
        float cur;
        fs.read((char*) &cur, sizeof(float));
        res.push_back(cur);
    }

    // Close file and return
    fs.close();
    return res;
}

// ---------------------------------------------------------------------------

vector<string> readFileList(const char* path)
{
    vector<string> res;
    fstream fs;

    // Load file
    fs.open(path, ios::in);
    if(!fs.is_open())
    {
        std::cout << "Error when opening directory" << std::endl;
        return res;
    }

    // Fill the vector
    string s;
    while (fs >> s)
    {
        res.push_back(s);
    }

    // Close file and return
    fs.close();
    return res;
}

// ---------------------------------------------------------------------------

vector<cv::KeyPoint> readFeatFromFile(const char* path)
{
    vector<cv::KeyPoint> res;
    fstream fs;

    // Load file
    fs.open(path, ios::in);
    if(!fs.is_open())
    {
        std::cout << "Error when opening directory" << std::endl;
        return res;
    }

    // Fill the vector
    float x, y, size, angle;
    while (fs >> x >> y >> size >> angle)
    {
        res.push_back(cv::KeyPoint(x,y,size,angle));
    }

    // Close file and return
    fs.close();
    return res;
}

// ---------------------------------------------------------------------------

void storeDescFeat(const char* directory, vector<std::string>& names, bool desc)
{
    // image extensions or .desc
    std::vector<std::string> extensions;
    if (desc)
    {
        extensions.push_back("desc");
    }
    else
    {
        extensions.push_back("feat");
    }

    DIR * repertory = opendir(directory);

    if (repertory == NULL)
    {
        std::cout << "The images directory " << directory
            << " cannot be found" << std::endl;
    }
    else
    {
        struct dirent * ent;
        while ( (ent = readdir(repertory)) != NULL)
        {
            std::string file_name = ent->d_name;
            std::string extension = file_name.substr(file_name.find_last_of(".")+1);
            locale loc;
            for (std::string::size_type j = 0; j < extension.size(); ++j)
            {
                extension[j] = std::tolower(extension[j], loc);
            }
            for (unsigned int i = 0; i < extensions.size(); ++i)
            {
                if (extension == extensions[i])
                {
                    names.push_back(file_name);
                }
            }
        }
    }
}

// ---------------------------------------------------------------------------

template<class TVocabulary, class TDetector, class TDescriptor>
void demoDetector<TVocabulary, TDetector, TDescriptor>::run
  (const std::string &name, const FeatureExtractor<TDescriptor> &extractor,
   const std::string &db_path, int di_lvl, string LIST_FILE)
{
  cout << "DLoopDetector Demo" << endl
    << "Dorian Galvez-Lopez" << endl
    << "http://doriangalvez.com" << endl << endl;

  std::vector<std::string> descNames;
  std::vector<std::string> featNames;
  storeDescFeat(m_descdir.c_str(), descNames, true);
  storeDescFeat(m_descdir.c_str(), featNames, false);
  if (descNames.size() == 0)
  {
      std::cout << "There is no .desc file in the directory " << m_descdir
          << "... The program cannot work." << std::endl;
      return;
  }

  if (descNames.size() != featNames.size())
  {
    std::cout << "There are " << descNames.size() << " .desc files whereas there are "
        << featNames.size() << " .feat files... The program cannot work." << std::endl;
    return;
  }

  std::vector<unsigned int> mapping;
  std::vector<std::string> originNames = readFileList(LIST_FILE.c_str());

  if (originNames.size() != descNames.size())
  {
    std::cout << "There are " << descNames.size() << " .desc files whereas there are "
        << originNames.size() << " images listed in the list file... The program cannot work." << std::endl;
  }

  mapping.resize(originNames.size());

  for (unsigned int i = 0; i < originNames.size();++i)
  {
    string fileName = originNames[i].substr(0,originNames[i].find_last_of("."));
    string fileName2;
    bool found = false;
    for (unsigned int j = 0; j < originNames.size(), !found; ++j)
    {
      fileName2 = descNames[j].substr(0,descNames[i].find_last_of("."));
      if (fileName == fileName2)
      {
        found = true;
        mapping[i] = j;
      }
    }
    if (!found)
    {
      std::cout << "There is no descriptor associated to the file " << originNames[i]
        << "... The program cannot work." << std::endl;
      return;
    }
  }

//  std::cout << "Mapping finished : " << std::endl;
//  for (unsigned int i = 0; i < mapping.size(); ++i)
//  {
//    std::cout << "mapping[" << i << "] = " << mapping[i] << std::endl;
//  }

  // Set loop detector parameters
  typename TDetector::Parameters params(m_height, m_width);

  // Parameters given by default are:
  // use nss = true
  // alpha = 0.3
  // k = 3
  // geom checking = GEOM_DI
  // di levels = 0

  // We are going to change these values individually:
  params.use_nss = true; // use normalized similarity score instead of raw score
  params.alpha = 0.3; // nss threshold
  params.k = 1; // a loop must be consistent with 1 previous matches
  params.geom_check = GEOM_DI; // use direct index for geometrical checking
  params.di_levels = di_lvl; // use two direct index levels

  // To verify loops you can select one of the next geometrical checkings:
  // GEOM_EXHAUSTIVE: correspondence points are computed by comparing all
  //    the features between the two images.
  // GEOM_FLANN: as above, but the comparisons are done with a Flann structure,
  //    which makes them faster. However, creating the flann structure may
  //    be slow.
  // GEOM_DI: the direct index is used to select correspondence points between
  //    those features whose vocabulary node at a certain level is the same.
  //    The level at which the comparison is done is set by the parameter
  //    di_levels:
  //      di_levels = 0 -> features must belong to the same leaf (word).
  //         This is the fastest configuration and the most restrictive one.
  //      di_levels = l (l < L) -> node at level l starting from the leaves.
  //         The higher l, the slower the geometrical checking, but higher
  //         recall as well.
  //         Here, L stands for the depth levels of the vocabulary tree.
  //      di_levels = L -> the same as the exhaustive technique.
  // GEOM_NONE: no geometrical checking is done.
  //
  // In general, with a 10^6 vocabulary, GEOM_DI with 2 <= di_levels <= 4
  // yields the best results in recall/time.
  // Check the T-RO paper for more information.
  //

  // Load the vocabulary to use
  cout << "Loading " << name << " vocabulary..." << endl;
  SiftVocabulary voc(m_vocfile);

//  SiftDatabase db;
//  db.load("/home/terry/datasetsNQueries/ImageDataset_CapitoleTLS/output/dbCapitoleDI.yml.gz");
//  db.load(db_path);

  // Initiate loop detector with the vocabulary
  cout << "Processing sequence..." << endl;
  TDetector detector(voc, params);
//  detector.setDatabase(db);
  detector.m_database = new SiftDatabase();
  detector.m_database->load(db_path);
  std::cout << *detector.m_database << std::endl;

  // load image filenames

  vector<string> filenames, datanames;
  storeImages((m_imagedir+"/").c_str(), filenames, false);
  std::string ppp = "~/datasetsNQueries/ImageDataset_CapitoleTLS/images/";
  storeImages(ppp.c_str(), datanames, true);

//  std::cout << std::endl;
//  for (unsigned int i = 0; i < filenames.size(); ++i)
//    std::cout << "filenames[" << i << "] = " << filenames[i] << std::endl;
//
//  std::cout << std::endl;
//  for (unsigned int i = 0; i < datanames.size(); ++i)
//    std::cout << "datanames[" << i << "] = " << datanames[i] << std::endl;

  std::cout << std::endl;

//  vector<string> filenames =
//    DUtils::FileFunctions::Dir(m_imagedir.c_str(), ".JPG", true);
//
//  if (filenames.size() == 0) {
//    filenames = DUtils::FileFunctions::Dir(m_imagedir.c_str(), ".png", true);
//  }

  // load robot poses
  vector<double> xs, ys;
  readPoseFile(m_posefile.c_str(), xs, ys);

  // prepare profiler to measure times
  DUtils::Profiler profiler;

  int count = 0;

  detector.m_image_descriptors.resize(descNames.size());
  detector.m_image_keys.resize(featNames.size());
  for (unsigned int i = 0; i < descNames.size(); ++i)
  {
      std::string pathFeat = m_descdir + "/" + featNames[i];
      detector.m_image_keys[i] = readFeatFromFile(pathFeat.c_str());
      std::string pathDesc = m_descdir + "/" + descNames[i];
      std::vector<float> descriptors = readDescFromBinFile(pathDesc.c_str());
      std::cout << "Descriptor " << i << ": " << descNames[i]  << " size is " << descriptors.size()/128 << std::endl;
      detector.m_image_descriptors[i].resize(descriptors.size()/128);
      for (unsigned int j1 = 0; j1 < descriptors.size()/128; ++j1)
      {
          detector.m_image_descriptors[i][j1].resize(128);
          for (unsigned int j2 = 0; j2 < 128; ++j2)
          {
              detector.m_image_descriptors[i][j1][j2] = descriptors[128*j1+j2];
          }
      }
  }

  std::cout << std::endl;

//    const FeatureVector &oldvec = detector.m_database->retrieveFeatures(i);
//    const FeatureVector::const_iterator old_end = oldvec.end();
//    FeatureVector::const_iterator old_it = oldvec.begin();
//    while (old_it != old_end)
//    {
//      for (unsigned int j = 0; j < old_it->second.size(); j++)
//      {
//        std::cout << j << "/" << old_it->second.size() << std::endl;
//        detector.m_image_descriptors[i].resize(15000);
//        std::cout << "detector.m_image_descriptors[i].size() = " << detector.m_image_descriptors[i].size() <<" taken in " <<  old_it->second[j] << " took the value of word " <<old_it->first << std::endl;
//        std::cout << *detector.m_database << std::endl;
//        detector.m_image_descriptors[i][old_it->second[j]] = detector.m_database->getVocabulary()->getWordDI(old_it->first);
//      }
//      ++old_it;
//    }
//  }

  // go
  for(unsigned int i = 0; i < filenames.size(); ++i)
  {
    cout << "Adding image " << i << ": " << filenames[i] << "... " << endl;

    // get image
    std::cout << filenames[i] << std::endl;
    cv::Mat im = cv::imread(filenames[i].c_str(), 0); // grey scale

    // Process images
    vector<cv::KeyPoint> keys;
    vector<TDescriptor> descriptors;

    // get features
    profiler.profile("features");
    cout << "Extraction" << endl;
    extractor(im, keys, descriptors);
    profiler.stop();

    // add image to the collection and check if there is some loop
    DetectionResult result;

    profiler.profile("detection");
    cout << "Loop detection" << endl;
    detector.detectLoopDB(keys, descriptors, result, mapping);
    profiler.stop();

    if(result.detection())
    {
      cout << "- Loop found with image " << result.match << "!"
        << endl;
      ++count;
    }
    else
    {
      cout << "- No loop: ";
      switch(result.status)
      {
        case CLOSE_MATCHES_ONLY:
          cout << "All the images in the database are very recent" << endl;
          break;

        case NO_DB_RESULTS:
          cout << "There are no matches against the database (few features in"
            " the image?)" << endl;
          break;

        case LOW_NSS_FACTOR:
          cout << "Little overlap between this image and the previous one"
            << endl;
          break;

        case LOW_SCORES:
          cout << "No match reaches the score threshold (alpha: " <<
            params.alpha << ")" << endl;
          break;

        case NO_GROUPS:
          cout << "Not enough close matches to create groups. "
            << "Best candidate: " << result.match << endl;
          break;

        case NO_TEMPORAL_CONSISTENCY:
          cout << "No temporal consistency (k: " << params.k << "). "
            << "Best candidate: " << result.match << endl;
          break;

        case NO_GEOMETRICAL_CONSISTENCY:
          cout << "No geometrical consistency. Best candidate: "
            << result.match << endl;
          break;

        default:
          break;
      }
    }

    cout << endl;

  }

  if(count == 0)
  {
    cout << "No loops found in this image sequence" << endl;
  }
  else
  {
    cout << count << " loops found in this image sequence!" << endl;
  }

  cout << endl << "Average execution time:" << endl
    << " - Feature computation: " << profiler.getMeanTime("features") * 1e3
    << " ms/image" << endl
    << " - Loop detection: " << profiler.getMeanTime("detection") * 1e3
    << " ms/image" << endl;
}

// ---------------------------------------------------------------------------

template<class TVocabulary, class TDetector, class TDescriptor>
void demoDetector<TVocabulary, TDetector, TDescriptor>::readPoseFile
  (const char *filename, std::vector<double> &xs, std::vector<double> &ys)
  const
{
  xs.clear();
  ys.clear();

  fstream f(filename, ios::in);

  string s;
  double ts, x, y, t;
  while(!f.eof())
  {
    getline(f, s);
    if(!f.eof() && !s.empty())
    {
      sscanf(s.c_str(), "%lf, %lf, %lf, %lf", &ts, &x, &y, &t);
      xs.push_back(x);
      ys.push_back(y);
    }
  }

  f.close();
}

// ---------------------------------------------------------------------------

#endif

