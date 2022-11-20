#ifndef WIDGET_H
#define WIDGET_H

#include <QWidget>

namespace Ui {
class BoVideoUI;
}

class BoVideoUI : public QWidget {
    Q_OBJECT

  public:
    explicit BoVideoUI(QWidget *parent = 0);
    ~BoVideoUI();

  private slots:
    void openFile();

  private:
    Ui::BoVideoUI *ui;
};

#endif // WIDGET_H
