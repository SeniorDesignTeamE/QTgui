#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT


public:
    explicit MainWindow(QWidget *parent = 0);
    //QMetaObject::connectSlotsByName(MainWindow);
    ~MainWindow();

private slots:
    void on_confirmbutton_clicked();

private:
    Ui::MainWindow *ui;
    void writeInputNumbers(int x, int y, int z);
};

#endif // MAINWINDOW_H
