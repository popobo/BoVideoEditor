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

    int getFps() const;

    double getPlayPosition();

    bool seek(double position);

    // 开始保存视频
    bool startSave(const std::string &filename, int width = 0, int height = 0);

    // 停止保存视频，写入视频帧的索引
    bool stopSave();

    bool getIsWrite() const;

  private:
    BoVideoThread();
    BoVideoThread(const BoVideoThread &videoThread) = delete;
    BoVideoThread &operator=(const BoVideoThread &videoThread) = delete;

  private:
    cv::VideoCapture m_videoCaptureOne;
    cv::VideoWriter m_videoWriter;
    std::mutex m_mutexOpenFile;
    bool m_isExit = false;
    double m_fps = 25;
    double m_totalFrameOne = 0;
    bool m_isWrite = false;

  signals:
    // 保证信号能被处理，否则可能内存溢出
    void ViewImageOne(cv::Mat mat);

    void ViewImageResult(cv::Mat mat);

    void threadExit();

    void saveEnd();
};

#endif // BOVIDEOTHREAD_H
