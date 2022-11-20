#include "BoVideoWidget.h"
#include <QPainter>
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
    if (m_imageOne.isNull()) {
        uchar *buf = new uchar[width() * height() * 3];
        m_imageOne = QImage(buf, width(), height(), QImage::Format_RGB888);
    }
    cv::Mat des;
    cv::resize(mat, des,
               cv::Size(m_imageOne.size().width(), m_imageOne.size().height()));

    cv::cvtColor(des, des, cv::COLOR_BGR2RGB);
    memcpy(m_imageOne.bits(), des.data, des.cols * des.rows * des.elemSize());
    update();
}
