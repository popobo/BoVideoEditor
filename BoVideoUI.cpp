#include "BoVideoUI.h"
#include "ui_BoVideoUI.h"
#include <BoVideoThread.h>
#include <QFileDialog>
#include <QMessageBox>
#include <opencv2/core.hpp>
#include <string>

BoVideoUI::BoVideoUI(QWidget *parent) : QWidget(parent), ui(new Ui::BoVideoUI) {
    ui->setupUi(this);

    connect(ui->pushButtonOpenFile, SIGNAL(clicked()), this, SLOT(openFile()));
    connect(ui->pushButtonCloseWindow, SIGNAL(clicked()), this,
            SLOT(killThread()));

    qRegisterMetaType<cv::Mat>("cv::Mat");
    connect(&BoVideoThread::getInstance(), SIGNAL(ViewImageOne(cv::Mat)),
            ui->openGLWidgetSrcOne, SLOT(setImage(cv::Mat)));
    connect(&BoVideoThread::getInstance(), SIGNAL(threadExit()), this,
            SLOT(close()));

    connect(ui->horizontalSliderProgress, SIGNAL(sliderPressed()), this,
            SLOT(sliderPressed()));
    connect(ui->horizontalSliderProgress, SIGNAL(sliderReleased()), this,
            SLOT(sliderPressed()));
    connect(ui->horizontalSliderProgress, SIGNAL(sliderMoved(int)), this,
            SLOT(sliderMoved(int)));

    setWindowFlags(Qt::FramelessWindowHint);
    // 记录timerID然后才能关闭定时器
    startTimer(40);
}

BoVideoUI::~BoVideoUI() { delete ui; }

void BoVideoUI::timerEvent(QTimerEvent *event) {
    if (m_sliderPressed)
        return;
    double position = BoVideoThread::getInstance().getPlayPosition();
    ui->horizontalSliderProgress->setValue(
        (int)(position * ui->horizontalSliderProgress->maximum()));
}

void BoVideoUI::openFile() {
    // nullptr 改为 this会报错，且界面有可能卡死
    QString filename = QFileDialog::getOpenFileName(
        nullptr, QString::fromLocal8Bit("please choose a video file"));
    if (filename.isEmpty()) {
        return;
    }
    // 确保中文文件名不出错
    std::string localFileName = filename.toLocal8Bit().data();
    // QMessageBox::information(this, "", filename);
    if (!BoVideoThread::getInstance().openFile(localFileName)) {
        QMessageBox::information(this, "", "failed to open" + filename);
        return;
    }
}

void BoVideoUI::killThread() { BoVideoThread::getInstance().setIsExit(true); }

void BoVideoUI::sliderPressed() { m_sliderPressed = true; }

void BoVideoUI::sliderReleased() { m_sliderPressed = false; }

void BoVideoUI::sliderMoved(int value) {
    BoVideoThread::getInstance().seek((double)value /
                                      ui->horizontalSliderProgress->maximum());
}
