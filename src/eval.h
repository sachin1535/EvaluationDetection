#pragma once
#include <string>
#include <vector>
#include <unordered_map>
#include <opencv2/core.hpp>
#include <iostream>
#include <fstream>
#include <cmath>
#include <iostream>
#include <memory>
#include <iterator>
#include <algorithm>
#include <functional>
#include <opencv2/flann/flann.hpp>
struct blobGTInfo {
    int trackID; 
    int xmin;
    int ymin;
    int xmax;
    int ymax;
    int frameno;
    bool inscene;
    bool occluded;
    bool interpolated;
    std::string label;
};
struct blobDTInfo {
    int detections; 
    int blobID;
    int xmin;
    int ymin;
    int xmax;
    int ymax;
    float area;
    int frameno;
};

class EvalDetectPerformance {
public:
    //static Collection& get();

    ~EvalDetectPerformance();
    void readData();
    void processData(float ratio);
    void printResults();
    void reset();
    EvalDetectPerformance(std::string filenameGT,std::string filenameDT, std::string filenameRES);
    int totalFrames = 0;
    int totalGTBlobs = 0;
    int falsePos  = 0 ;
    int falseNeg = 0;
    int truePos = 0;
    int trueNeg = 0;
    int missMatches = 0;
    float sfda = 0.0;

private:
    static EvalDetectPerformance* instance;

    std::ifstream fileGT;
    std::ifstream fileDT;
    std::ofstream fileRES;
    std::unordered_map<int, std::vector<blobGTInfo> >gtDict;
    std::unordered_map<int, std::vector<blobDTInfo> > dtDict;
    
};
