#include "BoImagePro.h"

void BoImagePro::setData(cv::Mat mat1, cv::Mat mat2) {
    if (mat1.empty()) {
        return;
    }
    m_src1 = mat1;
    m_src2 = mat2;
    m_src1.copyTo(m_result);
}

cv::Mat BoImagePro::getResult() const { return m_result; }

void BoImagePro::gain(double bright, double contrast) {
    if (m_src1.empty()) {
        return;
    }
    m_result.convertTo(m_result, -1, contrast, bright);
}

BoImagePro::BoImagePro() {}
