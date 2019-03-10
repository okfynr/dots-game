#ifndef CHAINING_H
#define CHAINING_H

#include <QMainWindow>
#include <QWidget>
#include <QThread>
#include <vector>

using std::vector;

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
    vector<int> edges_color;
    size_t ncycle;
    vector<int> path;
    QVector<QVector<int>> edges;
    vector<vector<int>> cycle;

    size_t add_cycle(int, int);
    void dfs(int);
    //int true_cycle(vector<vector<int> > &, int);

};

#endif // CHAINING_H
