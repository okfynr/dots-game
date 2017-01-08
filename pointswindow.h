#ifndef POINTSWINDOW_H
#define POINTSWINDOW_H

#include <QWidget>
#include <QGraphicsScene>
#include<QGraphicsSceneMouseEvent>


#include <QMainWindow>

class QGridLayout;
class QPixmap;
class QLabel;
class QPen;
class QPushButton;
class QLineEdit;

class PointsWindow : public QMainWindow
{
    Q_OBJECT

public:
    PointsWindow(QWidget *parent = 0);
    ~PointsWindow();

signals:
    void reDraw();

private slots:
    void EXIT();
    void NEWgame();
    void REDRAW();
    void MOVEright();
    void MOVEleft();
    void MOVEup();
    void MOVEdown();
    void STEPback();
private:
    QWidget *widget;
    QGridLayout *grid;
    QLabel *redLost, *blueLost, *stepCount, *playgnd;
    QPixmap *pixmap;
    QPen *black, *red, *blue;
    QLineEdit *maxStep;
    QPushButton *buttonBack, *buttonNew, *buttonExit;
    QPushButton *buttonRight, *buttonLeft, *buttonUp, *buttonDown;
    int redChained, blueChained, steps;

    void mousePressEvent(QMouseEvent *event);
    void wheelEvent(QWheelEvent *event);
    void mouseMoveEvent(QWheelEvent *event);


    //void mouseMoveEvent(QMouseEvent *event);
    int gamesCount=0, maximumStep=2;
    bool ok;
    double cellSize, cellSize0=25, offsetX0, offsetY0, offsetX=400, offsetY=400, scale=1, scaleMin=0.25;
    double u[4000][2];

};

#endif // POINTSWINDOW_H
