#include <QtGui>
#include <QApplication>
#include <QtWidgets>
#include <stdio.h>
#include <string>
#include <QGraphicsScene>
#include <QDebug>
#include <cmath>
#include <iostream>
#include <fstream>
#include "pointswindow.h"

using namespace std;

const int playSize=600;

PointsWindow::PointsWindow(QWidget *parent)
    : QMainWindow(parent)
{
    setWindowTitle("Points game");
    //showFullScreen();
    setMouseTracking(true);
    redChained=0; blueChained=0; steps=0;
    offsetX0 = -10;
    offsetY0 = -40;
    //offsetX = 0;
    //offsetY = 0;


    //QString redLostr = QString::fromStdString("red points chained: "+to_string(redChained));
    //QString blueLostr = QString::fromStdString("blue points chained: "+to_string(blueChained));
    //QString stepstr = QString::fromStdString("steps passed: "+to_string(steps));



    widget = new QWidget();
    grid = new QGridLayout (this);
    maxStep = new QLineEdit("Maximum step number", this);
    redLost = new QLabel("red points chained: ", this);
    blueLost = new QLabel("blue points chained: ", this);
    stepCount = new QLabel("steps passed: 0",this);


    redLost->setText("red points chained: " + QString::number(redChained));
    blueLost->setText("blue points chained: " + QString::number(blueChained));


    buttonBack = new QPushButton("-1 step", this);
    buttonBack->resize(20 , 20);
    buttonNew = new QPushButton("new game", this);
    buttonNew->resize(20 , 20);
    buttonExit = new QPushButton("exit", this);
    buttonExit->resize(20 , 20);

    buttonRight = new QPushButton("right", this);
    buttonRight->resize(10 , 40);
    buttonLeft = new QPushButton("left", this);
    buttonLeft->resize(10 , 40);
    buttonUp = new QPushButton("up", this);
    buttonUp->resize(20 , 20);
    buttonDown = new QPushButton("down", this);
    buttonDown->resize(20 , 20);

    pixmap = new QPixmap(playSize,playSize);


    cellSize = cellSize0*scale;

    QPainter painter;
        painter.begin( pixmap);
        painter.fillRect(0, 0, pixmap->width(), pixmap->height(),QColor (255, 255, 255));
        painter.drawRect(-1, -1, playSize+1, playSize+1);
        for (int i=1;i<playSize/cellSize0/scaleMin;i++){painter.drawLine(0, i*cellSize + (offsetY-1000)*scale , playSize, i*cellSize + (offsetY-1000)*scale);}
        for (int i=1;i<playSize/cellSize0/scaleMin;i++){painter.drawLine(i*cellSize + (offsetX-1000)*scale, 0 , i*cellSize + (offsetX-1000)*scale, playSize);}
        painter.end();


    playgnd = new QLabel(this);
    playgnd->setPixmap(*pixmap);

    QColor redColor(255, 0, 0);
    QColor blueColor(0, 0, 255);
    QColor blackColor(255, 255, 255);
    red = new QPen( redColor );
    blue = new QPen( blueColor );
    black = new QPen( blackColor );
    red->setWidth( 3 );
    blue->setWidth( 3 );
    black->setWidth( 1 );

    grid->addWidget( buttonBack, 0, 0);
    grid->addWidget( maxStep, 0, 1);
    grid->addWidget( buttonNew, 0, 2);
    grid->addWidget( buttonExit, 0, 4, 1, 2);
    grid->addWidget( playgnd, 1, 0, 3,3);
 //       grid->setColumnMinimumWidth(0, playSize+10);
 //       grid->setRowMinimumHeight(1, playSize+10);

    grid->addWidget( buttonUp, 1, 3, 1, 2);
    grid->addWidget( buttonLeft, 2, 3, 2, 1);
    grid->addWidget( buttonRight, 2, 4, 2, 1);
    grid->addWidget( buttonDown, 3, 3, 1, 2);

    grid->addWidget(redLost, 4 , 0);
    grid->addWidget(blueLost, 4 , 1);
    grid->addWidget(stepCount, 4 , 2);

    widget->setLayout(grid);
    setCentralWidget(widget);                             //WHY it is NOT working w/out this variable widget??

    connect(buttonExit, SIGNAL(clicked()), this, SLOT(EXIT()));
    connect(buttonNew, SIGNAL(clicked()), this, SLOT(NEWgame()));
    connect(this, SIGNAL(reDraw()), this, SLOT(REDRAW()));
    connect(buttonRight, SIGNAL(clicked()), this, SLOT(MOVEright()));
    connect(buttonLeft, SIGNAL(clicked()), this, SLOT(MOVEleft()));
    connect(buttonUp, SIGNAL(clicked()), this, SLOT(MOVEup()));
    connect(buttonDown, SIGNAL(clicked()), this, SLOT(MOVEdown()));



}


