#include <cstdlib>
#include <string>
#include <memory>
#include <iostream>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <opencv2/core.hpp>
#include <opencv2/highgui.hpp>

#include "eval.h"
#include <opencv2/calib3d.hpp>


void printUsage(std::string progName) {
    std::cout << "Usage: " << progName << " <id> [path]" << std::endl;
    std::cout << std::endl;
    std::cout << "Where [path] is optional and can be an IP address, a URL, or a video file." << std::endl;
    std::cout << "If [path] is an IP address, the correct URL will be guessed based on the camera ID." << std::endl;
    std::cout << "If [path] is omitted, camera parameters will be read from the configuration file." << std::endl;
}
/*
std::string parseURL(int camId, std::string arg) {
    in_addr ipaddr;
    if(inet_pton(AF_INET, arg.c_str(), &ipaddr) == 1) {
        Config& config = Config::get();
        if(config.isCameraDefined(camId)) {
            CameraInfo camInfo = config.getCameraInfo(camId);
            CameraClass classInfo = config.getCameraClassInfo(camInfo.className);
            std::cout << "Using camera " << camId << " (" << camInfo.description << ")" << std::endl;
            std::string url("http://");
            url += classInfo.username + ":" + classInfo.password + "@";
            url += arg + "/" + classInfo.path;
            return url;
        } else {
            std::cout << "Using camera at address " << arg << std::endl;
            return std::string("http://admin:admin@") + arg + std::string("/video.cgi?.mjpg");
        }
    } else {
        std::cout << "Using file/URL " << arg << std::endl;
        return arg;
    }
}

std::string getURL(int camId) {
    CameraInfo camInfo = Config::get().getCameraInfo(camId);
    return parseURL(camId, camInfo.ip);
}
*/
int main(int argc, char *argv[]) {
    
    std::string filenameGT;
    std::string filenameDT;
    std::string pathres = "/home/mw4vision/Desktop/sachin/Evaluation/EvaluationDetectionPerformance/Results/DT/";
    float ratio= 0.0;
    // Read arguments and set parameters
    if(argc == 1) {
        // Defaults
        std::cout << "Using default video input" << std::endl;
        printUsage(argv[0]);
    } else if((argc == 2 && std::string(argv[1]) == "--help") || argc > 5) {
        printUsage(argv[0]);
        return 0;
    } else if(argc == 2) {
        printUsage(argv[0]);
    } else if(argc==3){
        filenameGT = argv[1];
        filenameDT = argv[2];
    } else{
        filenameGT = argv[1];
        filenameDT = argv[2];
        ratio = float(std::atoi(argv[3]))/100.0;

    }

    // Initialize system objects
  
  
    // Start video processing
    try {
            EvalDetectPerformance eval(filenameGT,filenameDT, pathres+ argv[4] +"_results");
            ratio = 0.1;
            for(int i=0;i<10;i++)
            {
                eval.readData();
                cv::waitKey(1000);
                eval.processData(ratio+float((i/10.0)));
                eval.printResults(); 
                eval.reset();
                cv::waitKey(1000);
            }
            

        /*    int key = cv::waitKey(10) & 0xFF;
            if(key == 27) { // Escape pressed
                break;
            }*/
        
    } catch(const std::exception& ex) {
        std::cerr << "Error occurred: " << ex.what() << std::endl;
        return 1;
    }
    return 0;
}
