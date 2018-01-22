#ifndef POINTSWINDOW_H
#define POINTSWINDOW_H

#include <QWidget>

#include <QMainWindow>

#include "chaining.h"



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
    void NEWchain(sending_type);

private:
    QWidget *widget;
    QGridLayout *grid;
    QLabel *redLost, *blueLost, *stepCount, *playgnd;
    QPixmap *pixmap;
    QPen *black, *red, *blue;
    QLineEdit *maxStep;
    QPushButton *buttonBack, *buttonNew, *buttonExit;
    QPushButton *buttonRight, *buttonLeft, *buttonUp, *buttonDown;
    Chaining *thread;

    int redChained, blueChained, steps;

    void mousePressEvent(QMouseEvent *event);
    void wheelEvent(QWheelEvent *event);
    void mouseMoveEvent(QMouseEvent *event);

    QPair<double, double> mouse_pressed_pos;
    QPair<double, double> offset_pressed_pos;

    void OLDchains();
    void rotate(int, double &, double &);

    int gamesCount=0, maximumStep=2;
    bool ok;
    double cellSize, cellSize0=25, offsetX0, offsetY0, offsetX=-600, offsetY=-600, scale=1, scaleMin=0.25;
    double u[4000][2];  //MUST be changed to vector<pair<double, double>> or to vector<vector<double>>   !!!

};

#endif // POINTSWINDOW_H
