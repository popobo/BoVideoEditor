#include "BoVideoUI.h"
#include "BoVideoFilter.h"
#include "ui_BoVideoUI.h"
#include <BoVideoThread.h>
#include <QFileDialog>
#include <QMessageBox>
#include <iostream>
#include <opencv2/core.hpp>
#include <string>
extern "C" {
#include "libavformat/avformat.h"
}

BoVideoUI::BoVideoUI(QWidget *parent) : QWidget(parent), ui(new Ui::BoVideoUI) {
    ui->setupUi(this);

    connect(ui->pushButtonOpenFile, SIGNAL(clicked()), this, SLOT(openFile()));
    connect(ui->pushButtonCloseWindow, SIGNAL(clicked()), this,
            SLOT(killThread()));

    qRegisterMetaType<cv::Mat>("cv::Mat");
    connect(&BoVideoThread::getInstance(), SIGNAL(ViewImageOne(cv::Mat)),
            ui->openGLWidgetSrcOne, SLOT(setImage(cv::Mat)));
    // 输出视频显示信号
    connect(&BoVideoThread::getInstance(), SIGNAL(ViewImageResult(cv::Mat)),
            ui->openGLWidgetResult, SLOT(setImage(cv::Mat)));

    connect(&BoVideoThread::getInstance(), SIGNAL(threadExit()), this,
            SLOT(close()));
    // 导出视频结束
    connect(&BoVideoThread::getInstance(), SIGNAL(saveEnd()), this,
            SLOT(exportEnd()));

    connect(ui->horizontalSliderProgress, SIGNAL(sliderPressed()), this,
            SLOT(sliderPressed()));
    connect(ui->horizontalSliderProgress, SIGNAL(sliderReleased()), this,
            SLOT(sliderReleased()));
    connect(ui->horizontalSliderProgress, SIGNAL(sliderMoved(int)), this,
            SLOT(sliderMoved(int)));

    connect(ui->pushButtonOpenExport, SIGNAL(clicked()), this,
            SLOT(exportFile()));

    connect(ui->pushButtonSet, SIGNAL(clicked()), this, SLOT(setFilter()));

    connect(ui->pushButtonPlay, SIGNAL(clicked()), this, SLOT(play()));
    connect(ui->pushButtonPause, SIGNAL(clicked()), this, SLOT(pause()));

    connect(ui->pushButtonSelectMark, SIGNAL(clicked()), this,
            SLOT(selectMark()));

    pause();

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

void BoVideoUI::setFilter() {
    BoVideoFilter::getInstance()->clearTask();
    if (ui->spinBoxBright->value() > 0 ||
        ui->doubleSpinBoxContrast->value() > 1) {
        BoVideoFilter::getInstance()->addTask(
            Task{TASK_GAIN,
                 {(double)(ui->spinBoxBright->value()),
                  ui->doubleSpinBoxContrast->value()}});
    }

    // 视频旋转 1 90 2 180 3 270
    switch (ui->comboBoxRotate->currentIndex()) {
    case 1:
        BoVideoFilter::getInstance()->addTask(Task{TASK_ROTATE90});
        break;
    case 2:
        BoVideoFilter::getInstance()->addTask(Task{TASK_ROTATE180});
        break;
    case 3:
        BoVideoFilter::getInstance()->addTask(Task{TASK_ROTATE270});
        break;
    default:
        break;
    }

    // 镜像
    switch (ui->comboBoxFlip->currentIndex()) {
    case 1:
        BoVideoFilter::getInstance()->addTask(Task{TASK_FLIPX});
        break;
    case 2:
        BoVideoFilter::getInstance()->addTask(Task{TASK_FLIPY});
        break;
    case 3:
        BoVideoFilter::getInstance()->addTask(Task{TASK_FLIPXY});
        break;
    default:
        break;
    }

    bool isResize = false;

    // resize
    if (ui->spinBoxResultWidth->value() > 0 &&
        ui->spinBoxResultHeight->value() > 0) {
        BoVideoFilter::getInstance()->addTask(
            {TASK_RESIZE,
             {(double)ui->spinBoxResultWidth->value(),
              (double)ui->spinBoxResultHeight->value()}});
        ui->spinBoxPyUp->setValue(0);
        ui->spinBoxPyDown->setValue(0);
        isResize = true;
    } else if (ui->spinBoxPyUp->value() > 0) {
        BoVideoFilter::getInstance()->addTask(
            {TASK_PYUP, {(double)ui->spinBoxPyUp->value()}});
        ui->spinBoxResultWidth->setValue(0);
        ui->spinBoxResultHeight->setValue(0);
        ui->spinBoxPyDown->setValue(0);
        isResize = true;
    } else if (ui->spinBoxPyDown->value() > 0) {
        BoVideoFilter::getInstance()->addTask(
            {TASK_PYDOWN, {(double)ui->spinBoxPyDown->value()}});
        ui->spinBoxResultWidth->setValue(0);
        ui->spinBoxResultHeight->setValue(0);
        ui->spinBoxPyUp->setValue(0);
        isResize = true;
    }

    // clip
    double posX = (double)ui->spinBoxClipPosX->value();
    double posY = (double)ui->spinBoxClipPosY->value();
    double width = (double)ui->spinBoxClipWidth->value();
    double height = (double)ui->spinBoxClipHeight->value();
    if ((posX >= 0 || posY >= 0) && (width > 0 && height > 0) && !isResize) {
        BoVideoFilter::getInstance()->addTask(
            {TASK_CLIP, {posX, posY, width, height}});
    }

    // currentIndex 1 Gray
    if (ui->comboBoxColor->currentIndex() == 1) {
        BoVideoFilter::getInstance()->addTask({TASK_GRAY});
    }

    // ad waterMark
    if (!BoVideoThread::getInstance().getMarkPath().empty()) {
        BoVideoFilter::getInstance()->addTask(
            {TASK_MARK,
             {(double)ui->spinBoxMarkPosX->value(),
              (double)ui->spinBoxMarkPosY->value(),
              (double)ui->spinBoxMarkWidth->value(),
              (double)ui->spinBoxMarkHeight->value(),
              ui->doubleSpinBoxMarkAlpha->value()}});
    }
}

void BoVideoUI::exportFile() {
    if (BoVideoThread::getInstance().getIsWrite()) {
        BoVideoThread::getInstance().stopSave();
        ui->pushButtonOpenExport->setText("Export");
        return;
    }
    QString filename = QFileDialog::getSaveFileName(nullptr, "save", "out.avi");
    if (filename.isEmpty()) {
        return;
    }

    std::string filenameStd = filename.toLocal8Bit().data();
    if (BoVideoThread::getInstance().startSave(filenameStd)) {
        ui->pushButtonOpenExport->setText("Stop Export");
    }
}

void BoVideoUI::exportEnd() { ui->pushButtonOpenExport->setText("Export"); }

void BoVideoUI::play() {
    ui->pushButtonPause->show();
    ui->pushButtonPause->setGeometry(ui->pushButtonPlay->geometry());
    ui->pushButtonPlay->hide();
    BoVideoThread::getInstance().play();
}

void BoVideoUI::pause() {
    ui->pushButtonPlay->show();
    ui->pushButtonPause->hide();
    BoVideoThread::getInstance().pause();
}

AVFormatContext *aVFormatContext = 0;
void BoVideoUI::selectMark() {
    QString filename = QFileDialog::getOpenFileName(
        nullptr, QString::fromLocal8Bit("Please choose a mark image"));
    std::string stdFilename = filename.toLocal8Bit().data();
    //    BoVideoThread::getInstance().setMarkPath(stdFilename);
    //    ui->labelMarkPath->sdusetText(filename);
    int re = avformat_open_input(&aVFormatContext, stdFilename.c_str(), 0, 0);
    if (re != 0) {
        char buf[1024] = {0};
        av_strerror(re, buf, sizeof(buf));
        std::cout << buf << std::endl;
        return;
    }
    std::cout << "avformat_open_input successfully" << std::endl;
}
