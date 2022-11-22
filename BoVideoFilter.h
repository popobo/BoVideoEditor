#ifndef BOVIDEOFILTER_H
#define BOVIDEOFILTER_H
#include <opencv2/core.hpp>

enum TaskType {
    TASK_NONE,
    TASK_GAIN,
    TASK_ROTATE90,
    TASK_ROTATE180,
    TASK_ROTATE270,
    TASK_FLIPX,
    TASK_FLIPY,
    TASK_FLIPXY,
};

struct Task {
    TaskType type;
    std::vector<double> parameter;
};

class BoVideoFilter {
  public:
    virtual cv::Mat filter(cv::Mat mat1, cv::Mat mat2 = cv::Mat()) = 0;
    virtual void addTask(Task task) = 0;
    virtual void clearTask() = 0;
    static BoVideoFilter *getInstance();
    BoVideoFilter();
    virtual ~BoVideoFilter();
};

#endif // BOVIDEOFILTER_H
