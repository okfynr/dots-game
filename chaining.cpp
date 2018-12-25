#include "qmath.h"
#include <iostream>
#include <fstream>
#include <QDebug>
#include <vector>


#include "chaining.h"


using std::vector;

static QVector<QPair<int, int>> Dots;
static vector<int> color;
static size_t ncycle;
static vector<int> path;
static QVector<QVector<int>> edges;
static vector<vector<int>> cycle;



Chaining::Chaining(int xP, int yP, int step, QObject *parent)
    : QThread(parent)
{
    this -> xDot = xP;
    this -> yDot = yP;
    this -> Step = step;
}


Chaining::~Chaining()
{
    qDebug() << "destroyed";
}

void Chaining::run()
{
    color.resize(Step);
    for (auto i : color) color[i] = 0;
    ncycle = 0;     //0 cycles found right here
    path.clear();
    path.resize(Step);
    edges.clear();
    edges.resize(Step);
    cycle.clear();

    //qDebug() << "no DFS is coming!" << Edges[0] << " " << Edges[1] << endl;
    //QVector<QPair<double, double>> redDots, blueDots;
    //vector<vector<int>> redEdges, blueEdges;

    Dots.push_back(qMakePair(xDot, yDot));

    //qDebug() << "hello, i am here to chain smth: ";
    //qDebug() << "size of dots vector is " << Dots.size() << endl;

    //cycle to make edges instead of dots
    for (int i = 0; i < Step; ++i) {
        for (int j = 0; j < Step; ++j) {

            if ((    (Dots[i].first == Dots[j].first)
                  || (Dots[i].first == (Dots[j].first - 1))
                  || (Dots[i].first == (Dots[j].first + 1)) )
             && (    (Dots[i].second == Dots[j].second)
                  || (Dots[i].second == (Dots[j].second - 1))
                  || (Dots[i].second == (Dots[j].second + 1)) )
             && (i != j)
             && ( ((i & 1) && (j & 1)) || !((i & 1) || (j & 1)) ))
            {
                edges[i].push_back(j);
            }

        }
    }



    //qDebug() << "then DFS is coming! " << Edges;
    dfs(Step - 1);               // iterator Step begins with 1 and it is useful not here

    size_t max = 0;
    for (size_t i = 0; i < cycle.size(); ++i) {
        for (size_t j = 0; j < cycle[i].size(); ++j) {
            if ((cycle[i][j] == (Step - 1)) && (cycle[i].size() > max)) {
                max = i;
            }
        }
    }
    typedef QVector<int>  sending_type;             //crutch, wtf
    qRegisterMetaType<sending_type>("sending_type");
    if (max != 0) {
        sending_type sended;
        sended.resize(static_cast<int>(cycle[max].size()));
        sended = sending_type::fromStdVector(cycle[max]);

        emit newChain(sended);
        this->quit();
    }
    //qDebug() << cycle << endl;
    //qDebug() << "but true cycle is " << cycle[max];

}



size_t Chaining::add_cycle(int cycle_end, int cycle_st)
{
    cycle.resize(ncycle + 1); // if you're here, 1 cycle already found, so, you need 1 vector to write it
    //qDebug() << cycle << endl;
    cycle[ncycle].clear();
    cycle[ncycle].push_back(cycle_st);

    qDebug() << "current path: " << path;
    for(int v = cycle_end; v != cycle_st; v = path[v])
    {
        qDebug() << v << "not eq " << cycle_st;
        cycle[ncycle].push_back(v);
    }
    cycle[ncycle].push_back(cycle_st);

    reverse(cycle[ncycle].begin(), cycle[ncycle].end());

    return cycle[ncycle].size();
}

void Chaining::dfs(int dfsed_dot)
{

    color[dfsed_dot] = 1;
    //qDebug() << "DFS is coming! " << Edges[dfsed_dot].size() << endl;// << Edges.size();        if (i & 1)

    // roaming through all neighbours
    for(int i = 0; i < edges[dfsed_dot].size(); ++i)
    {
        //qDebug() << "DFS is coming!";

        //i-th neighbour of dfsed dot with given key
        int next_dot = edges[dfsed_dot][i];
        if(color[next_dot] == 0)
        {
            path[next_dot] = dfsed_dot;
            dfs(next_dot);
        }
        else if(color[next_dot] == 1)
        {
            if(add_cycle(dfsed_dot, next_dot) > 4) {  // avoiding of trivial cases
                //qDebug() << "cycle found: " << cycle[ncycle] << ". ";
                ncycle++;
                path.clear();
            }
            else cycle.pop_back();
        }
    }
    color[dfsed_dot] = 0;
}

/*int Chaining::true_cycle(vector<vector<int>> & cycles, int point)
{
    int max = 0;
    for (auto i = 0; i < (int)cycles.size(); ++i) {
        for (auto j = 0; j < (int)cycles[i].size(); ++j) {
            if ((cycles[i][j] == (point - 1)) && ((cycles[i].end() - cycles[i].begin()) > max)) {
                max = i;
            }
        }
    }
    return max;
}*/
