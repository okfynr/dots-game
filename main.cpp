#include <QApplication>
#include "pointswindow.h"

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    PointsWindow w;
    w.show();

    return a.exec();
}
