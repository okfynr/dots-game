#include <QtGui>
#include <QApplication>
#include <QtWidgets>
#include <stdio.h>
#include <string>            //god, why are there so many directives...
#include <QGraphicsScene>
#include <QDebug>
#include <cmath>
#include <iostream>
#include <fstream>
#include <vector>
#include "pointswindow.h"

using namespace std;

const int playSize=600;
QVector<QVector<QPair<int, int>>> old_chains;
QVector<QPair<int, int>> blueChaind, redChaind;


    // CREATING PROGRAM INTERFACE

    //wery big constructor, better to be changed with composite(?) pattern
PointsWindow::PointsWindow(QWidget *parent)
    : QMainWindow(parent)
{
    setWindowTitle("Dots game");
    //showFullScreen();
    setMouseTracking(true);    
    redChained = 0; blueChained = 0; steps = 0;
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


    maximumStep = maxStep -> text().toInt(&ok);
    if (!ok)
        maximumStep = 2000;
    if ((maximumStep > 2000) || (maximumStep<0))
    {
        maximumStep = 2000;
        maxStep  ->  setText("2000");
    }

    redLost->setText("red dots chained: " + QString::number(redChained));
    blueLost->setText("blue dots chained: " + QString::number(blueChained));


    buttonBack = new QPushButton("-1 step", this);
    buttonBack->resize(20 , 20);
    buttonNew = new QPushButton(" new game", this);
    buttonNew->resize(20 , 20);
    buttonExit = new QPushButton("save && exit", this);
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
        painter.fillRect(0, 0, pixmap->width(), pixmap->height(), QColor (255, 255, 255));
        painter.drawRect(-1, -1, playSize + 1, playSize + 1);
        for (int i = 1; i < playSize / cellSize0 / scaleMin; i++) {
            painter.drawLine(0, i * cellSize + (offsetY - 1000) * scale,
                      playSize, i * cellSize + (offsetY - 1000) * scale);
        }
        for (int i = 1; i < playSize / cellSize0 / scaleMin; i++) {
            painter.drawLine(i * cellSize + (offsetX - 1000) * scale, 0,
                             i * cellSize + (offsetX - 1000) * scale, playSize);
        }
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
    connect(buttonBack, SIGNAL(clicked()), this, SLOT(STEPback()));

    QMessageBox::information(this, tr("new game will start right now"), tr("first dot is blue                                          \n be prepared!"));

}

  // file for game logging, may be used for replays, loadings, etc

ofstream myfile ("gamelog.txt");


     // new dot is added with following:

void PointsWindow::mouseDoubleClickEvent(QMouseEvent *event)
{
    //myfile.open("gamelog.txt");
    //setMouseTracking(true);
    double modX, modY, actPy, actPx;
    int Px, Py, whX, whY;
    bool nextstep = true;


    cellSize = 25 * scale;
    steps++;    //should be placed in 212 row?


    QPen *pen;
    if (steps & 1) {
        pen = blue;
    } else {
        pen = red;
    }


    Px = event->x() + offsetX0 - (offsetX - 1000) * scale;
    Py = event->y() + offsetY0 - (offsetY - 1000) * scale;

    whX = Px / cellSize;
    whY = Py / cellSize;
    modX = Px - whX * cellSize;
    modY = Py - whY * cellSize;
    if ((modX <= cellSize / 2)) {actPx = Px - modX;} else actPx = Px - modX + cellSize;
    if ((modY <= cellSize / 2)) {actPy = Py - modY;} else actPy = Py - modY + cellSize;



    for (int k = 1; k < steps; k++) {
        if ((actPx / cellSize == u[k][0]) && (actPy / cellSize == u[k][1]))
        {
            nextstep = false;
            QMessageBox::warning(this, tr("Hey, you"), tr("Point the dot more accurately!"));
            steps--;

        }
    }


    if (nextstep)
    {
        u[steps][0] = actPx / cellSize;
        u[steps][1] = actPy / cellSize;
        if (myfile.is_open())
        {
            myfile << u[steps][0]  << " " << u[steps][1] << " " << steps;
            myfile << "\n";
        }


        /*QPainter painter;
        painter.begin( pixmap );
        painter.setPen( *pen );
        painter.drawEllipse(actPx - 2.5 * scale + (offsetX-1000) * scale,
                            actPy - 2.5 * scale + (offsetY-1000) * scale,
                            5 * scale,
                            5 * scale);

        playgnd->setPixmap(*pixmap);*/

        stepCount->setText("steps passed: " + QString::number(steps));
        reDraw();


        //it must invoke calculations for chainind dots, after pointing of new dot


        typedef QVector<int>  sending_type;
        qRegisterMetaType<sending_type>("sending_type");

        thread = new Chaining(u[steps][0], u[steps][1], steps, this);

        connect(thread, SIGNAL(newChain(sending_type)), this, SLOT(NEWchain(sending_type)));

        thread  ->  start();

        qDebug() << "point added " << u[steps][0] << u[steps][1] << "by step " << steps << "max " << maximumStep;
        update();
    }







    if ((steps > (maximumStep - 1)))
    {
        QMessageBox::information(this, tr("Game over"), tr("your game was saved"));
        EXIT();
    }
}

  // changing the scale of game grid

