#include <QtGui>
#include <QApplication>
#include <QtWidgets>
#include <string>
#include <QDebug>
#include <iostream>
#include <fstream>
#include <vector>
#include "pointswindow.h"

using namespace std;

static const int playSize = 600;


    // CREATING PROGRAM INTERFACE

PointsWindow::PointsWindow(QWidget *parent)
    : QMainWindow(parent)
{
    setWindowTitle("Dots game");
    //showFullScreen();
    setMouseTracking(true);

    gamesCount = 0;
    maximumStep = 2;
    cellSize0 = 25;
    offsetX = -600;
    offsetY = -600;
    scale = 1;
    scaleMin = 0.25;
    offsetX0 = -10;
    offsetY0 = -40;
    redChained = 0;
    blueChained = 0;
    steps = 0;

    current_game = new Game();




    //QString redLostr = QString::fromStdString("red points chained: "+to_string(redChained));
    //QString blueLostr = QString::fromStdString("blue points chained: "+to_string(blueChained));
    //QString stepstr = QString::fromStdString("steps passed: "+to_string(steps));


    widget = new QWidget(this);
    grid = new QGridLayout ();
    maxStep = new QLineEdit("Maximum step number", this);
    redLost = new QLabel("red points chained: ", this);
    blueLost = new QLabel("blue points chained: ", this);
    stepCount = new QLabel("steps passed: 0",this);


    maximumStep = maxStep -> text().toInt(&maxstep_value_ok);
    if (!maxstep_value_ok)
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
    connect(current_game, SIGNAL(chains_changed()), this, SLOT(REDRAW()));
    connect(buttonRight, SIGNAL(clicked()), this, SLOT(MOVEright()));
    connect(buttonLeft, SIGNAL(clicked()), this, SLOT(MOVEleft()));
    connect(buttonUp, SIGNAL(clicked()), this, SLOT(MOVEup()));
    connect(buttonDown, SIGNAL(clicked()), this, SLOT(MOVEdown()));
    connect(buttonBack, SIGNAL(clicked()), this, SLOT(STEPback()));

    QMessageBox::information(this, tr("new game will start right now"),
                             tr("first dot is blue                                          \n be prepared!"));

    reDraw();

}

  // file for game logging, may be used for replays, loadings, etc

static ofstream myfile ("gamelog.txt");


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


    bool nextstep = true;
    cellSize = 25 * scale;
    steps++;
    double pointX = std::floor((event->x() + offsetX0 - (offsetX - 1000) * scale) / cellSize + 0.5);
    double pointY = std::floor((event->y() + offsetY0 - (offsetY - 1000) * scale) / cellSize + 0.5);


    for (size_t k = 1; k < current_game->getSize(); ++k) {
        if (fabs(pointX - current_game->getPointX(k)) < 1e-3
         && fabs(pointY - current_game->getPointY(k)) < 1e-3)
        {
            nextstep = false;
            //QMessageBox::warning(this, tr("Hey, you"), tr("Point the dot more accurately!"));
            steps--;

        }
    }

    if (nextstep)
    {
        vector<int> added{static_cast<int>(pointX),
                          static_cast<int>(pointY)};
        current_game->addPoint(added);

        blueLost->setText("blue points chained: " + QString::number(current_game->getBlueChainedSize()));
        redLost->setText("red points chained: " + QString::number(current_game->getRedChainedSize()));

        if (myfile.is_open())
        {
            myfile << current_game->getPointX(current_game->getSize() - 1) << " "
                   << current_game->getPointY(current_game->getSize() - 1) << " "
                   << current_game->getSize();
            myfile << "\n";
        }

        stepCount->setText(QString("steps passed: ") + QString::number(current_game->getSize()));
        reDraw();
        update();
    }







    if ((steps > static_cast<size_t>(maximumStep - 1)))
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
    if (mouse_pressed_pos.first == 0 && mouse_pressed_pos.second == 0)
        return;
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

 // it will operate every new game

