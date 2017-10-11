#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QFile>
#include <QTextStream>
#include <QString>
#include <stdio.h>
#include<stdlib.h>
#include <iostream>
#include <string>
#include<QProcess>
#include <fstream>
#include<QMediaPlayer>
#include<QVideoWidget>
#include<QMediaPlaylist>
#include<windows.h>


//Some unnecessary includes, added and removed includes while figuring out the file
//will remove later

//COM port necessary for communication, changes depending on machine connected to the Teensy

#define stepSize 0.000475
using namespace std;


int runs=0;
bool simulated=true;
bool init=false;
vector <vector <float>> cableLenMat;
vector <vector <float>> stepMat;


void writePoint(float a, float b, float c);
void stepCalc();
void serialCom();
void packetizer();

class point {
   public:
      float x;
      float y;
      float z;
};
point::point(float x, float y, float z){
    x=x;
    y=y;
    z=z;

}

class trajectory {
   public:
      QString trajDir;
      QString fileHeader;
      QString fileCloser;
      QString openPoint;
      QString closePoint;
      QString qDot;
      int len;
      vector<point> points;
};
trajectory::trajectory(void){
    trajDir="C:\\Users\\Martin\\Downloads\\CASPR-master\\CASPR-master\\data\\model_config\\models\\SCDM\\spatial_manipulators\\PoCaBot_spatial\\PoCaBot_spatial_trajectories.xml";
    fileHeader="<?xml version=\"1.0\" encoding=\"utf-8\"?>\n<!DOCTYPE trajectories SYSTEM \"../../../../templates/trajectories.dtd\">\n<trajectories>\n\t<joint_trajectories>\n\t\t<quintic_spline_trajectory id=\"traj_1\" time_definition = \"relative\" time_step=\"0.05\">\n\t\t\t<points>";
    fileCloser="\n\t\t</quintic_spline_trajectory>\n\t</joint_trajectories>\n</trajectories>";
    openPoint="\n\t\t\t\t<point>";
    closePoint="\n\t\t\t</points>";
    qDot="\n\t\t\t\t\t<q_dot>0.0 0.0 0.0 0.0 0.0 0.0</q_dot>\n\t\t\t\t\t<q_ddot>0.0 0.0 0.0 0.0 0.0 0.0</q_ddot>";
    len=0;
    return;
}
void trajectory::addPoint(float x, float y, float z){
    len++;
    point temp(x,y,z);
    points.push_back(temp);
    return;
}
void trajectory::clearTraj(void){
    points.clear();
    len=0;
}
void trajectory::writeToFile(void){
    int count, temp;
    QFile outputFile(trajDir);
    outputFile.open((QIODevice::WriteOnly | QIODevice::Text));
    QTextStream out(&outputFile);
    out<<fileHeader;
    temp=points.size();
    for(count=0; count<temp; count++){
        float a=points[count].x, b=points[count].y, c=points[count].z;
        a=a/100;
        b=b/100;
        c=c/100;
        QString coor = "\n\t\t\t\t\t<q>" + QString::number(a) + " " + QString::number(b) + " " + QString::number(c)+" 0.0 0.0 0.0</q>";
        out<<openPoint<<coor<<qDot<<closePoint;
    }
    out<<fileCloser;
    outputFile.close();
    return;
}


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

/*Add button clicked:
 * adds a set of XYZ coordinates to the trajectory XML file
*/
void MainWindow::on_confirmbutton_clicked()
{
    if(!init){
        trajectory traj();
        init=true;
    }
    int x = ui->lineEdit->text().toFloat();
    int y = ui->lineEdit_2->text().toFloat();
    int z = ui->lineEdit_3->text().toFloat();
    traj.addPoint(x,y,z);
}
/*Finish button pressed:
 * XML trajectory is completed with tailing strings to match CASPR format.
*/
void MainWindow::on_confirmbutton_2_pressed(){
    traj.writeToFile();
    return;
}


/*Simulate button pressed:
 * calls batch file that runs MATLAB scripts
 * will run through the MATLAB simulation and close when finished.
 * CASPR saves a video file and txt file with cable length over time
*/
void MainWindow::on_confirmbutton_3_clicked()
{
    //write a bat file to call matlab scripts and fix directory
    system("start C:\\Users\\Martin\\Desktop\\script.bat");
    simulated=true;
    return;
}

/* Display button pressed:
 * check if simulation has ran by checking the simulated flag
 * if true, video player setup and runs the trajectory.avi in a loop in a new window. Can be closed by user at any time.
*/
void MainWindow::on_confirmbutton_4_clicked()
{
    if(simulated==true){
        QMediaPlayer* player = new QMediaPlayer;
        QVideoWidget* vw= new QVideoWidget;
        player->setVideoOutput(vw);
        QMediaPlaylist* playlist =new QMediaPlaylist();
        //match directory with the directory where video is saved
        playlist->addMedia(QUrl::fromLocalFile("C:/Users/Martin/Downloads/CASPR-master/CASPR-master/data/videos/kinematics_gui_output.avi"));
        playlist->setPlaybackMode(QMediaPlaylist::Loop);
        vw->setGeometry(100,100,300,400);
        vw->show();
        player->setPlaylist(playlist);
        player->play();
    }
    else{
        cout<<"simulation not ran yet."<<endl;
    }
}

/*Run button pressed:
 * loads the matrix from MATLAB simulation file, cable lengths over time
 * values stored in cableLenMat
*/
void MainWindow::on_confirmbutton_5_clicked()
{
    //match directory
    ifstream infile("C:\\Users\\Martin\\Desktop\\array.txt");
    string line;
    string delimiter= ",";
    string subvalue;
    size_t pos = 0;
    vector<float> temp;
    while(getline(infile, line)){
        while ((pos = line.find(delimiter)) != string::npos){
            subvalue=line.substr(0, line.find(delimiter));
            //cout<<subvalue<<endl;
            temp.push_back(atof(subvalue.c_str()));
            line.erase(0, pos + delimiter.length());
        }
        cableLenMat.push_back(temp);
        temp.clear();
    }
    stepCalc();
    return;

}

/*Called by run button (5) clicked
 * converts values of cable lengths to length in steps
 * checks for the difference in lengths
 * stores the change in stepLengths in stepMat
 * stepMat= 8xN matrix where each row is a cable.
*/
void stepCalc(){
    int row,rowLen,col,diffSteps;
    float delta;
    int Step, prevStep;
    std::vector <float> temp;
    std::vector <float> stepTemp;
    for(row=0; row<8; row++){
        temp=cableLenMat[row];
        rowLen=temp.size();
        for(col=0; col<rowLen-1; col++){
            Step=temp[col+1]/stepSize;
            prevStep=temp[col]/stepSize;
            delta=Step-prevStep;
            stepTemp.push_back(delta);
            //cout<<Step<<" "<<prevStep<<" "<<delta <<endl;//erase line, only for bebugging
        }
        stepMat.push_back(stepTemp);
        stepTemp.clear();
        temp.clear();
    }
    serialCom();// not working
    return;
}

//NOT WORKING
//will setup Teensy comms and convert stepMAT values to packets then communicate the packets
void serialCom(){

    return;
}

//NOTWORKING
void packetizer(){
    return;
}


