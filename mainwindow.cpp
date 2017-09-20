#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QFile>
#include <QTextStream>
#include <stdio.h>
#include <iostream>

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);

}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::on_confirmbutton_clicked()
{
    int x = ui->lineEdit->text().toInt();
    int y = ui->lineEdit_2->text().toInt();
    int z = ui->lineEdit_3->text().toInt();
    
    //change he file output location
    QFile outputFile("C:/Users/Taylor/Desktop/fall17/output.txt");
    outputFile.open((QIODevice::WriteOnly | QIODevice::Text));
    QTextStream out(&outputFile);
    out<< x<<endl;
    out<< y<<endl;
    out<< z<<endl;

    outputFile.close();

    std::cout<<"button"<<endl;
}
