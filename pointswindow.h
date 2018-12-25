#ifndef POINTSWINDOW_H
#define POINTSWINDOW_H

#include <QWidget>

#include <QMainWindow>

#include "game.h"



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
    PointsWindow(QWidget *parent = nullptr);
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
    Game *current_game;
    QWidget *widget;
    QGridLayout *grid;
    QLabel *redLost, *blueLost, *stepCount, *playgnd;
    QPixmap *pixmap;
    QPen *black, *red, *blue;
    QLineEdit *maxStep;
    QPushButton *buttonBack, *buttonNew, *buttonExit;
    QPushButton *buttonRight, *buttonLeft, *buttonUp, *buttonDown;

    int redChained, blueChained;
    size_t steps;

    void mousePressEvent(QMouseEvent *event);
    void wheelEvent(QWheelEvent *event);
    void mouseMoveEvent(QMouseEvent *event);

    QPair<int, int> mouse_pressed_pos;
    QPair<double, double> offset_pressed_pos;

    void OLDchains();

    int gamesCount, maximumStep;
    bool maxstep_value_ok;
    double cellSize;
    double cellSize0;
    int offsetX0;
    int offsetY0;
    double offsetX;
    double offsetY;
    double scale;
    double scaleMin;

};

#endif // POINTSWINDOW_H