void PointsWindow::NEWgame() //chains are not updating
{

    QMessageBox::information(this, tr("new game"), tr("will start now \n(changes are lost)"));

    cellSize = 25 * scale;

    current_game->setClear();

    steps = 0;
    stepCount->setText("steps passed: " + QString::number(steps));


    gamesCount++;
    maximumStep = maxStep -> text().toInt(&maxstep_value_ok);
    if (!maxstep_value_ok)
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

  // latest pointed dot deleting

void PointsWindow::STEPback() //not fully works: for chains, also needed for counters
{

    qDebug() << "point deleted: " << current_game->getPointX(steps) << current_game->getPointY(steps)
             << "by step " << steps <<  endl;
    if (myfile.is_open())
    {
        myfile << current_game->getPointX(steps)  << " " << current_game->getPointY(steps)<< " " << steps;
        myfile << "\n";
    }

    current_game->deletePoint();

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
        for (int i = 1; i < playSize / cellSize0 / scaleMin; ++i) {
            int linePos(static_cast<int>(i * cellSize + (offsetY - 1000) * scale));
            painter.drawLine(0, linePos, playSize, linePos);
        }
        for (int i = 1; i < playSize / cellSize0 / scaleMin; ++i) {
            int linePos(static_cast<int>(i * cellSize + (offsetX - 1000) * scale));
            painter.drawLine(linePos, 0, linePos, playSize);
        }
    painter.end();

    painter.begin( pixmap);
        for (size_t j = 1; j < current_game->getSize(); ++j)
        {
            QPen *pen;
            if (j & 1) {pen = blue;} else pen = red;
            painter.setPen( *pen );
            int pointX(static_cast<int>(current_game->getPointX(j) * cellSize - 2.5 * scale + (offsetX - 1000) * scale));
            int pointY(static_cast<int>(current_game->getPointY(j) * cellSize - 2.5 * scale + (offsetY - 1000) * scale));
            painter.drawEllipse(pointX, pointY, 5 * static_cast<int>(scale), 5 * static_cast<int>(scale));
        }

        painter.end();
        //myfile ("gamelog"  + to_string(gamesCount) +".txt");
        OLDchains();

        playgnd->setPixmap(*pixmap);
        update();
}

// some new functions for actions, bounded with chaining dots

void PointsWindow::OLDchains()
{
    QPainter painter;
    painter.begin( pixmap);

    // for every old chain
    for (int m = 0; m < current_game->getChainsSize(); ++m) {

        int j = current_game->getChainSize(m) - 2;

        QPainterPath path;
        path.moveTo(current_game->getChainsElem(m, 0).first * cellSize  + (offsetX - 1000) * scale,
                    current_game->getChainsElem(m, 0).second * cellSize + (offsetY - 1000) * scale);

        // for every dot/edge in old chain
        for (int i = 0; i < (current_game->getChainSize(m) - 1); ++i) {

            //except last -- it is for color of the chain
            QPen *pen;

            if (current_game->getChainsElem(m, current_game->getChainSize(m) - 1/* ??? */).first & 1)
                pen = red;
            else
                pen = blue;

            painter.setPen( *pen );
            int x1(static_cast<int>(current_game->getChainsElem(m, j).first * cellSize  + (offsetX - 1000) * scale));
            int y1(static_cast<int>(current_game->getChainsElem(m, j).second * cellSize  + ( offsetY - 1000) * scale));
            int x2(static_cast<int>(current_game->getChainsElem(m, i).first * cellSize  + (offsetX - 1000) * scale));
            int y2(static_cast<int>(current_game->getChainsElem(m, i).second * cellSize  + (offsetY - 1000) * scale));
            painter.drawLine(x1, y1, x2, y2);
            j = i;
            path.lineTo(x2, y2);

        }
        //qDebug() << path;
        QColor semiRed(255, 0, 0, 100), semiBlue(0, 0, 255, 100);
        if (current_game->getChainsElem(m, current_game->getChainSize(m) - 1/* ??? */).first & 1) {
            painter.fillPath(path, semiRed);
        } else {
            painter.fillPath(path, semiBlue);
        }


    }
    painter.end();
}

PointsWindow::~PointsWindow()
{
    delete current_game;
}
