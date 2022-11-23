#include "BoImagePro.h"
#include <opencv2/imgcodecs.hpp>
#include <opencv2/imgproc.hpp>
#include <opencv2/videoio.hpp>

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

void BoImagePro::rotate90() {
    if (m_result.empty()) {
        return;
    }
    rotate(m_result, m_result, cv::ROTATE_90_CLOCKWISE);
}

void BoImagePro::rotate180() {
    if (m_result.empty()) {
        return;
    }
    rotate(m_result, m_result, cv::ROTATE_180);
}

void BoImagePro::rotate270() {
    if (m_result.empty()) {
        return;
    }
    rotate(m_result, m_result, cv::ROTATE_90_COUNTERCLOCKWISE);
}

void BoImagePro::flipX() {
    if (m_result.empty()) {
        return;
    }
    cv::flip(m_result, m_result, 0);
}

void BoImagePro::flipY() {
    if (m_result.empty()) {
        return;
    }
    cv::flip(m_result, m_result, 1);
}

void BoImagePro::flipXY() {
    if (m_result.empty()) {
        return;
    }
    cv::flip(m_result, m_result, -1);
}

void BoImagePro::resize(int width, int height) {
    if (m_result.empty()) {
        return;
    }
    cv::resize(m_result, m_result, cv::Size(width, height));
}

void BoImagePro::pyDown(int count) {
    if (m_result.empty()) {
        return;
    }
    for (int i = 0; i < count; ++i) {
        cv::pyrDown(m_result, m_result);
    }
}

void BoImagePro::pyUp(int count) {
    if (m_result.empty()) {
        return;
    }
    for (int i = 0; i < count; ++i) {
        cv::pyrUp(m_result, m_result);
    }
}

void BoImagePro::clip(int posX, int posY, int width, int height) {
    if (posX < 0 || posY < 0 || width < 0 || height < 0) {
        return;
    }
    if (posX > m_result.cols || posY > m_result.cols) {
        return;
    }
    m_result = m_result(cv::Rect(posX, posY, width, height));
}

void BoImagePro::gray() {
    if (m_result.empty()) {
        return;
    }
    cv::cvtColor(m_result, m_result, cv::COLOR_RGB2GRAY);
}

BoImagePro::BoImagePro() {}
