#ifndef BOVIDEOWIDGET_H
#define BOVIDEOWIDGET_H

#include <QOpenGLWidget>
#include <opencv2/core.hpp>

class BoVideoWidget : public QOpenGLWidget {
    Q_OBJECT

  public:
    BoVideoWidget(QWidget *widget);
    ~BoVideoWidget();

    void paintEvent(QPaintEvent *paintEvent);

  public slots:
    void setImage(cv::Mat mat);

  private:
    QImage m_imageOne;
};

#endif // BOVIDEOWIDGET_H
