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

    void rotate90();
    void rotate180();
    void rotate270();

    void flipX();
    void flipY();
    void flipXY();

    void resize(int width, int height);

    void pyDown(int count);
    void pyUp(int count);

    void clip(int posX, int posY, int width, int height);

    void gray();

    void mark(int posX, int posY, int width, int height, double alpha);

  private:
    //原图
    cv::Mat m_src1, m_src2;

    cv::Mat m_result;
};

#endif // BOIMAGEPRO_H
