#include "BoVideoThread.h"
#include "BoVideoFilter.h"
#include <iostream>
#include <opencv2/core.hpp>
#include <opencv2/imgcodecs.hpp>
#include <opencv2/imgproc.hpp>
#include <opencv2/videoio.hpp>

BoVideoThread::BoVideoThread() { start(); }

bool BoVideoThread::getIsWrite() const { return m_isWrite; }

int BoVideoThread::getFps() const { return m_fps; }

double BoVideoThread::getPlayPosition() {
    double position = 0;
    std::unique_lock<std::mutex> guard{m_mutexOpenFile};
    if (!m_videoCaptureOne.isOpened()) {
        return 0;
    }
    position = m_videoCaptureOne.get(cv::CAP_PROP_POS_FRAMES) / m_totalFrameOne;
    return position;
}

// 后续要考虑两个视频不同帧率的影响
bool BoVideoThread::seek(double position) {
    std::unique_lock<std::mutex> guard{m_mutexOpenFile};
    if (!m_videoCaptureOne.isOpened()) {
        return false;
    }
    int ret = m_videoCaptureOne.set(cv::CAP_PROP_POS_FRAMES,
                                    (int)(position * m_totalFrameOne));
    return ret;
}

bool BoVideoThread::startSave(const std::string &filename, int width,

                              int height) {
    std::unique_lock<std::mutex> guard{m_mutexOpenFile};
    if (!m_videoCaptureOne.isOpened()) {
        return false;
    }
    if (width <= 0) {
        width = m_videoCaptureOne.get(cv::CAP_PROP_FRAME_WIDTH);
    }
    if (height <= 0) {
        height = m_videoCaptureOne.get(cv::CAP_PROP_FRAME_HEIGHT);
    }

    m_videoWriter.open(filename, cv::VideoWriter::fourcc('X', '2', '6', '4'),
                       m_fps, cv::Size(width, height));
    if (!m_videoWriter.isOpened()) {
        std::cout << "failed to open save file" << std::endl;
        return false;
    }

    m_isWrite = true;
    return true;
}

bool BoVideoThread::stopSave() {
    std::unique_lock<std::mutex> guard{m_mutexOpenFile};
    m_videoWriter.release();
    m_isWrite = false;
    return true;
}

void BoVideoThread::setIsExit(bool value) {
    std::unique_lock<std::mutex> guard{m_mutexOpenFile};
    m_isExit = value;
}

// C++11规定了local
// static在多线程条件下的初始化行为，要求编译器保证了内部静态变量的线程安全性
BoVideoThread &BoVideoThread::getInstance() {
    static BoVideoThread instnace;
    return instnace;
}

void BoVideoThread::run() {
    for (;;) {
        std::unique_lock<std::mutex> guard{m_mutexOpenFile};
        if (m_isExit) {
            guard.unlock();
            threadExit();
            break;
        }
        if (!m_videoCaptureOne.isOpened()) {
            guard.unlock();
            msleep(5);
            continue;
        }
        cv::Mat matOne;
        if (!m_videoCaptureOne.read(matOne) || matOne.empty()) {
            guard.unlock();
            // 导出到结尾
            if (m_isWrite) {
                stopSave();
                saveEnd();
            }
            msleep(5);
            continue;
        }
        // msleep放在lock中避免退出时crash
        // 看是否需要优化
        // To Do
        // linux下，必须在msleep之前解锁, 否则会卡顿，具体原因待查
        guard.unlock();

        auto result = BoVideoFilter::getInstance()->filter(matOne);

        int sleepTime = 1000 / m_fps;
        if (m_isWrite) {
            m_videoWriter.write(result);
            sleepTime = 1;
        } else {
            ViewImageOne(matOne);
            ViewImageResult(result);
        }

        msleep(sleepTime);
        // guard.unlock();
    }
}

bool BoVideoThread::openFile(const std::string &filename) {
    std::lock_guard<std::mutex> lock{m_mutexOpenFile};
    bool ret = m_videoCaptureOne.open(filename);
    m_fps = m_videoCaptureOne.get(cv::CAP_PROP_FPS);
    m_fps = m_fps == 0 ? 25 : m_fps;

    m_totalFrameOne = m_videoCaptureOne.get(cv::CAP_PROP_FRAME_COUNT);
    m_totalFrameOne = m_totalFrameOne == 0 ? 1 : m_totalFrameOne;
    return ret;
}

// 添加Q_OBJECT后必须实现析构函数
BoVideoThread::~BoVideoThread() { setIsExit(true); }
