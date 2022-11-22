#include "BoVideoUI.h"
#include "BoVideoFilter.h"
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
