/*
 * main.cpp
 *
 */

#include <string>
#include <list>
#include <algorithm>
#include <iostream>
#include <iomanip>
#include <unistd.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>

#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/highgui/highgui.hpp>

#include <log4cpp/Category.hh>
#include <log4cpp/Appender.hh>
#include <log4cpp/FileAppender.hh>
#include <log4cpp/OstreamAppender.hh>
#include <log4cpp/Layout.hh>
#include <log4cpp/PatternLayout.hh>
#include <log4cpp/SimpleLayout.hh>
#include <log4cpp/Priority.hh>

#include "Directory.h"
#include "ImageProcessor.h"
#include "KNearestOcr.h"
#include "Plausi.h"
#include "MySQLDatabase.h"

static int delay = 1000;

#ifndef VERSION
#define VERSION "0.9.6"
#endif

static void testOcr(ImageInput* pImageInput) {
    log4cpp::Category::getRoot().info("testOcr");

    Config config;
    config.loadConfig();
    ImageProcessor proc(config);
    proc.debugWindow();
    proc.debugDigits();

    Plausi plausi;

    KNearestOcr ocr(config);
    if (! ocr.loadTrainingData()) {
        std::cout << "Failed to load OCR training data\n";
        return;
    }
    std::cout << "OCR training data loaded.\n";
    std::cout << "<q> to quit.\n";

    while (pImageInput->nextImage()) {
        proc.setInput(pImageInput->getImage());
        proc.process();

        std::string result = ocr.recognize(proc.getOutput());
        std::cout << result;
        if (plausi.check(result, pImageInput->getTime())) {
            std::cout << "  " << std::fixed << std::setprecision(1) << plausi.getCheckedValue() << std::endl;
        } else {
            std::cout << "  -------" << std::endl;
        }
        int key = cv::waitKey(delay);
        if (key >= 0x100000) key -= 0x100000;  // workaround for waitKey bug

        if (key == 'q') {
            std::cout << "Quit\n";
            break;
        }
    }
}

static void learnOcr(ImageInput* pImageInput) {
    log4cpp::Category::getRoot().info("learnOcr");

    Config config;
    config.loadConfig();
    ImageProcessor proc(config);
    proc.debugWindow();

    KNearestOcr ocr(config);
    ocr.loadTrainingData();
    std::cout << "Entering OCR training mode!\n";
    std::cout << "<0>..<9> to answer digit, <space> to ignore digit, <s> to save and quit, <q> to quit without saving.\n";

    int key = 0;
    while (pImageInput->nextImage()) {
        proc.setInput(pImageInput->getImage());
        proc.process();

        key = ocr.learn(proc.getOutput());
        std::cout << std::endl;

        if (key == 'q' || key == 's') {
            std::cout << "Quit\n";
            break;
        }
    }

    if (key != 'q') {
        std::cout << "Saving training data\n";
        ocr.saveTrainingData();
    }
}

static void adjustCamera(ImageInput* pImageInput) {
    log4cpp::Category::getRoot().info("adjustCamera");

    Config config;
    config.loadConfig();
    ImageProcessor proc(config);
    proc.debugWindow();
    proc.debugDigits();
    //proc.debugEdges();
    //proc.debugSkew();

    std::cout << "Adjust camera.\n";
    std::cout << "<r>, <p> to select raw or processed image, <s> to save config and quit, <q> to quit without saving.\n";

    bool processImage = true;
    int key = 0;
    while (pImageInput->nextImage()) {
        proc.setInput(pImageInput->getImage());
        if (processImage) {
            proc.process();
        } else {
            proc.showImage();
        }

        key = cv::waitKey(delay);
        if (key >= 0x100000) key -= 0x100000; // workaround for waitKey bug

        if (key == 'q' || key == 's') {
            std::cout << "Quit\n";
            break;
        } else if (key == 'r') {
            processImage = false;
        } else if (key == 'p') {
            processImage = true;
        }
    }
    if (key != 'q') {
        std::cout << "Saving config\n";
        config.saveConfig();
    }
}