void PointsWindow::wheelEvent(QWheelEvent *event)
{
    QPoint numDegrees = event->angleDelta() / 8;
    int Inc = numDegrees.y();
    double scaleInc = Inc;
    if ((scale > (scaleMin + 0.01))) {
        scale += scaleInc / 100;
    } else {
        scale += abs(scaleInc / 200) + scaleInc / 200;
    }


    event->accept();
    qDebug() << "scale tuned " << scale;
    reDraw();
}


  // changing place of playground, implied, that you will move it with pushed button

void PointsWindow::mouseMoveEvent(QMouseEvent *event)
{

    QPointF Point = event->screenPos();
    offsetX = Point.x() / scale;
    offsetY = Point.y() / scale;

    qDebug() << "offset tuned " << (offsetX - 1000) << (offsetY - 1000) << endl;

    reDraw();
}

// buttons to move playgnd, if you despise mouse for this

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
    myfile.close();
    QCoreApplication::exit(0);
}

 // it will operates every new game

void PointsWindow::NEWgame()
{

    QMessageBox::information(this, tr("new game"), tr("will start now \n(changes are lost)"));

    cellSize = 25 * scale;

    steps = 0;
    old_chains.clear();
    stepCount->setText("steps passed: " + QString::number(steps));

    for (int i = 0; i < 4000; i++ ){ u[i][0] = 0; u[i][1] = 0;}

    gamesCount++;
    maximumStep = maxStep -> text().toInt(&ok);
    if (!ok)
    {
        maximumStep = 2000;
        maxStep  ->  setText("Maximum step number"); // will not be needed with vectors
    }
    if ((maximumStep > 2000) || (maximumStep < 0))
    {
        maximumStep = 2000;
        maxStep  ->  setText("2000");
    }
    reDraw();
    myfile.close();
    myfile.open("gamelog.txt");
}

  // member function for latest pointed dot deleting

void PointsWindow::STEPback() //not fully works: for chains, also needed foe counters
{

    qDebug() << "point deleted: " << u[steps][0] << u[steps][1] << "by step " << steps <<  endl;
    if (myfile.is_open())
    {
        myfile << u[steps][0]  << " " << u[steps][1]<< " " << steps;
        myfile << "\n";
    }

    vector<int> deleted_chains;
    // at first, let's delete old chain(s), contains deleting dot, find it:
    for (int m = 0; m < old_chains.size(); m++) {        //for every old chain
        for (int j = 0; j < old_chains[m].size(); j++) {     //for every point in this old chain
            if ((u[steps][0] == old_chains[m][j].first)
             && (u[steps][1] == old_chains[m][j].second)) {
                deleted_chains.push_back(m);
                qDebug() << "old chain" << m << "founded to delete because of dot" << old_chains[m][j];
            }
        }
    }
    //delete it:
    for (int i = 0; i < (int)deleted_chains.size(); i++) {
        old_chains[deleted_chains[i]].clear();
        qDebug() << "chain" << deleted_chains[i] << "deleted";
    }
    qDebug() << "old_chains:" << old_chains;

    //if (old_chains[0].size() < 2) old_chains.clear();

    // then, delete our dot:
    u[steps][0] = 0;
    u[steps][1] = 0;

    if ((steps > 0)) {
        steps--;
    } else {
        steps = 0; QMessageBox::warning(this, tr("Hey, you"), tr("No more steps back!"));
    }
    stepCount->setText("steps passed: " + QString::number(steps));

    reDraw();
}

 // it will operate after every dot pointed or deleted

