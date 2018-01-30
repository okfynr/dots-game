#include <QtGui>
#include <QApplication>
#include <QtWidgets>
#include <string>
#include <QDebug>
#include <cmath>
#include <iostream>
#include <fstream>
#include <vector>
#include "pointswindow.h"

using namespace std;

static const int playSize=600;



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


    //QString redLostr = QString::fromStdString("red points chained: "+to_string(redChained));
    //QString blueLostr = QString::fromStdString("blue points chained: "+to_string(blueChained));
    //QString stepstr = QString::fromStdString("steps passed: "+to_string(steps));


    widget = new QWidget(this);
    grid = new QGridLayout ();
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

    auto buttonLong = [&](QString text){
        QPushButton * toButton = new QPushButton(text, this);
        toButton->resize(10 , 40);
        return toButton;
    };

    auto buttonSquare = [&](QString text){
        QPushButton * toButton = new QPushButton(text, this);
        toButton->resize(20 , 20);
        return toButton;
    };


    buttonBack = buttonSquare("-1 step");
    buttonNew = buttonSquare(" new game");
    buttonExit = buttonSquare("save && exit");

    buttonRight = buttonLong("right");
    buttonLeft = buttonLong("left");
    buttonUp = buttonSquare("up");
    buttonDown = buttonSquare("down");

    pixmap = new QPixmap(playSize,playSize);


    cellSize = cellSize0*scale;

    QPainter painter;
        painter.begin( pixmap);
        painter.fillRect(0, 0, pixmap->width(), pixmap->height(), QColor (255, 255, 255));
        painter.drawRect(-1, -1, playSize + 1, playSize + 1);

        for (int i = 1; i < playSize / cellSize0 / scaleMin; i++) {
            painter.drawLine(0, i * cellSize + (offsetY - 1000) * scale, //why -1000? to avoid ugly effects near 0
                      playSize, i * cellSize + (offsetY - 1000) * scale);//just a quite big number, nothing else
        }
        for (int i = 1; i < playSize / cellSize0 / scaleMin; i++) {
            painter.drawLine(i * cellSize + (offsetX - 1000) * scale, 0,
                             i * cellSize + (offsetX - 1000) * scale, playSize);
        }
        painter.end();


    playgnd = new QLabel(this);
    playgnd->setPixmap(*pixmap);

    red = new QPen( QColor(255, 0, 0) );
    blue = new QPen( QColor(0, 0, 255) );
    black = new QPen( QColor(255, 255, 255) );
    red->setWidth( 3 );
    blue->setWidth( 3 );
    black->setWidth( 1 );

    grid->addWidget( buttonBack, 0, 0);
    grid->addWidget( maxStep, 0, 1);
    grid->addWidget( buttonNew, 0, 2);
    grid->addWidget( buttonExit, 0, 4, 1, 2);
    grid->addWidget( playgnd, 1, 0, 3,3);

    grid->addWidget( buttonUp, 1, 3, 1, 2);
    grid->addWidget( buttonLeft, 2, 3, 2, 1);
    grid->addWidget( buttonRight, 2, 4, 2, 1);
    grid->addWidget( buttonDown, 3, 3, 1, 2);

    grid->addWidget(redLost, 4 , 0);
    grid->addWidget(blueLost, 4 , 1);
    grid->addWidget(stepCount, 4 , 2);

    widget->setLayout(grid);
    setCentralWidget(widget);

    connect(buttonExit, SIGNAL(clicked()), this, SLOT(EXIT()));
    connect(buttonNew, SIGNAL(clicked()), this, SLOT(NEWgame()));
    connect(this, SIGNAL(reDraw()), this, SLOT(REDRAW()));
    connect(buttonRight, SIGNAL(clicked()), this, SLOT(MOVEright()));
    connect(buttonLeft, SIGNAL(clicked()), this, SLOT(MOVEleft()));
    connect(buttonUp, SIGNAL(clicked()), this, SLOT(MOVEup()));
    connect(buttonDown, SIGNAL(clicked()), this, SLOT(MOVEdown()));
    connect(buttonBack, SIGNAL(clicked()), this, SLOT(STEPback()));

    QMessageBox::information(this, tr("new game will start right now"),
                             tr("first dot is blue                                          \n be prepared!"));
    u.push_back({0, 0}); //will be removed, when step will begin with 0

}

  // file for game logging, may be used for replays, loadings, etc

