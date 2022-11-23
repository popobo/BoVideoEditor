#include "BoVideoWidget.h"
#include <QPainter>
#include <opencv2/core.hpp>
#include <opencv2/imgproc.hpp>

BoVideoWidget::BoVideoWidget(QWidget *widget) : QOpenGLWidget(widget) {}

BoVideoWidget::~BoVideoWidget() {}

void BoVideoWidget::paintEvent(QPaintEvent *paintEvent) {
    QPainter painter;
    painter.begin(this);
    painter.drawImage(QPoint(0, 0), m_imageOne);
    painter.end();
}

void BoVideoWidget::setImage(cv::Mat mat) {
    if (mat.empty()) {
        return;
    }
    QImage::Format format = QImage::Format_RGB888;

    bool isGray = mat.type() == CV_8UC1;
    // CV_8UC1不是cv中的函数，它只是一个参数值，不需要添加命名空间，修改后编译成功
    if (isGray) {
        format = QImage::Format_Grayscale8;
    }

    if (m_imageOne.isNull() || m_imageOne.format() != format) {
        // 这边用delete[]还是delete
        delete m_imageOne.bits();
        uchar *buf = new uchar[width() * height() * mat.elemSize()];
        m_imageOne = QImage(buf, width(), height(), format);
    }
    cv::Mat des;
    cv::resize(mat, des,
               cv::Size(m_imageOne.size().width(), m_imageOne.size().height()));
    if (!isGray) {
        cv::cvtColor(des, des, cv::COLOR_BGR2RGB);
    }
    memcpy(m_imageOne.bits(), des.data, des.cols * des.rows * des.elemSize());
    update();
}