void PointsWindow::REDRAW()
{
    cellSize = cellSize0 * scale;

    QPainter painter;
        painter.begin( pixmap);
        painter.fillRect(0, 0, pixmap->width(), pixmap->height(),QColor (255, 255, 255));
        painter.drawRect(-1, -1, playSize + 1, playSize + 1);
        for (int i = 1; i < playSize / cellSize0 / scaleMin; i++) {
            painter.drawLine(0, i * cellSize + (offsetY - 1000) * scale,
                      playSize, i * cellSize + (offsetY - 1000) * scale);
        }
        for (int i = 1; i < playSize / cellSize0 / scaleMin; i++) {
            painter.drawLine(i * cellSize + (offsetX - 1000) * scale, 0,
                             i * cellSize + (offsetX - 1000) * scale, playSize);
        }
    painter.end();

    painter.begin( pixmap);
        for (int j = 1; j < 4000; j++)
        {
            QPen *pen;
            if (j & 1) {pen = blue;} else pen = red;
            painter.setPen( *pen );
            painter.drawEllipse(u[j][0] * cellSize - 2.5 * scale + (offsetX - 1000) * scale,
                                u[j][1] * cellSize - 2.5 * scale + (offsetY - 1000) * scale,
                                5 * scale,
                                5 * scale);
        }

        painter.end();
        //myfile ("gamelog"  + to_string(gamesCount) +".txt");
        OLDchains();

        playgnd->setPixmap(*pixmap);
        update();
}

 //function for checking new chain for intersections with old chains;


