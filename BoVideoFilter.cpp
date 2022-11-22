#include "BoVideoFilter.h"
#include "BoImagePro.h"
#include <mutex>

class CBoVideoFilter : public BoVideoFilter {
  public:
    cv::Mat filter(cv::Mat mat1, cv::Mat mat2 = cv::Mat()) override {
        std::unique_lock<std::mutex> guard{m_mutexTask};
        BoImagePro imagePro;
        imagePro.setData(mat1, mat2);
        // 注意循环中vector.size()的效率问题
        for (const auto &task : m_tasks) {
            switch (task.type) {
            case TASK_GAIN:
                imagePro.gain(task.parameter[0], task.parameter[1]);
                break;
            case TASK_ROTATE90:
                imagePro.rotate90();
                break;
            case TASK_ROTATE180:
                imagePro.rotate180();
                break;
            case TASK_ROTATE270:
                imagePro.rotate270();
                break;
            case TASK_FLIPX:
                imagePro.flipX();
                break;
            case TASK_FLIPY:
                imagePro.flipY();
                break;
            case TASK_FLIPXY:
                imagePro.flipXY();
                break;
            case TASK_RESIZE:
                m_resultWidth = task.parameter[0];
                m_resultHeight = task.parameter[1];
                imagePro.resize(task.parameter[0], task.parameter[1]);
            default:
                break;
            }
        }
        cv::Mat result = imagePro.getResult();
        guard.unlock();
        return result;
    }

    void addTask(Task task) override {
        std::unique_lock<std::mutex> guard{m_mutexTask};
        m_tasks.push_back(task);
    }

    void clearTask() override {
        std::unique_lock<std::mutex> guard{m_mutexTask};
        m_tasks.clear();
    }

    int getResultWidht() override { return m_resultWidth; }
    int getResultHeight() override { return m_resultHeight; }

  private:
    std::vector<Task> m_tasks;
    std::mutex m_mutexTask;
    int m_resultWidth;
    int m_resultHeight;
};

BoVideoFilter *BoVideoFilter::getInstance() {
    static CBoVideoFilter filter;
    return &filter;
}

BoVideoFilter::BoVideoFilter() {}

BoVideoFilter::~BoVideoFilter() {}
