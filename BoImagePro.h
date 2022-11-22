#ifndef BOIMAGEPRO_H
#define BOIMAGEPRO_H
#include <opencv2/core.hpp>

class BoImagePro {
  public:
    void setData(cv::Mat mat1, cv::Mat mat2 = cv::Mat());
    BoImagePro();

    cv::Mat getResult() const;

    // bright 0~100
    // 对比度 contrast 1.0~3.0
    void gain(double bright, double contrast);

  private:
    //原图
    cv::Mat m_src1, m_src2;

    cv::Mat m_result;
};

#endif // BOIMAGEPRO_H
