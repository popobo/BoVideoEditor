#include "BoVideoUI.h"
#include <QApplication>

int main(int argc, char *argv[]) {
    QApplication a(argc, argv);
    BoVideoUI ui;
    ui.show();

    return a.exec();
}