void PointsWindow::NEWchain(sending_type chain)
{
    if (chain[0] & 1) {
        qDebug() << "received red chain: " << chain << endl;
    }
    else {
        qDebug() << "received blue chain: " << chain << endl;
    }
    bool circuit_broken = true, chained = false, chain_added = false;
    int broken = 0; //iterator for chained dots in new cycle: if not 0, cycle will not be added in old chains

    //meet the old_chains massive of coordinats of points of chains and color in last member for every chain
    for (int m = 0; m < (int)old_chains.size(); m++) {      //for every old chain
        chained = false;
        chain_added = true; // so chain will not be added in old chains by default
        qDebug() << steps << "dots will be checked";
        for (int k = 0; k < steps; k++){        // for every dot, except "0" damned dot -> k+1 used

            qDebug() << "checking dot chaining...";
            chained = false;
            int j = old_chains[m].size() - 2;
            for (int i = 0; i < ((int)old_chains[m].size() - 1); i++) { // for every dot in chaining (around) dot

                //now, meet the ith and jth points of chaining edge and chained point coordinates
                double xi = old_chains[m][i].first, yi = old_chains[m][i].second,
                        xj = old_chains[m][j].first, yj = old_chains[m][j].second,
                        point_x = u[k+1][0], point_y = u[k+1][1];

                int beam_angle = 25; // angle for our beam, searching for chaining edges
                rotate(beam_angle, xi, yi);
                rotate(beam_angle, xj, yj);
                rotate(beam_angle, point_x, point_y);

                if ( (yi < point_y && yj >= point_y || yj < point_y && yi >= point_y)
                     &&
                     (xi + (point_y - yi) / (yj - yi) * (xj - xi) < point_x) ) {
                    chained = !chained;
                    qDebug() << "edge at left!";
                }
                j = i;
            }
            if (old_chains[m].last().first == (k & 1)) {
                chained = false; // is our dot not same color as chain?

            }

            qDebug()  << k << "th dot, chained in old chain " << m << " info is: " << chained;
            qDebug() << "color is same, it is" << (old_chains[m].last().first == (k & 1));


            if (chained) {
                qDebug() << "chained dot" << k << "detected";
                QPair<int, int> k_th{(int)u[k][0], (int)u[k][1]};

                if (old_chains[m].last().first & 1) {
                    blueChaind.push_back(k_th);
                    qDebug() << "so, blueChaind:" << blueChaind;
                } else {
                    redChaind.push_back(k_th);
                    qDebug() << "so, redChaind:" << redChaind;
                }

                for (auto checked : chain) {
                    qDebug() << "checking chain for chained dots...";
                    if (k == checked) {
                        broken++;
                        qDebug() << "in new chain found chained dots:" << broken;
                    }
                }
            }

        }

    }

    if ((!chain_added) || (broken == 0)) circuit_broken = false;
    qDebug() << "circuit is" << circuit_broken << "broken!";

    if (!circuit_broken) {
        qDebug() << "accepted!" << endl;
        QVector<QPair<int, int>> NewOld;

        for (int k = 0; k < (int)chain.size(); k++){
            QPair<int, int> k_th{(int)u[chain[k]+1][0], (int)u[chain[k]+1][1]};
            NewOld.push_back(k_th);
        }
        QPair<int, int> color{(chain[0] & 1), 0};
        qDebug() << "got chain, colored" << color << "in old chains";
        NewOld.push_back(color);    //color of chain should be written also
        old_chains.push_back(NewOld);
    }
    qDebug() << "old_chains:" << old_chains << endl;


    //to remove double counting of chaind dots:

    sort(blueChaind.begin(), blueChaind.end());
    blueChaind.resize(unique(blueChaind.begin(), blueChaind.end()) - blueChaind.begin());

    sort(redChaind.begin(), redChaind.end());
    redChaind.resize(unique(redChaind.begin(), redChaind.end()) - redChaind.begin());

    blueChained = blueChaind.size();
    redChained = redChaind.size();

    blueLost->setText("blue points chained: " + QString::number(blueChained));
    redLost->setText("red points chained: " + QString::number(redChained));

}

// some new functions for actions, bounded with chaining dots

void PointsWindow::OLDchains()
{
    QPainter painter;
    painter.begin( pixmap);
    for (int m = 0; m < (int)old_chains.size(); m++) {  // for every old chain
        int j = old_chains[m].size() - 2;

        QPainterPath path;
        path.moveTo(old_chains[m][0].first * cellSize  + (offsetX - 1000) * scale,
                    old_chains[m][0].second * cellSize  + ( offsetY - 1000) * scale);

        for (int i = 0; i < ((int)old_chains[m].size() - 1); i++) { // for every dot/edge in old chain
            QPen *pen;                                              //except last -- it is for color

            if (old_chains[m].last().first & 1) {pen = red;} else pen = blue;
            painter.setPen( *pen );
            painter.drawLine(old_chains[m][j].first * cellSize  + (offsetX - 1000) * scale,
                             old_chains[m][j].second * cellSize  + ( offsetY - 1000) * scale,
                             old_chains[m][i].first * cellSize  + (offsetX - 1000) * scale,
                             old_chains[m][i].second * cellSize  + (offsetY - 1000) * scale);
            j = i;
            path.lineTo(old_chains[m][i].first * cellSize  + (offsetX - 1000) * scale,
                        old_chains[m][i].second * cellSize  + ( offsetY - 1000) * scale);

        }
        //qDebug() << path;
        QColor semiRed(255, 0, 0, 100), semiBlue(0, 0, 255, 100);
        if (old_chains[m].last().first & 1) {
            painter.fillPath(path, semiRed);
        } else {
            painter.fillPath(path, semiBlue);
        }


    }
    painter.end();
}

void PointsWindow::rotate(int angle, double &x, double &y)
{
    int x1 = x * cos(angle) - y * sin(angle);
    y = x * sin(angle) + y * cos(angle);
    x = x1;
}

  // empty destructor

PointsWindow::~PointsWindow()
{

}