ofstream myfile ("gamelog.txt");


     // new dot is added with following:

void PointsWindow::mousePressEvent(QMouseEvent *event)
{
    mouse_pressed_pos.first = event->screenPos().toPoint().x();
    mouse_pressed_pos.second = event->screenPos().toPoint().y();
    offset_pressed_pos.first = offsetX;
    offset_pressed_pos.second = offsetY;
    if (event->button() == 1) {
        mouse_pressed_pos.first = 0;
        mouse_pressed_pos.second = 0;
    } else
        return;

    //myfile.open("gamelog.txt");
    //setMouseTracking(true);
    double modX, modY, actPy, actPx;
    int Px, Py, whX, whY;
    bool nextstep = true;


    cellSize = 25 * scale;
    steps++;    //should be placed in 212 row?


    /*QPen *pen;
    if (steps & 1) {
        pen = blue;
    } else {
        pen = red;
    }*/


    Px = event->x() + offsetX0 - (offsetX - 1000) * scale;
    Py = event->y() + offsetY0 - (offsetY - 1000) * scale;

    whX = Px / cellSize;
    whY = Py / cellSize;
    modX = Px - whX * cellSize;
    modY = Py - whY * cellSize;
    if ((modX <= cellSize / 2)) {actPx = Px - modX;} else actPx = Px - modX + cellSize;
    if ((modY <= cellSize / 2)) {actPy = Py - modY;} else actPy = Py - modY + cellSize;



    for (size_t k = 1; k < u.size(); k++) {
        if ((actPx / cellSize == u[k][0]) && (actPy / cellSize == u[k][1]))
        {
            nextstep = false;
            QMessageBox::warning(this, tr("Hey, you"), tr("Point the dot more accurately!"));
            steps--;

        }
    }


    if (nextstep)
    {
        double fir = actPx / cellSize, sec = actPy / cellSize;
        vector<int> added{static_cast<int>(fir), static_cast<int>(sec)};
        u.push_back(added);

        //u[steps][0] = actPx / cellSize;
        //u[steps][1] = actPy / cellSize;
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


        typedef QVector<int>  sending_type;               // crutch again
        qRegisterMetaType<sending_type>("sending_type");

        thread = new Chaining(u[steps][0], u[steps][1], steps, this);

        connect(thread, SIGNAL(newChain(sending_type)), this, SLOT(NEWchain(sending_type)));

        thread  ->  start();

        //qDebug() << "point added " << u[steps][0] << u[steps][1] << "by step " << steps << "max " << maximumStep;
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
    //setMouseTracking(true);
    //QPointF Point = event->screenPos();
    int Inc = numDegrees.y();
    double scaleInc = Inc;
    if ((scale > (scaleMin + 0.01))) {
        scale += scaleInc / 100;
        //offsetX -= scale*Point.x();
        //offsetY -= scale*Point.y();
    } else {
        scale += abs(scaleInc / 200) + scaleInc / 200;
    }


    event->accept();
    qDebug() << "scale tuned " << scale;
    reDraw();
}


  // changing place of playground, implied, that you will move it with pushed right mouse button

void PointsWindow::mouseMoveEvent(QMouseEvent *event)
{
    if (mouse_pressed_pos.first == 0 && mouse_pressed_pos.second == 0) return;
    QPointF Point = event->screenPos();
    offsetX = offset_pressed_pos.first + (Point.x() - mouse_pressed_pos.first) / scale;
    offsetY = offset_pressed_pos.second + (Point.y() - mouse_pressed_pos.second) / scale;

    //qDebug() << "offset tuned " << (offsetX - 1000) << (offsetY - 1000) << endl;

    reDraw();
}

// buttons to move playgnd, if you despise mouse to use

void PointsWindow::MOVEright()
{
    offsetX += -100;
    qDebug() << "offset tuned " << (offsetX-1000) << (offsetY-1000) << endl;
    reDraw();
}

void PointsWindow::MOVEleft()
{
    offsetX += 100;
    qDebug() << "offset tuned " << (offsetX-1000) << (offsetY-1000) << endl;
    reDraw();
}

void PointsWindow::MOVEup()
{
    offsetY += 100;
    qDebug() << "offset tuned " << (offsetX-1000) << (offsetY-1000) << endl;
    reDraw();
}

void PointsWindow::MOVEdown()
{
    offsetY += -100;
    qDebug() << "offset tuned " << (offsetX-1000) << (offsetY-1000) << endl;
    reDraw();
}





void PointsWindow::EXIT()
{
    myfile.close();
    QCoreApplication::exit(0);
}

 // it will operates every new game

void PointsWindow::NEWgame() //chains are not updating
{

    QMessageBox::information(this, tr("new game"), tr("will start now \n(changes are lost)"));

    cellSize = 25 * scale;

    steps = 0;
    old_chains.clear();
    stepCount->setText("steps passed: " + QString::number(steps));

    u.clear();
    u.push_back({0, 0});


    gamesCount++;
    maximumStep = maxStep -> text().toInt(&ok);
    if (!ok)
    {
        maximumStep = 2000;
        maxStep  ->  setText("Maximum step number");
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

void PointsWindow::STEPback() //not fully works: for chains, also needed for counters
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
    for (size_t i = 0; i < deleted_chains.size(); i++) {
        old_chains[deleted_chains[i]].clear();
        qDebug() << "chain" << deleted_chains[i] << "deleted";
    }
    qDebug() << "old_chains:" << old_chains;

    //if (old_chains[0].size() < 2) old_chains.clear();

    // then, delete our dot:

    u.pop_back();
    //u[steps][0] = 0;
    //u[steps][1] = 0;

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
        for (size_t j = 1; j < u.size(); j++)
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
    for (size_t m = 0; m < old_chains.size(); m++) {      //for every old chain

        chained = false;
        chain_added = true; // so chain will not be added in old chains by default
        qDebug() << steps << "dots will be checked";


        for (size_t k = 0; k < (u.size() - 1); k++){        // for every dot, except "0" damned dot -> k+1 used

            qDebug() << "checking dot chaining...";
            chained = false;
            size_t j = old_chains[m].size() - 2;


            for (size_t i = 0; i < (old_chains[m].size() - 1); i++) { // for every dot in chaining (around) dot

                //now, meet the ith and jth points of chaining edge and chained point coordinates
                double xi = old_chains[m][i].first, yi = old_chains[m][i].second;
                double xj = old_chains[m][j].first, yj = old_chains[m][j].second;
                double point_x = u[k+1][0], point_y = u[k+1][1];

                int beam_angle = 25; // angle for our beam, searching for chaining edges
                rotate(beam_angle, xi, yi);
                rotate(beam_angle, xj, yj);
                rotate(beam_angle, point_x, point_y);

                if ( ( (yi < point_y && yj >= point_y) || (yj < point_y && yi >= point_y) )
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

                if (old_chains[m].last().first & 1) {
                    blueChaind.push_back(qMakePair(u[k][0], u[k][1]));
                    qDebug() << "so, blueChaind:" << blueChaind;
                } else {
                    redChaind.push_back(qMakePair(u[k][0], u[k][1]));
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

        for (size_t k = 0; k < chain.size(); k++){
            NewOld.push_back(qMakePair(u[chain[k]+1][0], u[chain[k]+1][1]));
        }
        qDebug() << "got chain, colored" << (chain[0] & 1) << "in old chains";
        NewOld.push_back(qMakePair((chain[0] & 1), 0));    //color of chain should be written also???????
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

    for (size_t m = 0; m < old_chains.size(); m++) {  // for every old chain

        int j = old_chains[m].size() - 2;

        QPainterPath path;
        path.moveTo(old_chains[m][0].first * cellSize  + (offsetX - 1000) * scale,
                    old_chains[m][0].second * cellSize  + ( offsetY - 1000) * scale);

        for (size_t i = 0; i < (old_chains[m].size() - 1); i++) { // for every dot/edge in old chain

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