static void capture(ImageInput* pImageInput) {
    log4cpp::Category::getRoot().info("capture");

    std::cout << "Capturing images into directory.\n";
    std::cout << "<Ctrl-C> to quit.\n";

    while (pImageInput->nextImage()) {
        usleep(delay*1000L);
    }
}

static void writeData(ImageInput* pImageInput, std::string timeDevidor) {
    log4cpp::Category::getRoot().info("writeData");

    Config config;
    config.loadConfig();
    ImageProcessor proc(config);

    Plausi plausi;

    MySQLDatabase mysql(config);

    struct stat st;

    char type = *timeDevidor.rbegin();
    int devidor = atoi(timeDevidor.substr(0, timeDevidor.length()-1).c_str());
    time_t timeNext = 0;
    if ((type == 'h' || type == 'm' || type == 's') && devidor) {
        time_t now = time(0);
        struct tm * y2k;
        y2k = localtime(&now);
        switch(type) {
            case 'h':
                y2k->tm_hour = y2k->tm_hour - (y2k->tm_hour % devidor);
                y2k->tm_min = 0;
                y2k->tm_sec = 0;
                timeNext = mktime(y2k) + (3600 * devidor); // 60*60*devidor
                break;
            case 'm':
                y2k->tm_min = y2k->tm_min - (y2k->tm_min % devidor);
                y2k->tm_sec = 0;
                timeNext = mktime(y2k) + (60 * devidor); // 60*devidor
                break;
            case 's':
                y2k->tm_sec = y2k->tm_sec - (y2k->tm_sec % devidor);
                timeNext = mktime(y2k) + devidor; // devidor
                break;
        }
        std::cout << "Update MySQL DB every " << devidor << type << ".\n";
    }

    KNearestOcr ocr(config);
    if (! ocr.loadTrainingData()) {
        std::cout << "Failed to load OCR training data\n";
        return;
    }
    std::cout << "OCR training data loaded.\n";
    std::cout << "<Ctrl-C> to quit.\n";

    while (pImageInput->nextImage()) {
        proc.setInput(pImageInput->getImage());
        proc.process();

        if (proc.getOutput().size() == 7) {
            std::string result = ocr.recognize(proc.getOutput());
            if (plausi.check(result, pImageInput->getTime())) {
                if (timeNext == 0) {
                    mysql.insert("emeter", plausi.getCheckedTime(), plausi.getCheckedValue());
                }
            }
        }
        if (0 == stat("imgdebug", &st) && S_ISDIR(st.st_mode)) {
            // write debug image
            pImageInput->setOutputDir("imgdebug");
            pImageInput->saveImage();
            pImageInput->setOutputDir("");
        }
        if (timeNext != 0 && timeNext <= time(0)) {
            if (plausi.getCheckedValue() != -1) {
                mysql.insert("emeter", timeNext, plausi.getCheckedValue());
                while (difftime(timeNext, time(0)) <= 0) {
                    switch(type) {
                        case 'h':
                            timeNext += 3600 * devidor; // 60*60*devidor
                            break;
                        case 'm':
                            timeNext += 60 * devidor; // 60*devidor
                            break;
                        case 's':
                            timeNext += devidor; // devidor
                            break;
                    }
                }
            }
        }
        usleep(delay*1000L);
    }
}