void PointsWindow::mousePressEvent(QMouseEvent *event)
{
    //setMouseTracking(true);

    cellSize = 25*scale;

    QPen *pen;
    if (steps & 1) {pen = blue;} else pen = red;

    Px = event->x() + offsetX0 - (offsetX-1000) * scale;
    Py = event->y() + offsetY0 - (offsetY-1000) * scale;

    //double u[1000][2];
    double modX, modY;
    int whX, whY;

    whX = Px / cellSize;
    whY = Py / cellSize;


    modX = Px - whX * cellSize;
    modY = Py - whY * cellSize;


    if ((modX <= cellSize/2)) {actPx = Px - modX;} else actPx = Px - modX + cellSize;
    if ((modY <= cellSize/2)) {actPy = Py - modY;} else actPy = Py - modY + cellSize;

    ofstream myfile ("gamelog.txt");
    if (myfile.is_open())
    {
        myfile << actPx  << " " << actPy;  //????????????
        myfile << "\n";
    }
    myfile.close();


    QPainter painter;
    painter.begin( pixmap );
    painter.setPen( *pen );
    painter.drawEllipse(actPx - 2.5 * scale + (offsetX-1000)*scale, actPy - 2.5 * scale + (offsetY-1000)*scale, 5 * scale, 5 * scale);


    steps++;
    stepCount->setText("steps passed: " + QString::number(steps));
    u[steps][0] = actPx/cellSize;
    u[steps][1] = actPy/cellSize;

    playgnd->setPixmap(*pixmap);


    //qDebug << u[steps][0] << u[steps][1] << steps << endl;
    qDebug() << "point added " << Px << Py << "by step " << steps <<  endl;
    update();
}

void PointsWindow::wheelEvent(QWheelEvent *event)
{
    QPoint numDegrees = event->angleDelta() / 8;
    int Inc = numDegrees.y();
    double scaleInc = Inc;
    if ((scale>(scaleMin+0.01))) {scale += scaleInc/100;} else {scale += abs(scaleInc/200)+scaleInc/200;}


    event->accept();
    qDebug() << "scale tuned " << scale <<  endl;
    reDraw();
}


void PointsWindow::mouseMoveEvent(QWheelEvent *event)
{


    offsetX = event->x();
    offsetY = event->y();

    qDebug() << "offset tuned " << (offsetX-1000) << (offsetY-1000) << endl;

    reDraw();
}

void PointsWindow::MOVEright()
{
    offsetX += -30;
    qDebug() << "offset tuned " << (offsetX-1000) << (offsetY-1000) << endl;
    reDraw();
}

void PointsWindow::MOVEleft()
{
    offsetX += 30;
    qDebug() << "offset tuned " << (offsetX-1000) << (offsetY-1000) << endl;
    reDraw();
}

void PointsWindow::MOVEup()
{
    offsetY += 30;
    qDebug() << "offset tuned " << (offsetX-1000) << (offsetY-1000) << endl;
    reDraw();
}

void PointsWindow::MOVEdown()
{
    offsetY += -30;
    qDebug() << "offset tuned " << (offsetX-1000) << (offsetY-1000) << endl;
    reDraw();
}

void PointsWindow::EXIT()
{
    QCoreApplication::exit(0);
}

void PointsWindow::NEWgame()
{

    QMessageBox::information(this, tr("new game"), tr("will start"));

    cellSize = 25*scale;

    steps=0;
    stepCount->setText("steps passed: " + QString::number(steps));

    for (int i = 0; i<1000; i++ ){ u[i][0]=0; u[i][1]=0;}

    reDraw();
}


void PointsWindow::REDRAW()
{
    cellSize = cellSize0*scale;

    QPainter painter;
        painter.begin( pixmap);
        painter.fillRect(0, 0, pixmap->width(), pixmap->height(),QColor (255, 255, 255));
        painter.drawRect(-1, -1, playSize+1, playSize+1);
        for (int i=1;i<playSize/cellSize0/scaleMin;i++){painter.drawLine(0, i*cellSize + (offsetY-1000)*scale , playSize, i*cellSize + (offsetY-1000)*scale);}
        for (int i=1;i<playSize/cellSize0/scaleMin;i++){painter.drawLine(i*cellSize + (offsetX-1000)*scale, 0 , i*cellSize + (offsetX-1000)*scale, playSize);}

        for (int j=0; j<4000; j++)
        {
            QPen *pen;
            if (j & 1) {pen = blue;} else pen = red;
            painter.setPen( *pen );
            painter.drawEllipse(u[j][0]*cellSize - 2.5 * scale + (offsetX-1000)*scale, u[j][1]*cellSize - 2.5 * scale + (offsetY-1000)*scale, 5 * scale, 5 * scale);
        }

        painter.end();
        playgnd->setPixmap(*pixmap);
        update();


}


PointsWindow::~PointsWindow()
{

}
