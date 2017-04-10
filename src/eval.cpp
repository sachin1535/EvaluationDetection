#include "eval.h"
#include <iostream>
#include <typeinfo>
//Collection* Collection::instance = nullptr; 
const int kdTreeLeafMax = 2;
const int nnMaxResults = 1;
EvalDetectPerformance::EvalDetectPerformance(std::string filenameGT, std::string filenameDT, std::string filenameRES) : fileGT(filenameGT), fileDT(filenameDT) ,fileRES(filenameRES){
   std::cout<<"filenameDT"<<filenameDT<<std::endl;
   std::cout<<"filenameGT"<<filenameGT<<std::endl;
   std::cout<<"filenameRES"<<filenameRES<<std::endl;
   std::string line;
   std::getline(fileDT, line);
   //std::cout<<line<<std::endl;
}

EvalDetectPerformance::~EvalDetectPerformance() {
}
bool findOverlapGT(std::vector<blobGTInfo> gtRects, blobGTInfo gtRect2)
{
    for(blobGTInfo gtRect1:gtRects)
    {
        cv::Rect r1(gtRect1.xmin,gtRect1.ymin,std::abs(gtRect1.xmax - gtRect1.xmin),std::abs(gtRect1.ymax - gtRect1.ymin));
        cv::Rect r2(gtRect2.xmin,gtRect2.ymin,std::abs(gtRect2.xmax - gtRect2.xmin),std::abs(gtRect2.ymax - gtRect2.ymin));

        cv::Rect overlap = r1 & r2;
        cv::Rect combine = r1 | r2;
        // std::cout<<"ratio \t "<<(float(overlap.area()) / float(combine.area()))<<std::endl;
        if ((float(overlap.area()) / float(combine.area())) > 0.5)
            return true;     

    }
    return false;
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
                std::vector<blobGTInfo> firstgt = gtDict[gtdata.frameno];
                if(!findOverlapGT(firstgt,gtdata))
                {
                    firstgt.push_back(gtdata);
                    gtDict[gtdata.frameno] = firstgt;
                    if(firstgt.size()>2)
                        std::cout<<"Extra"<< firstgt.size() <<"Frame No:"<< gtdata.frameno<<std::endl;
                    totalGTBlobs++;      
                }
                
            }
            else{
                if(!gtdata.inscene)
                {
                    gtDict[gtdata.frameno].push_back(gtdata);
                    totalFrames += 1; 
                    totalGTBlobs++;   
                }
                
            }
        }

    }
    //extracting detection data 
    while(fileDT >> dtdata.detections >> dtdata.blobID >> dtdata.xmin >> dtdata.ymin >> dtdata.xmax >> dtdata.ymax >> dtdata.area >> dtdata.frameno)
    {
        if(dtDict.find(dtdata.frameno) != dtDict.end() && dtdata.detections > 0)
        {
            std::vector<blobDTInfo> firstdt = dtDict[dtdata.frameno];
            firstdt.push_back(dtdata);
            dtDict[dtdata.frameno] = firstdt;
            if(gtDict.find(dtdata.frameno) == gtDict.end())
                    falsePos = falsePos + 1;
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
float findOverlapThresh(blobGTInfo gtRect,blobDTInfo dtRect)
{
    cv::Rect r1(gtRect.xmin,gtRect.ymin,std::abs(gtRect.xmax - gtRect.xmin),std::abs(gtRect.ymax - gtRect.ymin));

    cv::Rect r2(dtRect.xmin,dtRect.ymin,std::abs(dtRect.xmax - dtRect.xmin),std::abs(dtRect.ymax - dtRect.ymin));

    cv::Rect overlap = r1 & r2;
    cv::Rect combine = r1 | r2;

    return (float(overlap.area()) / float(r1.area()));
}
float findOverlap(blobGTInfo gtRect,blobDTInfo dtRect)
{
    cv::Rect r1(gtRect.xmin,gtRect.ymin,std::abs(gtRect.xmax - gtRect.xmin),std::abs(gtRect.ymax - gtRect.ymin));

    cv::Rect r2(dtRect.xmin,dtRect.ymin,std::abs(dtRect.xmax - dtRect.xmin),std::abs(dtRect.ymax - dtRect.ymin));

    cv::Rect overlap = r1 & r2;
    cv::Rect combine = r1 | r2;

    return (float(overlap.area()) / float(combine.area()));
}
void EvalDetectPerformance::printResults()
{
    std::cout<<"Analysis Results: "<<std::endl;
    std::cout<<"True Positives\t "<< truePos <<std::endl;
    std::cout<<"missMatches\t"<< missMatches <<std::endl;
    std::cout<<"False Positives\t"<< falsePos <<std::endl;
    std::cout<<"False Negatives\t"<< falseNeg <<std::endl;
    std::cout<<"Total frames  with DT or GT valid\t"<< totalFrames <<std::endl;
    std::cout<<"Total GT Detections\t"<< totalGTBlobs <<std::endl;
    // Calculating the Precision and Recall measures based on the true postives and false positive values
    float precision = float(truePos)/float(truePos+falsePos);
    std::cout<<"Precision <TP/TP+FP> \t"<< precision <<std::endl;
    float recall = float(truePos)/float(truePos+falseNeg);
    std::cout<<"Recall  <TP/TP+FN>\t"<< recall <<std::endl;
    // Measure of FPR and FNR
    std::cout<<"False Positive Rate <FP/FP+TN>\t"<< float(falsePos)/float(falsePos) <<std::endl;
    std::cout<<"False Negative Rate <FN/FN+TP>\t"<< float(falseNeg)/float(truePos+falseNeg) <<std::endl;
    // F -measure 
    std::cout<<"F- Measure <2*precision*recall/precision+recall>\t"<< (2*precision*recall)/ (precision+recall) <<std::endl;

    fileRES<<"Analysis Results: "<<std::endl;
    fileRES<<"True Positives\t "<< truePos <<std::endl;
    fileRES<<"missMatches\t"<< missMatches <<std::endl;
    fileRES<<"False Positives\t"<< falsePos <<std::endl;
    fileRES<<"False Negatives\t"<< falseNeg <<std::endl;
    fileRES<<"Total frames  with DT or GT valid\t"<< totalFrames <<std::endl;
    fileRES<<"Total GT Detections\t"<< totalGTBlobs <<std::endl;
    // Calculating the Precision and Recall measures based on the true postives and false positive values

    fileRES<<"Precision <TP/TP+FP> \t"<< precision <<std::endl;

    fileRES<<"Recall  <TP/TP+FN>\t"<< recall <<std::endl;
    // Measure of FPR and FNR
    fileRES<<"False Positive Rate <FP/FP+TN>\t"<< float(falsePos)/float(falsePos) <<std::endl;
    fileRES<<"False Negative Rate <FN/FN+TP>\t"<< float(falseNeg)/float(truePos+falseNeg) <<std::endl;
    // F -measure 
    fileRES<<"F- Measure <2*precision*recall/precision+recall>\t"<< (2*precision*recall)/ (precision+recall) <<std::endl;

}
void EvalDetectPerformance::processData(float ratio)
{
    try{

        float sumFDAt = 0.0;
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
                float totalOvrLapRatio = 0.0;
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
                    float ovrLapThresh = findOverlapThresh(keyval.second[i],dtDict[keyval.first][index]);
                    if(ovrLapThresh >= ratio)
                    {
                        truePos = truePos + 1;
                        totalOvrLapRatio = totalOvrLapRatio +1.0;
                    }
                    else
                    {
                        missMatches++;
                        float ovrLap = findOverlap(keyval.second[i],dtDict[keyval.first][index]);
                        totalOvrLapRatio = totalOvrLapRatio + ovrLap; 
                    }

                    //removing currently compared element 
                    dtDict[keyval.first].erase(dtDict[keyval.first].begin()+index);
                    dtleftCorners.erase(dtleftCorners.begin()+index);
                }
                // Calculating the FDA values 
                float FDAt = totalOvrLapRatio / float((lendt+lengt)/2);
                sumFDAt += FDAt;
                //std::cout << "Frame\t"<<keyval.first<<"\tFDA\t"<<FDAt<<std::endl;    
                //fileRES << "Frame\t"<<keyval.first<<"\tFDA\t"<<FDAt<<std::endl;    
            }
            // fileRES << "SFDA normalised Value\t"<< (sumFDAt / float(totalFrames)) <<std::endl;    

        
        }
        std::cout << "SFDA normalised Value\t"<< (sumFDAt / float(totalFrames)) <<std::endl;
    }catch(const std::exception& ex) {
        std::cerr << "Error occurred: " << ex.what() << std::endl;
    }
  
    
}