static void usage(const char* progname) {
    std::cout << "Program to read and recognize the counter of an electricity meter with OpenCV.\n";
    std::cout << "Version: " << VERSION << std::endl;
    std::cout << "Usage: " << progname << " [-i <dir>|-c <cam>] [-l|-t|-a|-w|-o <dir>] [-d <schedule>] [-s <delay>] [-v <level>] [-p]\n";
    std::cout << "\nImage input:\n";
    std::cout << "  -i <image directory> : read image files (png) from directory.\n";
    std::cout << "  -c <camera number> : read images from camera.\n";
    std::cout << "\nOperation:\n";
    std::cout << "  -a : adjust camera.\n";
    std::cout << "  -o <directory> : capture images into directory.\n";
    std::cout << "  -l : learn OCR.\n";
    std::cout << "  -t : test OCR.\n";
    std::cout << "  -w : write OCR data to MySQL database. This is the normal working mode.\n";
    std::cout << "\nOptions:\n";
    std::cout << "  -d <t> : ONLY WORKS WITH OPERATION ""-w"" ! If time is divisible without remainder by t, write to DB.\n";
    std::cout << "           e.g.: 1h = every full hour or 10m = every full 10 minutes. (t=<number>[h|m|s]) (default=None).\n";
    std::cout << "  -s <n> : Sleep n milliseconds after processing of each image (default=1000).\n";
    std::cout << "  -v <l> : Log level. One of DEBUG, INFO, ERROR (default).\n";
    std::cout << "  -p : Print log also in Console (default=False).\n";
}

static void configureLogging(const std::string & priority = "INFO", bool toConsole = false) {
    log4cpp::Appender *fileAppender = new log4cpp::FileAppender("default", "emeocv.log");
    log4cpp::PatternLayout* layout = new log4cpp::PatternLayout();
    layout->setConversionPattern("%d{%d.%m.%Y %H:%M:%S} %p: %m%n");
    fileAppender->setLayout(layout);
    log4cpp::Category& root = log4cpp::Category::getRoot();
    root.setPriority(log4cpp::Priority::getPriorityValue(priority));
    root.addAppender(fileAppender);
    if (toConsole) {
        log4cpp::Appender *consoleAppender = new log4cpp::OstreamAppender("console", &std::cout);
        consoleAppender->setLayout(new log4cpp::SimpleLayout());
        root.addAppender(consoleAppender);
    }
}

int main(int argc, char **argv) {
    int opt;
    ImageInput* pImageInput = 0;
    int inputCount = 0;
    std::string timeDevidor;
    std::string outputDir;
    std::string logLevel = "ERROR";
    bool toConsole = false;
    char cmd = 0;
    int cmdCount = 0;

    while ((opt = getopt(argc, argv, "i:c:ltawd:s:o:v:ph")) != -1) {
        switch (opt) {
            case 'i':
                pImageInput = new DirectoryInput(Directory(optarg, ".png"));
                inputCount++;
                break;
            case 'c':
                pImageInput = new CameraInput(atoi(optarg));
                inputCount++;
                break;
            case 'a':
                toConsole = true;
            case 'l':
            case 't':
            case 'w':
                cmd = opt;
                cmdCount++;
                break;
            case 'd':
                timeDevidor = optarg;
                break;
            case 'o':
                cmd = opt;
                cmdCount++;
                outputDir = optarg;
                break;
            case 's':
                delay = atoi(optarg);
                break;
            case 'v':
                logLevel = optarg;
                break;
            case 'p':
                toConsole = true;
                break;
            case 'h':
            default:
                usage(argv[0]);
                exit(EXIT_FAILURE);
                break;
        }
    }
    if (inputCount != 1) {
        std::cerr << "*** You should specify exactly one camera or input directory!\n\n";
        usage(argv[0]);
        exit(EXIT_FAILURE);
    }
    if (cmdCount != 1) {
        std::cerr << "*** You should specify exactly one operation!\n\n";
        usage(argv[0]);
        exit(EXIT_FAILURE);
    }

    configureLogging(logLevel, toConsole);

    switch (cmd) {
        case 'o':
            pImageInput->setOutputDir(outputDir);
            capture(pImageInput);
            break;
        case 'l':
            learnOcr(pImageInput);
            break;
        case 't':
            testOcr(pImageInput);
            break;
        case 'a':
            adjustCamera(pImageInput);
            break;
        case 'w':
            writeData(pImageInput, timeDevidor);
            break;
    }

    delete pImageInput;
    exit(EXIT_SUCCESS);
}
