#ifndef BOVIDEOTHREAD_H
#define BOVIDEOTHREAD_H
#include <QThread>
#include <mutex>
#include <opencv2/core.hpp>
#include <opencv2/videoio.hpp>
#include <string>

class BoVideoThread : public QThread {
    Q_OBJECT

  public:
    static BoVideoThread &getInstance();
    void run();
    bool openFile(const std::string &filename);
    ~BoVideoThread();

    void setIsExit(bool value);

  private:
    BoVideoThread();
    BoVideoThread(const BoVideoThread &videoThread) = delete;
    BoVideoThread &operator=(const BoVideoThread &videoThread) = delete;

  private:
    cv::VideoCapture videoCapture1;
    std::mutex mutexOpenFile;
    bool isExit = false;

  signals:
    // 保证信号能被处理，否则可能内存溢出
    void ViewImageOne(cv::Mat mat);
};

#endif // BOVIDEOTHREAD_H
