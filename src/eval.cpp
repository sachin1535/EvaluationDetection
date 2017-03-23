#include "eval.h"
#include <iostream>
#include <typeinfo>
//Collection* Collection::instance = nullptr; 
const int kdTreeLeafMax = 2;
const int nnMaxResults = 1;
EvalDetectPerformance::EvalDetectPerformance(std::string filenameGT, std::string filenameDT) : fileGT(filenameGT), fileDT(filenameDT){
   std::cout<<"filenameDT"<<filenameDT<<std::endl;
   std::cout<<"filenameGT"<<filenameGT<<std::endl;
}

EvalDetectPerformance::~EvalDetectPerformance() {
}


void EvalDetectPerformance::readData()
{
    std::string line;
    blobGTInfo gtdata;
    blobDTInfo dtdata;
    //Extracting ground truth data 
    while(std::getline(fileGT, line))
    {
        std::istringstream iss(line);
        if (!(iss >> gtdata.trackID >> gtdata.xmin >> gtdata.ymin >> gtdata.xmax >> gtdata.ymax >> gtdata.frameno >> gtdata.inscene >> gtdata.occluded >> gtdata.interpolated >> gtdata.label))
        {
            std::cout<<"file correcpted"<<std::endl;
        }
        else
        {
            if(gtDict.find(gtdata.frameno) != gtDict.end())
            {
                std::vector<blobGTInfo> firstgt;
                firstgt.push_back(gtdata);
                gtDict[gtdata.frameno] = firstgt;
            }
            else
                gtDict[gtdata.frameno].push_back(gtdata);
        }

    }
    //extracting detection data 
    while(fileDT >> dtdata.detections >> dtdata.blobID >> dtdata.xmin >> dtdata.ymin >> dtdata.xmax >> dtdata.ymax >> dtdata.area >> dtdata.frameno)
    {
        if(dtDict.find(dtdata.frameno) == dtDict.end() && dtdata.detections != 0)
        {
            std::vector<blobDTInfo> firstdt;
            firstdt.push_back(dtdata);
            dtDict[dtdata.frameno] = firstdt;
            totalFrames += 1;
        }
        else
        {
            if(dtdata.detections != 0)
            {
                dtDict[dtdata.frameno].push_back(dtdata);
                if(gtDict.find(dtdata.frameno) == gtDict.end())
                    falsePos = falsePos + 1;
            }
            
        }
                    
        
    }
  
    
}
bool findOverlap(blobGTInfo gtRect,blobDTInfo dtRect)
{
    cv::Rect r1(gtRect.xmin,gtRect.ymin,std::abs(gtRect.xmax - gtRect.xmin),std::abs(gtRect.ymax - gtRect.ymin));

    cv::Rect r2(dtRect.xmin,dtRect.ymin,std::abs(dtRect.xmax - dtRect.xmin),std::abs(dtRect.ymax - dtRect.ymin));

    cv::Rect overlap = r1 & r2;
    cv::Rect combine = r1 | r2;
    return ((r1 & r2).area() > 0);
}
void EvalDetectPerformance::printResults()
{
    std::cout<<"The analysis Results are:"<<std::endl;
    std::cout<<"True Positives  "<< truePos <<std::endl;
    std::cout<<"False Positives  "<< falsePos <<std::endl;
    std::cout<<"False Negatives  "<< falseNeg <<std::endl;
    std::cout<<"Total frames  "<< totalFrames <<std::endl;

}
void EvalDetectPerformance::processData()
{
    try{


        for(auto keyval :gtDict)
        {
            std::vector<cv::Point2f> gtleftCorners;
            std::vector<cv::Point2f> dtleftCorners;
            // groudtruth left corner extraction
            for(blobGTInfo point : keyval.second )
            {
                gtleftCorners.push_back(cv::Point2f(point.xmin,point.ymin));
            }

            // detections left corner extraction
            if(dtDict.find(keyval.first) == dtDict.end())
            {
                falseNeg += gtleftCorners.size();
            }
            else
            {
                for(blobDTInfo point : dtDict[keyval.first] )
                {
                    dtleftCorners.push_back(cv::Point2f(point.xmin,point.ymin));
                }    
            }
            // calculating stats 
            int lengt = gtleftCorners.size();
            int lendt = dtleftCorners.size();
            // std::cout<<"lengt \t" <<lengt<<"\t lendt \t"<<lendt<<std::endl;
            if(lendt > lengt )
            {
                falsePos = falsePos + (lendt-lengt);
            }
            else
            {
                falseNeg = falseNeg+ (lengt-lendt);   
            }
            for(int i =0;i<gtleftCorners.size();i++)
            {
                if(dtleftCorners.size()==0)
                    break;
                cv::flann::KDTreeIndexParams indexParams(kdTreeLeafMax);
            
                // build KD Tree with corner points
                cv::flann::Index kdTree(cv::Mat(dtleftCorners).reshape(1), indexParams);

                std::vector<int> indices;
                std::vector<float> dists;
                // Find k nearest neighbors
                kdTree.knnSearch(cv::Matx12f(gtleftCorners[i].x, gtleftCorners[i].y), indices, dists, nnMaxResults);

                int index = indices[0];
                if(findOverlap(keyval.second[i],dtDict[keyval.first][index]))
                {
                    
                    truePos = truePos + 1;
                }
                //removing currently compared element 
                dtDict[keyval.first].erase(dtDict[keyval.first].begin()+index);
                dtleftCorners.erase(dtleftCorners.begin()+index);
            }

        
        }
    }catch(const std::exception& ex) {
        std::cerr << "Error occurred: " << ex.what() << std::endl;
    }
  
    
}