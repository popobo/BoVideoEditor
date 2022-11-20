#include "BoVideoThread.h"
#include <opencv2/core.hpp>
#include <opencv2/imgcodecs.hpp>
#include <opencv2/imgproc.hpp>
#include <opencv2/videoio.hpp>

BoVideoThread::BoVideoThread() { start(); }

void BoVideoThread::setIsExit(bool value) {
    std::lock_guard<std::mutex> lock{mutexOpenFile};
    isExit = value;
}

// C++11规定了local
// static在多线程条件下的初始化行为，要求编译器保证了内部静态变量的线程安全性
BoVideoThread &BoVideoThread::getInstance() {
    static BoVideoThread instnace;
    return instnace;
}

void BoVideoThread::run() {
    for (;;) {
        mutexOpenFile.lock();
        if (isExit) {
            mutexOpenFile.unlock();
            break;
        }
        if (!videoCapture1.isOpened()) {
            mutexOpenFile.unlock();
            msleep(5);
            continue;
        }
        cv::Mat matOne;
        if (!videoCapture1.read(matOne) || matOne.empty()) {
            mutexOpenFile.unlock();
            msleep(5);
            continue;
        }
        // msleep放在lock中避免退出时crash
        ViewImageOne(matOne);
        msleep(30);
        mutexOpenFile.unlock();
    }
}

bool BoVideoThread::openFile(const std::string &filename) {
    std::lock_guard<std::mutex> lock{mutexOpenFile};
    bool ret = videoCapture1.open(filename);
    return ret;
}

// 添加Q_OBJECT后必须实现析构函数
BoVideoThread::~BoVideoThread() { setIsExit(true); }
