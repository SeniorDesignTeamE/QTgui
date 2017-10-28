#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QVideoWidget>
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

public slots:
    void saveF();
    void openF();
    void newF();

private slots:
    void on_confirmbutton_clicked();

    void on_confirmbutton_2_pressed();

    void on_confirmbutton_3_clicked();

    void on_confirmbutton_4_clicked();

    void on_confirmbutton_5_clicked();

private:
    Ui::MainWindow *ui;
    void writeInputNumbers(int x, int y, int z);

};

#endif // MAINWINDOW_H
