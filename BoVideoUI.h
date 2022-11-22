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

    void timerEvent(QTimerEvent *event) override;

  private slots:
    void openFile();
    void killThread();

    void sliderPressed();
    void sliderReleased();
    void sliderMoved(int value);
    void setFilter();

    void exportFile();

    void exportEnd();

  private:
    Ui::BoVideoUI *ui;
    bool m_sliderPressed;
};

#endif // WIDGET_H
