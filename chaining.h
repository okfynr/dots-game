#ifndef CHAINING_H
#define CHAINING_H

#include <QMainWindow>
#include <QWidget>
#include <QThread>
#include <vector>
#include <stack>


typedef QVector<int>  sending_type;

class Chaining : public QObject
{
    Q_OBJECT

public:
    Chaining(int, int, int, QVector<QPair<int, int>> & );
    ~Chaining();

signals:
    void newChain(sending_type);
public slots:
     void check_chains();

private:
    int xDot, yDot;
    int Step;


    QVector<QPair<int, int>> & Dots;
    std::vector<int> dots_colors;
    std::vector<int> previous_dot;
    std::vector<std::vector<int>> dots_edges;


    void add_cycle(int, int);

};

#endif // CHAINING_H
