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
    qRegisterMetaType<cv::Mat>("cv::Mat");
    connect(&BoVideoThread::getInstance(), SIGNAL(ViewImageOne(cv::Mat)),
            ui->openGLWidgetSrcOne, SLOT(setImage(cv::Mat)));
    setWindowFlags(Qt::FramelessWindowHint);
}

BoVideoUI::~BoVideoUI() { delete ui; }

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
