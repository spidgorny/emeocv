/*
 * Config.cpp
 *
 */

#include <opencv2/highgui/highgui.hpp>

#include "Config.h"

Config::Config() :
        _flipHorizontal(false), _flipVertical(false), _rotationDegrees(0), _ocrMaxDist(
                5e5), _digitMinHeight(20), _digitMaxHeight(90), _digitYAlignment(
                10), _blurKernelSize(1), _cannyThreshold1(100), _cannyThreshold2(200),
				_mysqlHost("localhost"), _mysqlUser("root"), _mysqlPassword("root"),
				_mysqlDatabase("emeter"), _trainingDataFilename("trainctr.yml") {
}

void Config::saveConfig() {
    cv::FileStorage fs("config.yml", cv::FileStorage::WRITE);
    fs << "flipHorizontal" << _flipHorizontal;
    fs << "flipVertical" << _flipVertical;
    fs << "rotationDegrees" << _rotationDegrees;
    fs << "blurKernelSize" << _blurKernelSize;
    fs << "cannyThreshold1" << _cannyThreshold1;
    fs << "cannyThreshold2" << _cannyThreshold2;
    fs << "digitMinHeight" << _digitMinHeight;
    fs << "digitMaxHeight" << _digitMaxHeight;
    fs << "digitYAlignment" << _digitYAlignment;
    fs << "ocrMaxDist" << _ocrMaxDist;
    fs << "mysqlHost" << _mysqlHost;
    fs << "mysqlUser" << _mysqlUser;
    fs << "mysqlPassword" << _mysqlPassword;
    fs << "mysqlDatabase" << _mysqlDatabase;
    fs << "trainingDataFilename" << _trainingDataFilename;
    fs.release();
}

void Config::loadConfig() {
    cv::FileStorage fs("config.yml", cv::FileStorage::READ);
    if (fs.isOpened()) {
        fs["flipHorizontal"] >> _flipHorizontal;
        fs["flipVertical"] >> _flipVertical;
        fs["rotationDegrees"] >> _rotationDegrees;
        fs["blurKernelSize"] >> _blurKernelSize;
        fs["cannyThreshold1"] >> _cannyThreshold1;
        fs["cannyThreshold2"] >> _cannyThreshold2;
        fs["digitMinHeight"] >> _digitMinHeight;
        fs["digitMaxHeight"] >> _digitMaxHeight;
        fs["digitYAlignment"] >> _digitYAlignment;
        fs["ocrMaxDist"] >> _ocrMaxDist;
        fs["mysqlHost"] >> _mysqlHost;
        fs["mysqlUser"] >> _mysqlUser;
        fs["mysqlPassword"] >> _mysqlPassword;
        fs["mysqlDatabase"] >> _mysqlDatabase;
        fs["trainingDataFilename"] >> _trainingDataFilename;
        fs.release();
    } else {
        // no config file - create an initial one with default values
        saveConfig();
    }
}
